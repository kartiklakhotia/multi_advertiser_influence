
typedef pair<double,int> dipair;


//#include "iheap.h"
#include <queue>	
#include <utility>  
#include <math.h>

struct CompareBySecond {
	bool operator()(pair<int, int> a, pair<int, int> b)
	{
		return a.second < b.second;
	}
};


class InfGraph: public Graph
{
private:
    vector<bool> visit;
    vector<int> visit_mark;
public:
    sfmt_t sfmtSeed;
	vector<vector<int>> seedSet;
    //vector<vector<int>> hyperG;
    //vector<vector<int>> hyperGT;
    vector<vector<vector<int>>> hyperG;
    vector<vector<vector<int>>> hyperGT;
    vector<vector<double>> x; //probabilities of picking a vertex returned by LP
    vector<vector<int>> xR; //rounded probabilities (0 or 1)
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
        seedSet = vector<vector<int>> (numAdvert);
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

        //initialize 2D array to store probabilities
        x = vector<vector<double>> (n);
        xR = vector<vector<int>> (n);
        for (int i=0; i<n; i++)
        {
            x[i] = vector<double> (numAdvert); 
            xR[i] = vector<int> (numAdvert);
        }
    }

    void updateScaledBudgets()
    {
        FOR(i, numAdvert)
            scaledBudgets[i] = (int)((((int64)budgets[i])*((int64)numRR))/n);
    }

    void build_hyper_graph_r(int64 R, const Argument & arg)
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
            random_number.push_back(sfmt_genrand_uint32(&sfmtSeed) % n);
        }


        //trying BFS start from same node
        

        for (int i = 0; i < R; i++)
            for (int j = 0; j < numNetworks; j++) //build for all ads
                BuildHypergraphNode(random_number[i], i, j);


        int totAddedElement = 0;
        //for (int i = prevSize; i < R; i++)
        //{
        //    for (int t : hyperGT[i])
        //    {
        //        hyperG[t].push_back(i);
        //        //hyperG.addElement(t, i);
        //        totAddedElement++;
        //    }
        //}
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
//include rounding functions
#ifdef CORRELATED
#include "correlated.h"
#else
#include "direct.h"
#endif 

    int payoff()
    {
        //count influence and payoff
        vector<bool> elemInclude(numRR, false);
        int totalPayoff = 0;
        for (int j=0; j<numAdvert; j++)
        {
            influence[j] = 0;
            int hyperGraphId = (j % numNetworks); //find the influence network used
            for (int i=0; i<n; i++)
            {
                if (xR[i][j] == 0)
                    continue;
                seedSet[j].push_back(i);
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
            int scaledInfluence = (influence[j]*n)/numRR;
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

};





