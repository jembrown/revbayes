/**
 * @file
 * This file contains the declaration of the DAG node class, which is our base class for all DAG node is in the model graph.
 * It is merely used as a common base class to connect the entire graph.
 *
 * @brief Declaration of the base class DagNode.
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2012-06-17, version 1.0
 * @interface DagNode
 *
 * $Id$
 */



#ifndef DagNode_H
#define DagNode_H

#include <map>
#include <set>
#include <string>
#include <vector>

namespace RevBayesCore {

    class DagNode {
    
    public:
    
        virtual                                            ~DagNode(void);                                                                  //!< Virtual destructor

        // pure virtual methods
        virtual DagNode*                                            clone(void) const = 0;
        virtual DagNode*                                            cloneDAG(std::map<const DagNode*, DagNode*> &nodesMap) const = 0;               //!< Clone the entire DAG which is connected to this node
        virtual double                                              getLnProbability(void) = 0;
        virtual double                                              getLnProbabilityRatio(void) = 0;
        virtual void                                                printValue(std::ostream &o) const = 0;                                          //!< Monitor/Print this variable 
        virtual void                                                printValue(std::ostream &o,size_t i) const = 0;                                 //!< Monitor/Print the i-th element of this variable 
        virtual void                                                redraw(void) = 0;                                                               //!< Redraw the current value of the node (applies only to stochastic nodes)
        
        // public member functions
        void                                                        addChild(DagNode *child) const;                                                 //!< Add a new child node
        void                                                        addParent(const DagNode *p);                                                    //!< Add a parent node
        void                                                        addTouchedElementIndex(size_t i);                                               //!< Add the index of an element that has been touch (usually for vector-like values)
        void                                                        clearTouchedElementIndices(void);
        DagNode*                                                    cloneDownstreamDag(std::map<const DagNode*, DagNode*> &nodesMap) const;         //!< Clone the DAG which is downstream to this node (all children)
        void                                                        collectDownstreamGraph(std::set<DagNode*> &nodes);                              //!< Collect all nodes downstream from this node (incl the node)
        void                                                        getAffectedNodes(std::set<DagNode *>& affected);                                //!< get affected nodes
        //    DagNode*                                            getChild(size_t index);                                                         //!< Get the child at the index
        const std::set<DagNode*>&                                   getChildren(void) const;                                                        //!< Get the set of parents
        DagNode*                                                    getFirstChild(void) const;                                                      //!< Get the first child from a our set
        const std::string&                                          getName(void) const;                                                            //!< Get the of the node
        size_t                                                      getNumberOfChildren(void) const;                                                //!< Get the number of children for this node
        const std::set<const DagNode*>&                             getParents(void) const;                                                         //!< Get the set of parents
        const std::set<size_t>&                                     getTouchedElementIndices(void) const;                                           //!< Get the indices of the touches elements. If the set is empty, then all elements might have changed.
        virtual bool                                                isClamped(void) const;                                                          //!< Is this node clamped? Only stochastic nodes might be clamped.
        virtual bool                                                isConstant(void) const;                                                         //!< Is this DAG node constant?
        virtual bool                                                isStochastic(void) const;                                                       //!< Is this DAG node stochastic?
        void                                                        keep(void);
        virtual void                                                keepAffected(void);                                                             //!< Keep value of affected nodes
        void                                                        removeChild(DagNode *child) const;
        void                                                        removeParent(const DagNode *p);                                                 //!< Remove a parent
        void                                                        replace(DagNode *n);                                                            //!< Replace this node with node p.
        void                                                        restore(void);
        virtual void                                                restoreAffected(void);                                                          //!< Restore value of affected nodes recursively
        void                                                        setName(const std::string &n);                                                  //!< Set the name of this variable for identification purposes.
        void                                                        swapParent(const DagNode *oldP, const DagNode *newP);                           //!< Exchange the parent node which includes setting myself as a child of the new parent and removing myself from my old parents children list
        void                                                        touch(void);
        virtual void                                                touchAffected(void);                                                            //!< Touch affected nodes (flag for recalculation)
  
    protected:
                                                                    DagNode(const std::string &n);                                                  //!< Constructor
                                                                    DagNode(const DagNode &n);                                                      //!< Constructor
    
        virtual void                                                getAffected(std::set<DagNode *>& affected, DagNode* affecter) = 0;              //!< get affected nodes
        virtual void                                                keepMe(DagNode* affecter) = 0;                                                  //!< Keep value of myself
        virtual void                                                restoreMe(DagNode *restorer) = 0;                                               //!< Restore value of this nodes
        virtual void                                                swapParameter(const DagNode *oldP, const DagNode *newP) = 0;                    //!< Swap the parameter of this node (needs overwriting in deterministic and stochastic nodes)
        virtual void                                                touchMe(DagNode *toucher) = 0;                                                  //!< Touch myself (flag for recalculation)
    
        // members
        mutable std::set<DagNode*>                                  children;                                                                      //!< The children in the model graph of this node
        std::string                                                 name;
        std::set<const DagNode*>                                    parents;                                                                        //!< The parents in the DAG of this node
        std::set<size_t>                                            touchedElements;
    };

}

#endif

