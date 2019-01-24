
typedef struct nodeAdQuad {
    int node;
    int ad;
    int inf;
    int pay;
} nodeAdQuad;

//#include "iheap.h"
#include <queue>	
#include <utility>  
#include <math.h>

struct comparePay {
    bool operator()(nodeAdQuad a, nodeAdQuad b)
    {
        return a.pay < b.pay;
    }
};


class InfGraph: public Graph
{
private:
    vector<bool> visit;
    vector<int> visit_mark;
    //return the number of edges visited
public:
    sfmt_t sfmtSeed;
	vector<vector<int>> seedSet;
    vector<vector<vector<int>>> hyperG;
    vector<vector<vector<int>>> hyperGT;
    vector<int> scaledBudgets; 
    vector<int> influence;
    vector<int> indPayoff;
    deque<int> q;
    int numRR; 

    InfGraph(string folder, string graph_file): Graph(folder, graph_file)
    {
        sfmt_init_gen_rand(&sfmtSeed , 95082);
        init_hyper_graph();
        visit = vector<bool> (n, false);
        visit_mark = vector<int> (n);
        scaledBudgets = vector<int> (numAdvert);
        influence = vector<int> (numAdvert);
        indPayoff = vector<int> (numAdvert);
    }

    void init_hyper_graph(){
        numRR = 0;
        hyperG.clear();
        for (int i=0; i < numNetworks; i++)
        {
            hyperG.push_back(vector<vector<int>>());
            for (int j = 0; j < n; j++)
                hyperG[i].push_back(vector<int>());
        }
        hyperGT.clear();
        for (int i=0; i < numNetworks; i++)
            hyperGT.push_back(vector<vector<int>>());

    }

    void updateScaledBudgets()
    {
        FOR(i, numAdvert)
            scaledBudgets[i] = (int)((((int64)budgets[i])*((int64)numRR))/n);
    }


    void build_hyper_graph_r(int64 R)
    {
        if( R > INT_MAX ){
            cout<<"Error:R too large"<<endl;
            exit(1);
        }
        numRR = R;
        updateScaledBudgets();
        //INFO("build_hyper_graph_r", R);

        int prevSize = 0;
        for (int i=0; i<numNetworks; i++)
        {
            while ((int)hyperGT[i].size() <= R)
                hyperGT[i].push_back( vector<int>() );
        }



        vector<int> random_number;
        for (int i = 0; i < R; i++)
        {
            random_number.push_back(  sfmt_genrand_uint32(&sfmtSeed) % n);
        }

        //trying BFS start from same node
        

        for (int i = 0; i < R; i++)
            for (int j = 0; j < numNetworks; j++) //build for all ads
                BuildHypergraphNode(random_number[i], i, j);


        int totAddedElement = 0;
        for (int j = 0; j < numNetworks; j++) //create reverse mapping for all advertisers
        {
            for (int i = prevSize; i < R; i++)
            {
                for (int t : hyperGT[j][i])
                {
                    hyperG[j][t].push_back(i);
                    //hyperG.addElement(t, i);
                    totAddedElement++;
                }
            }
        }
    }

#include "discrete_rrset.h"

    int payoff()
    {
        //count influence and payoff
        vector<bool> elemInclude(numRR, false);
        int totalPayoff = 0;
        for (int j=0; j<numAdvert; j++)
        {
            influence[j] = 0;
            int hyperGraphId = (j % numNetworks); //find the influence network used
            for (int i : seedSet[j])
            {
                for (int t : hyperG[hyperGraphId][i])
                {
                    ASSERT (t < numRR);
                    if (!elemInclude[t])
                    {
                        elemInclude[t] = true;
                        influence[j]++;
                    }
                }  
            }
            for (int i=0; i<numRR; i++)
                elemInclude[i] = false;
            int scaledInfluence = (int)(((long long int)influence[j])*((long long int)n))/((long long int)numRR);
            indPayoff[j] = (scaledInfluence > budgets[j]) ? budgets[j] : scaledInfluence;
            totalPayoff += indPayoff[j];
            printf("seed set of advertiser %d :", j);
            for (int t : seedSet[j])
                printf("%d ", t);
            printf("\n");
            printf("number of nodes influenced by advertiser %d is %d and payoff is %d\n", j, scaledInfluence, indPayoff[j]);
        }
        printf("total payoff is %d\n", totalPayoff);
        return totalPayoff;
    }


	//This is build on Mapped Priority Queue
    void build_seedset(int l, int h)
	{
        priority_queue<nodeAdQuad, vector<nodeAdQuad>, comparePay> heap;
        vector<vector<int>> coverage;
        coverage = vector<vector<int>> (n);

		int maxInd, totalAds, hyperGraphId;

    //remnant budgets of advertisers
        vector<int> remBudgets;
        for (int i=0; i<numAdvert; i++)
            remBudgets.push_back(scaledBudgets[i]);

    //initialize heap with individual influence of all node,ad pairs
		for (int i = 0; i < n; i++)
		{
            coverage[i] = vector<int> (numAdvert);
            for (int j=0; j<numAdvert; j++)
            {
                nodeAdQuad temp;
                temp.node = i;
                temp.ad = j;
                hyperGraphId = (j%numNetworks); 
                temp.inf = hyperG[hyperGraphId][i].size();
			    coverage[i][j] = temp.inf;
                temp.pay = (temp.inf > scaledBudgets[j]) ? scaledBudgets[j] : temp.inf; 
			    heap.push(temp);
            }
		}


        //count individual influence for each advertiser
        vector<int> influence;
        influence = vector<int> (numAdvert, 0);

        //count number of ads per user
        vector<int> adsPerUser;
        adsPerUser = vector<int> (n, 0);

		// check if an edge is removed
        vector<vector<bool>> edgeMark;
        edgeMark = vector<vector<bool>> (numAdvert);
        for (int i=0; i<numAdvert; i++)
            edgeMark[i] = vector<bool> (numRR, false);
        
		seedSet.clear();
        seedSet = vector<vector<int>> (numAdvert); 
        ASSERT (l <= h*n);
        totalAds = 0;
		while (totalAds<l)
		{
			nodeAdQuad ele = heap.top();
			heap.pop();
            if (adsPerUser[ele.node] >= h)
                continue;
			if ((ele.inf > coverage[ele.node][ele.ad]) || (ele.pay > remBudgets[ele.ad]))
			{
				ele.inf = coverage[ele.node][ele.ad];
                int temp = (ele.inf > remBudgets[ele.ad]) ? remBudgets[ele.ad] : ele.inf; 
                if (temp != ele.pay)
                {
                    ele.pay = temp;
                    heap.push(ele);
                    continue; 
                }
			}

            totalAds++;
			maxInd = ele.node;
            hyperGraphId = (ele.ad % numNetworks);

			influence[ele.ad] += ele.inf;
            remBudgets[ele.ad] = max(remBudgets[ele.ad]-ele.inf, 0);
            adsPerUser[ele.node]++;
			seedSet[ele.ad].push_back(ele.node);

			vector<int>e = hyperG[hyperGraphId][maxInd];  
			for (unsigned int j = 0; j < e.size(); j++){
				if (edgeMark[ele.ad][e[j]])continue;

				edgeMark[ele.ad][e[j]] = true;

                for (int t : hyperGT[hyperGraphId][e[j]])
                    coverage[t][ele.ad]--;

			}
		}
	}

};





