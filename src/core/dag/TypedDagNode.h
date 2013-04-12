/**
 * @file
 * This file contains the declaration of the typed DAG node class, which is our base class for all DAG node with a specific type.
 * This class is used as the typed base class for DAG nodes as in cases as when parameters (parents) of other DAG nodes are set.
 *
 * @brief Declaration of the typed base class DagNode.
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2012-06-17, version 1.0
 * @interface TypedDagNode
 *
 * $Id$
 */



#ifndef TypedDagNode_H
#define TypedDagNode_H

#include "DagNode.h"
#include "RbUtil.h"

#include <ostream>
#include <string>

namespace RevBayesCore {
    
    template<class valueType>
    class TypedDagNode : public DagNode {
    
    public:
        TypedDagNode(const std::string &n);
        TypedDagNode(const TypedDagNode<valueType> &n);                                                                                 //!< Copy constructor
        virtual                                            ~TypedDagNode(void);                                                         //!< Virtual destructor
    
        // pure virtual methods
        virtual TypedDagNode<valueType>*                    clone(void) const = 0;
    
        // member functions
        virtual void                                        printValue(std::ostream &o) const;                                          //!< Monitor/Print this variable 
        virtual void                                        printValue(std::ostream &o, size_t i) const;                                //!< Monitor/Print the i-th element of this variable 

        // getters and setters
        virtual valueType&                                  getValue(void) = 0;
        virtual const valueType&                            getValue(void) const = 0;

        // @deprecated:
//        virtual valueType*                                  getValuePtr(void);

    };
    
}

#include "RbUtil.h"
#include <vector>

template<class valueType>
RevBayesCore::TypedDagNode<valueType>::TypedDagNode(const std::string &n) : DagNode( n ) {
    
}


template<class valueType>
RevBayesCore::TypedDagNode<valueType>::TypedDagNode(const TypedDagNode<valueType> &n) : DagNode( n ) {
    
}


template<class valueType>
RevBayesCore::TypedDagNode<valueType>::~TypedDagNode( void ) {
}


template<class valueType>
void RevBayesCore::TypedDagNode<valueType>::printValue(std::ostream &o) const {
    o << getValue();
}


template<class valueType>
void RevBayesCore::TypedDagNode<valueType>::printValue(std::ostream &o, size_t index) const {
    if ( RbUtils::is_vector<valueType>::value ) {
        o << RbUtils::sub_vector<valueType>::getElement( getValue(), index );
    } else {
        o << getValue();
    }
}


//template <typename T, typename Alloc>
//void RevBayesCore::TypedDagNode<std::vector<T,Alloc> >::printValue(std::ostream &o, size_t index) const {
////    if ( is_vector<valueType>::value ) {
//        o << getValue()[index];
////    } else {
////        o << getValue();
////    }
//}

#endif

