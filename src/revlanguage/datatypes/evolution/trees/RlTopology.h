/**
 * @file
 * This file contains the declaration of a Topology, which is
 * the class that holds the topological structure of a tree.
 *
 * @brief Declaration of Topology
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date: $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 *
 * $Id$
 */

#ifndef RlTopology_H
#define RlTopology_H

#include "Topology.h"
#include "RlModelVariableWrapper.h"

#include <set>
#include <string>
#include <vector>


namespace RevLanguage {
    
    
    class Topology : public RlModelVariableWrapper<RevBayesCore::Topology> {
        
    public:
        Topology(void);                                                                                                                         //!< Constructor requires character type
        Topology(RevBayesCore::Topology *v);                                                                                                    //!< Constructor requires character type
        Topology(const RevBayesCore::Topology &v);                                                                                              //!< Constructor requires character type
        Topology(RevBayesCore::TypedDagNode<RevBayesCore::Topology> *n);                                                                        //!< Constructor requires character type
        Topology(const Topology& d);                                                                                                            //!< Constructor requires character type
        
        typedef RevBayesCore::Topology valueType;
        
        // Basic utility functions
        Topology*                           clone(void) const;                                                                                  //!< Clone object
        static const std::string&           getClassName(void);                                                                                 //!< Get class name
        static const TypeSpec&              getClassTypeSpec(void);                                                                             //!< Get class type spec
        const TypeSpec&                     getTypeSpec(void) const;                                                                            //!< Get language type of the object
        
        // Member method inits
        const MethodTable&                  getMethods(void) const;                                             //!< Get methods
        RbLanguageObject*                   executeMethod(const std::string& name, const std::vector<Argument>& args);  //!< Override to map member methods to internal functions
        
    };
    
}

#endif