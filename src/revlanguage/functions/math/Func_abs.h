/**
 * @file
 * This file contains the declaration of the RevLanguage exponential function, which
 * is used to created deterministic variable associated with the exponential function.
 *
 * @brief Declaration and implementation of Func_exp
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-04-20 04:06:14 +0200 (Fri, 20 Apr 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id: Func_add.h 1406 2012-04-20 02:06:14Z hoehna $
 */


#ifndef Func_abs_H
#define Func_abs_H

#include "RlFunction.h"

#include <string>

namespace RevLanguage {
    
    class Func_abs :  public Function {
        
    public:
        Func_abs( void );
        
        // Basic utility functions
        Func_abs*                                       clone(void) const;                                                              //!< Clone the object
        static const std::string&                       getClassName(void);                                                             //!< Get class name
        static const TypeSpec&                          getClassTypeSpec(void);                                                         //!< Get class type spec
        const TypeSpec&                                 getTypeSpec(void) const;                                                        //!< Get the type spec of the instance
        
        // Function functions you have to override
        RbLanguageObject*                               execute(void);                                                                  //!< Execute function
        const ArgumentRules&                            getArgumentRules(void) const;                                                   //!< Get argument rules
        const TypeSpec&                                 getReturnType(void) const;                                                      //!< Get type of return value
        
    };
    
}

#endif