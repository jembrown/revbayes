/**
 * @file
 * This file contains the implementation of Dist_multinomial, which is used to hold
 * parameters and functions related to a multinomial distribution.
 *
 * @brief Implementation of Dist_multinomial
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (Mån, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#include "ArgumentRule.h"
#include "DAGNode.h"
#include "Dist_multinomial.h"
#include "DistributionMultinomial.h"
#include "Move_mmultinomial.h"
#include "RealPos.h"
#include "RandomNumberGenerator.h"
#include "Real.h"
#include "RbException.h"
#include "RbUtil.h"
#include "Simplex.h"
#include "ValueRule.h"
#include "VectorInteger.h"
#include "VectorReal.h"
#include "VectorString.h"
#include "Workspace.h"

#include <cmath>
#include <sstream>
#include <vector>


// Definition of the static type spec member
const TypeSpec Dist_multinomial::typeSpec(Dist_multinomial_name);
const TypeSpec Dist_multinomial::varTypeSpec(VectorNatural_name);

/** Default constructor for parser use */
Dist_multinomial::Dist_multinomial( void ) : DistributionDiscrete( getMemberRules() ), probabilities( NULL ) {
}


/** Clone this object */
Dist_multinomial* Dist_multinomial::clone( void ) const {

    return new Dist_multinomial( *this );
}


/** Get class vector showing type of object */
const VectorString& Dist_multinomial::getClass( void ) const {

    static VectorString rbClass = VectorString(Dist_multinomial_name) + DistributionDiscrete::getClass();
    return rbClass;
}


/** Get member variable rules */
const MemberRules& Dist_multinomial::getMemberRules( void ) const {

    static MemberRules memberRules = MemberRules();
    static bool        rulesSet = false;

    if ( !rulesSet )
		{
        memberRules.push_back( new ValueRule( "probabilities", Simplex_name ) );

        rulesSet = true;
		}

    return memberRules;
}


/** Get the number of states in the distribution */
size_t Dist_multinomial::getNumberOfStates( void ) const {
    
    return static_cast<const Simplex&>( probabilities->getValue() ).size();
}


/** Get the probability mass vector */
const Simplex& Dist_multinomial::getProbabilityMassVector( void ) {
    
    return static_cast<Simplex&>( probabilities->getValue() );
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& Dist_multinomial::getTypeSpec(void) const {
    return typeSpec;
}


/** Get random variable type */
const TypeSpec& Dist_multinomial::getVariableType( void ) const {

    return varTypeSpec;
}


/**
 * This function calculates the natural log of the probability
 * density for a multinomially-distributed random variable.
 *
 * @brief Natural log of multinomial probability density
 *
 * @param value Observed value
 * @return      Natural log of the probability density
 */
double Dist_multinomial::lnPdf( const RbLanguageObject& value ) const {

	// Get the value and the parameters of the Dirichlet
    std::vector<double>       p = static_cast<const Simplex&      >( probabilities->getValue() ).getValue();
    std::vector<unsigned int> x = static_cast<const VectorNatural&>( value ).getValue();

	// Check that the vectors are both the same size
	if ( p.size() != x.size() )
		throw RbException( "Inconsistent size of vectors when calculating Multinomial log probability density" );

	return RbStatistics::Multinomial::lnPdf( p, x );
}


/**
 * This function calculates the probability
 * for a multinomially-distributed random variable.
 *
 * @brief Multinomial probability
 *
 * @param value Observed value
 * @return      Probability density
 */
double Dist_multinomial::pdf( const RbLanguageObject& value ) const {

	// Get the value and the parameters of the Dirichlet
    std::vector<double>       p = static_cast<const Simplex&      >( probabilities->getValue() ).getValue();
    std::vector<unsigned int> x = static_cast<const VectorNatural&>( value ).getValue();

	// check that the vectors are both the same size
	if ( p.size() != x.size() )
		throw RbException( "Inconsistent size of vectors when calculating Multinomial log probability density" );

	return RbStatistics::Multinomial::pdf( p, x );
}


/**
 * This function generates an multinomial-distributed
 * random variable.
 *
 * @brief Random draw from multinomial distribution
 *
 * @return      Random draw from multinomial distribution
 */
const RbLanguageObject& Dist_multinomial::rv( void ) {

    std::vector<double> p      = static_cast<Simplex&>( probabilities->getValue() ).getValue();
    RandomNumberGenerator* rng = GLOBAL_RNG;
	std::vector<int> r( p.size() );

	r = RbStatistics::Multinomial::rv( p, *rng );
    randomVariable.setValue( r );
    
    return randomVariable;
}


/** We catch here the setting of the member variables to store our parameters. */
void Dist_multinomial::setMemberVariable(std::string const &name, Variable *var) {
    
    if ( name == "probabilities" ) {
        probabilities = var;
    }
    else {
        DistributionDiscrete::setMemberVariable(name, var);
    }
}

