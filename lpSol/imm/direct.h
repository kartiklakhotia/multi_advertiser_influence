void roundlp (const Argument &arg)
{
    int totalAds = 0;
    int violatedUsers = 0;
    for (int i=0; i<n; i++)
    {
        int adsPerUser = 0;
        for (int j=0; j<numAdvert; j++)
        {
            if (x[i][j] == 1.0)
            {
                xR[i][j] = 1;
                adsPerUser++;
                continue;
            }
            else if (x[i][j] == 0)
            {
                xR[i][j] = 0;
                continue;
            }
            double randDouble = sfmt_genrand_real1(&sfmtSeed);
            if (x[i][j] > randDouble)
            {
                xR[i][j] = 1;
                adsPerUser++;
            }
            else
            {
                xR[i][j] = 0;
            }
        } 
        if (adsPerUser > arg.h)
            violatedUsers++; 
        totalAds += adsPerUser;
    }
    printf("totalAdBudget was %d; number of sponsored ads were %d\n number of users with violated ad constraints were %d\n", arg.k, totalAds, violatedUsers);
}
