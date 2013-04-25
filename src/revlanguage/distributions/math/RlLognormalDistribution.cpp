//
//  NormalDistribution.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 8/6/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//


#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "LognormalDistribution.h"
#include "RlLognormalDistribution.h"
#include "Real.h"
#include "RealPos.h"

using namespace RevLanguage;

LognormalDistribution::LognormalDistribution() : PositiveContinuousDistribution() {
    
}


LognormalDistribution::~LognormalDistribution() {
    
}



LognormalDistribution* LognormalDistribution::clone( void ) const {
    return new LognormalDistribution(*this);
}


RevBayesCore::LognormalDistribution* LognormalDistribution::createDistribution( void ) const {
    // get the parameters
    RevBayesCore::TypedDagNode<double>* m   = static_cast<const Real &>( mean->getValue() ).getValueNode();
    RevBayesCore::TypedDagNode<double>* s   = static_cast<const RealPos &>( sd->getValue() ).getValueNode();
    RevBayesCore::LognormalDistribution* d  = new RevBayesCore::LognormalDistribution(m, s);
    
    return d;
}



/* Get class name of object */
const std::string& LognormalDistribution::getClassName(void) { 
    
    static std::string rbClassName = "Lognormal distribution";
    
	return rbClassName; 
}

/* Get class type spec describing type of object */
const TypeSpec& LognormalDistribution::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( Distribution::getClassTypeSpec() ) );
    
	return rbClass; 
}




/** Return member rules (no members) */
const MemberRules& LognormalDistribution::getMemberRules(void) const {
    
    static MemberRules distUnifMemberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet ) {
        distUnifMemberRules.push_back( new ArgumentRule( "mean", true, Real::getClassTypeSpec(), new Real(0.0) ) );
        distUnifMemberRules.push_back( new ArgumentRule( "sd"  , true, RealPos::getClassTypeSpec(), new RealPos(1.0) ) );
        
        rulesSet = true;
    }
    
    return distUnifMemberRules;
}


const TypeSpec& LognormalDistribution::getTypeSpec( void ) const {
    
    static TypeSpec ts = getClassTypeSpec();
    
    return ts;
}


/** Print value for user */
void LognormalDistribution::printValue(std::ostream& o) const {
    
    o << "lognormal(mean=";
    if ( mean != NULL ) {
        o << mean->getName();
    } else {
        o << "?";
    }
    o << ", sd=";
    if ( sd != NULL ) {
        o << sd->getName();
    } else {
        o << "?";
    }
    o << ")";
}


/** Set a member variable */
void LognormalDistribution::setConstMemberVariable(const std::string& name, const RbPtr<const Variable> &var) {
    
    if ( name == "mean" ) {
        mean = var;
    }
    else if ( name == "sd" ) {
        sd = var;
    }
    else {
        Distribution::setConstMemberVariable(name, var);
    }
}