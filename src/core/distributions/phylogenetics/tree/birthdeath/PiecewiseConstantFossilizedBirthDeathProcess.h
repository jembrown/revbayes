#ifndef PiecewiseConstantFossilizedBirthDeathProcess_H
#define PiecewiseConstantFossilizedBirthDeathProcess_H

#include "AbstractBirthDeathProcess.h"
#include "RbVector.h"

#include <vector>
#include <set>

namespace RevBayesCore {
    
    class Clade;
    class Taxon;
    
    /**
     * @brief Piecewise-constant fossilized birth-death process with serially sampled fossils.
     *
     * The piecewise-constant birth-death process has constant rates for each time interval.
     * At the end of each time interval there may be an abrupt rate-shift (jump) for each
     * of the rates. Additionally, there may be sampling at the end of each interval.
     * Finally, fossils are sampled with rate psi, the others (fossils and extant taxa) are
     * sampled at sampling times (including the present).
     *
     * We assume that the rate vectors have one more element than the rate-change vectors.
     * Thus, one rate-change means always two interval, two rate-changes three interval, and so on.
     *
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2014-03-18, version 1.0
     *
     */
    class PiecewiseConstantFossilizedBirthDeathProcess : public AbstractBirthDeathProcess {
        
    public:
        PiecewiseConstantFossilizedBirthDeathProcess (const TypedDagNode<double> *ra,
                                                      const DagNode *s,
                                                      const DagNode *e,
                                                      const DagNode *p,
                                                      const TypedDagNode<double >* r, const TypedDagNode<RbVector<double> > *t,
                                                      bool uo, const std::string &cdt, const std::vector<Taxon> &tn);  //!< Constructor
        
        // public member functions
        PiecewiseConstantFossilizedBirthDeathProcess*   clone(void) const;                                         //!< Create an independent clone
        virtual double                                  getRootAge(void) const;
        virtual void                                    setValue(Tree *v, bool f=false);

        double                                          getExtinctionRate( size_t index = 0 ) const;
        double                                          getFossilizationRate( size_t index = 0 ) const;
        double                                          getSpeciationRate( size_t index = 0 ) const;

    protected:
        // Parameter management functions
        double                                          computeLnProbabilityDivergenceTimes(void) const;                            //!< Compute the log-transformed probability of the current value.
        // Parameter management functions
        void                                            swapParameterInternal(const DagNode *oldP, const DagNode *newP);                //!< Swap a parameter

        // virtual methods that may be overwritten, but then the derived class should call this methods
        virtual void                                    restoreSpecialization(DagNode *restorer);
        virtual void                                    touchSpecialization(DagNode *toucher, bool touchAll);
        
    private:
        
        // helper functions
        double                                          computeLnProbabilityTimes(void) const;                     //!< Compute the log-transformed probability of the current value.
        size_t                                          l(double t) const;                                         //!< Find the index so that times[index-1] < t < times[index]
        double                                          lnProbNumTaxa(size_t n, double start, double end, bool MRCA) const { throw RbException("Cannot compute P(nTaxa)."); }
        double                                          pSurvival(double start, double end) const;                 //!< Compute the probability of survival of the process (without incomplete taxon sampling).
        double                                          p(size_t i, double t) const;
        void                                            prepareProbComputation(void) const;
        double                                          q(size_t i, double t) const;
        double                                          simulateDivergenceTime(double origin, double present) const;    //!< Simulate a speciation event.
        int                                             survivors(double t) const;                                 //!< Number of species alive at time t.
        
        // members
        const TypedDagNode<double >*                    homogeneous_lambda;                                    //!< The homogeneous speciation rates.
        const TypedDagNode<RbVector<double> >*          heterogeneous_lambda;                                  //!< The heterogeneous speciation rates.
        const TypedDagNode<double >*                    homogeneous_mu;                                        //!< The homogeneous speciation rates.
        const TypedDagNode<RbVector<double> >*          heterogeneous_mu;                                      //!< The heterogeneous speciation rates.
        const TypedDagNode<double >*                    homogeneous_psi;                                       //!< The homogeneous speciation rates.
        const TypedDagNode<RbVector<double> >*          heterogeneous_psi;                                     //!< The heterogeneous speciation rates.
        const TypedDagNode<double >*                    homogeneous_rho;                                       //!< The homogeneous speciation rates.
        const TypedDagNode<RbVector<double> >*          sampling_times;                                        //!< The times of the instantaneous sampling events.
        
        bool                                            useOrigin;

        mutable std::vector<double>                     times;
        mutable std::vector<double>                     birth;
        mutable std::vector<double>                     death;
        mutable std::vector<double>                     fossil;
    };
}

#endif
