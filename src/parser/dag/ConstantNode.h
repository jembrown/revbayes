/**
 * @file
 * This file contains the declaration of ConstantNode, which is derived
 * from DAGNode. ConstantNode is used for DAG nodes holding constant
 * values in a model DAG.
 *
 * @brief Declaration of ConstantNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-16, version 1.0
 * @extends DAGNode
 *
 * $Id$
 */

#ifndef ConstantNode_H
#define ConstantNode_H

#include "DAGNode.h"
#include "RbLanguageObject.h"
#include "RbPtr.h"

class InferenceDagNode;
class RbObject;

class ConstantNode : public DAGNode {

public:
    ConstantNode(const RbPtr<RbLanguageObject> &val);                                                                   //!< Constructor from value
    ConstantNode(const RbPtr<RbLanguageObject> &val, const RbPtr<const Plate> &p);                                      //!< Constructor from value
    ConstantNode(const ConstantNode &x);                                                                                //!< Copy constructor

    // overloaded operators
    ConstantNode&                       operator=(const ConstantNode &c);                                               //!< Overloaded assignment operator
    
    // Basic utility functions
    ConstantNode*                       clone(void) const;                                                              //!< Clone this object
    static const std::string&           getClassName(void);                                                             //!< Get class name
    static const TypeSpec&              getClassTypeSpec(void);                                                         //!< Get class type spec
    const TypeSpec&                     getTypeSpec(void) const;                                                        //!< Get language type of the object
    void                                printStruct(std::ostream& o) const;                                             //!< Print struct for user
    void                                printValue(std::ostream& o) const;                                              //!< Print value for user

    // ConstantNode functions
    const RbLanguageObject&             getValue(void) const;                                                           //!< Get value 
    RbLanguageObject&                   getValue(void);                                                                 //!< Get value 
    void                                setValue(const RbPtr<RbLanguageObject> &val);                                   //!< Set the value of the constant node

    // DAG functions
    DAGNode*                            cloneDAG(std::map<const DAGNode*, RbPtr<DAGNode> >& newNodes) const;               //!< Clone entire graph
    InferenceDagNode*                   createLeanDag(std::map<const DAGNode*, InferenceDagNode*>& newNodes) const;     //!< Create a lean DAG from this "fat" DAG
    void                                expand(void);                                                                   //!< Expand the current value n times. This is equivalent to dropping this node on a plate of size n.
    bool                                isTouched (void) const { return false; }                                        //!< Touched by a move?
    bool                                isEliminated(void) const;
    bool                                isNotInstantiated(void) const;


protected:

    void                                getAffected(std::set<RbPtr<StochasticNode> >& affected);                              //!< Mark and get affected nodes
    void                                keepMe(void);                                                                   //!< Keep value of this and affected nodes
    void                                restoreMe(void);                                                                //!< Restore value of this nodes
    void                                touchMe(void);                                                                  //!< Tell affected nodes value is reset

private:

    RbPtr<RbLanguageObject>             value;                                                                          //!< Value

};

#endif
