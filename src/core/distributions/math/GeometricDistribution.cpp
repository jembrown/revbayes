#include "GeometricDistribution.h"
#include "DistributionGeometric.h"
#include "RandomNumberFactory.h"

using namespace RevBayesCore;

GeometricDistribution::GeometricDistribution(const TypedDagNode<double> *p) : TypedDistribution<int>( new int( 1 ) ), p( p ) {
    // add the parameters to the parents set
    addParameter( p );
    
    *value = RbStatistics::Geometric::rv(p->getValue(), *GLOBAL_RNG);
}


GeometricDistribution::~GeometricDistribution( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}


GeometricDistribution* GeometricDistribution::clone( void ) const {
    return new GeometricDistribution( *this );
}


double GeometricDistribution::computeLnProbability( void ) {
    return RbStatistics::Geometric::lnPdf(p->getValue(), *value);
}


void GeometricDistribution::redrawValue( void ) {
    *value = RbStatistics::Geometric::rv(p->getValue(), *GLOBAL_RNG);
}


void GeometricDistribution::swapParameter(const DagNode *oldP, const DagNode *newP) {
    if (oldP == p) {
        p = static_cast<const TypedDagNode<double>* >( newP );
    }
}
