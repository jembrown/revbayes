#ifndef RlMultispeciesCoalescentConstantPopulationProcess_H
#define RlMultispeciesCoalescentConstantPopulationProcess_H

#include "MultispeciesCoalescent.h"
#include "RlTimeTree.h"
#include "RlTypedDistribution.h"

namespace RevLanguage {
    
    /**
     * The RevLanguage wrapper of the MultispeciesCoalescent process
     *
     * The RevLanguage wrapper of the Multispecies Coalescent with constant population process connects
     * the variables/parameters of the process and creates the internal MultispeciesCoalescent object.
     * Please read the MultispeciesCoalescent.h for more info.
     *
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2014-01-26, version 1.0
     *
     */
    class MultispeciesCoalescentConstantPopulationProcess :  public  TypedDistribution<TimeTree> {
        
    public:
        MultispeciesCoalescentConstantPopulationProcess( void );
        
        // Basic utility functions
        MultispeciesCoalescentConstantPopulationProcess*                  clone(void) const;                                                                      //!< Clone the object
        static const std::string&                       getClassName(void);                                                                     //!< Get class name
        static const TypeSpec&                          getClassTypeSpec(void);                                                                 //!< Get class type spec
        const TypeSpec&                                 getTypeSpec(void) const;                                                                //!< Get the type spec of the instance
        const MemberRules&                              getMemberRules(void) const;                                                             //!< Get member rules (const)
        
        
        // Distribution functions you have to override
        RevBayesCore::MultispeciesCoalescent*    createDistribution(void) const;
        
    protected:
        
        void                                            setConstMemberVariable(const std::string& name, const RbPtr<const Variable> &var);      //!< Set member variable
        
        
    private:

        RbPtr<const Variable>                           Ne;                                                                                 //!< The population size
        RbPtr<const Variable>                           speciesTree;                                                          //!< The species tree
        RbPtr<const Variable>                           taxons;                                                              //!< The taxons
        
        
    };
    
}

#endif