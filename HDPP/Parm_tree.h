#ifndef ParmTree_H
#define ParmTree_H

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "Parm.h"

class MbBitfield;
class MbRandom;
class MenuItem;
class Model;
class StateSets;
class Tree;

class Node {

	public:
                            Node(Tree* t);
						   ~Node(void);
					Node*   getAnc(void) { return anc; }
					  int   getFlag(void) { return flag; }
					  int   getIndex(void) { return index; }
					 bool   getIsLeaf(void) { return isLeaf; }
					Node*   getLft(void) { return lft; }
					 bool   getMarked(void) { return marked; }
			  std::string   getName(void) { return name; }
			       double   getP(void) { return proportion; }
					Node*   getRht(void) { return rht; }
			          int   getScratch(void) { return scratch; }
					 void   setAnc(Node *p) { anc = p; }
					 void   setFlag(int x) { flag = x; }
					 void   setIndex(int x) { index = x; }
					 void   setIsLeaf(bool tf) { isLeaf = tf; }
				     void   setLft(Node *p) { lft = p; }
					 void   setMarked(bool tf) { marked = tf; }
					 void   setName(std::string s) { name = s; }
					 void   setP(double x) { proportion = x; }
					 void   setRht(Node *p) { rht = p; }
					 void   setScratch(int x) { scratch = x; }

	private:
				     Node   *anc;
					  int   flag;
				      int   index;
					 bool   isLeaf;
				     Node   *lft;
					 bool   marked;
			  std::string   name;
				   double   proportion;
				     Node   *rht;
			          int   scratch;
					Tree*   treePtr;
};

class NodePair {

	public:
	                               NodePair(void);
                                   NodePair(Node* n1, Node* n2);
							       NodePair(const NodePair& a);
                            bool   operator==(const NodePair& a) const;
                            bool   operator<(const NodePair& a) const;
					       Node*   getNode1(void) const { return node1; }
					       Node*   getNode2(void) const { return node2; }
				            void   print(void);
							void   setNode1(Node* p) { node1 = p; }
							void   setNode2(Node* p) { node2 = p; }

	private:
					       Node*   node1;
					       Node*   node2;
};

class Tree {

	public:
                            Tree(MbRandom *rp, Model* mp, std::vector<std::string>& taxonNames, double lam);
							Tree(Tree& t);
						   ~Tree(void);
				     Tree   &operator=(const Tree& t);
					 void   addStateSetToTree(StateSets* s) { stateSets.push_back(s); }
					 void   clone(const Tree& t);
					 void   getDownPassSequence(Node* r, std::vector<Node*>& dps);
			  std::string   getNewick(void);
					  int   getNumNodes(void) { return numNodes; }
					  int   getNumAffectedNucleotideSites(void) { return numAffectedNucleotideSites; }
					Node*   getDownPassNode(int i) { return downPassSequence[i]; }
					Node*   getRoot(void) { return root; }
				   double   lnPriorProb(void);
					 void   print(void);
					 void   setNumAffectedNucleotideSites(int x) { numAffectedNucleotideSites = x; }
				   double   update(void);

	private:
	                 void   addNodeToBranch(Node* p, Node* des, Node* spare);
				     void   breakTreeAtRandomBranch(std::vector<Node*>& subTreeOne, std::vector<Node*>& subTreeTwo, Node* &rootOne, Node* &rootTwo, std::vector<Node*>& spareNodes, double& oldBrlenConnectionLengthOne, double& oldBrlenConnectionLengthTwo, NodePair& oldPair);
	                 void   buildRandomTree(std::vector<std::string>& taxonNames);
				   double   getTreeFraction(std::vector<Node*>& dps);
				     void   markBranchesDown(Node* p);
					  int   nodeIdx(Node* p);
					 void   normalizeProbs(std::map<NodePair,double>& lnProbs);
					 void   passDown(Node* p, std::vector<Node*>& dps);
					Node*   pickBranchAtRandom(void);
				 NodePair   pickNodePairAtRandom(std::map<NodePair,double>& probs);
				     void   printNodeInfo(std::string msg);
				     void   reconnectSubtrees(std::vector<Node*>& subTreeOne, std::vector<Node*>& subTreeTwo, Node* rootOne, Node* rootTwo, std::vector<Node*>& spareNodes, Node* p, Node* q, double& newBrlenConnectionLengthOne, double& newBrlenConnectionLengthTwo);
                     void   rerootOnNode(Node* p, Node* r);
					 void   setAllFlags(int x);
					 void   showNodes(std::vector<Node*>& dps, std::string treeTitle);
					 void   showNodes(Node* p, int indent, std::ofstream& o);
				   double   updateBrlen(void);
				   double   updateLocal(void);
				   double   updateTbr(void);
				     void   writeTree(Node* p, std::stringstream &ss);
				   double   alpha0;
	   std::vector<Node*>   nodes;
	   std::vector<Node*>   downPassSequence;
					Node*   root;
				   double   brlenLambda;
				   Model*   modelPtr;
					  int   numTaxa;
					  int   numNodes;
					  int   numAffectedNucleotideSites;
				MbRandom*   ranPtr;
  std::vector<StateSets*>   stateSets;
};

class ParmTree : public Parm {

	public:
                            ParmTree(Model* mp, MenuItem* ip, long int initSeed, std::string pn, std::vector<std::string>& taxonNames, double lam);
						   ~ParmTree(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
					Tree*   getActiveTree(void) { return trees[activeParm]; }
					Tree*   getTree(int i) { return trees[i]; }

	private:
	                 Tree   *trees[2];
};

#endif