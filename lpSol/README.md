LP based solution of multi advertiser seed set selection problem (no penalty).

Directory structure

1. imm - C code to construct reverse reachability sets; call the LP and round its solutions to find influence
2. lp - python PULP interface based LP construction and solving

Some notations:

1. S_{ij} - RR simulations for advertiser j's network in which vertex i was reached.
2. n - number of vertices
3. m - number of advertisers
4. x_{ij} - indicator variable for vertex i being picked as seed of advertiser j

C++ code and Python code interact through text files.

1. rrDump.txt - input to lp solver.   
Format:  
<number of vertices in the graph>  
<number of advertisers>  
<number of influence networks>
<number of RR simulations ran>  
<upper bound on number of advertisements per user>  
<upper bound on total number of advertisements>  
Budgets of all advertisers (in terms of number of RR sets) separated by space - <B0 B1 B2 ...>  
elements of S_{00} separated by space - <e0, e1, ...>  
elements of S_{01} separated by space - <e0, e1, ...>  
.  
.  
.  
elements of S_{0m} separated by space - <e0, e1, ...>  
elements of S_{10} separated by space - <e0, e1, ...>  
elements of S_{11} separated by space - <e0, e1, ...>  
.  
.  
.  
elements of S_{n0} separated by space - <e0, e1, ...>  
.  
.  
.  
elements of S_{nm} separated by space - <e0, e1, ...>  

2. lpOut.txt  - for each vertex, space separated probabilities of selection as seed set of different advertisers 
Format:  
<number of vertices in the graph>  
<number of advertisers>  
x_{00} x_{01} ... x_{0m}  
x_{10} x_{11} ... x_{1m}  
.  
.  
.  
x_{n0} x_{n1} ... x_{nm}  


LP output is rounded by direct randomized rounding. Define the CORRELATED flag to use correlated rounding.


**Note that number of influence networks can be different than number of advertisers as multiple advertisers may share the same network. We support either 1 network for all advertisers or individual networks.
