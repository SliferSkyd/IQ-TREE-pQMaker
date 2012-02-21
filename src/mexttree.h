/***************************************************************************
 *   Copyright (C) 2006 by BUI Quang Minh, Steffen Klaere, Arndt von Haeseler   *
 *   minh.bui@univie.ac.at   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MEXTTREE_H
#define MEXTTREE_H

#include "mtree.h"
#include "mtreeset.h"

/**
extended tree, for bootstrap, cluster, etc (do not related to PDA main topic)

@author BUI Quang Minh, Steffen Klaere, Arndt von Haeseler
*/
class MExtTree : public MTree
{
public:

/********************************************************
	CONSTRUCTORs, INITIALIZATION AND DESTRUCTORs
********************************************************/

	/**
		constructor, read tree from user file
		@param userTreeFile the name of the user tree
		@param is_rooted (IN/OUT) true if tree is rooted
	*/
	MExtTree(const char *userTreeFile, bool &is_rooted) : MTree(userTreeFile, is_rooted) {};

	/**
		constructor, get from another tree
		@param tree another MTree
	*/
	MExtTree(MTree &tree) : MTree(tree) {};

	/**
		constructor
	*/
    MExtTree() : MTree() {};

/********************************************************
	READ TREE FROM FILE
********************************************************/

	/**
		read the tree in nodes.dmp file from NCBI taxonomy
		@param infile the input file file.
		@param is_rooted (IN/OUT) true if tree is rooted
	*/
	void readNCBITree(const char *infile, bool &is_rooted); 
	void readNCBITree(istream &in, bool &is_rooted); 


/********************************************************
	GENERATE RANDOM TREE PROCEDURES
********************************************************/

	/**
		generate a random tree with given tree type
		@param tree_type can be YULE_HARDING, UNIFORM, BALANCED, or CATERPILLAR
		@param params program parameters
		@param binary TRUE if you want to generate a binary tree
	*/
	void generateRandomTree(TreeGenType tree_type, Params &params, bool binary = true);

	/**
		generate a random tree following Yule-Harding model
		@param params program parameters
		@param binary TRUE if you want to generate a binary tree
	*/
	void generateYuleHarding(Params &params, bool binary = true);

	/**
		generate a random tree following uniform model
		@param size number of taxa
		@param binary TRUE if you want to generate a binary tree
	*/
	void generateUniform(int size, bool binary = true);

	/**
		generate a caterpillar tree
		@param size number of taxa
	*/
	void generateCaterpillar(int size);

	/**
		generate a balanced tree
		@param size number of taxa
	*/
	void generateBalanced(int size);

	/**
		set the leaf ID and names when generating random tree
		@param myleaves vector of leaves
	*/
	void setLeavesName(NodeVector &myleaves);


/********************************************************
	BOOTSTRAP
********************************************************/

	/**
		create support value for each internal node to the weight of split in the split graph
		@param node the starting node, NULL to start from the root
		@param dad dad of the node, used to direct the search
		@param sg split graph
		@param hash_ss hash split set
		@param taxname vector of taxa names
		@param trees set of trees
	*/
	void createBootstrapSupport(vector<string> &taxname, MTreeSet &trees, SplitGraph &sg, SplitIntMap &hash_ss, 
		Node *node = NULL, Node *dad = NULL);

	void reportDisagreedTrees(vector<string> &taxname, MTreeSet &trees, Split &mysplit);

/********************************************************
	CLUSTER for each branch, useful for likelihood mapping analysis
********************************************************/

	/**
		create CLUSTER for each branch, useful for likelihood mapping analysis
		@param taxa an order of taxa
		@param clusters (OUT) list of all clusters
		@param node the starting node, NULL to start from the root
		@param dad dad of the node, used to direct the search
	*/
	void createCluster(NodeVector &taxa, matrix(int) &clusters, Node *node = NULL, Node *dad = NULL);

	/**
		create CLUSTER for each branch, useful for likelihood mapping analysis
		@param clu_num cluster number
		@param node the starting node, NULL to start from the root
		@param dad dad of the node, used to direct the search
	*/
	void createCluster(int clu_num, Node *node, Node *dad);


};

#endif
