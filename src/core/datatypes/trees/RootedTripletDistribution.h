/**
 * @file
 * This file contains the declaration of a rooted triplet distribution,
 * that holds a vector of triplets along with their frequencies.
 *
 *
 * @brief Declaration of the RootedTripletDistribution
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-07-05 16:47:08 +0200 (Thu, 05 Jul 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2011-04-13, version 1.0
 *
 * $Id: RootedTripletDistribution.h 1651 2014-08-31 14:47:08Z Boussau $
 */

#ifndef RootedTripletDistribution_H
#define RootedTripletDistribution_H

#include "Cloneable.h"
#include "MemberObject.h"
#include "Taxon.h"
#include "TopologyNode.h"
#include "BranchLengthTree.h"
#include "TimeTree.h"
#include "Tree.h"
#include "TypedDagNode.h"


#include <map>
#include <vector>
#include <set>
#include <string>


namespace RevBayesCore {
    
    class RootedTripletDistribution : public Cloneable, public MemberObject<std::vector < std::string > > {
        
    public:
    
        RootedTripletDistribution(void);                                                                           //!< Default constructor
        RootedTripletDistribution(const RootedTripletDistribution& t);                                             //!< Copy constructor
        virtual                                    ~RootedTripletDistribution(void);                                                                                                        //!< Destructor
        RootedTripletDistribution( const std::vector<TimeTree>& ts, const std::vector< std::string > spNames ) ;
        RootedTripletDistribution( const std::vector<BranchLengthTree>& ts, const std::vector< std::string > spNames ) ;

        RootedTripletDistribution( const std::vector< std::string > spNames ) ;

        RootedTripletDistribution&                                   operator=(const RootedTripletDistribution& t);
        
        // Basic utility functions
        RootedTripletDistribution*                                   clone(void) const;                                                      //!< Clone object
        
        void executeMethod(const std::string &n, const std::vector<const DagNode *> &args, std::vector < std::string > &rv) const ;                                 //!< Execute the member-method
        
        // RootedTripletDistribution functions
        void extractTriplets( const Tree& t );                           //!< Getting the triplets from a tree and adding them to the tripletDistribution
        void extractTriplets(  );             //!< Getting the triplets from all trees and adding them to the tripletDistribution
        void populateTripletDistribution ( const TopologyNode* node, std::vector< size_t >& allTips ) ;
        void addAllTriplets(std::vector< size_t >& leftTips, std::vector< size_t >& rightTips) ; //!< Get all rooted triplets given vectors of left and right tips
        void addAllTripletsOneWay( std::vector< size_t >& leftTips, std::vector< size_t >& rightTips, size_t leftSize,size_t rightSize ); //!< Get rooted triplets given vectors of left and right tips, one way only

        size_t getNumberOfTrees() const;                                                                                     //!< Get the number of trees that were used to build the object
        size_t getNumberOfTriplets() const;                                                       //!< Get the number of triplets in the object
        
        void setSpecies ( std::vector< std::string > s );
        void setTaxa ( std::vector< Taxon > t );
       // void setTrees( TypedDagNode< std::vector< Tree > >* ts ) ;
        void setTrees ( const std::vector< TimeTree >& ts );
        void setTrees ( const std::vector< BranchLengthTree >& ts );
        std::vector< std::string > getSpecies ( ) const;
        std::vector< Taxon > getTaxa ( ) const;
        std::string getSpecies ( size_t i ) const;
        Taxon getTaxon ( size_t i ) const;
        void resetDistribution (  ) ;
        const std::string getStringRepresentation() const ;
        
 //   protected:
        // Nothing
    private:

        
        //Members:
        std::vector< TimeTree >                                                      ttrees;
        std::vector< BranchLengthTree >                                             bltrees;
        std::vector< Taxon >                                                           taxa;
        std::vector< std::string >                                                  species;
        std::map < std::pair < size_t, std::pair < size_t, size_t > >, size_t >                         tripletDistribution;
        size_t                                                                numberOfTrees;
        std::set< Taxon >                                                           taxaSet;
        std::set< std::string >                                                  speciesSet;
        std::map< Taxon, size_t >                                              taxonToIndex;
        std::map< std::string, size_t >                                      speciesToIndex;
        bool                                                                    speciesOnly;
        
        
    };
    
    // Global functions using the class
    std::ostream& operator<<(std::ostream& o, const RootedTripletDistribution& x);       //!< Overloaded output operator
    
}


#endif
