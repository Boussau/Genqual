#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <mpi.h>
#include <string>
#include "MbRandom.h"
#include "Model.h"
#include "Msg.h"
#include "Parm.h"
#include "Parm_tree.h"
#include "StateSets.h"
#include "Util.h"

#define	BRLENS_MIN					0.000001
#define	BRLENS_MAX					10.0
#define	BRPROP_MIN					0.000001
#define	BRPROP_MAX					10.0

#define DEBUG_TBR



Node::Node(Tree* t) {

	anc        = NULL;
	flag       = 0;
	index      = 0;
	isLeaf     = false;
	lft        = NULL;
	marked     = false;
	name       = "";
	proportion = 0.0;
	rht        = NULL;
	scratch    = 0;
	treePtr    = t;
}

Node::~Node(void) {
	
}

NodePair::NodePair(void) {

	node1 = NULL;
	node2 = NULL;
}

NodePair::NodePair(Node* n1, Node* n2) {

	node1 = n1;
	node2 = n2;
}

NodePair::NodePair(const NodePair& a) {

	node1 = a.node1;
	node2 = a.node2;
}

bool NodePair::operator==(const NodePair& a) const {

	return ( node1->getIndex() == a.node1->getIndex() && node2->getIndex() == a.node2->getIndex() );
}

bool NodePair::operator<(const NodePair& a) const {

	if ( node1->getIndex() < a.node1->getIndex() )
		return true;
	else if ( node1->getIndex() == a.node1->getIndex() )
		{
		if ( node2->getIndex() < a.node2->getIndex() )
			return true;
		}
	return false;
}

void NodePair::print(void) {

	std::cout << "Node Pair: " << node1->getIndex() << " " << node2->getIndex() << std::endl;
}

Tree::Tree(MbRandom *rp, Model* mp, std::vector<std::string>& taxonNames, double lam) {

	ranPtr      = rp;
	modelPtr    = mp;
	numTaxa     = taxonNames.size();
	numNodes    = (2 * numTaxa) - 2;
	brlenLambda = lam;
	alpha0      = 500.0;
	numAffectedNucleotideSites = 0;

	for (int i=0; i<numNodes; i++)
		nodes.push_back( new Node(this) );
	
	buildRandomTree(taxonNames);
}

Tree::Tree(Tree &t) {

	brlenLambda = 0.0;
	numTaxa     = 0;
	numNodes    = 0;
	root        = NULL;
	numAffectedNucleotideSites = 0;
	clone(t);
}

Tree::~Tree(void) {

	for (std::vector<Node*>::iterator p=nodes.begin(); p != nodes.end(); p++)
		delete (*p);
}

Tree& Tree::operator=(const Tree &t) {

	if (this != &t)
		clone(t);
	return *this;
}

void Tree::addNodeToBranch(Node* p, Node* des, Node* spare) {

	if (des->getAnc() == NULL)
		Msg::error("Problem inserting node");
	Node* desAnc = des->getAnc();
	if (desAnc->getLft() == des)
		{
		desAnc->setLft(spare);
		spare->setAnc(desAnc);
		spare->setLft(des);
		des->setAnc(spare);
		spare->setRht(p);
		p->setAnc(spare);
		}
	else 
		{
		desAnc->setRht(spare);
		spare->setAnc(desAnc);
		spare->setRht(des);
		des->setAnc(spare);
		spare->setLft(p);
		p->setAnc(spare);
		}
}

void Tree::breakTreeAtRandomBranch(std::vector<Node*>& subTreeOne, std::vector<Node*>& subTreeTwo, Node* &rootOne, Node* &rootTwo, std::vector<Node*>& spareNodes, double& oldBrlenConnectionLengthOne, double& oldBrlenConnectionLengthTwo, NodePair& oldPair) {

	// set all of the node flags to -1
	setAllFlags(1);
	
	// clear the vectors that will eventually hold the node pointers
	subTreeOne.clear();
	subTreeTwo.clear();
	spareNodes.clear();
	rootOne = NULL;
	rootTwo = NULL;
	
	// pick a branch at random and mark the selected branch
	Node* p = pickBranchAtRandom();
	Node* q = p->getAnc();
	if (q->getAnc() == NULL)
		{
		p = q;
		q = p->getLft();
		}
		
	// divide up the tree 
	Node *a, *b, *c, *d;
	if ( p->getLft() == NULL || p->getRht() == NULL || p->getAnc() == NULL )
		{
		/* we cut at a tip branch */
		if ( p->getLft() == NULL && p->getRht() == NULL )
			{
			/* tip branch pointing up */
			oldBrlenConnectionLengthOne = 1.0;
			if (q->getLft() == p)
				a = q->getRht();
			else
				a = q->getLft();
			b = q->getAnc();
			if (b->getLft() == q)
				b->setLft(a);
			else
				b->setRht(a);
			a->setAnc(b);
			a->setP( a->getP() + q->getP() );
			oldBrlenConnectionLengthTwo = a->getP();
			rootTwo = a;
			while (rootTwo->getAnc() != NULL)
				rootTwo = rootTwo->getAnc();
			p->setAnc(NULL);
			spareNodes.push_back( q );
			q->setLft(NULL);
			q->setRht(NULL);
			q->setAnc(NULL);
			q->setP( 0.0 );
			p->setP( 0.0 );
			rootOne = p;
			a->setFlag(0);
			p->setFlag(0);
			}
		else
			{
			/* tip branch pointing down (at root) */
			oldBrlenConnectionLengthOne = 1.0;
			rootOne = p;
			p->setLft(NULL);
			p->setRht(NULL);
			p->setAnc(NULL);
			p->setP( 0.0 );
			q->setAnc(NULL);
			a = q->getLft();
			b = q->getRht();
			a->setFlag(0);
			p->setFlag(0);
			oldBrlenConnectionLengthTwo = a->getP() + b->getP();
			while (b->getRht() != NULL)
				{
				c = b->getRht();
				d = b->getLft();
				
				a->setP( a->getP()+b->getP() );
				b->setP( c->getP() / 2.0 );
				c->setP( c->getP() / 2.0 );

				b->setLft(a);
				b->setRht(d);
				b->setAnc(q);
				a->setAnc(b);
				d->setAnc(b);
				q->setLft(b);
				q->setRht(c);
				c->setAnc(q);
				
				a = q->getLft();
				b = q->getRht();
				}
			b->setLft(a);
			b->setRht(NULL);
			b->setAnc(NULL);
			a->setAnc(b);
			a->setP( a->getP()+b->getP() );
			b->setP( 0.0 );
			rootTwo = b;
			spareNodes.push_back( q );
			q->setLft(NULL);
			q->setRht(NULL);
			q->setAnc(NULL);
			q->setP( 0.0 );
			}
		}
	else
		{
		/* cut at an interior branch */
		if (q->getLft() == p)
			a = q->getRht();
		else
			a = q->getLft();
		b = q->getAnc();
		if (b->getLft() == q)
			b->setLft(a);
		else
			b->setRht(a);
		a->setAnc(b);
		a->setP( a->getP()+q->getP() );
		a->setFlag(0);
		oldBrlenConnectionLengthTwo = a->getP();
		rootTwo = a;
		while (rootTwo->getAnc() != NULL)
			rootTwo = rootTwo->getAnc();
		spareNodes.push_back( q );
		q->setLft(NULL);
		q->setRht(NULL);
		q->setAnc(NULL);
		q->setP( 0.0 );
		p->setAnc(NULL);

		a = p->getLft();
		b = p->getRht();
		a->setFlag(0);
		oldBrlenConnectionLengthOne = a->getP() + b->getP();
		while (b->getRht() != NULL)
			{

			c = b->getRht();
			d = b->getLft();
			a->setP( a->getP()+b->getP() );
			b->setP( c->getP() / 2.0 );
			c->setP( c->getP() / 2.0 );

			b->setLft(a);
			b->setRht(d);
			b->setAnc(p);
			a->setAnc(b);
			d->setAnc(b);
			p->setLft(b);
			p->setRht(c);
			c->setAnc(p);
			
			a = p->getLft();
			b = p->getRht();
			}
		b->setLft(a);
		b->setRht(NULL);
		b->setAnc(NULL);
		a->setAnc(b);
		a->setP( a->getP()+b->getP() );
		b->setP( 0.0 );
		rootOne = b;
		spareNodes.push_back( p );
		p->setLft(NULL);
		p->setRht(NULL);
		p->setAnc(NULL);
		p->setP( 0.0 );
		}

	// get the down pass sequence for each subtree; incidentally, keep all of the nodes for each
	// subtree in a vector
	getDownPassSequence(rootOne, subTreeOne);
	getDownPassSequence(rootTwo, subTreeTwo);
	
	// find original connection pair
	bool foundPair = false;
	for (std::vector<Node*>::iterator p=subTreeOne.begin(); p != subTreeOne.end(); p++)
		{
		if ( (*p)->getAnc() != NULL || ((*p)->getAnc() == NULL && subTreeOne.size() == 1) )
			{
			for (std::vector<Node*>::iterator q=subTreeTwo.begin(); q != subTreeTwo.end(); q++)
				{
				if ( (*q)->getAnc() != NULL || ((*q)->getAnc() == NULL && subTreeTwo.size() == 1) )
					{
					if ( (*p)->getFlag() + (*q)->getFlag() == 0 )
						{
						oldPair.setNode1(*p);
						oldPair.setNode2(*q);
						foundPair = true;
						break;
						}
					}
				}
			}
		if (foundPair == true)
			break;
		}
}

void Tree::buildRandomTree(std::vector<std::string>& taxonNames) {

	// set indices
	for (int i=0; i<numNodes; i++)
		nodes[i]->setIndex(i);
		
	// set names
	for (int i=0; i<numTaxa; i++)
		{
		nodes[i]->setName(taxonNames[i]);
		nodes[i]->setIsLeaf(true);
		}
		
	int nextTipNode = 0;
	int nextIntNode = numTaxa;
	Node **availableNodes = new Node*[numNodes];
	int numAvailableNodes = 0;
	
	// make a three taxon tree
	Node *p = nodes[nextTipNode++];
	root = p;
	Node *q = nodes[nextIntNode++];
	availableNodes[numAvailableNodes++] = q;
	p->setLft(q);
	q->setAnc(p);
	p = q;
	q = nodes[nextTipNode++];
	availableNodes[numAvailableNodes++] = q;
	p->setLft(q);
	q->setAnc(p);
	q = nodes[nextTipNode++];
	availableNodes[numAvailableNodes++] = q;
	p->setRht(q);
	q->setAnc(p);
	
	// add the remaining branches to the tree-species star tree
	for (int n=3; n<numTaxa; n++)
		{
		int whichNode = (int)(ranPtr->uniformRv()*numAvailableNodes);
		p = nodes[nextTipNode++];
		q = nodes[nextIntNode++];
		Node *a = availableNodes[whichNode];
		Node *b = a->getAnc();
		if (b->getLft() == a)
			{
			b->setLft(q);
			q->setAnc(b);
			q->setLft(a);
			a->setAnc(q);
			q->setRht(p);
			p->setAnc(q);
			}
		else
			{
			b->setRht(q);
			q->setAnc(b);
			q->setRht(a);
			a->setAnc(q);
			q->setLft(p);
			p->setAnc(q);
			}
		availableNodes[numAvailableNodes++] = p;
		availableNodes[numAvailableNodes++] = q;
		}
	delete [] availableNodes;

	// get the down pass sequence for the tree
	getDownPassSequence(root, downPassSequence);
	
	// initialize the branch lengths
	double sum = 0.0;
	for (int i=0; i<numNodes; i++)
		{
		p = nodes[i];
		if (p->getAnc() != NULL)
			{
			double x = ranPtr->exponentialRv(brlenLambda);
			sum += x;
			p->setP( x );
			}
		}
}

void Tree::clone(const Tree &t) {

	// reallocate nodes, if necessary
	if (numNodes != t.numNodes)
		{
		for (std::vector<Node*>::iterator p=nodes.begin(); p != nodes.end(); p++)
			delete (*p);
		nodes.clear();
		for (int i=0; i<t.numNodes; i++)
			nodes.push_back( new Node(this) );
		}
		
	alpha0      = t.alpha0;
	brlenLambda = t.brlenLambda;
	modelPtr    = t.modelPtr;
	numTaxa     = t.numTaxa;
	numNodes    = t.numNodes;
	ranPtr      = t.ranPtr;
	numAffectedNucleotideSites = t.numAffectedNucleotideSites;
	
	for (int i=0; i<numNodes; i++)
		{
		Node* p = t.nodes[i];
		Node* q = nodes[i];
		
		if (p->getAnc() != NULL)
			{
			int pAncOffset = p->getAnc()->getIndex();
			q->setAnc( nodes[pAncOffset] );
			}
		else
			q->setAnc(NULL);
		if (p->getLft() != NULL)
			{
			int pLftOffset = p->getLft()->getIndex();
			q->setLft( nodes[pLftOffset] );
			}
		else
			q->setLft(NULL);
		if (p->getRht() != NULL)
			{
			int pRhtOffset = p->getRht()->getIndex();
			q->setRht( nodes[pRhtOffset] );
			}
		else
			q->setRht(NULL);
		
		q->setFlag( p->getFlag() );
		q->setIndex( p->getIndex() );
		q->setIsLeaf( p->getIsLeaf() );
		q->setMarked( p->getMarked() );
		q->setName( p->getName() );
		q->setP( p->getP() );
		q->setScratch( p->getScratch() );

		if (p == t.root)
			root = q;
		}

	downPassSequence.resize( t.downPassSequence.size() );
	for (int i=0; i<numNodes; i++)
		{
		Node* p = t.downPassSequence[i];
		int pOffset = p->getIndex();
		downPassSequence[i] = nodes[pOffset];
		}
}

void Tree::getDownPassSequence(Node* r, std::vector<Node*>& dps) {

	dps.clear();
	passDown(r, dps);
}

std::string Tree::getNewick(void) {

	std::stringstream ss;
	writeTree(root->getLft(), ss);
	std::string newick = ss.str();
	return newick;
}

double Tree::getTreeFraction(std::vector<Node*>& dps) {

	double sum = 0.0;
	for (std::vector<Node*>::iterator p=dps.begin(); p != dps.end(); p++)
		{
		if ( (*p)->getAnc() != NULL )
			sum += (*p)->getP();
		}
	return sum;
}

double Tree::lnPriorProb(void) {

	double lnP = 0.0;
	return lnP;
}

void Tree::markBranchesDown(Node* p) {

	Node* r = p;
	while (r->getAnc() != NULL)
		{
		r->setMarked(true);
		r = r->getAnc();
		}
}

int Tree::nodeIdx(Node* p) {

	if (p == NULL)
		return -1;
	return p->getIndex();
}

void Tree::normalizeProbs(std::map<NodePair,double>& lnProbs) {

	double maxLnProb = -100000000000.0;
	for (std::map<NodePair,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		{
		if ( it->second > maxLnProb )
			maxLnProb = it->second;
		}
	double sum = 0.0;
	for (std::map<NodePair,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		{
		it->second -= maxLnProb;
		if (it->second < -300.0)
			it->second = 0.0;
		else
			it->second = exp(it->second);
		sum += it->second;
		}
	for (std::map<NodePair,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		it->second /= sum;
}

void Tree::passDown(Node *p, std::vector<Node*>& dps) {

	if (p != NULL)
		{
		passDown(p->getLft(), dps);
		passDown(p->getRht(), dps);
		dps.push_back( p );
		}
}

Node* Tree::pickBranchAtRandom(void) {

	Node* p;
	do
		{
		p = nodes[ (int)(ranPtr->uniformRv()*numNodes) ];
		} while ( p == NULL || p->getAnc() == NULL );
	return p;
}

NodePair Tree::pickNodePairAtRandom(std::map<NodePair,double>& probs) {

	double u = ranPtr->uniformRv();
	double sum = 0.0;
	for (std::map<NodePair,double>::iterator it=probs.begin(); it != probs.end(); it++)
		{
		sum += it->second;
		if (u < sum)
			{
			return it->first;
			}
		}
	NodePair nullReturn(NULL,NULL);
	return nullReturn;
}

void Tree::print(void) {

	showNodes(downPassSequence, "");
}

void Tree::printNodeInfo(std::string msg) {

	std::cout << msg << std::endl;
	for (std::vector<Node*>::iterator p=nodes.begin(); p != nodes.end(); p++)
		{
		std::cout << std::setw(4) << nodeIdx(*p) << " " << (*p) << " -- (" << std::setw(4) << nodeIdx((*p)->getLft()) << "," << std::setw(4) << nodeIdx((*p)->getRht()) << "," << std::setw(4) << nodeIdx((*p)->getAnc()) << ")" << std::endl;
		}
}

void Tree::reconnectSubtrees(std::vector<Node*>& subTreeOne, std::vector<Node*>& subTreeTwo, Node* rootOne, Node* rootTwo, std::vector<Node*>& spareNodes, Node* p, Node* q, double& newBrlenConnectionLengthOne, double& newBrlenConnectionLengthTwo) {

	/* Get the tree length fractions for both subtrees. */
	double treeFractionOne = getTreeFraction(subTreeOne);
	double treeFractionTwo = getTreeFraction(subTreeTwo);

	/* link up the two subtrees at branches p & q */
	if (spareNodes.size() == 2)
		{
		/* two spare nodes and we have subtrees of some size */
		Node* s1 = spareNodes[0];
		Node* s2 = spareNodes[1];
		newBrlenConnectionLengthOne = p->getP();
		rerootOnNode(p, s1);

		double v = s1->getLft()->getP() + s1->getRht()->getP();
		double v1 = ranPtr->uniformRv()*v;
		s1->getLft()->setP(v1);
		s1->getRht()->setP(v-v1);
		Node* a = q;
		Node* b = q->getAnc();
		newBrlenConnectionLengthTwo = a->getP();
		v = a->getP();
		v1 = ranPtr->uniformRv()*v;
		addNodeToBranch(s1, a, s2);
		a->setP(v1);
		s2->setP(v-v1);
		s1->setP( 1.0 - treeFractionOne - treeFractionTwo );
		root = rootTwo;
		}
	else
		{
		/* one spare node and one of the subtrees is a single node */
		Node* s1 = spareNodes[0];
		if (subTreeOne.size() == 1)
			{
			newBrlenConnectionLengthOne = 1.0;
			newBrlenConnectionLengthTwo = q->getP();
			addNodeToBranch(p, q, s1);
			double v = q->getP();
			double v1 = ranPtr->uniformRv() * v;
			s1->setP( v1 );
			q->setP( v-v1 );
			root = rootTwo;
			}
		else if (subTreeTwo.size() == 1)
			{
			newBrlenConnectionLengthOne = p->getP();
			newBrlenConnectionLengthTwo = 1.0;
			addNodeToBranch(q, p, s1);
			double v = p->getP();
			double v1 = ranPtr->uniformRv() * v;
			s1->setP( v1 );
			p->setP( v-v1 );
			root = rootOne;
			}
		else
			{
			Msg::error("Why am I here");
			}
		s1->setP( 1.0 - treeFractionOne - treeFractionTwo );
		}
}

void Tree::rerootOnNode(Node* p, Node* r) {

	// make certain that r's pointers are clean
	r->setLft(NULL);
	r->setRht(NULL);
	r->setAnc(NULL);
	
	// check that the tree isn't trivial
	if (p->getAnc() == NULL)
		{
		r->setLft(p);
		p->setAnc(r);
		return;
		}

	// find root of this subtree
	Node* localRoot = p;
	while (localRoot->getAnc() != NULL)
		localRoot = localRoot->getAnc();
		
	// fill in a container with all of the nodes in this tree
	std::vector<Node*> localNodes;
	getDownPassSequence(localRoot, localNodes);
	
	// mark the path from p to the root
	for (std::vector<Node*>::iterator it=localNodes.begin(); it != localNodes.end(); it++)
		(*it)->setMarked(false);
	markBranchesDown(p);
	localRoot->setMarked(false);
	
	// add the spare node to the bottom of the tree
	Node* a = localRoot->getLft();
	Node* b = localRoot;
	r->setLft(a);
	r->setRht(b);
	a->setAnc(r);
	b->setAnc(r);
	b->setLft(NULL);
	b->setRht(NULL);
	a->setP( a->getP()*0.5 );
	b->setP( a->getP() );
	
	while ( r->getLft() != p && r->getRht() != p )
		{
		if (r->getLft()->getMarked() == true && r->getRht()->getMarked() == false)
			{
			// the marked not is to the left of the root
			a = r->getLft();
			b = r->getRht();
			if (a->getLft() == NULL || a->getRht() == NULL)
				Msg::error("Problem rotating tree");
			Node *c, *d;
			if (a->getLft()->getMarked() == true && a->getRht()->getMarked() == false)
				{
				c = a->getLft();
				d = a->getRht();
				}
			else if (a->getLft()->getMarked() == false && a->getRht()->getMarked() == true)
				{
				c = a->getRht();
				d = a->getLft();
				}
			else 
				Msg::error("Nodes to the left and right of the a are marked");
			r->setLft(c);
			r->setRht(a);
			c->setAnc(r);
			a->setAnc(r);
			a->setLft(d);
			a->setRht(b);
			d->setAnc(a);
			b->setAnc(a);
			a->setMarked(false);
			b->setP(b->getP()+a->getP());
			c->setP(c->getP()*0.5);
			a->setP(c->getP());
			}
		else if (r->getLft()->getMarked() == false && r->getRht()->getMarked() == true)
			{
			// the marked not is to the right of the root
			a = r->getRht();
			b = r->getLft();
			if (a->getLft() == NULL || a->getRht() == NULL)
				Msg::error("Problem rotating tree");
			Node *c, *d;
			if (a->getLft()->getMarked() == true && a->getRht()->getMarked() == false)
				{
				c = a->getLft();
				d = a->getRht();
				}
			else if (a->getLft()->getMarked() == false && a->getRht()->getMarked() == true)
				{
				c = a->getRht();
				d = a->getLft();
				}
			else 
				Msg::error("Nodes to the left and right of the a are marked");
			r->setLft(a);
			r->setRht(c);
			c->setAnc(r);
			a->setAnc(r);
			a->setLft(b);
			a->setRht(d);
			d->setAnc(a);
			b->setAnc(a);
			a->setMarked(false);
			b->setP(b->getP()+a->getP());
			c->setP(c->getP()*0.5);
			a->setP(c->getP());
			}
		else 
			Msg::error("Nodes to the left and right of the root are marked");
		}
}

void Tree::setAllFlags(int x) {

	for (int n=0; n<numNodes; n++)
		nodes[n]->setFlag(x);
}

void Tree::showNodes(std::vector<Node*>& dps, std::string treeTitle) {

	if (treeTitle != "")
		std::cout << treeTitle << std::endl;
	for (std::vector<Node*>::reverse_iterator p=dps.rbegin(); p != dps.rend(); p++)
		{
		std::cout << nodeIdx(*p) << " (" << nodeIdx((*p)->getLft()) << ", " << nodeIdx((*p)->getRht()) << ", " << nodeIdx((*p)->getAnc()) << ") " << std::fixed << std::setprecision(5) << (*p)->getP();
		if ( (*p)->getLft() == NULL || (*p)->getAnc() == NULL )
			std::cout << " (" << (*p)->getName() << ") ";
		if ((*p)->getAnc() == NULL)
			std::cout << " <- Root" << std::endl;
		else
			std::cout << std::endl;
		std::cout.flush();
		}
}

void Tree::showNodes(Node* p, int indent, std::ofstream& o) {

	if (p != NULL)
		{
		for (int i=0; i<indent; i++)
			o << " ";
		o << nodeIdx(p) << " (" << nodeIdx(p->getLft()) << ", " << nodeIdx(p->getRht()) << ", " << nodeIdx(p->getAnc()) << ") " << std::fixed << std::setprecision(5) << p->getP();
		if (p->getLft() == NULL || p->getAnc() == NULL )
			o << " (" << p->getName() << ") ";
		if (p->getAnc() == NULL)
			o << " <- Root" << std::endl;
		else
			o << std::endl;
		showNodes (p->getLft(), indent + 2, o);
		showNodes (p->getRht(), indent + 2, o);
		}
}


double Tree::update(void) {

	double lnP = 0.0;
	double u = ranPtr->uniformRv();
	if (u <= 0.40)
		lnP = updateLocal();
	else if (u > 0.40 && u <= 0.80)
		lnP = updateBrlen();
	else if (u > 0.80 && u <= 1.00)
		lnP = updateTbr();
	return lnP;
}

double Tree::updateLocal(void) {

	if (numTaxa <= 3)
		{
		double lnProposalProb = updateBrlen();
		return lnProposalProb;
		}
		
	/* pick an internal branch at random */
	Node *u;
	do
		{
		u = downPassSequence[(int)(ranPtr->uniformRv()*numNodes)];
		} while( !(u->getIsLeaf() == false && root->getLft() != u) );
	Node *v = u->getAnc();
	bool isLeft = false;
	if (v->getLft() == u)
		isLeft = true;
		
	/* pick two other branches incident to either end of branch a, to form the backbone of the move */
	Node *a, *b, *c, *d;
	if (ranPtr->uniformRv() < 0.5)
		{
		a = u->getLft();
		c = u->getRht();
		}
	else
		{
		a = u->getRht();
		c = u->getLft();
		}
	bool isBelow = false;
	if (ranPtr->uniformRv() < 0.5)
		{
		b = v->getAnc();
		if (isLeft == true)
			d = v->getRht();
		else
			d = v->getLft();
		isBelow = true;
		}
	else
		{
		d = v->getAnc();
		if (isLeft == true)
			b = v->getRht();
		else
			b = v->getLft();
		}
	
	/* store path in vector */
	Node *path[3];
	path[0] = a;
	path[1] = u;
	if (isBelow == true)
		path[2] = v;
	else
		path[2] = b;
		
	/* pick a new path length */
	double oldM = a->getP() + u->getP();
	if (isBelow == true)
		oldM += v->getP();
	else
		oldM += b->getP();
	std::vector<double> alp(2);
	std::vector<double> z(2);
	alp[0] = oldM * alpha0;
	alp[1] = (1.0 - oldM) * alpha0;
	ranPtr->dirichletRv(alp, z);
	ranPtr->renormalize(z, 0.0001);
	double newM = z[0];
	double lnForwardProb = ranPtr->lnDirichletPdf(alp, z);
	alp[0] = newM * alpha0;
	alp[1] = (1.0 - newM) * alpha0;
	z[0] = oldM;
	z[1] = 1.0 - oldM;
	double lnReverseProb = ranPtr->lnDirichletPdf(alp, z);
	
	/* calculate the log of the Hastings ratio and Jacobian here */
	double lnProposalProb = lnReverseProb - lnForwardProb;
	lnProposalProb += 2.0 * (log(newM) - log(oldM)) + (numNodes-4) * (log(1.0-newM) - log(1.0-oldM));
	
	/* reset all of the branch lengths */
	double sum = 0.0;
	for (int n=0; n<numNodes; n++)
		{
		Node *p = downPassSequence[n];
		if (p != root)
			{
			double v = p->getP();
			if ( p == path[0] || p == path[1] || p == path[2] )
				p->setP( v*(newM/oldM) );
			else
				p->setP( v*((1.0-newM)/(1.0-oldM)) );
			sum += p->getP();
			}
		}
	for (int n=0; n<numNodes; n++) // rescale branch lengths, just in case (not much should be going on here)
		{
		Node *p = downPassSequence[n];
		if (p != root)
			p->setP( p->getP()/sum );
		}
		
	/* randomly pick one of the two branches that are incident ot the path */
	Node *nodeToMove = c;
	if (ranPtr->uniformRv() < 0.5)
		nodeToMove = d;

	/* randomly reattach the selected node to the path */
	double newPos = ranPtr->uniformRv() * newM;
	bool topologyChanged = false;
	if (nodeToMove == c)
		{
		/* node c is moved */
		double criticalVal = a->getP() + u->getP();
		if (newPos < criticalVal)
			{
			/* no topology change */
			a->setP(newPos);
			u->setP(criticalVal-newPos);
			}
		else
			{
			/* topology change */
			a->setP( a->getP()+u->getP() );
			if (v->getLft() == u)
				v->setLft(a);
			else
				v->setRht(a);
			a->setAnc(v);
			if (isBelow == true)
				{
				if (b->getLft() == v)
					{
					b->setLft(u);
					u->setAnc(b);
					u->setLft(v);
					v->setAnc(u);
					u->setRht(c);
					c->setAnc(u);
					v->setP(newPos - criticalVal);
					u->setP(newM - newPos);
					}
				else
					{
					b->setRht(u);
					u->setAnc(b);
					u->setLft(c);
					c->setAnc(u);
					u->setRht(v);
					v->setAnc(u);
					}
				v->setP(newPos - criticalVal);
				u->setP(newM - newPos);
				}
			else
				{
				if (v->getLft() == b)
					{
					v->setLft(u);
					u->setAnc(v);
					u->setLft(b);
					b->setAnc(u);
					u->setRht(c);
					c->setAnc(u);
					}
				else
					{
					v->setRht(u);
					u->setAnc(v);
					u->setLft(c);
					c->setAnc(u);
					u->setRht(b);
					b->setAnc(u);
					}
				b->setP(newPos - criticalVal);
				u->setP(newM - newPos);
				}
			topologyChanged = true;
			}
		}
	else
		{
		/* node d is moved */
		double criticalVal = a->getP();
		if (newPos > criticalVal)
			{
			/* no topology change */
			if (isBelow == true)
				{
				u->setP(newPos - criticalVal);
				v->setP(newM - newPos);
				}
			else
				{
				u->setP(newPos - criticalVal);
				b->setP(newM - newPos);
				}
			}
		else
			{
			/* topology change */
			if (isBelow == true)
				{
				u->setP( u->getP()+v->getP() );
				if (b->getLft() == v)
					{
					b->setLft(u);
					u->setAnc(b);
					}
				else
					{
					b->setRht(u);
					u->setAnc(b);
					}
				if (u->getLft() == a)
					{
					u->setLft(v);
					v->setAnc(u);
					v->setLft(a);
					a->setAnc(v);
					v->setRht(d);
					d->setAnc(v);
					}
				else
					{
					u->setRht(v);
					v->setAnc(u);
					v->setLft(d);
					d->setAnc(v);
					v->setRht(a);
					a->setAnc(v);
					}
				a->setP(newPos);
				v->setP(criticalVal - newPos);
				}
			else
				{
				b->setP( b->getP()+u->getP() );
				v->setLft(u);
				v->setRht(a);
				u->setAnc(v);
				a->setAnc(v);
				u->setLft(b);
				u->setRht(c);
				b->setAnc(u);
				c->setAnc(u);
				u->setP(criticalVal - newPos);
				a->setP(newPos);
				}			
			topologyChanged = true;
			}
		}
	
	if (topologyChanged == true)
		getDownPassSequence(root, downPassSequence);
	
	return lnProposalProb;
}

double Tree::updateBrlen(void) {

	/* randomly pick a branch */
	Node *u;
	do
		{
		u = downPassSequence[(int)(ranPtr->uniformRv()*numNodes)];
		} while ( u->getAnc() == NULL );
	
	/* select new proportions */
	double oldP = u->getP();
	std::vector<double> alp(2);
	std::vector<double> z(2);
	alp[0] = oldP * alpha0;
	alp[1] = (1.0 - oldP) * alpha0;
	//std::cout << std::fixed << std::setprecision(25) << alp[0] << " " << alp[1] << std::endl;
	ranPtr->dirichletRv(alp, z);
	ranPtr->renormalize(z, 0.0001);
	double newP = z[0];
	if (newP < 0.000001)
		{
		newP = 0.000001;
		z[0] = newP;
		z[1] = 1.0 - newP;
		}
	//cout << fixed << setprecision(25) << newP << " " << 1.0 - newP << endl;
	double lnForwardProb = ranPtr->lnDirichletPdf(alp, z);
	alp[0] = newP * alpha0;
	alp[1] = (1.0 - newP) * alpha0;
	z[0] = oldP;
	z[1] = 1.0 - oldP;
	double lnReverseProb = ranPtr->lnDirichletPdf(alp, z);
	
	/* update the branch lengths */
	double sum = 0.0;
	for (int n=0; n<numNodes; n++)
		{
		Node *p = downPassSequence[n];
		if (p->getAnc() != NULL)
			{
			double v = p->getP();
			if ( p == u )
				p->setP( newP );
			else
				p->setP( v*((1.0-newP)/(1.0-oldP)) );
			sum += p->getP();
			}
		}
	for (int n=0; n<numNodes; n++) // rescale branch lengths, just in case (not much should be going on here)
		{
		Node *p = downPassSequence[n];
		if (p != root)
			p->setP( p->getP()/sum );
		}
	
	return (lnReverseProb - lnForwardProb) + (numNodes-2)*log((1.0-newP)/(1.0-oldP));
}

double Tree::updateTbr(void) {

	/* parameters that should eventually be set in settings.cpp */
	double heat = 0.6;

	/* Modify a branch length if there are three or fewer taxa in the tree. */
	if (numTaxa <= 3)
		{
		double lnProposalProb = updateBrlen();
		return lnProposalProb;
		}

	/* Break the tree into two */
	std::vector<Node*> subTreeOne;
	std::vector<Node*> subTreeTwo;
	std::vector<Node*> spareNodes;
	Node* rootOne;
	Node* rootTwo;
	double oldBrlenConnectionLengthOne = 1.0;
	double oldBrlenConnectionLengthTwo = 1.0;
	NodePair oldPair;
	breakTreeAtRandomBranch( subTreeOne, subTreeTwo, rootOne, rootTwo, spareNodes, oldBrlenConnectionLengthOne, oldBrlenConnectionLengthTwo, oldPair );
	
	/* initialize state sets on the two subtrees */
	for (std::vector<StateSets*>::iterator s=stateSets.begin(); s != stateSets.end(); s++)
		{
		(*s)->setLengthOne( (*s)->initializeStateSets(subTreeOne) );
		(*s)->setLengthTwo( (*s)->initializeStateSets(subTreeTwo) );
		}

	/* Calculate parsimony scores for all possible reattachment points. */
	std::map<NodePair,double> parsimonyScores;
	std::map<NodePair,double> probs;
	for (std::vector<Node*>::iterator p=subTreeOne.begin(); p != subTreeOne.end(); p++)
		{
		if ( (*p)->getAnc() != NULL || ((*p)->getAnc() == NULL && subTreeOne.size() == 1) )
			{
			for (std::vector<Node*>::iterator q=subTreeTwo.begin(); q != subTreeTwo.end(); q++)
				{
				if ( (*q)->getAnc() != NULL || ((*q)->getAnc() == NULL && subTreeTwo.size() == 1) )
					{
					// calculate the parsimony score for this pair of insertion points
					int parsimonyLength = 0;
					for (std::vector<StateSets*>::iterator s=stateSets.begin(); s != stateSets.end(); s++)
						{
						unsigned *ss1 = (*s)->getStsPtr( 1, (*p)->getIndex() );
						unsigned *ss2 = (*s)->getStsPtr( 1, (*q)->getIndex() );
						int length = (*s)->getLengthOne() + (*s)->getLengthTwo();
						for (int c=0; c<(*s)->getNumChar(); c++)
							{
							unsigned x = (*ss1);
							unsigned y = (*ss2);
							unsigned zA = (x & y);
							if ( zA == 0 )
								length++;
							ss1++;
							ss2++;
							}
						parsimonyLength = length;
						//std::cerr << "parsimonyLength = " << parsimonyLength << std::endl;
						}
					
					// update the parismony score keys
					NodePair nodeKey((*p),(*q));
					parsimonyScores.insert( std::make_pair(nodeKey,(double)parsimonyLength) );
					probs.insert( std::make_pair(nodeKey,(double)parsimonyLength) );
					}
				}
			}
		}
		
	/* Calculate the probability of reattaching for each pair of branches. */
	double z = (brlenLambda / (4.0/3.0 + brlenLambda));
	double a0 = log(0.25 + 0.75 * z);
	double a1 = log(0.25 - 0.25 * z);
	double marginalLikelihoodTerm = getNumAffectedNucleotideSites() * ((2 * numTaxa - 3) * a0 - log(4.0) );
	double tempFactor = a1 - a0;
	for (std::map<NodePair,double>::iterator it=probs.begin(), it2=parsimonyScores.begin(); it != probs.end(); it++, it2++)
		{
		double localParsimonyLength = it->second;
		double parsimonyLength = 0.0;
		MPI::COMM_WORLD.Allreduce(&localParsimonyLength, &parsimonyLength, 1, MPI::DOUBLE, MPI::SUM);
		double prob = tempFactor * parsimonyLength + marginalLikelihoodTerm;
		it->second = prob;
		it2->second = parsimonyLength;
		}
	normalizeProbs(probs);
		
	// Pick a pair of nodes to form the reattachment points. */
	NodePair chosenPair = pickNodePairAtRandom(probs);
		
	/* Get the marginal likelihood of the old and new reattachment point. */
	std::map<NodePair,double>::iterator pit = parsimonyScores.find( oldPair );
	if ( pit == parsimonyScores.end() )
		Msg::error("Cannot find old pair in the map");
	double oldLnLike = tempFactor * pit->second + marginalLikelihoodTerm;

	pit = parsimonyScores.find( chosenPair );
	if ( pit == parsimonyScores.end() )
		Msg::error("Cannot find chosen pair in the map");
	double newLnLike = tempFactor * pit->second + marginalLikelihoodTerm;
	MPI::COMM_WORLD.Barrier();
	
	/* Reconnect the two subtrees. */
	double newBrlenConnectionLengthOne = 1.0;
	double newBrlenConnectionLengthTwo = 1.0;
	reconnectSubtrees( subTreeOne, subTreeTwo, rootOne, rootTwo, spareNodes, chosenPair.getNode1(), chosenPair.getNode2(), newBrlenConnectionLengthOne, newBrlenConnectionLengthTwo );

	/* update downpass sequence */
	getDownPassSequence(root, downPassSequence);
	
	/* Calculate the proposal ratio. */
	double lnProposalProb = heat * (oldLnLike - newLnLike) + 
	                        (log(newBrlenConnectionLengthTwo) - log(oldBrlenConnectionLengthTwo)) + 
							(log(newBrlenConnectionLengthOne) - log(oldBrlenConnectionLengthOne));
	
	/* check all of the branch lengths, to make certain that they are a minimum length */
	bool resetBrProp = false;
	double sum = 0.0;
	for (std::vector<Node*>::iterator p=downPassSequence.begin(); p != downPassSequence.end(); p++)
		{
		if ( (*p)->getAnc() != NULL )
			{
			if ( (*p)->getP() < BRPROP_MIN )
				{
				(*p)->setP( BRPROP_MIN );
				resetBrProp = true;
				}
			sum += (*p)->getP();
			}
		}
	if ( resetBrProp == true )
		{
		for (std::vector<Node*>::iterator p=downPassSequence.begin(); p != downPassSequence.end(); p++)
			{
			if ((*p)->getAnc() != NULL)
				(*p)->setP( (*p)->getP() / sum );
			}
		}

	return lnProposalProb;
}

void Tree::writeTree(Node* p, std::stringstream &ss) {

	if (p != NULL)
		{
		
		if (p->getLft() == NULL && p->getRht() == NULL)
			{
			ss << p->getName() << ":" << std::fixed << std::setprecision(6) << p->getP();
			}
		else
			{
			if (p->getAnc() != NULL)
				{
				ss << "(";
				}
			writeTree(p->getLft(), ss);
			ss << ",";
			writeTree(p->getRht(), ss);	
			if (p->getAnc() != NULL)
				{
				if (p->getAnc()->getAnc() == NULL)
					{
					ss << "," << p->getAnc()->getName() << ":" << std::fixed << std::setprecision(6) << p->getP();
					}
				
				if (p->getAnc()->getAnc() != NULL)
					ss << "):" << std::fixed << std::setprecision(6) << p->getP();
				else
					ss << ")";
				}
			}
		}

}

ParmTree::ParmTree(Model *mp, MenuItem *ip, long int initSeed, std::string pn, std::vector<std::string>& taxonNames, double lam) : Parm(mp, ip, initSeed, pn) {

	trees[0] = new Tree(ranPtr, modelPtr, taxonNames, lam);
	trees[1] = new Tree( *trees[0] );
}

ParmTree::~ParmTree(void) {

	delete trees[0];
	delete trees[1];
}

double ParmTree::drawFromPrior(void) {

	Msg::error("Cannot redraw tree from the prior");
	return 0.0;
}

void ParmTree::getParmString(std::string& s) {

	s = getActiveTree()->getNewick();
}

void ParmTree::keepUpdate(void) {

	*trees[ flip(activeParm) ] = *trees[ activeParm ];
}

double ParmTree::lnPriorProb(void) {

	return trees[activeParm]->lnPriorProb();
}

void ParmTree::print(void) {

	return trees[activeParm]->print();
}

void ParmTree::restore(void) {

	*trees[activeParm] = *trees[ flip(activeParm) ];
}

double ParmTree::update(void) {

	return trees[activeParm]->update();
}



