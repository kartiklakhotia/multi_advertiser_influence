Code for greedy and LP rounding based seed set selection in a mult-advertiser scenario.
Author - Kartik Lakhotia


Directory structure

1. greedy  - greedy algorithm
2. lpSol - LP rounding based algorithm 

To run a particular algorithm, go to the corresponding directory and use following commands.

## Compile
   make

## Run the code
    ./imm_greedy -dataset *** -epsilon *** -k ***  -h ***"
    
    k is the total seed set size for all advertisers
    h is the max number of advertisements per user

## Input dataset
    Directory containing the following files:
        
        attribute.txt - specify problem parameters
            n - number of vertices
            m - number of edges
            a - number of advertisers
            g - number of influence networks (either 1 or a)

        budgets.txt - max budget for each advertiser

        graph_ic.inf - edge list

        prob<i> - edge probabilities for each influence network

## Reference Dataset
    For reference, checkout the ./testGraph/ directory 

## Acknowledgements
    This code was built on top of the single greedy influence maximization code available at\
    https://sourceforge.net/projects/im-imm/

    
