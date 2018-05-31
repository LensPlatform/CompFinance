
/*
Written by Antoine Savine in 2018

This code is the strict IP of Antoine Savine

License to use and alter this code for personal and commercial applications
is freely granted to any person or company who purchased a copy of the book

Modern Computational Finance: AAD and Parallel Simulations
Antoine Savine
Wiley, 2018

As long as this comment is preserved at the top of the file
*/

#pragma once

#include "mcBase.h"

#define ONE_HOUR 0.000114469

template <class T>
class UOC : public Product<T>
{
    double          myStrike;
    double          myBarrier;
    Time            myMaturity;
    vector<Time>    myTimeline;

public:

    //  Constructor: store data and build timeline
    //  Timeline = system date to maturity, 
    //  with steps every monitoring frequency
    UOC(const double strike, 
        const double barrier, 
        const Time maturity, 
        const Time monitorFreq)
        : myStrike(strike), 
        myBarrier(barrier), 
        myMaturity(maturity)
    {
        myTimeline.push_back(systemTime);
        Time t = systemTime + monitorFreq;

        while (myMaturity - t > ONE_HOUR)
        {
            myTimeline.push_back(t);
            t += monitorFreq;
        }

        if (myTimeline.back() < myMaturity)
            myTimeline.push_back(myMaturity);
    }

    //  Virtual copy constructor
    unique_ptr<Product<T>> clone() const override
    {
        return unique_ptr<Product<T>>(new UOC<T>(*this));
    }

    //  Timeline
    const vector<Time>& timeline() const override
    {
        return myTimeline;
    }

    //  Payoff
    T payoff(const vector<scenario<T>>& path) const override
    {
        //  We apply the smooth barrier technique to stabilize risks
        //  See Savine's presentation on Fuzzy Logic, Global Derivatives 2016
        //  Or Andreasen and Savine's publication on scripting

        //  We apply a smoothing factor of 1% of the spot both ways, untemplated
        const double smooth = convert<double>(path[0].spot * 0.01);

        //  We start alive
        T alive = convert<T>(1.0);

        //  Go through path, update alive status
        for (size_t i = 0; i < path.size(); ++i)
        {
            //  Breached
            if (path[i].spot > myBarrier + smooth) return convert<T>(0.0);

            //  Semi-breached: apply smoothing
            if (path[i].spot > myBarrier - smooth)
            {
                alive *= (myBarrier + smooth - path[i].spot) / (2 * smooth);
            }
        }

        //  Payoff
        return alive * max<T>(path.back().spot - myStrike, convert<T>(0.0));
    }
};
