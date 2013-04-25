/**
 * @file
 * This file contains the implementation of SimpleMemberFunction, which is used
 * to map member function calls (member method calls) of complex objects
 * to internal functions instead of providing regular Function objects
 * implementing the member functions.
 *
 * @brief Implementation of SimpleMemberFunction
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-05-15 18:59:11 +0200 (Tue, 15 May 2012) $
 * @author The RevBayes core team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-17, version 1.0
 *
 * $Id: SimpleMemberFunction.cpp 1544 2012-05-15 16:59:11Z hoehna $
 */

#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "Ellipsis.h"
#include "UserFunction.h"
#include "RbException.h"
#include "RbUtil.h"
#include "Signals.h"
#include "TypeSpec.h"

#include <sstream>

using namespace RevLanguage;

/** Constructor */
UserFunction::UserFunction( const ArgumentRules*  argRules,
                           const TypeSpec&             retType,
                           std::list<SyntaxElement*>*  stmts,
                           Environment*                defineEnv)
: Function(), argumentRules(argRules), returnType(retType), code(stmts), defineEnvironment(defineEnv) {
    
}


/** Copy constructor */
UserFunction::UserFunction(const UserFunction &x) : Function(x), argumentRules( new ArgumentRules(*x.argumentRules) ), returnType( x.returnType ), code( NULL ), defineEnvironment( NULL ) {
    
    // clone the environment
    defineEnvironment   = x.defineEnvironment->clone();
    
    // create a new list for the code
    code = new std::list<SyntaxElement*>();
    for (std::list<SyntaxElement*>::const_iterator i=x.code->begin(); i!=x.code->end(); i++) {
        SyntaxElement* element = (*i)->clone();
        code->push_back(element);
    }
}


UserFunction& UserFunction::operator=(const UserFunction &f) {
    
    if ( this != &f ) {
        // call the base class assignment operator
        Function::operator=(f);
        
        delete defineEnvironment;
        // clone the environment
        defineEnvironment   = f.defineEnvironment->clone();
        
        
        delete argumentRules;
        argumentRules = new ArgumentRules(*f.argumentRules);
        
        for (std::list<SyntaxElement*>::iterator it = code->begin(); it != code->end(); it++) {
            SyntaxElement* theSyntaxElement = *it;
            delete theSyntaxElement;
        }
        code->clear();
        
        // create a new list for the code
        for (std::list<SyntaxElement*>::const_iterator i=f.code->begin(); i!=f.code->end(); i++) {
            SyntaxElement* element = (*i)->clone();
            code->push_back(element);
        }
    }
    
    return *this;
}


/** Destructor */
UserFunction::~UserFunction() {
    
    // defineEnvironment->destroyEnclosure();   // \TODO: or something like that
    
    delete defineEnvironment;
    delete argumentRules;
    
    for (std::list<SyntaxElement*>::iterator it = code->begin(); it != code->end(); it++) {
        SyntaxElement* theSyntaxElement = *it;
        delete theSyntaxElement;
    }
    
    delete code;
    
}



/** Clone the object */
UserFunction* UserFunction::clone(void) const {
    
    return new UserFunction(*this);
}


/** Execute function: call the object's internal implementation through executeOperation */
RbLanguageObject* UserFunction::execute( void ) {
    
    // Clear signals
    Signals::getSignals().clearFlags();
    
    // Set initial return value
    RbPtr<Variable> retVar = NULL;
    
    // Create new variable frame
    Environment functionEnvironment = Environment( defineEnvironment );
    // \TODO: Check that the arguments can still be accessed
    //    Environment functionEnvironment = Environment( args );
    for (std::vector<Argument>::iterator it = args.begin(); it != args.end(); ++it) {
        RbPtr<Variable> theVar = it->getVariable()->clone();
        functionEnvironment.addVariable( it->getLabel(), theVar );
    }
    
    // Execute code
    for ( std::list<SyntaxElement*>::iterator i=code->begin(); i!=code->end(); i++ ) {
        
        SyntaxElement* theSyntaxElement = *i;
        retVar = theSyntaxElement->evaluateContent( functionEnvironment );
        
        if ( Signals::getSignals().isSet( Signals::RETURN ) )
            break;
    }
    
    RbLanguageObject* retValue = retVar->getValue().clone();
        
    // Return the return value
    return retValue;
    
}


/** Get class name of object */
const std::string& UserFunction::getClassName(void) { 
    
    static std::string rbClassName = "User function";
    
	return rbClassName; 
}

/** Get class type spec describing type of object */
const TypeSpec& UserFunction::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return rbClass; 
}

/** Get type spec */
const TypeSpec& UserFunction::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get argument rules */
const ArgumentRules& UserFunction::getArgumentRules(void) const {
    
    return *argumentRules;
}


/** Get return type */
const TypeSpec& UserFunction::getReturnType(void) const {
    
    return returnType;
}


/** We catch here the setting of the argument variables to store our parameters. */
void UserFunction::setArgumentVariable(std::string const &name, const RbPtr<const Variable> &var) {
    
    // We actually just catch the call to the base class which would complain that the argument was not expected.
    // User functions handle their arguments in some unknown different way.
    
    if ( defineEnvironment->existsVariable( name ) ) {
        (*defineEnvironment)[name].setVariable( var->clone() );
    }
    else {
        defineEnvironment->addVariable(name, var->clone() );
    }
}
