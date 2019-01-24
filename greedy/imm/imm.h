
class Math{
    public:
        static double log2(int n){
            return log(n) / log(2);
        }
        static double logcnk(int n, int k) {
            double ans = 0;
            for (int i = n - k + 1; i <= n; i++)
            {
                ans += log(i);
            }
            for (int i = 1; i <= k; i++)
            {
                ans -= log(i);
            }
            return ans;
        }
};
class Imm
{
    private:
        static InfGraph g;

        static void step1 (InfGraph &g, const Argument & arg)
        {
            double epsilon = arg.epsilon;
            int n = g.n;
            int m = g.numAdvert; 
            int opt = arg.indOpt;
            int l = arg.k;
            int c = 1; //probability of RR being an epsilon estimate is 1-(n)^-c, default value = 1
//            int64 theta = (8*m + 2*epsilon)*m*n*(c*log(n) + Math::logcnk(n, l) + log(2*m*l))/(opt*epsilon*epsilon);
            int64 theta = 10*n;
            g.build_hyper_graph_r(theta);
//            g.read_hyper_graph_r(theta);
        }
    public:
        static void InfluenceMaximize(InfGraph &g, const Argument &arg)
        {
            // generate RR sets
            step1(g, arg ); 
#ifdef DEBUG
            dumpRRSets(g, arg);
#endif
            g.build_seedset(arg.k, arg.h); 
        }

        static void dumpRRSets(InfGraph &g, const Argument &arg)
        {
            // generate RR sets
            FILE* fp = fopen("imm/rrDump.txt", "w");
            fprintf(fp, "%d\n", g.n);
            fprintf(fp, "%d\n", g.numAdvert);
            fprintf(fp, "%d\n", g.numNetworks);
            fprintf(fp, "%d\n", g.numRR);
            fprintf(fp, "%d\n", arg.h);
            fprintf(fp, "%d\n", arg.k);
            for (int i=0; i<g.numAdvert; i++)
                fprintf(fp, "%d ", g.scaledBudgets[i]);
            fprintf(fp, "\n");
            for (int i=0; i<g.n; i++)
            {
                for (int j=0; j<g.numNetworks; j++)
                {
                    for (int t : g.hyperG[j][i])
                        fprintf(fp, "%d ", t);
                    fprintf(fp, "\n");
                }
            }
            fclose(fp);
        }

    
        static int computePayoff (InfGraph &g)
        {
            return g.payoff();
        } 
};
