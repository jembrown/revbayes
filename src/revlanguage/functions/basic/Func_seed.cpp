//
//  Func_seed.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 9/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Func_seed.h"
#include "ArgumentRule.h"
#include "DeterministicNode.h"
#include "Ellipsis.h"
#include "Natural.h"
#include "RandomNumberGenerator.h"
#include "RandomNumberFactory.h"
#include "RbNullObject.h"
#include "RbUtil.h"
#include "RealPos.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"

using namespace RevLanguage;

Func_seed::Func_seed() : Function() {
    
}

/* Clone object */
Func_seed* Func_seed::clone( void ) const {
    
    return new Func_seed( *this );
}


/** Execute function: We rely on getValue and overloaded push_back to provide functionality */
RbLanguageObject* Func_seed::execute( void ) {
    
    std::vector<unsigned int> s;
    const Natural &val1 = static_cast<const Natural &>( args[0].getVariable()->getValue() );
    s.push_back( val1.getValue() );
    const Natural &val2 = static_cast<const Natural &>( args[1].getVariable()->getValue() );
    s.push_back( val2.getValue() );
    
    RevBayesCore::RandomNumberGenerator *rng = RevBayesCore::GLOBAL_RNG;
    rng->setSeed( s );
        
    return NULL;
}


/** Get argument rules */
const ArgumentRules& Func_seed::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) {
        
        argumentRules.push_back( new ArgumentRule( "seed1", true, Natural::getClassTypeSpec() ) );
        argumentRules.push_back( new ArgumentRule( "seed2", true, Natural::getClassTypeSpec() ) );
        rulesSet = true;
    }
    
    return argumentRules;
}


/** Get class name of object */
const std::string& Func_seed::getClassName(void) { 
    
    static std::string rbClassName = "Func_seed";
    
	return rbClassName; 
}


/** Get class type spec describing type of object */
const RevLanguage::TypeSpec& Func_seed::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( "Func_seed", new TypeSpec( Function::getClassTypeSpec() ) );
    
	return rbClass; 
}


/** Get type spec */
const TypeSpec& Func_seed::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get return type */
const TypeSpec& Func_seed::getReturnType( void ) const {
    
    return RbNullObject::getClassTypeSpec();
}