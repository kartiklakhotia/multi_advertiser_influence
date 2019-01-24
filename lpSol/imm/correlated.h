
void swap (vector<dipair> &vect, int i, int j)
{
    dipair temp = vect[i];
    vect[i] = vect[j];
    vect[j] = temp;
}

void roundlp (const Argument &arg)
{
    int totalAds = 0;
    vector<dipair> xI;
    vector<vector<dipair>> xA;  
    xA = vector<vector<dipair>> (numAdvert);
    xI = vector<dipair> (numAdvert);
    for (int i=0; i<n; i++)
    {
        int lastElem = 0;
//////////////////////////////////
/////////// initialize ///////////
//////////////////////////////////
        for (int j=0; j<numAdvert; j++)
        {
            xI[lastElem].first = x[i][j]; 
            xI[lastElem++].second = j;
        }

        lastElem--;
        
///////////////////////////////////////////////////////
/////////// remove already rounded elements ///////////
///////////////////////////////////////////////////////
        int adId = 0;
        for (int j=0; j<numAdvert; j++)
        {   
            if ((xI[adId].first == 0) || (xI[adId].first == 1))
            {
                swap(xI, adId, lastElem);
                lastElem--;
                continue;
            }
            adId++;
        }

///////////////////////////////////////////////////////
/////////// round until one fraction remains //////////
///////////////////////////////////////////////////////
        while (lastElem > 0)
        {
            double x0 = xI[0].first;
            double x1 = xI[1].first;
            double alpha = min(x0, 1-x1);
            double beta = min(1-x0, x1);
            double randDouble = sfmt_genrand_real1(&sfmtSeed);
            if (randDouble < (alpha)/(alpha + beta))
            {
                xI[0].first = x0 + beta;
                xI[1].first = x1 - beta;
                if (1-x0 <= x1)
                {
                    swap(xI, 0, lastElem);
                    lastElem--;
                }
                if (1-x0 >= x1)
                {
                    swap(xI, 1, lastElem);
                    lastElem--;
                }   
            }
            else
            {
                xI[0].first = x0 - alpha;
                xI[1].first = x1 + alpha;
                if (x0 <= 1-x1)
                {
                    swap(xI, 0, lastElem);
                    lastElem--;
                }
                if (x0 >= 1-x1)
                {
                    swap(xI, 1, lastElem);
                    lastElem--;
                }   
            }
        }
///////////////////////////////////////////////////////
/////////// save last fraction for advertisers/////////
///////////////////////////////////////////////////////
        if (lastElem == 0)
        {
            dipair temp;
            temp.first = xI[0].first;
            temp.second = i;
            xA[xI[0].second].push_back(temp);
//            double randDouble = sfmt_genrand_real1(&sfmtSeed);
//            xI[0].first = (xI[0].first > randDouble) ? 1 : 0;
        }
///////////////////////////////////////////////////////
///////// store the rounded floats back in x //////////
///////////////////////////////////////////////////////
        for (int j=0; j<numAdvert; j++)
        {
            x[i][xI[j].second] = xI[j].first;
        }
//        for (int j=0; j<numAdvert; j++)
//        {
//            xR[i][xI[j].second] = (int)std::round(xI[j].first);
//            totalAds += xR[i][xI[j].second];
//        }
    }

    //correlated rounding of potential seed nodes for same advertiser
    for (int i=0; i<numAdvert; i++)
    {
        int lastElem = xA[i].size()-1; 
        while(lastElem > 0)
        {
            double x0 = xA[i][0].first;
            double x1 = xA[i][1].first;
            double alpha = min(x0, 1-x1);
            double beta = min(1-x0, x1);
            double randDouble = sfmt_genrand_real1(&sfmtSeed);
            if (randDouble < (alpha)/(alpha + beta))
            {
                xA[i][0].first = x0 + beta;
                xA[i][1].first = x1 - beta;
                if (1-x0 <= x1)
                {
                    swap(xA[i], 0, lastElem);
                    lastElem--;
                }
                if (1-x0 >= x1)
                {
                    swap(xA[i], 1, lastElem);
                    lastElem--;
                }   
            }
            else
            {
                xA[i][0].first = x0 - alpha;
                xA[i][1].first = x1 + alpha;
                if (x0 <= 1-x1)
                {
                    swap(xA[i], 0, lastElem);
                    lastElem--;
                }
                if (x0 >= 1-x1)
                {
                    swap(xA[i], 1, lastElem);
                    lastElem--;
                }   
            }
        }
        if (lastElem == 0)
        {
            double randDouble = sfmt_genrand_real1(&sfmtSeed);
            xA[i][0].first = (xA[i][0].first > randDouble) ? 1 : 0;
        }
        for (auto t : xA[i])
        {
            x[t.second][i] = t.first;
        } 
    }

    //put the rounded value in an integer array
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<numAdvert; j++)
            xR[i][j] = (int)std::round(x[i][j]);
    }
}

