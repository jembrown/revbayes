/**
 * @file
 * This file contains the declaration of the random variable class for a character state evolving along a tree.
 * This class is derived from the stochastic node and each instance will represent a random variable.
 *
 * @brief Declaration of the character state evolution along a tree class.
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date:$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2012-06-17, version 1.0
 * @interface TypedDagNode
 *
 * $Id:$
 */



#ifndef SimpleSiteHomogeneousGtrCharEvoModel_H
#define SimpleSiteHomogeneousGtrCharEvoModel_H

#include "AbstractSiteHomogeneousCharEvoModel.h"
#include "CharacterData.h"
#include "DnaState.h"
#include "RateMatrix.h"
#include "TopologyNode.h"
#include "TransitionProbabilityMatrix.h"
#include "TreeChangeEventListener.h"
#include "TypedDistribution.h"

namespace RevBayesCore {
    
    template<class charType, class treeType>
    class SimpleSiteHomogeneousCharEvoModel : public AbstractSiteHomogeneousCharEvoModel<charType, treeType> {
        
    public:
        SimpleSiteHomogeneousCharEvoModel(const TypedDagNode<treeType> *t, const TypedDagNode<RateMatrix> *rm, const TypedDagNode<double> *ra, bool c, size_t nSites);
        SimpleSiteHomogeneousCharEvoModel(const SimpleSiteHomogeneousCharEvoModel &n);                                                                                                //!< Copy constructor
        virtual                                            ~SimpleSiteHomogeneousCharEvoModel(void);                                                                   //!< Virtual destructor
        
        // public member functions
        SimpleSiteHomogeneousCharEvoModel*                  clone(void) const;                                                                          //!< Create an independent clone
        void                                                swapParameter(const DagNode *oldP, const DagNode *newP);                                    //!< Implementation of swaping parameters
        
    protected:
        void                                                computeRootLikelihood(const std::vector<size_t>& c);
        void                                                computeInternalNodeLikelihood(const TopologyNode &node, size_t left, size_t right);
        void                                                computeTipLikelihood(const TopologyNode &node);
        
    private:
        CharacterData<charType>*                            simulate(const TopologyNode& node);
        
        // members
        const TypedDagNode<double>*                         rateMultiplier;
        const TypedDagNode<RateMatrix>*                     rateMatrix;
        TransitionProbabilityMatrix                         transitionProbabilities;        
        
    };
    
}


#include "DiscreteCharacterState.h"
#include "RandomNumberFactory.h"
#include "TopologyNode.h"
#include "TransitionProbabilityMatrix.h"

#include <cmath>
#include <cstring>

template<class charType, class treeType>
RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::SimpleSiteHomogeneousCharEvoModel(const TypedDagNode<treeType> *t, const TypedDagNode<RateMatrix> *rm, const TypedDagNode<double> *ra, bool c, size_t nSites) : AbstractSiteHomogeneousCharEvoModel<charType, treeType>(  t, c, nSites ), rateMatrix( rm ), rateMultiplier( ra ), transitionProbabilities( rm->getValue().getNumberOfStates() ) {
    // add the parameters to the parents list
    this->addParameter( rateMatrix );
    this->addParameter( rateMultiplier );
    
    delete this->value;
    this->value = simulate(this->tau->getValue().getRoot());
}


template<class charType, class treeType>
RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::SimpleSiteHomogeneousCharEvoModel(const SimpleSiteHomogeneousCharEvoModel &n) : AbstractSiteHomogeneousCharEvoModel<charType, treeType>( n ), rateMatrix( n.rateMatrix ), rateMultiplier( n.rateMultiplier ), transitionProbabilities( n.transitionProbabilities ) {
    // parameters are automatically copied
    
}


template<class charType, class treeType>
RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::~SimpleSiteHomogeneousCharEvoModel( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
    
}


template<class charType, class treeType>
RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>* RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::clone( void ) const {
    return new SimpleSiteHomogeneousCharEvoModel<charType, treeType>( *this );
}


template<class charType, class treeType>
void RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::computeRootLikelihood( const std::vector<size_t> &children ) {
    
    // sum the per site probabilities
    const std::vector<double> &f = rateMatrix->getValue().getStationaryFrequencies();
    this->lnProb = 0.0;
    std::vector<std::vector<std::vector<double> >::const_iterator > p_children;
    for (std::vector<size_t>::const_iterator it = children.begin(); it != children.end(); ++it) {
        p_children.push_back( this->partialLikelihoods[this->activeLikelihood[*it]][*it].begin() );
    }

    std::vector<size_t>::const_iterator                 patternFreq     = this->patternCounts.begin();
    std::vector<size_t>::const_iterator                 end             = this->patternCounts.end();
    // iterate over all sites
    for (; patternFreq != end; ++patternFreq) {
        
        double tmp = 0.0;
        std::vector<double>::const_iterator f_j             = f.begin();
        std::vector<double>::const_iterator f_end           = f.end();
        for (size_t i = 0; f_j != f.end(); ++f_j, ++i) {
            double tmp2 = *f_j;
            for (size_t j = 0; j < children.size(); ++j) {
                tmp2 *= (*p_children[j])[i];
            }
            
            tmp += tmp2;
        }
        this->lnProb += log(tmp) * *patternFreq;
        
        for (size_t i = 0; i < children.size(); ++i) {
            p_children[i] = ++p_children[i];
        }
    }
}



template<class charType, class treeType>
void RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::computeInternalNodeLikelihood(const TopologyNode &node, size_t left, size_t right) {    
    
    // get the index in the node sequence
    size_t nodeIndex = node.getIndex();
    
    // compute the transition probability matrix
    rateMatrix->getValue().calculateTransitionProbabilities( node.getBranchLength()*rateMultiplier->getValue(), transitionProbabilities);
    
    std::vector<std::vector<double> > &p_left = this->partialLikelihoods[this->activeLikelihood[left]][left];
    std::vector<std::vector<double> > &p_right = this->partialLikelihoods[this->activeLikelihood[right]][right];
    std::vector<std::vector<double> > &p_node = this->partialLikelihoods[this->activeLikelihood[nodeIndex]][nodeIndex];
    
    // compute the per site probabilities
    std::vector<std::vector<double> >::const_iterator   p_site_left     = p_left.begin();
    std::vector<std::vector<double> >::const_iterator   p_site_right    = p_right.begin();
    std::vector<std::vector<double> >::iterator         p_site          = p_node.begin();
    std::vector<std::vector<double> >::iterator         p_end           = p_node.end();
    for (; p_site != p_end; ++p_site, ++p_site_left, ++p_site_right) {
        
        // iterate over the possible starting states
        std::vector<double>::iterator                       p_a     = p_site->begin();
        std::vector<std::vector<double> >::const_iterator   tp_a    = transitionProbabilities.begin();
        std::vector<std::vector<double> >::const_iterator   tp_end  = transitionProbabilities.end();
        for (; tp_a != tp_end; ++tp_a, ++p_a) {
            
            double sum = 0.0;
            
            // iterate over all possible terminal states
            std::vector<double>::const_iterator p_site_left_d   = p_site_left->begin();
            std::vector<double>::const_iterator p_site_right_d  = p_site_right->begin();
            std::vector<double>::const_iterator tp_a_d          = tp_a->begin();
            std::vector<double>::const_iterator tp_a_end        = tp_a->end();
            for (; tp_a_d != tp_a_end; ++tp_a_d, ++p_site_left_d, ++p_site_right_d ) {
                sum += *p_site_left_d * *p_site_right_d * *tp_a_d;
            }
            *p_a = sum;
            
        }
    }
}


template<class charType, class treeType>
void RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::computeTipLikelihood(const TopologyNode &node) {    
    
    // get the index in the node sequence
    size_t nodeIndex = node.getIndex();
    
    std::vector<std::vector<double> > &p_node = this->partialLikelihoods[this->activeLikelihood[nodeIndex]][nodeIndex];
    
    std::vector<bool> &gap_node = this->gapMatrix[nodeIndex];
    
    std::vector<unsigned int> &char_node = this->charMatrix[nodeIndex];
    
    // compute the transition probabilities
    rateMatrix->getValue().calculateTransitionProbabilities( node.getBranchLength()*rateMultiplier->getValue(), transitionProbabilities );
    
    // compute the per site probabilities
    std::vector<std::vector<double> >::iterator p_site      = p_node.begin();
    std::vector<std::vector<double> >::iterator p_end       = p_node.end();
    std::vector<bool>::const_iterator           gap_site    = gap_node.begin();
    std::vector<unsigned int>::const_iterator   char_site   = char_node.begin();
    for (; p_site != p_end; ++p_site, ++char_site, ++gap_site) {
        
        if ( *gap_site == true ) {
            std::vector<double>::iterator                       p_a     = p_site->begin();
            std::vector<std::vector<double> >::const_iterator   tp_a    = transitionProbabilities.begin();
            std::vector<std::vector<double> >::const_iterator   tp_end  = transitionProbabilities.end();
            for (; tp_a != tp_end; ++tp_a, ++p_a) {
                
                double tmp = 0.0;
                
                std::vector<double>::const_iterator d   = tp_a->begin();
                std::vector<double>::const_iterator end = tp_a->end();
                for (; d != end; ++d) {
                    tmp += *d;
                }
                *p_a = tmp;
            } 
        }
        else {
            unsigned int org_val = *char_site;
            std::vector<double>::iterator                       p_a     = p_site->begin();
            std::vector<std::vector<double> >::const_iterator   tp_a    = transitionProbabilities.begin();
            std::vector<std::vector<double> >::const_iterator   tp_end  = transitionProbabilities.end();
            for (; tp_a != tp_end; ++tp_a, ++p_a) {
                
                double tmp = 0.0;
                
                unsigned int val = org_val;
                std::vector<double>::const_iterator d   = tp_a->begin();
                std::vector<double>::const_iterator end = tp_a->end();
                for (; d != end; ++d) {
                    if ( (val & 1) == 1 ) {
                        tmp += *d;
                    }
                    
                    val >>= 1;
                }
                *p_a = tmp;
            }
        }
    }
    
}




template<class charType, class treeType>
RevBayesCore::CharacterData<charType>* RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::simulate(const TopologyNode &node) {
    return new CharacterData<charType>();
}



template<class charType, class treeType>
void RevBayesCore::SimpleSiteHomogeneousCharEvoModel<charType, treeType>::swapParameter(const DagNode *oldP, const DagNode *newP) {
    if (oldP == rateMatrix) {
        rateMatrix = static_cast<const TypedDagNode<RateMatrix>* >( newP );
    }
    else if (oldP == rateMultiplier) {
        rateMultiplier = static_cast<const TypedDagNode<double>* >( newP );
    }
    else {
        AbstractSiteHomogeneousCharEvoModel<charType, treeType>::swapParameter(oldP,newP);
    }
}


#endif
