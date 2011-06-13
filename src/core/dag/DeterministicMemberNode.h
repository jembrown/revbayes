/**
 * @file
 * This file contains the declaration of MemberNode, which is used to
 * hold RbComplex values, i.e., data types with member variables, in
 * model DAGs.
 *
 * @brief Declaration of MemberNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2010-01-12 21:51:54 +0100 (Tis, 12 Jan 2010) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @since 2010-09-28
 *
 * $Id: MemberNode.h 221 2010-01-12 20:51:54Z Hoehna $
 */

#ifndef DeterministicMemberNode_H
#define DeterministicMemberNode_H

#include "DeterministicNode.h"

class MemberObject;
class RbString;
class TypeSpec;
class VectorString;


class DeterministicMemberNode : public DeterministicNode {

    public:
                                    DeterministicMemberNode(const std::string& valType);                        //!< Constructor of empty member node
                                    DeterministicMemberNode(MemberObject* val);                                 //!< Constructor from member object
                                    DeterministicMemberNode(const DeterministicMemberNode& x);                               //!< Copy constructor (for member object)
        virtual                    ~DeterministicMemberNode(void);                                              //!< Destructor (member object)

        // Assignment operator
        DeterministicMemberNode&    operator=(const DeterministicMemberNode& x);                                //!< Assignment operator

        // Basic utility functions
        DeterministicMemberNode*    clone(void) const;                                                          //!< Clone the member node
        const VectorString&         getDAGClass(void) const;                                                    //!< Get DAG node class vector
        bool                        isConst(void) const;                                                        //!< Is this node a const value?
        void                        printStruct(std::ostream& o) const;                                         //!< Print struct for user
        std::string                 richInfo(void) const;                                                       //!< Complete info about object

        // Parser element access functions
        bool                        existsElement(VectorInteger& index);                                        //!< Does element exist?
        DAGNode*                    getElement(VectorInteger& index);                                           //!< Give the parser an element
        DAGNode*                    getElementOwner(VectorInteger& index);                                      //!< Give the parser the element owner
//        void                        setElement(const VectorNatural& index, DAGNode* var, bool convert=true);    //!< Set element
        void                        setElement(size_t index, DAGNode* var, bool convert=true);                  //!< Set element

        // DAG function
        DeterministicMemberNode*    cloneDAG(std::map<const DAGNode*, DAGNode*>& newNodes) const;               //!< Clone entire graph

        // MemberNode functions
        DAGNode*                    executeMethod(  const std::string&              name,
                                                const std::vector<Argument>&    args) const;                    //!< Execute member method
        const TypeSpec&             getMemberTypeSpec(const RbString& name) const;                              //!< Get type spec of a named member variable
        MemberObject*               getMemberObject(void) { return memberObject; }                              //!< Get member object 

    protected:
        // Utility function
        void                        update(void);                                                               //!< Update value and storedValue

        // Member variable
        MemberObject*               memberObject;                                                               //!< The member object
};

#endif
