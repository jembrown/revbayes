//
//  AdmixtureTree.cpp
//  revbayes_mlandis
//
//  Created by Michael Landis on 1/21/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//


/**
 * @file
 * This file contains the implementation of a time-calibrated tree. This version of a tree
 * contains nodes with times.
 *
 * @brief Implementation of a time calibrated tree
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2012-07-05 16:47:08 +0200 (Thu, 05 Jul 2012) $
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2012-07-17, version 1.0
 *
 * $Id: AdmixtureTree.cpp 1651 2012-07-05 14:47:08Z hoehna $
 */



#include "AdmixtureTree.h"
#include "TopologyNode.h"
#include "RbOptions.h"
#include <iostream>

using namespace RevBayesCore;


/* Default constructor */
AdmixtureTree::AdmixtureTree(void) : Tree(), root( NULL ), numTips( 0 ), numNodes( 0 ) {
    
}


/* Copy constructor */
AdmixtureTree::AdmixtureTree(const AdmixtureTree& t) : Tree( t ), root( NULL ) {
    
    // set the parameters
    binary      = t.binary;
    numTips     = t.numTips;
    numNodes    = t.numNodes;
    
    // need to perform a deep copy of the AdmixtureTree nodes
    if (t.root != NULL) {
        root = t.getRoot().clone();
        
        // fill the nodes vector
        //    fillNodesByPreorderTraversal(root);
        fillNodesByPhylogeneticTraversal(root);
        
        // set this topology for the nodes
        root->setTopology( this );
        
        fillNodesByIndex();
    }
    
}


/* Destructor */
AdmixtureTree::~AdmixtureTree(void) {
    
    nodes.clear();
    nodesByIndex.clear();
    
    delete root;
}


AdmixtureTree& AdmixtureTree::operator=(const AdmixtureTree &t) {
    
    if (this != &t) {
        Tree::operator=(t);
        
        nodes.clear();
        nodesByIndex.clear();
        
        delete root;
        root = NULL;
        
        binary      = t.binary;
        numTips     = t.numTips;
        numNodes    = t.numNodes;
        nodesByIndex = t.nodesByIndex;
        
        TopologyNode* newRoot = t.root->clone();
        
        // set the root. This will also set the nodes vector.
        setRoot(newRoot);
        
        // set this topology for the nodes
        root->setTopology( this );
        
    }
    
    return *this;
}


/* Clone function */
AdmixtureTree* AdmixtureTree::clone(void) const {
    
    return new AdmixtureTree(*this);
}


void AdmixtureTree::equalizeBranchLengths( void ) {
    
    equalizeBranchLengths( *root );
}



void AdmixtureTree::equalizeBranchLengths( AdmixtureNode &node ) {
    
    if ( !node.isTip() ) {
        equalizeBranchLengths( node.getChild(0) );
        equalizeBranchLengths( node.getChild(1) );
        
        double left = node.getChild(0).getAge() + node.getChild( 0 ).getBranchLength();
        double right = node.getChild(1).getAge() + node.getChild( 1 ).getBranchLength();
        std::cout << node.getAge() << " = " << ((left + right) / 2.0) << std::endl;
        node.setAge( (left + right) / 2.0 );
    }
    
}





void AdmixtureTree::fillNodeTimes( void ) {
    
    fillNodeTimes( *root );
}


void AdmixtureTree::fillNodeTimes( AdmixtureNode &node ) {
    
    if ( !node.isTip() ) {
        fillNodeTimes( node.getChild(0) );
        fillNodeTimes( node.getChild(1) );
    }
    
    node.getAge();
    node.getBranchLength();
    node.getTime();
    
}

bool AdmixtureTree::updateTopologicalSort(void)
{
    nodesByTopologicalSort.clear();
    markTopologicalSortGlobal.clear();
    std::map<AdmixtureNode*,bool> markTopologicalSortLocal;
    
    return fillNodesByTopologicalSort(root, markTopologicalSortLocal);
}

bool AdmixtureTree::fillNodesByTopologicalSort(AdmixtureNode* node, std::map<AdmixtureNode*,bool> markTopologicalSortLocal)
{
    
    // if local topological sort recursion finds a repeated node, then graph contains a cycle
    if (markTopologicalSortLocal.find(node) != markTopologicalSortLocal.end())
        return false;
    
    // if global sort has not visited node, proceed
    if (markTopologicalSortGlobal.find(node) == markTopologicalSortGlobal.end())
    {

        // flag global and local visits
        markTopologicalSortLocal[node] = true;
        markTopologicalSortGlobal[node] = true;
        
        // recurse over tree children
        size_t nCh = node->getNumberOfChildren();
        for (size_t i = 0; i < nCh; i++)
            if (fillNodesByTopologicalSort(&node->getChild(i), markTopologicalSortLocal) == false)
                return false;
        
        // recure over admixture children
        if (&node->getAdmixtureChild() != NULL)
            if (fillNodesByTopologicalSort(&node->getAdmixtureChild(), markTopologicalSortLocal) == false)
                return false;
        
        nodesByTopologicalSort.push_back(node);
        
    }
    
    // if no cycle was found, then the admixture graph is a DAG
    return true;
    
}


/* fill the nodes vector by a preorder traversal recursively starting with this node. */
void AdmixtureTree::fillNodesByPreorderTraversal(AdmixtureNode* node) {
    
    // this is preorder so add yourself first
    nodes.push_back(node);
    
    // now call this function recursively for all your children
    for (size_t i=0; i<node->getNumberOfChildren(); i++) {
        fillNodesByPreorderTraversal(&node->getChild(i));
    }
}

/* fill the nodes vector by a phylogenetic traversal recursively starting with this node.
 * The tips fill the slots 0,...,n-1 followed by the internal nodes and then the root.
 */
void AdmixtureTree::fillNodesByPhylogeneticTraversal(AdmixtureNode* node) {
    
    // now call this function recursively for all your children
    for (size_t i=0; i<node->getNumberOfChildren(); i++) {
        fillNodesByPhylogeneticTraversal(&node->getChild(i));
    }
    
    if (node->isTip()) {
        // all the tips go to the beginning
        nodes.insert(nodes.begin(), node);
    }
    else {
        // this is phylogenetic ordering so the internal nodes come last
        nodes.push_back(node);
    }
}



std::vector<std::string> AdmixtureTree::getNames( void ) const {
    std::vector<std::string> names;
    for (size_t i = 0; i < getNumberOfTips(); ++i) {
        const TopologyNode& n = getTipNode( i );
        names.push_back( n.getName() );
    }
    
    return names;
}


std::vector<TopologyNode *> AdmixtureTree::getNodes( void ) const {
    std::vector<TopologyNode *> tmp( nodes.size() );
    for (unsigned i = 0; i < nodes.size(); ++i) {
        tmp[i] = nodes[i];
    }
    return tmp;
}


AdmixtureNode& AdmixtureTree::getNode(size_t idx) {
    return *nodes[idx];
}


const AdmixtureNode& AdmixtureTree::getNode(size_t idx) const {
    return *nodes[idx];
}

AdmixtureNode& AdmixtureTree::getNodeByIndex(size_t idx) {
    return *nodesByIndex[idx];
}

const AdmixtureNode& AdmixtureTree::getNodeByIndex(size_t idx) const {
    return *nodesByIndex[idx];
}

std::vector<TopologyNode *> AdmixtureTree::getNodesByIndex( void ) const {
    std::vector<TopologyNode *> tmp( nodesByIndex.size() );
    for (unsigned i = 0; i < nodesByIndex.size(); ++i) {
        tmp[i] = nodesByIndex[i];
    }
    return tmp;
}

void AdmixtureTree::fillNodesByIndex(void)
{
    size_t n = nodes.size();
    nodesByIndex= std::vector<AdmixtureNode*>(n);
    
    for (size_t i = 0; i < n; i++)
        nodesByIndex[i] = nodes[i];
}



double AdmixtureTree::getAge(size_t idx) const {
    return ages[idx];
}



double AdmixtureTree::getBranchLength(size_t idx) const {
    size_t parentIdx = nodesByIndex[idx]->getParent().getIndex();
    return ages[parentIdx] - ages[idx];
}

/** Calculate the number of interior nodes in the AdmixtureTree by deducing the number of
 tips from number of nodes, and then subtract 1 more if the AdmixtureTree is rooted. */
size_t AdmixtureTree::getNumberOfInteriorNodes( void ) const {
    
    size_t n = 0;
    for (size_t i=0; i<nodes.size(); i++) {
        if (nodes[i]->getNumberOfChildren() == 2)
            n++;
    }
    
    if ( isRooted() )
        return n - 1;
    else
        return n;

}


size_t AdmixtureTree::getNumberOfNodes( void ) const {
    return nodes.size();
}

size_t AdmixtureTree::getNumberOfAdmixtureParents() const {
    
    size_t n = 0;
    for (size_t i=0; i<nodes.size(); i++) {
        if (&nodes[i]->getAdmixtureChild() != NULL)
            n++;
    }
    return n;
}

std::vector<AdmixtureNode*> AdmixtureTree::getAdmixtureParents() const {
    
    std::vector<AdmixtureNode *> tmp;
    for (unsigned i = 0; i < nodes.size(); i++) {
        if (&nodes[i]->getAdmixtureChild() != NULL)
            tmp.push_back(nodes[i]);
    }
    return tmp;
}

size_t AdmixtureTree::getNumberOfAdmixtureChildren() const {
    
    size_t n = 0;
    for (size_t i=0; i<nodes.size(); i++) {
        if (&nodes[i]->getAdmixtureParent() != NULL)
            n++;
    }
    //std::cout << n << "\t" << nodes.size() << "\n";
    return n;
}


double AdmixtureTree::getTime(size_t idx) const {
    return times[idx];
}

void AdmixtureTree::eraseAdmixtureNode(AdmixtureNode *p)
{
    std::vector<AdmixtureNode*>::iterator it;

    //@Michael: This doesn't compile on Windows using GCC 4.2
//    it = find(nodes.begin(), nodes.end(), p);
//    nodes.erase(it);

}

void AdmixtureTree::checkAllEdges(void)
{
    for (size_t i = 0; i < nodes.size(); i++)
    {
        std::cout << &nodes[i];
        
        if (!nodes[i]->isRoot())
            std::cout << "\tcP=" << (nodes[i]->checkParent() ? "T" : "F");
    
        if (!nodes[i]->isTip())
            std::cout << "\tcC=" << (nodes[i]->checkChildren() ? "T" : "F");
        
        std::cout << "\n";
    }
}

void AdmixtureTree::checkAllEdgesRecursively(AdmixtureNode* p) const
{
    std::cout << "p " << p << "    ";
    
    if (p->getAge() == 0.0)
        std::cout << "age 0.0        ";
    else
        std::cout << "age " << p->getAge() << "   ";

    if (!p->isRoot())
        std::cout << "P " << &p->getParent() << "    ";
    else
        std::cout << "P null           ";

    size_t nCh = p->getNumberOfChildren();
    if (nCh == 0)
    {
        std::cout << "C0 null           ";
        std::cout << "C1 null           ";
    }
    else if (nCh == 1)
    {
        std::cout << "C0 " << &p->getChild(0) << "    ";
        std::cout << "C1 null           ";
    }
    else if (nCh == 2)
    {
        std::cout << "C0 " << &p->getChild(0) << "    ";
        std::cout << "C1 " << &p->getChild(1) << "    ";
    }
    
    if (&p->getAdmixtureParent() != NULL)
        std::cout << "AP " << &p->getAdmixtureParent() << "    ";
    else
        std::cout << "AP null          ";

    if (&p->getAdmixtureChild() != NULL)
        std::cout << "AC " << &p->getAdmixtureChild() << "    ";
    else
        std::cout << "AC null           ";
    
    if (!p->isRoot())
        std::cout << "\tcP=" << (p->checkParent() ? "T" : "F");
    
    if (!p->isTip())
        std::cout << "\tcC=" << (p->checkChildren() ? "T" : "F");
    
    if (p->getNumberOfChildren() == 1 && p->isAdmixtureNode() && &p->getAdmixtureChild() != NULL && &p->getAdmixtureParent() == NULL)
        std::cout << "\tAP";
    
    else if (p->getNumberOfChildren() == 1 && p->isAdmixtureNode() && &p->getAdmixtureChild() == NULL && &p->getAdmixtureParent() != NULL)
        std::cout << "\tAC";
    
    else if (p->getNumberOfChildren() == 0 && p->isTip() && (&p->getAdmixtureChild() == NULL || &p->getAdmixtureParent() == NULL))
        std::cout << "\tTN";
    
    else if (p->getNumberOfChildren() == 2 && p->isInteriorNode() && (&p->getAdmixtureChild() == NULL || &p->getAdmixtureParent() == NULL))
        std::cout << "\tIN";
    
    else
        std::cout << "\t??";
        
    std::cout << "\n";

    for (size_t i = 0; i < nCh; i++)
        checkAllEdgesRecursively(&p->getChild(i));
}

void AdmixtureTree::pushAdmixtureNode(AdmixtureNode *p)
{
    nodes.push_back(p);
}

const std::string& AdmixtureTree::getNewickRepresentation( void ) const {
    return root->computeNewick();
}

/** Calculate and return the number of tips on the AdmixtureTree by going through the vector
 of nodes, querying each about its tip status. */
size_t AdmixtureTree::getNumberOfTips( void ) const {
    
    size_t n = 0;
    for (size_t i=0; i<nodes.size(); i++) {
        if (nodes[i]->isTip() == true)
            n++;
    }
    return n;
}


/** We provide this function to allow a caller to randomly pick one of the interior nodes.
 This version assumes that the root is always the last and the tips the first in the nodes vector. */
const AdmixtureNode& AdmixtureTree::getInteriorNode( int indx ) const {
    
    // \TODO: Bound checking, maybe draw from downpass array instead
    return *nodesByIndex[ indx + getNumberOfTips() ];
}


AdmixtureNode& AdmixtureTree::getRoot( void ) {
    
    return *root;
}


const AdmixtureNode& AdmixtureTree::getRoot( void ) const {
    
    return *root;
}


/** We provide this function to allow a caller to randomly pick one of the interior nodes.
 This version assumes that the tips are first in the nodes vector. */
AdmixtureNode& AdmixtureTree::getTipNode( size_t indx ) {
    
    return *nodes[ indx ];
}


/** We provide this function to allow a caller to randomly pick one of the interior nodes.
 This version assumes that the tips are first in the nodes vector. */
const AdmixtureNode& AdmixtureTree::getTipNode( size_t indx ) const {
    
    // \TODO: Bound checking
    return *nodes[ indx ];
}

double AdmixtureTree::getTreeLength(void) const {
    double treeLength = 0;
    for (size_t i = 0; i < nodes.size(); i++)
        treeLength += nodes[i]->getBranchLength();
    return treeLength;
}

double AdmixtureTree::getTreeHeight(void) const {
    return root->getAge();
}

double AdmixtureTree::getUnitTreeLength(void) const {
    
    return getTreeLength() / root->getAge();
}

bool AdmixtureTree::isBinary( void ) const {
    return binary;
}


bool AdmixtureTree::isRooted( void ) const {
    return true;
}


void AdmixtureTree::resizeElementVectors(size_t n) {
    // remove all elements
    ages.clear();
    times.clear();
    
    // resize to new dimension
    ages.resize( n );
    times.resize( n );
}


void AdmixtureTree::setAge(size_t idx, double a) {
    ages[idx] = a;
}


void AdmixtureTree::setRooted(bool tf) {
    // time tree are always rooted!!!
}



void AdmixtureTree::setRoot( TopologyNode* r) {
    
    AdmixtureNode *tn = dynamic_cast<AdmixtureNode*>( r );
    setRoot( tn );
    
}

void AdmixtureTree::setRoot( AdmixtureNode* r) {
    
    // set the root
    root = r;
    
    root->setTopology( this );
    
    nodes.clear();
    
    // bootstrap all nodes from the root and add the in a pre-order traversal
    // fillNodesByPreorderTraversal(r);
    fillNodesByPhylogeneticTraversal(r);
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        nodes[i]->setIndex(i);
    }
    
    if (nodesByIndex.size() == 0)
        fillNodesByIndex();
}

std::ostream& RevBayesCore::operator<<(std::ostream& o, const AdmixtureTree& x) {
    o << x.getNewickRepresentation();
    
    return o;
}


