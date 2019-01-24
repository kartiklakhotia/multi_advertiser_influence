#define HEAD_INFO
//#define HEAD_TRACE
#include "sfmt/SFMT.h"
#include "head.h"
using namespace std;
typedef double (*pf)(int, int);
class Graph
{
public:
    int n, m, numAdvert, numNetworks, k; //numNetworks is 1 if all advertisers have same influence network
    vector<int> inDeg;
    vector<vector<int>> gT;

//probability different for different advertisers
    vector<vector<vector<double>>> probT;
//budgets of different advertisers
    vector<int> budgets;

//    enum InfluModel {IC, LT, CONT};
//    InfluModel influModel;
//    void setInfuModel(InfluModel p)
//    {
//        influModel = p;
//        TRACE(influModel == IC);
//        TRACE(influModel == LT);
//        TRACE(influModel == CONT);
//    }

    string folder;
    string graph_file;
    void readNM()
    {
        ifstream cin((folder + "attribute.txt").c_str());
        ASSERT(!cin == false);
        string s;
        while (cin >> s)
        {
            if (s.substr(0, 2) == "n=")
            {
                n = atoi(s.substr(2).c_str());
                continue;
            }
            if (s.substr(0, 2) == "m=")
            {
                m = atoi(s.substr(2).c_str());
                continue;
            }
///////////////////////////////////////
////////number of advertisers//////////
///////////////////////////////////////
            if (s.substr(0, 2) == "a=")
            {
                numAdvert = atoi(s.substr(2).c_str());
//#ifdef SINGLE_GRAPH
//                numNetworks = 1;
//#else
//                numNetworks = numAdvert;
//#endif
                continue;
            }
            if (s.substr(0,2) == "g=")
            {
                numNetworks = atoi(s.substr(2).c_str());
                numNetworks = (numNetworks == 1) ? 1 : numAdvert;
                continue;
            }
            ASSERT(false);
        }
        TRACE(n, m );
        cin.close();
    }
    

///////////////////////////////////////
/////// advertiser budgets ////////////
///////////////////////////////////////
    void readBudgets()
    {
        FILE* fp = fopen((folder + "budgets.txt").c_str(), "r");
        FOR(i, numAdvert)
        {
            int b;
            int c = fscanf(fp, "%d", &b);
            ASSERTT(c == 1, b);
            budgets.push_back(b);
        }
        fclose(fp);
    }
//    void add_edge(int a, int b, double p)
//    {
//        probT[b].push_back(p);
//        gT[b].push_back(a);
//        inDeg[b]++;
//    }
    void add_edge(int a, int b, double* p)
    {
        FOR (i, numNetworks)
        {
            probT[i][b].push_back(p[i]);
        }
        gT[b].push_back(a);
        inDeg[b]++;
    }
//    vector<bool> hasnode;
    void readGraph()
    {
///////////////////////////////////////////////////////
///////read probabilities for all advertisers//////////
///////////////////////////////////////////////////////
        double** pIn = new double* [numNetworks];
        FOR(i,numNetworks)
        {
            pIn[i] = new double [m];
        }
        FOR(i,numNetworks)
        {
            FILE* fp = fopen((folder+"prob"+to_string(i)).c_str(), "r"); //assuming folder = folder_name + '/'
            FOR (j, m)
            {
                double p;
                int c = fscanf(fp, "%lf", &p);
                ASSERTT(c == 1, p);
                pIn[i][j] = p;
            } 
            fclose(fp);
        }
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
        FILE *fin = fopen((graph_file).c_str(), "r");
        ASSERT(fin != false);
        int readCnt = 0;
        //double p;
        double* p = new double [numNetworks];
        for (int i = 0; i < m; i++)
        {
            readCnt ++;
            int a, b;
            // set the probabilities read before
            FOR(j,numNetworks)
            {
                p[j] = pIn[j][i];
            }
            //int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
            //ASSERTT(c == 3, a, b, p, c);
            int c = fscanf(fin, "%d%d", &a, &b);
            ASSERTT(c == 2, a, b, c);


            //TRACE_LINE(a, b);
            ASSERT( a < n );
            ASSERT( b < n );
            //hasnode[a] = true;
            //hasnode[b] = true;
            add_edge(a, b, p);
        }
        TRACE_LINE_END();
//        int s = 0;
//        for (int i = 0; i < n; i++)
//            if (hasnode[i])
//                s++;
//        INFO(s);
        ASSERT(readCnt == m);
        fclose(fin);
///////////////////////////////////////////////////////
///////delete allocations no longer required///////////
///////////////////////////////////////////////////////
        FOR(i, numNetworks)
        {
            delete[] pIn[i];
        }
        delete[] pIn;
        delete[] p;
    }

//#ifdef DISCRETE
//    void readGraphBin()
//    {
//        string graph_file_bin = graph_file.substr(0, graph_file.size() - 3) + "bin";
//        INFO(graph_file_bin);
//        FILE *fin = fopen(graph_file_bin.c_str(), "rb");
//        //fread(fin);
//        struct stat filestatus;
//        stat( graph_file_bin.c_str(), &filestatus );
//        int64 sz = filestatus.st_size;
//        char *buf = new char[sz];
//        int64 sz2 = fread(buf, 1, sz, fin);
//        INFO("fread finish", sz, sz2);
//        ASSERT(sz == sz2);
//        for (int64 i = 0; i < sz / 12; i++)
//        {
//            int a = ((int *)buf)[i * 3 + 0];
//            int b = ((int *)buf)[i * 3 + 1];
//            float p = ((float *)buf)[i * 3 + 2];
//            //INFO(a,b,p);
//            add_edge(a, b, p);
//        }
//        delete []buf;
//        fclose(fin);
//    }
    void readGraphBin()
    {
///////////////////////////////////////////////////////
///////read probabilities for all advertisers//////////
///////////////////////////////////////////////////////
        double** pIn = new double* [numNetworks];
        FOR(i,n)
        {
            pIn[i] = new double [m];
        }
        FOR(i,numNetworks)
        {
            FILE* fp = fopen((folder+"prob"+to_string(i)+".bin").c_str(), "rb"); //assuming folder = folder_name + '/'
            fread(pIn[i], sizeof(double), m, fp); 
            fclose(fp);
        }
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
        string graph_file_bin = graph_file.substr(0, graph_file.size() - 3) + "bin";
        FILE *fin = fopen((graph_file_bin).c_str(), "rb");
        int* buf = new int [2*m]; 
        fread(buf, sizeof(int), 2*m, fin);
        fclose(fin);
        double* p = new double [numNetworks];
        FOR(i, m)
        {
            FOR(j,numNetworks)
            {
                p[j] = pIn[j][i];
            }
            add_edge(buf[2*i], buf[2*i+1], p);
        } 
///////////////////////////////////////////////////////
///////delete allocations no longer required///////////
///////////////////////////////////////////////////////
        FOR(i, numNetworks)
        {
            delete[] pIn[i];
        }
        delete[] pIn;
        delete[] p;
        delete[] buf;
    }
//#endif

    Graph(string folder, string graph_file): folder(folder), graph_file(graph_file)
    {
        readNM();
        readBudgets();

        //init vector
        FOR(i, numNetworks)
        {
            probT.push_back(vector<vector<double>>());
        }
        FOR(i, n)
        {
            gT.push_back(vector<int>());
//            hasnode.push_back(false);
            FOR(j, numNetworks)
                probT[j].push_back(vector<double>());
            //hyperGT.push_back(vector<int>());
            inDeg.push_back(0);
        }
        readGraph();
        //system("sleep 10000");
    }

};
double sqr(double t)
{
    return t * t;
}



