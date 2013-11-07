//
//  PhylowoodNhxMonitor.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 10/16/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#include "PhylowoodNhxMonitor.h"
#include "DagNode.h"
#include "Model.h"
#include "Monitor.h"
#include "RbException.h"

#include <sstream>

using namespace RevBayesCore;

/* Constructor */
PhylowoodNhxMonitor::PhylowoodNhxMonitor(TypedDagNode<TimeTree>* t,  std::vector<StochasticNode<BranchHistory>* > bh, std::vector<std::vector<double> > gc, int g, int mg, int b, const std::string &fname, const std::string &del, bool pp, bool l, bool pr, bool ap, bool sm, bool sr) : Monitor(g,t), outStream(), tree( t ), branchHistories(bh), filename( fname ), separator( del ), posterior( pp ), likelihood( l ), prior( pr ), append(ap), showMetadata(sm), showRates(sr), numSamples(0), geographicCoordinates(gc), maxGen(mg), burn(b) {
    
    std::cout << g << " " << mg << "\n";
    
    nodes.push_back(t);
    for (size_t i = 0; i < branchHistories.size(); i++)
        nodes.push_back(branchHistories[i]);
    
    numHistories = bh.size();
    numCharacters = bh[0]->getValue().getNumCharacters();
    parentCharacterCounts.resize(numHistories);
    childCharacterCounts.resize(numHistories);
    for (size_t i = 0; i < numHistories; i++)
    {
        parentCharacterCounts[i].resize(numCharacters,0);
        childCharacterCounts[i].resize(numCharacters,0);
    }
}

PhylowoodNhxMonitor::PhylowoodNhxMonitor(const PhylowoodNhxMonitor &m) : Monitor( m ), outStream( ), tree( m.tree ), branchHistories( m.branchHistories), nodeVariables( m.nodeVariables ), showMetadata(m.showMetadata), showRates(m.showRates), numSamples(m.numSamples), parentCharacterCounts(m.parentCharacterCounts), childCharacterCounts(m.childCharacterCounts), geographicCoordinates(m.geographicCoordinates), maxGen(m.maxGen), numCharacters(m.numCharacters), numHistories(m.numHistories), burn(m.burn) {
    
    filename    = m.filename;
    separator   = m.separator;
    prior       = m.prior;
    posterior   = m.posterior;
    likelihood  = m.likelihood;
    append      = m.append;
}


/* Clone the object */
PhylowoodNhxMonitor* PhylowoodNhxMonitor::clone(void) const {
    
    return new PhylowoodNhxMonitor(*this);
}


void PhylowoodNhxMonitor::closeStream() {
    outStream.close();
}


std::string PhylowoodNhxMonitor::buildExtendedNewick( void ) {
    //tree->getValue().getRoot().setNewickNeedsRefreshing(true);
    numSamples++;
    std::string newick = buildExtendedNewick( &tree->getValue().getRoot() );
    return newick;
}

void PhylowoodNhxMonitor::updateCharacterCounts(TopologyNode* n, std::string brEnd)
{
    BranchHistory bh = branchHistories[n->getIndex()]->getValue();
    std::vector<CharacterEvent*> characters;
    if (brEnd=="child")
    {
        characters = bh.getChildCharacters();
        for (size_t i = 0; i < characters.size(); i++)
            childCharacterCounts[n->getIndex()][i] += characters[i]->getState();
    }
    else if (brEnd=="parent")
    {
        characters = bh.getParentCharacters();
        for (size_t i = 0; i < characters.size(); i++)
            parentCharacterCounts[n->getIndex()][i] += characters[i]->getState();
    }
    
}

std::string PhylowoodNhxMonitor::buildCharacterHistoryString(TopologyNode* n, std::string brEnd)
{
    size_t nd_idx = n->getIndex();
    BranchHistory bh = branchHistories[nd_idx]->getValue();
    std::vector<CharacterEvent*> characters;
    std::stringstream ss;
    
    if (brEnd=="child")
    {
        characters = bh.getChildCharacters();
        for (size_t i = 0; i < characters.size(); i++)
        {
            if (i != 0)
                ss << ",";
            ss << (double)childCharacterCounts[nd_idx][i] / numSamples;
        }
    }
    else if (brEnd=="parent")
    {
        characters = bh.getParentCharacters();
        for (size_t i = 0; i < characters.size(); i++)
        {
            if (i != 0)
                ss << ",";
            ss << (double)parentCharacterCounts[nd_idx][i] / numSamples;
        }
    }

    return ss.str();
}

/* Build newick string */
std::string PhylowoodNhxMonitor::buildExtendedNewick( TopologyNode* n ) {
    // create the newick string
    std::stringstream o;
    
    // extended data is only found on admixture nodes
    std::string additionalInfo = "";
    
    // loop over admixture nodes per branch
    std::stringstream characterStream;
    
    double br = 1.0;
    
    if (showMetadata)
    {
        updateCharacterCounts(n,"child");
        
        characterStream << "[";
        
        // character history
        characterStream << "&ch={" << buildCharacterHistoryString(n,"child") << "}";
        
        // ... whatever else
        characterStream << "]";
        
        additionalInfo = characterStream.str();
    }
    
    // test whether this is a internal or external node
    if (n->isTip()) {
        // this is a tip so we just return the name of the node
        o << n->getIndex() << additionalInfo << ":" << n->getBranchLength();
    }
    
    else {
        o << "(";
        for (size_t i=0; i<(n->getNumberOfChildren()-1); i++) {
            o << buildExtendedNewick( &n->getChild(i) ) << ",";
        }
        o << buildExtendedNewick( &n->getChild(n->getNumberOfChildren()-1) ) << ")" << additionalInfo << ":" << n->getBranchLength() * br;
    }
    
    return o.str();
}


/** Monitor value at generation gen */
void PhylowoodNhxMonitor::monitor(long gen) {
    
    // get the printing frequency
    int samplingFrequency = printgen;
    
    if (gen % samplingFrequency == 0 && gen != maxGen) { // && gen >= burn) {
        
        std::string en = buildExtendedNewick();
        std::cout << "\n" << en << "\n\n";

    }
    
    if (gen == maxGen)
    {
        std::string nhxStr = buildNhxString();
        outStream << nhxStr << std::endl;
    }
}

std::string PhylowoodNhxMonitor::buildNhxString(void)
{
    std::stringstream nhxStrm;
    
    // begin nexus file
    nhxStrm << "#NEXUS" << "\n\n";
    
    // phylowood settings block
    nhxStrm << "Begin phylowood;\n";
    nhxStrm << "\tdrawtype pie\n";
    nhxStrm << "\tmodeltype biogeography\n";
    nhxStrm << "\tareatype discrete\n";
    nhxStrm << "\tmaptype clean\n";
    nhxStrm << "\tpieslicestyle even\n";
    nhxStrm << "\tpiefillstyle outwards\n";
    nhxStrm << "\ttimestart -" << tree->getValue().getRoot().getAge() << "\n";
    nhxStrm << "\tmarkerradius " << 300 << "\n";
    nhxStrm << "\tminareaval " << 0.1 << "\n";
    nhxStrm << "End;\n\n";
    
    // bayarea-fig block
    nhxStrm << "Begin bayarea-fig;\n";
    nhxStrm << "\tmapheight\t100\n";
    nhxStrm << "\tmapwidth\t150\n";
    nhxStrm << "\tcanvasheight\t2000\n";
    nhxStrm << "\tcanvaswidth\t1000\n";
    nhxStrm << "\tminareaval\t0.15\n";
    nhxStrm << "\tareacolors black\n";
    nhxStrm << "\tareatypes";
    for (int i = 0; i < numCharacters; i++)
        nhxStrm << " 1";
    nhxStrm << "\n";
    nhxStrm << "\tareanames Default\n";
    nhxStrm << "End;\n\n";
    
    // taxa block
    nhxStrm << "Begin taxa;\n";
    nhxStrm << "\tDimensions ntax=" << tree->getValue().getNumberOfTips() << ";\n";
    nhxStrm << "\tTaxlabels\n";
    for (int i = 0; i < tree->getValue().getNumberOfNodes(); i++)
    {
        TopologyNode* p = &tree->getValue().getNode(i);
        if (p->isTip())
        {
            nhxStrm << "\t\t" << p->getName() << "\n";
        }
    }
    nhxStrm << "\t\t;\n";
    nhxStrm << "End;\n\n";
    
    // geo block
    nhxStrm << "Begin geo;\n";
    nhxStrm << "\tDimensions ngeo=" << numCharacters << ";\n";
    nhxStrm << "\tCoords\n";
    
    for (int i = 0; i < numCharacters; i++)
    {
        nhxStrm << "\t\t" << i << "\t" << geographicCoordinates[i][0] << "\t" << geographicCoordinates[i][1];
        if (i < (numCharacters - 1))
            nhxStrm << ",";
        nhxStrm << "\n";
    }
    nhxStrm << "\t\t;\n";
    nhxStrm << "End;\n\n";
    
    // tree block
    nhxStrm << "Begin trees;\n";
    nhxStrm << "\tTranslate\n";
    for (int i = 0; i < tree->getValue().getNumberOfNodes(); i++)
    {
        TopologyNode* p = &tree->getValue().getNode(i);
        if (p->isTip())
        {
            nhxStrm << "\t\t" << p->getIndex() << "\t" << p->getName();
            if (i < (tree->getValue().getNumberOfNodes() - 1))
                nhxStrm << ",";
            nhxStrm << "\n";
        }
    }
    nhxStrm << "\t\t;\n";
    
    // write tree string
    std::string treeStr = "";
    treeStr = buildExtendedNewick(); //buildExtendedNewick(&tree->getValue().getRoot());
    std::cout << treeStr << "\n";
    std::cout << "nhxStr\n" << treeStr << "\n";
    nhxStrm << "tree TREE1 = " << treeStr << ";\n";
    nhxStrm << "End;\n";
    
    return nhxStrm.str();
}


/** open the file stream for printing */
void PhylowoodNhxMonitor::openStream(void) {
    
    // open the stream to the file
    if (append)
        outStream.open( filename.c_str(), std::fstream::out | std::fstream::app);
    else
        outStream.open( filename.c_str(), std::fstream::out);
}

/** Print header for monitored values */
void PhylowoodNhxMonitor::printHeader() {
    
    // do nothing
    ;
}


void PhylowoodNhxMonitor::swapNode(DagNode *oldN, DagNode *newN) {
    
    bool found = false;
    if ( oldN == tree )
    {
        tree = static_cast< TypedDagNode< TimeTree > *>( newN );
        found = true;
    }
    for (size_t i = 0; i < branchHistories.size(); i++)
    {
        if (oldN == branchHistories[i])
        {
            branchHistories[i] = static_cast<StochasticNode<BranchHistory>* >(newN);
            found = true;
        }
    }
    
    /*
     if (found == false)
     {
     // error catching
     if ( nodeVariables.find(oldN) == nodeVariables.end() ) {
     throw RbException("Cannot replace DAG node in this monitor because the monitor doesn't hold this DAG node.");
     }
     
     nodeVariables.erase( oldN );
     nodeVariables.insert( newN );
     }
     */
    
    // delegate to base class
    Monitor::swapNode(oldN, newN);
}