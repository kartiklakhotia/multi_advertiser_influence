#define HEAD_INFO
#define CORRELATED
//#undef CORRELATED

#include "sfmt/SFMT.h"
#include "head.h"

class Argument{
public:
    int k;
    int h; //max number of advertisements per user
    string dataset;
    double epsilon;
    int indOpt;
    string model;
    double T;
};

#include "graph.h"
#include "infgraph.h"
#include "imm.h"



void run_with_parameter(InfGraph &g, const Argument & arg)
{
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << arg.dataset << " k=" << arg.k << " epsilon=" << arg.epsilon <<   " " << arg.model << endl;

        Imm::dumpRRSets(g, arg);

        string command = "python lp/cmsc.py";
        system(command.c_str()); //run the python script
        Imm::readLpOutput(g, arg); //read script output
        Imm::roundLpOut(g, arg); //round script output
        Imm::computePayoff(g);
//        INFO(g.seedSet);
//       
//    Timer::show();
}
void Run(int argn, char **argv)
{
    Argument arg;


    for (int i = 0; i < argn; i++)
    {
        if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
        {
            cout << "./tim -dataset *** -epsilon *** -k ***  -h ***" << endl;
            return ;
        }
        if (argv[i] == string("-dataset")) 
            arg.dataset = argv[i + 1];
        if (argv[i] == string("-epsilon")) 
            arg.epsilon = atof(argv[i + 1]);
        if (argv[i] == string("-T")) 
            arg.T = atof(argv[i + 1]);
        if (argv[i] == string("-k")) 
            arg.k = atoi(argv[i + 1]);
        if (argv[i] == string("-h"))
            arg.h = atoi(argv[i + 1]);
    }
    arg.model = "IC";

    string graph_file;
    graph_file = arg.dataset + "graph_ic.inf";

    InfGraph g(arg.dataset, graph_file);

    run_with_parameter(g, arg);
}


int main(int argn, char **argv)
{
    __head_version = "v1";
    OutputInfo info(argn, argv);


    Run( argn, argv );
    return 0;
}


