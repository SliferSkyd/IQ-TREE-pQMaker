/*
 * candidateset.cpp
 *
 *  Created on: Jun 1, 2014
 *      Author: Tung Nguyen
 */

#include "phylotree.h"
#include "candidateset.h"

void CandidateSet::init(int maxCandidates, int maxPop, char* root, bool rooted, Alignment* aln) {
    assert(maxPop <= maxCandidates);
    this->maxCandidates = maxCandidates;
    this->popSize = maxPop;
    this->root = root;
    this->isRooted = rooted;
    this->aln = aln;
}

CandidateSet::~CandidateSet() {
}

CandidateSet::CandidateSet() {
	aln = NULL;
	root = NULL;
	maxCandidates = 0;
	popSize = 0;
	isRooted = false;
}

vector<string> CandidateSet::getEquallyOptimalTrees() {
	vector<string> res;
	double bestScore = rbegin()->first;
	for (reverse_iterator rit = rbegin(); rit != rend() && rit->second.score == bestScore; rit++) {
		res.push_back(rit->second.tree);
	}
	return res;
}

string CandidateSet::getRandCandTree() {
	assert(!empty());
	if (empty())
		return "";
	int id = random_int(min(popSize, (int)size()) );
	for (reverse_iterator i = rbegin(); i != rend(); i++, id--)
		if (id == 0)
			return i->second.tree;
	assert(0);
	return "";
}

vector<string> CandidateSet::getHighestScoringTrees(int numTree) {
	assert(numTree <= maxCandidates);
	if (numTree == 0) {
		numTree = maxCandidates;
	}
	vector<string> res;
	int cnt = numTree;
	for (reverse_iterator rit = rbegin(); rit != rend() && cnt > 0; rit++, cnt--) {
		res.push_back(rit->second.tree);
	}
	return res;
}

vector<string> CandidateSet::getBestLOTrees(int numTree) {
	assert(numTree <= maxCandidates);
	if (numTree == 0) {
		numTree = maxCandidates;
	}
	vector<string> res;
	int cnt = numTree;
	for (reverse_iterator rit = rbegin(); rit != rend() && cnt > 0; rit++) {
		if (rit->second.localOpt) {
			res.push_back(rit->second.tree);
			cnt--;
		}
	}
	return res;
}

bool CandidateSet::replaceTree(string tree, double score) {
    CandidateTree candidate;
    candidate.tree = tree;
    candidate.score = score;
    candidate.topology = getTopology(tree);
    if (treeTopologyExist(candidate.topology)) {
        topologies[candidate.topology] = score;
        for (reverse_iterator i = rbegin(); i != rend(); i++) {
            if (i->second.topology == candidate.topology) {
                erase( --(i.base()) );
                break;
            }
            insert(CandidateSet::value_type(score, candidate));
        }
    } else {
        return false;
    }
    return true;
}

string CandidateSet::getNextCandTree() {
    string tree;
    assert(!empty());
    if (parentTrees.empty()) {
        initParentTrees();
    }
    tree = parentTrees.top();
    parentTrees.pop();
    return tree;
}

void CandidateSet::initParentTrees() {
    if (parentTrees.empty()) {
        int count = this->popSize;
        for (reverse_iterator i = rbegin(); i != rend() && count >0 ; i++, count--) {
            parentTrees.push(i->second.tree);
            //cout << i->first << endl;
        }
    }
}

bool CandidateSet::update(string tree, double score, bool localOpt) {
	bool newTree = true;
	CandidateTree candidate;
	candidate.score = score;
	candidate.topology = getTopology(tree);
	candidate.localOpt = localOpt;
	candidate.tree = tree;

	if (treeTopologyExist(candidate.topology)) {
		newTree = false;
	    /* If tree topology already exist but the score is better, we replace the old one
	    by the new one (with new branch lengths) and update the score */
		if (topologies[candidate.topology] < score) {
			removeCandidateTree(candidate.topology);
			topologies[candidate.topology] = score;
			// insert tree into candidate set
			insert(CandidateSet::value_type(score, candidate));
		} else if (candidate.localOpt) {
			CandidateSet::iterator treePtr = getCandidateTree(candidate.topology);
			treePtr->second.localOpt = candidate.localOpt;
		}
	} else {
		if (size() < maxCandidates) {
			// insert tree into candidate set
			insert(CandidateSet::value_type(score, candidate));
			topologies[candidate.topology] = score;
		} else if (getWorstScore() < score){
			// remove the worst-scoring tree
			topologies.erase(begin()->second.topology);
			erase(begin());
			// insert tree into candidate set
			insert(CandidateSet::value_type(score, candidate));
			topologies[candidate.topology] = score;
		}
	}
	return newTree;
}

vector<double> CandidateSet::getBestScores(int numBestScore) {
	if (numBestScore == 0)
		numBestScore = size();
	vector<double> res;
	for (reverse_iterator rit = rbegin(); rit != rend() && numBestScore > 0; rit++, numBestScore--) {
		res.push_back(rit->first);
	}
	return res;
}

double CandidateSet::getBestScore() {
	if (size() == 0)
		return -DBL_MAX;
	else
		return rbegin()->first;
}

double CandidateSet::getWorstScore() {
	return begin()->first;
}

string CandidateSet::getTopology(string tree) {
	PhyloTree mtree;
	mtree.rooted = false;
	mtree.aln = aln;
	mtree.readTreeString(tree);
    mtree.root = mtree.findNodeName(aln->getSeqName(0));
	ostringstream ostr;
	mtree.printTree(ostr, WT_TAXON_ID | WT_SORT_TAXA);
	return ostr.str();
}

double CandidateSet::getTopologyScore(string topology) {
	assert(topologies.find(topology) != topologies.end());
	return topologies[topology];
}

void CandidateSet::clear() {
	multimap<double, CandidateTree>::clear();
	clearTopologies();
}

void CandidateSet::clearTopologies() {
	topologies.clear();
}


CandidateSet CandidateSet::getBestCandidateTrees(int numTrees) {
	CandidateSet res;
	if (numTrees >= size() || numTrees == 0)
		numTrees = size();
	for (reverse_iterator rit = rbegin(); rit != rend() && numTrees > 0; rit++, numTrees--) {
		res.insert(*rit);
	}
	return res;
}

bool CandidateSet::treeTopologyExist(string topo) {
	return (topologies.find(topo) != topologies.end());
}

bool CandidateSet::treeExist(string tree) {
	return treeTopologyExist(getTopology(tree));
}

CandidateSet::iterator CandidateSet::getCandidateTree(string topology) {
	for (CandidateSet::reverse_iterator rit = rbegin(); rit != rend(); rit++) {
		if (rit->second.topology == topology)
			return --(rit.base());
	}
	return end();
}

void CandidateSet::removeCandidateTree(string topology) {
	bool removed = false;
	for (CandidateSet::reverse_iterator rit = rbegin(); rit != rend(); rit++) {
			if (rit->second.topology == topology) {
				erase( --(rit.base()) );
				topologies.erase(topology);
				removed = true;
				break;
			}
	}
	assert(removed);
}

bool CandidateSet::isStableSplit(Split& sp) {
	return stableSplit.containSplit(sp);
}

int CandidateSet::computeSplitSupport(int numTree) {
	stableSplit.clear();
	if (numTree == 0)
		numTree = getNumLocalOptTrees();
	SplitIntMap hash_ss;
	SplitGraph sg;
	MTreeSet boot_trees;
	int numMaxSupport = 0;
	vector<string> trees = getBestLOTrees(numTree);
	assert(trees.size() > 1);
	int maxSupport = trees.size();
	boot_trees.init(trees, aln->getSeqNames(), isRooted);
	boot_trees.convertSplits(aln->getSeqNames(), sg, hash_ss, SW_COUNT, -1, false);

	for (unordered_map<Split*,int>::iterator it = hash_ss.begin(); it != hash_ss.end(); it++) {
		if (it->second == maxSupport && it->first->countTaxa() > 1) {
			numMaxSupport++;
			Split* supportedSplit = new Split(*(it->first));
			stableSplit.push_back(supportedSplit);
		}
	}
	cout << "Number of supported splits = " << numMaxSupport << endl;
	return numMaxSupport;
}

void CandidateSet::setAln(Alignment* aln) {
	this->aln = aln;
}

int CandidateSet::getMaxCandidates() const {
	return maxCandidates;
}

void CandidateSet::setMaxCandidates(int maxCandidates) {
	this->maxCandidates = maxCandidates;
}

int CandidateSet::getPopSize() const {
	return popSize;
}

void CandidateSet::setPopSize(int popSize) {
	this->popSize = popSize;
}

void CandidateSet::setIsRooted(bool isRooted) {
	this->isRooted = isRooted;
}

int CandidateSet::getNumLocalOptTrees() {
	int numLocalOptima = 0;
	for (reverse_iterator rit = rbegin(); rit != rend(); rit++) {
		if (rit->second.localOpt) {
			numLocalOptima++;
		}
	}
	return numLocalOptima;
}
