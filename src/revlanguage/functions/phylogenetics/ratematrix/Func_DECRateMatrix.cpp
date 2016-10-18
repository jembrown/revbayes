//
//  Func_DECRateMatrix.cpp
//  revbayes-proj
//
//  Created by Michael Landis on 3/16/15.
//  Copyright (c) 2015 Michael Landis. All rights reserved.
//


#include "ConstantNode.h"
#include "DECRateMatrixFunction.h"
#include "Func_DECRateMatrix.h"
#include "Integer.h"
#include "Natural.h"
#include "OptionRule.h"
#include "RbConstants.h"
#include "Real.h"
#include "RealPos.h"
#include "RlBoolean.h"
#include "RlDeterministicNode.h"
#include "RlRateMatrix.h"
#include "RlSimplex.h"
#include "TypedDagNode.h"

using namespace RevLanguage;

/** default constructor */
Func_DECRateMatrix::Func_DECRateMatrix( void ) : TypedFunction<RateMatrix>( )
{
    
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Func_DECRateMatrix* Func_DECRateMatrix::clone( void ) const
{
    
    return new Func_DECRateMatrix( *this );
}


RevBayesCore::TypedFunction< RevBayesCore::RateGenerator >* Func_DECRateMatrix::createFunction( void ) const
{
    
    // dispersal rates
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<RevBayesCore::RbVector<double> > >* dr;
    dr = static_cast<const ModelVector<ModelVector<RealPos> > &>( this->args[0].getVariable()->getRevObject() ).getDagNode();
    
    // extirpation rates
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<double> >* er;
    er = static_cast<const ModelVector<RealPos> &>( this->args[1].getVariable()->getRevObject() ).getDagNode();
    
    size_t num_statesEr = er->getValue().size();
    std::vector<size_t> num_statesDr;
    for (size_t i = 0; i < dr->getValue().size(); i++)
    {
        num_statesDr.push_back( dr->getValue()[i].size() );
        if (num_statesDr[i] != num_statesEr)
        {
            throw RbException("The dimension between dispersal and extirpation rates does not match.");
        }
        for (size_t j = 0; j < i; j++)
        {
            if (num_statesDr[i] != num_statesDr[j])
            {
                throw RbException("The dispersal matrix is not square.");
            }
        }
    }
    if (dr->getValue().size() != num_statesEr)
    {
        throw RbException("The dimension between dispersal and extirpation rates does not match.");
    }

    // range size probabilities
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<double> >* rs = NULL;
    if ( this->args[2].getVariable() != NULL && this->args[2].getVariable()->getRevObject() != RevNullObject::getInstance()) {
    
        rs = static_cast<const Simplex&>( this->args[2].getVariable()->getRevObject() ).getDagNode();
        if (rs->getValue().size() != num_statesEr && rs->getValue().size() > 0) {
            throw RbException("The probabilities of range sizes must equal the number of areas.");
        }
        else {
            size_t n = num_statesEr;
            double p = 1.0 / n;
            rs = new RevBayesCore::ConstantNode<RevBayesCore::RbVector<double> >("", new RevBayesCore::RbVector<double>(n,p));
        }
    }
    else {
        size_t n = num_statesEr+1;
        double p = 1.0 / n;
        rs = new RevBayesCore::ConstantNode<RevBayesCore::RbVector<double> >("", new RevBayesCore::RbVector<double>(n,p));
    }
    
    std::string nullRangeStr = static_cast<const RlString &>( this->args[3].getVariable()->getRevObject() ).getValue();
    bool cs = nullRangeStr=="CondSurv";
    bool ex = nullRangeStr=="Exclude";
//    std::cout << nullRangeStr << " " << cs << " " << ex << "\n";
    
    bool os = static_cast<const RlBoolean&>(this->args[4].getVariable()->getRevObject() ).getValue();
    size_t mrs = static_cast<const Natural&>(this->args[5].getVariable()->getRevObject() ).getValue();
    
    if (mrs < 1 || mrs > er->getValue().size())
    {
        mrs = er->getValue().size();
    }
    bool uc = false;
    RevBayesCore::DECRateMatrixFunction* f = new RevBayesCore::DECRateMatrixFunction( dr, er, rs, cs, ex, os, uc, mrs );
    
    return f;
}


/* Get argument rules */
const ArgumentRules& Func_DECRateMatrix::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rules_set = false;
    
    if ( !rules_set )
    {
        
        argumentRules.push_back( new ArgumentRule( "dispersalRates"  , ModelVector<ModelVector<RealPos> >::getClassTypeSpec(), "Matrix of dispersal rates between areas.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "extirpationRates", ModelVector<RealPos>::getClassTypeSpec(), "The per area extinction rates.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        argumentRules.push_back( new ArgumentRule( "rangeSize",        Simplex::getClassTypeSpec(), "Relative proportions of range sizes.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY, new Simplex( RevBayesCore::RbVector<double>() ) ) );
        
        std::vector<std::string> options;
        options.push_back( "CondSurv" );
        options.push_back( "Exclude" );
        options.push_back( "Include" );
        argumentRules.push_back( new OptionRule( "nullRange", new RlString("CondSurv"), options, "How should DEC handle the null range?" ) );
        
        argumentRules.push_back( new ArgumentRule( "orderStatesBySize", RlBoolean::getClassTypeSpec(), "Order states by size?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean(false) ));
        
        argumentRules.push_back( new ArgumentRule( "maxRangeSize", Natural::getClassTypeSpec(), "Maximum range size.", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new Natural(0) ));
        

        rules_set = true;
    }
    
    return argumentRules;
}


const std::string& Func_DECRateMatrix::getClassType(void)
{
    
    static std::string revType = "Func_DECRateMatrix";
    
	return revType;
}

/* Get class type spec describing type of object */
const TypeSpec& Func_DECRateMatrix::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return rev_type_spec;
}


/**
 * Get the primary Rev name for this function.
 */
std::string Func_DECRateMatrix::getFunctionName( void ) const
{
    // create a name variable that is the same for all instance of this class
    std::string f_name = "fnDECRateMatrix";
    
    return f_name;
}


const TypeSpec& Func_DECRateMatrix::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}