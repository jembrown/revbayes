#include "Clade.h"
#include "Func_tmrca.h"
#include "ModelVector.h"
#include "RlBoolean.h"
#include "RlClade.h"
#include "RlTimeTree.h"
#include "RealPos.h"
#include "RlDeterministicNode.h"
#include "TmrcaStatistic.h"
#include "TypedDagNode.h"

using namespace RevLanguage;

/** default constructor */
Func_tmrca::Func_tmrca( void ) : TypedFunction<RealPos>( ) {
    
}


/** Clone object */
Func_tmrca* Func_tmrca::clone( void ) const {
    
    return new Func_tmrca( *this );
}


RevBayesCore::TypedFunction<double>* Func_tmrca::createFunction( void ) const
{
    
    RevBayesCore::TypedDagNode<RevBayesCore::Tree>* tau = static_cast<const TimeTree&>( this->args[0].getVariable()->getRevObject() ).getDagNode();
    const RevBayesCore::Clade& c = static_cast<const Clade &>( this->args[1].getVariable()->getRevObject() ).getValue();
    bool stemAge = static_cast<const RlBoolean &>( args[2].getVariable()->getRevObject() ).getValue();
    RevBayesCore::TmrcaStatistic* f = new RevBayesCore::TmrcaStatistic( tau, c, stemAge );
    
    return f;
}


/* Get argument rules */
const ArgumentRules& Func_tmrca::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet )
    {
        
        argumentRules.push_back( new ArgumentRule( "tree" , TimeTree::getClassTypeSpec(), "The tree variable.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "clade", Clade::getClassTypeSpec()   , "The clade.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "stemAge", RlBoolean::getClassTypeSpec(), "Do we want the stem age or crown age?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ) );
        rulesSet = true;
    }
    
    return argumentRules;
}


const std::string& Func_tmrca::getClassType(void)
{
    
    static std::string revType = "Func_tmrca";
    
	return revType; 
}


/* Get class type spec describing type of object */
const TypeSpec& Func_tmrca::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/**
 * Get the primary Rev name for this function.
 */
std::string Func_tmrca::getFunctionName( void ) const
{
    // create a name variable that is the same for all instance of this class
    std::string f_name = "tmrca";
    
    return f_name;
}


const TypeSpec& Func_tmrca::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}
