#ifndef Move_SpeciesNarrowExchange_H
#define Move_SpeciesNarrowExchange_H

#include "RlMove.h"
#include "TypedDagNode.h"

#include <ostream>
#include <string>

namespace RevLanguage {


    /**
     * The RevLanguage wrapper of the species-node-time-slide (uniform) move.
     *
     * The RevLanguage wrapper of the narrow-exchange move simply
     * manages the interactions through the Rev with our core.
     * That is, the internal move object can be constructed and hooked up
     * in a DAG-nove (variable) that it works on.
     * See the SpeciesNarrowExchangeProposal.h for more details.
     *
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2015-06-23, version 1.0
     *
     */
    class Move_SpeciesNarrowExchange : public Move {

    public:

        Move_SpeciesNarrowExchange(void);                                                                                                       //!< Default constructor

        // Basic utility functions
        virtual Move_SpeciesNarrowExchange*         clone(void) const;                                                                          //!< Clone object
        void                                        constructInternalObject(void);                                                              //!< We construct the a new internal SlidingMove.
        static const std::string&                   getClassType(void);                                                                         //!< Get Rev type
        static const TypeSpec&                      getClassTypeSpec(void);                                                                     //!< Get class type spec
        std::string                                 getMoveName(void) const;                                                                    //!< Get the name used for the constructor function in Rev.
        const MemberRules&                          getParameterRules(void) const;                                                              //!< Get member rules (const)
        virtual const TypeSpec&                     getTypeSpec(void) const;                                                                    //!< Get language type of the object
        virtual void                                printValue(std::ostream& o) const;                                                          //!< Print value (for user)

        // Member method functions
        virtual RevPtr<RevVariable>                 executeMethod(const std::string& name, const std::vector<Argument>& args, bool &f);         //!< Map member methods to internal functions

    protected:


        std::vector<std::string>                            getHelpAuthor(void) const;                                                              //!< Get the author(s) of this function
        std::vector<std::string>                            getHelpDescription(void) const;                                                         //!< Get the description for this function
        std::vector<std::string>                            getHelpDetails(void) const;                                                             //!< Get the more detailed description of the function
        std::string                                         getHelpExample(void) const;                                                             //!< Get an executable and instructive example
        std::vector<RevBayesCore::RbHelpReference>          getHelpReferences(void) const;                                                          //!< Get some references/citations for this function
        std::vector<std::string>                            getHelpSeeAlso(void) const;                                                             //!< Get suggested other functions
        std::string                                         getHelpTitle(void) const;                                                               //!< Get the title of this help entry

        void                                        setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var);           //!< Set member variable

        RevPtr<const RevVariable>                   speciesTree;

    };

}

#endif
