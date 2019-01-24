# Author: Kartik Lakhotia
# Email id: klakhoti@usc.edu
# Date: 08-Jul-2018
# 
# This code implements LP to solve the CMSC problem



from pulp import *

############################################
########### READ RR DUMP FILE ##############
############################################
fp = open("lp/rrDump.txt", "r")
flines = []
for line in fp:
    flines.append(line.rstrip())
flineCnt = 0

############################################
########### PARAMETERS INIT ################
############################################
numVertex = int(flines[0])
numAdvert = int(flines[1])
numGraphs = int(flines[2]) #number of different influence networks - either 1 or individual for all advertisers
numRRSets = int(flines[3]) 
adPerUser = int(flines[4])
adInTotal = int(flines[5])
flineCnt = 6


############################################
################# READ DATA ################
############################################
# Read budgets
B = map(int, flines[flineCnt].split(" "))
flineCnt = flineCnt + 1

# Read vertex subsets (RR simulations in which vertices were reached)
subset = []
for i in range(numVertex):
    subset_ad = []
    if (numGraphs==1): #same influence network for all advertisers
        if (flines[flineCnt]==""): #empty lines
            subset_ad = []
        else:
            subset_ad = map(int, flines[flineCnt].split(" "))
            flineCnt = flineCnt + 1
    else:
        for j in range(numAdvert):
            if (flines[flineCnt]==""): #empty lines
                subset_ij = []
            else:
                subset_ij = map(int, flines[flineCnt].split(" "))
            subset_ad.append(subset_ij)
            flineCnt = flineCnt + 1
    subset.append(subset_ad)
fp.close()

############################################
################ LP Problem ################
############################################
lpProb = LpProblem("cmsc", LpMaximize)

############################################
################ LP Variables ##############
############################################
x = LpVariable.dicts("x", (range(numVertex), range(numAdvert)), 0, 1)
z = LpVariable.dicts("z", (range(numRRSets), range(numAdvert)), 0, 1) 
p = LpVariable.dicts("p", range(numAdvert), lowBound = 0)

############################################
################ LP Objective ##############
############################################
objVar = 0
#for k in range(numRRSets):
#    for j in range(numAdvert):
#        objVar += z[k][j]
for j in range(numAdvert):
    objVar += p[j]
lpProb += objVar


############################################
############## LP Constraints ##############
############################################

# payoff is the minimum of budgets and total coverage
pUp = [0 for i in range(numAdvert)]
for j in range(numAdvert):
    for k in range(numRRSets):
        pUp[j] += z[k][j]
    lpProb += (p[j] <= pUp[j])
    lpProb += (p[j] <= B[j])

# element covered if subset containig it is selected
for i in range(numVertex):
    for j in range(numAdvert):
        if (numGraphs==1):
            for e in range(len(subset[i])):
                k = subset[i][e]
                lpProb += (z[k][j] >= x[i][j])
        else:
            for e in range(len(subset[i][j])):
                k = subset[i][j][e]
                lpProb += (z[k][j] >= x[i][j])

# element not covered if all subsets containing it are not selected
zUp = [[0 for col in range(numAdvert)] for row in range(numRRSets)]
for i in range(numVertex):
    for j in range(numAdvert):
        if (numGraphs==1):
            for e in range(len(subset[i])):
                k = subset[i][e]
                zUp[k][j] += x[i][j]
        else:
            for e in range(len(subset[i][j])):
                k = subset[i][j][e]
                zUp[k][j] += x[i][j]

for k in range(numRRSets):
    for j in range(numAdvert):
        lpProb += (z[k][j] <= zUp[k][j])

# ads per user are constrained
for i in range(numVertex):
    lpProb += (lpSum([x[i][j] for j in range(numAdvert)]) <= adPerUser)

# total ads are constrained
totalAds = 0
for i in range(numVertex):
    for j in range(numAdvert):
        totalAds += x[i][j]
lpProb += (totalAds <= adInTotal)
        
            

############################################
################# LP Solve #################
############################################
print("started solving\n")
#solver = CPLEX_DLL()
#solver.changeEpcomp(0.01)
#lpProb.solve(solver)
lpProb.solve(CPLEX())



############################################
############### Write output ###############
############################################
xOut = [[value(x[row][col]) for col in range(numAdvert)] for row in range(numVertex)] 

with open("imm/lpOut.txt", 'w') as f:
    f.writelines(str(numVertex)+'\n')
    f.writelines(str(numAdvert)+'\n')
    f.writelines(' '.join(str(j) for j in i) + '\n' for i in xOut)


#for v in lpProb.variables():
#    print(v.name, "=", v.varValue)
payoff = value(objVar)*numVertex
payoff = payoff/numRRSets
print("optimal coverage is = " + str(payoff))
