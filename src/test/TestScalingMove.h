/**
 * @file
 * This file contains the declaration of the normal model test class. 
 * This test runs an MCMC analysis on a normal model with the data being normal distributed, 
 * a uniform prior on mu and an exponential prior on sigma.
 *
 *
 * @brief Declaration of the normal model test class
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @since Version 1.0, 2012-04-17
 *
 * $Id$
 */


#ifndef TestScalingMove_H
#define TestScalingMove_H


class TestScalingMove {
    
public:
    TestScalingMove(int gen);
    virtual                    ~TestScalingMove(void);                                                          //!< Virtual destructor
    
    bool                        run();
    
private:
    
    // members
    int                         mcmcGenerations;
};

#endif
