//
//  AdmixtureConstantBirthDeathProcess.h
//  revbayes_mlandis
//
//  Created by Michael Landis on 1/21/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#ifndef __revbayes_mlandis__AdmixtureConstantBirthDeathProcess__
#define __revbayes_mlandis__AdmixtureConstantBirthDeathProcess__

#include "AdmixtureTree.h"
#include "TypedDagNode.h"
#include "TypedDistribution.h"

namespace RevBayesCore {
    
    class AdmixtureConstantBirthDeathProcess : public TypedDistribution<AdmixtureTree> {
        
    public:
        AdmixtureConstantBirthDeathProcess(const TypedDagNode<double> *d, const TypedDagNode<double> *t, unsigned int nTaxa, const std::vector<std::string> &tn);
        AdmixtureConstantBirthDeathProcess(const AdmixtureConstantBirthDeathProcess &n);                                                                                          //!< Copy constructor
        virtual                                            ~AdmixtureConstantBirthDeathProcess(void);                                                                    //!< Virtual destructor
        
        // public member functions
        AdmixtureConstantBirthDeathProcess*                          clone(void) const;                                                                                  //!< Create an independent clone
        double                                              computeLnProbability(void);
        void                                                redrawValue(void);
        void                                                swapParameter(const DagNode *oldP, const DagNode *newP);                                            //!< Implementation of swaping parameters
        
    private:
        
        // helper functions
        void                                                attachTimes(std::vector<AdmixtureNode *> &tips, int index, const std::vector<double> &times, double T);
        void                                                buildRandomBinaryTree(std::vector<AdmixtureNode *> &tips);
        double                                              p1(double t, double T) const;
        double                                              pSurvival(double start, double end, double T) const;
        double                                              simOrigin(void);
        double                                              simSpeciation(double origin);
        void                                                simulateTree(void);
        
        // members
        const TypedDagNode<double>*                         diversification;
        const TypedDagNode<double>*                         turnover;
        unsigned int                                        numTaxa;
        std::vector<std::string>                            taxonNames;
        double                                              logTreeTopologyProb;
        
    };
    
}

#endif /* defined(__revbayes_mlandis__AdmixtureConstantBirthDeathProcess__) */
