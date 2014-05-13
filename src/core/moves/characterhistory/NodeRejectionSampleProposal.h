//
//  NodeRejectionSampleProposal.h
//  rb_mlandis
//
//  Created by Michael Landis on 5/7/14.
//  Copyright (c) 2014 Michael Landis. All rights reserved.
//

#ifndef __rb_mlandis__NodeRejectionSampleProposal__
#define __rb_mlandis__NodeRejectionSampleProposal__

#include "BranchHistory.h"
#include "DeterministicNode.h"
#include "DiscreteCharacterData.h"
#include "DistributionBinomial.h"
#include "PathRejectionSampleProposal.h"
#include "Proposal.h"
#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "RateMap.h"
#include "RbException.h"
#include "StochasticNode.h"
//#include "TransitionProbability.h"
#include "TypedDagNode.h"

#include <cmath>
#include <iostream>
#include <set>
#include <string>

namespace RevBayesCore {
    
    /**
     * The scaling operator.
     *
     * A scaling proposal draws a random uniform number u ~ unif(-0.5,0.5)
     * and scales the current vale by a scaling factor
     * sf = exp( lambda * u )
     * where lambda is the tuning parameter of the Proposal to influence the size of the proposals.
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2009-09-08, version 1.0
     *
     */
    
    template<class charType, class treeType>
    class NodeRejectionSampleProposal : public Proposal {
        
    public:
        NodeRejectionSampleProposal( StochasticNode<AbstractCharacterData> *n, StochasticNode<treeType>* t, DeterministicNode<RateMap> *q, double l );                                                                //!<  constructor
        
        // Basic utility functions
        void                            assignNodeIndex(size_t idx);
        void                            assignSiteIndexSet(const std::set<size_t>& s);
        NodeRejectionSampleProposal*    clone(void) const;                                                                  //!< Clone object
        void                            cleanProposal(void);
        double                          doProposal(void);                                                                   //!< Perform proposal
        const std::vector<DagNode*>&    getNodes(void) const;                                                               //!< Get the vector of DAG nodes this proposal is working on
        const std::string&              getProposalName(void) const;                                                        //!< Get the name of the proposal for summary printing
        void                            printParameterSummary(std::ostream &o) const;                                       //!< Print the parameter summary
        void                            prepareProposal(void);                                                              //!< Prepare the proposal
        void                            sampleNodeCharacters(const TopologyNode& node, const std::set<size_t>& indexSet);   //!< Sample the characters at the node
        void                            swapNode(DagNode *oldN, DagNode *newN);                                             //!< Swap the DAG nodes on which the Proposal is working on
        void                            tune(double r);                                                                     //!< Tune the proposal to achieve a better acceptance/rejection ratio
        void                            undoProposal(void);                                                                 //!< Reject the proposal
        
    protected:
        
        // parameters
        StochasticNode<AbstractCharacterData>*  variable;
        StochasticNode<treeType>*               tau;
        DeterministicNode<RateMap>*             qmap;
        std::vector<DagNode*>                   nodes;

        // dimensions
        size_t                                  numNodes;
        size_t                                  numCharacters;
        size_t                                  numStates;
        
        // proposal
        std::map<size_t,BranchHistory*>         storedValues;
        std::map<size_t,BranchHistory*>         proposedValues;
        BranchHistory*                          storedValue;
        size_t                                  nodeIndex;
        std::set<size_t>                        siteIndexSet;
        double                                  storedLnProb;
        BranchHistory*                          proposedValue;
        double                                  proposedLnProb;
        
        PathRejectionSampleProposal<charType,treeType>* nodeProposal;
        PathRejectionSampleProposal<charType,treeType>* leftProposal;
        PathRejectionSampleProposal<charType,treeType>* rightProposal;
        
        TransitionProbabilityMatrix nodeTpMatrix;
        TransitionProbabilityMatrix leftTpMatrix;
        TransitionProbabilityMatrix rightTpMatrix;
        
        double                                  lambda;
        
        // flags
        bool                                    sampleNodeIndex;
        bool                                    sampleSiteIndexSet;
        
    };
    
}



/**
 * Constructor
 *
 * Here we simply allocate and initialize the Proposal object.
 */
template<class charType, class treeType>
RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::NodeRejectionSampleProposal( StochasticNode<AbstractCharacterData> *n, StochasticNode<treeType> *t, DeterministicNode<RateMap>* q, double l) : Proposal(),
variable(n),
tau(t),
qmap(q),
numNodes(t->getValue().getNumberOfNodes()),
numCharacters(n->getValue().getNumberOfCharacters()),
numStates(static_cast<const DiscreteCharacterState&>(n->getValue().getCharacter(0,0)).getNumberOfStates()),
storedValue(NULL),
proposedValue(NULL),
//nodeProposal(new PathRejectionSampleProposal<charType,treeType>(n,t,q,l)),
//leftProposal(new PathRejectionSampleProposal<charType,treeType>(n,t,q,l)),
//rightProposal(new PathRejectionSampleProposal<charType,treeType>(n,t,q,l)),
nodeTpMatrix(numStates),
leftTpMatrix(numStates),
rightTpMatrix(numStates),
lambda(l),
sampleNodeIndex(true),
sampleSiteIndexSet(true)

{
    nodes.push_back(variable);
    nodes.push_back(tau);
    nodes.push_back(qmap);

    nodeProposal = new PathRejectionSampleProposal<charType,treeType>(n,t,q,l);
    leftProposal = new PathRejectionSampleProposal<charType,treeType>(n,t,q,l);
    rightProposal = new PathRejectionSampleProposal<charType,treeType>(n,t,q,l);
    ;
}


template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::cleanProposal( void )
{
    const TopologyNode& node = tau->getValue().getNode(nodeIndex);
    nodeProposal->cleanProposal();
    if (!node.isTip())
    {
        rightProposal->cleanProposal();
        leftProposal->cleanProposal();
    }
}

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'B'.
 *
 * \return A new copy of the proposal.
 */
template<class charType, class treeType>
RevBayesCore::NodeRejectionSampleProposal<charType, treeType>* RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::clone( void ) const
{
    return new NodeRejectionSampleProposal( *this );
}

template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::assignNodeIndex(size_t idx)
{
    nodeIndex = idx;
    sampleNodeIndex = false;
}

template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::assignSiteIndexSet(const std::set<size_t>& s)
{
    siteIndexSet = s;
    sampleSiteIndexSet = false;
}


/**
 * Get Proposals' name of object
 *
 * \return The Proposals' name.
 */
template<class charType, class treeType>
const std::string& RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::getProposalName( void ) const
{
    static std::string name = "NodeRejectionSampleProposal";
    
    return name;
}


/**
 * Get the vector of nodes on which this proposal is working on.
 *
 * \return  Const reference to a vector of nodes pointer on which the proposal operates.
 */
template<class charType, class treeType>
const std::vector<RevBayesCore::DagNode*>& RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::getNodes( void ) const
{
    
    return nodes;
}


/**
 * Perform the Proposal.
 *
 * A scaling Proposal draws a random uniform number u ~ unif(-0.5,0.5)
 * and scales the current vale by a scaling factor
 * sf = exp( lambda * u )
 * where lambda is the tuning parameter of the Proposal to influence the size of the proposals.
 *
 * \return The hastings ratio.
 */
template<class charType, class treeType>
double RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::doProposal( void )
{
    
    proposedLnProb = 0.0;

    const TopologyNode& node = tau->getValue().getNode(nodeIndex);
    
    // update 1x pathEnd and 2x pathStart values
    sampleNodeCharacters(node, siteIndexSet);
    
    // update 3x incident paths
    double proposedLnProbRatio = 0.0;
    proposedLnProbRatio += nodeProposal->doProposal();
    if (!node.isTip())
    {
        proposedLnProbRatio += leftProposal->doProposal();
        proposedLnProbRatio += rightProposal->doProposal();
    }
    
    return proposedLnProbRatio;
}


/**
 *
 */
template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::prepareProposal( void )
{
    storedLnProb = 0.0;
    
    if (sampleNodeIndex)
    {
        nodeIndex = GLOBAL_RNG->uniform01() * numNodes;
    }
    
    if (sampleSiteIndexSet)
    {
        siteIndexSet.clear();
        siteIndexSet.insert(GLOBAL_RNG->uniform01() * numCharacters); // at least one is inserted
        for (size_t i = 0; i < numCharacters; i++)
        {
            if (GLOBAL_RNG->uniform01() < lambda)
            {
                siteIndexSet.insert(i);
            }
        }
    }

    sampleNodeIndex = true;
    sampleSiteIndexSet = true;
    
    const TopologyNode& node = tau->getValue().getNode(nodeIndex);

    nodeProposal->assignNodeIndex(node.getIndex());
    nodeProposal->assignSiteIndexSet(siteIndexSet);
    nodeProposal->prepareProposal();
    if (!node.isTip())
    {
        leftProposal->assignNodeIndex(node.getChild(0).getIndex());
        leftProposal->assignSiteIndexSet(siteIndexSet);
        leftProposal->prepareProposal();
        
        rightProposal->assignNodeIndex(node.getChild(1).getIndex());
        rightProposal->assignSiteIndexSet(siteIndexSet);
        rightProposal->prepareProposal();
    }
    
}


/**
 * Print the summary of the Proposal.
 *
 * The summary just contains the current value of the tuning parameter.
 * It is printed to the stream that it passed in.
 *
 * \param[in]     o     The stream to which we print the summary.
 */
template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::printParameterSummary(std::ostream &o) const
{
    o << "lambda = " << lambda;
}


template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::sampleNodeCharacters(const TopologyNode& node, const std::set<size_t>& indexSet)
{

    if (node.isTip())
    {
        // will specify update for noisy tip data in alt. proposal
        ;
    }
    else
    {
        AbstractTreeHistoryCtmc<charType, treeType>* p = static_cast< AbstractTreeHistoryCtmc<charType, treeType>* >(&variable->getDistribution());
        
        qmap->getValue().calculateTransitionProbabilities(node, nodeTpMatrix);
        qmap->getValue().calculateTransitionProbabilities(node.getChild(0), leftTpMatrix);
        qmap->getValue().calculateTransitionProbabilities(node.getChild(1), rightTpMatrix);
        
        std::vector<BranchHistory*> histories = p->getHistories();
        
        // for sampling probs
        const std::vector<CharacterEvent*>& parentState = histories[node.getIndex()]->getParentCharacters();
        const std::vector<CharacterEvent*>& leftChildState = histories[node.getChild(0).getIndex() ]->getChildCharacters();
        const std::vector<CharacterEvent*>& rightChildState = histories[node.getChild(1).getIndex()]->getChildCharacters();
        
        // to update
        std::vector<CharacterEvent*> childState = histories[node.getIndex()]->getChildCharacters();
        std::vector<CharacterEvent*> leftParentState = histories[node.getChild(0).getIndex() ]->getParentCharacters();
        std::vector<CharacterEvent*> rightParentState = histories[node.getChild(1).getIndex()]->getParentCharacters();

//        std::cout << "node old " << nodeIndex << "\n";
//        histories[node.getIndex()]->print();
        
        for (std::set<size_t>::iterator it = siteIndexSet.begin(); it != siteIndexSet.end(); it++)
        {
            unsigned int ancS = parentState[*it]->getState();
            unsigned int desS1 = leftChildState[*it]->getState();
            unsigned int desS2 = rightChildState[*it]->getState();
            
            double u = GLOBAL_RNG->uniform01();
            double g0 = nodeTpMatrix[ancS][0] * leftTpMatrix[0][desS1] * rightTpMatrix[0][desS2];
            double g1 = nodeTpMatrix[ancS][1] * leftTpMatrix[1][desS1] * rightTpMatrix[1][desS2];
            
            unsigned int s = 0;
            if (u < g1 / (g0 + g1))
                s = 1;
            
            childState[*it] = new CharacterEvent(*it, s, 1.0);
            leftParentState[*it] = new CharacterEvent(*it, s, 1.0);
            rightParentState[*it] = new CharacterEvent(*it, s, 1.0);
        }

        histories[node.getIndex()]->setChildCharacters(childState);
        histories[node.getChild(0).getIndex()]->setParentCharacters(leftParentState);
        histories[node.getChild(1).getIndex()]->setParentCharacters(rightParentState);

//        std::cout << "node new " << nodeIndex << "\n";
//        histories[node.getIndex()]->print();
        
//        std::cout << "new'\n";
//        p->getHistory(node.getIndex()).print();
        
//         p->setHistory(*histories[node.getIndex()], node.getIndex());
//         p->setHistory(*histories[node.getChild(0).getIndex()], node.getChild(0).getIndex());
//         p->setHistory(*histories[node.getChild(1).getIndex()], node.getChild(1).getIndex());
        ;
    }
}

/**
 * Reject the Proposal.
 *
 * Since the Proposal stores the previous value and it is the only place
 * where complex undo operations are known/implement, we need to revert
 * the value of the variable/DAG-node to its original value.
 */
template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::undoProposal( void )
{
    // swap current value and stored value
    const TopologyNode& node = tau->getValue().getNode(nodeIndex);
    nodeProposal->undoProposal();
    if (!node.isTip())
    {
        rightProposal->undoProposal();
        leftProposal->undoProposal();
    }
}


/**
 * Swap the current variable for a new one.
 *
 * \param[in]     oldN     The old variable that needs to be replaced.
 * \param[in]     newN     The new variable.
 */
template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::swapNode(DagNode *oldN, DagNode *newN)
{
    
    if (oldN == variable)
    {
        variable = static_cast<StochasticNode<AbstractCharacterData>* >(newN) ;
    }
    else if (oldN == tau)
    {
        tau = static_cast<StochasticNode<treeType>* >(newN);
    }
    else if (oldN == qmap)
    {
        qmap = static_cast<DeterministicNode<RateMap>* >(newN);
    }
    
    nodeProposal->swapNode(oldN, newN);
    leftProposal->swapNode(oldN, newN);
    rightProposal->swapNode(oldN, newN);
}


/**
 * Tune the Proposal to accept the desired acceptance ratio.
 */
template<class charType, class treeType>
void RevBayesCore::NodeRejectionSampleProposal<charType, treeType>::tune( double rate )
{
    ; // do nothing
}

#endif /* defined(__rb_mlandis__NodeRejectionSampleProposal__) */