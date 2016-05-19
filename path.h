/*
* BetaCome: betacome of future network routing
*
* Code to accompany the competition:
*   huawei software challenge : future network routing,
*   Yuanyuan Qin, Song Tang, Meiying Wu, 2016
*
* Copyright (C) 2016 Yuanyuan Qin, Peking University, Shenzhen, China
*
* This file is part of BetaCome.
*
* BetaCome is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* BetaCome is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with BetaCome.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef _PATH_H
#define _PATH_H

#include "environment.h"
#include "pathPiece.h"
#include "lib_record.h"
#include "graphVector.h"
#include <list>
#include <utility>
#include <vector>
#include <set>
#include <memory.h>

//falg that index whether we bringing cracked piece
#define HAVECRACK 2
#define NOCRACK 3
#define MAYBECRACK 4

//result of check used property
#define WRONG 2
#define RIGHT 3

using namespace std;

struct compareListPieceIter
{
	//func designed for using in set: means <
	//we think that different piece should have different firstnode, so we use this to compare
	bool operator ()(const list<PathPiece>::iterator &iter1, const list<PathPiece>::iterator &iter2)
	{
		return ((*iter1).getFirstNode() < (*iter2).getFirstNode());
	}
};

class Path
{
public:
	Path();

	Path(GraphVector &graph, char *demand, PATH_ID pathIdIn);

	~Path();//need to release dynamic memory

	Path& operator=(const Path &otherPath);

	pair<int, int> getPathWeight(GraphVector &graph);//get weight and num of pieces of path

	//add created pathpiece list to path
	int addPathPiece(GraphVector &graph, int firstNode, int secondNode);

	//bfs graph to find a path for two point
	int bfsPathPiece(GraphVector &graph, int firstNode, int secondNode, bool enableUsed);

	//set points used/pathpieceone/pathpiecetwo,create piecelist before iterIn
	//return the iter of first piece in our created piecelist
	list<PathPiece>::iterator createPieceList(GraphVector &graph, int firstNode, int secondNode,
		list<PathPiece>::iterator iterIn);
	list<PathPiece>::iterator createPieceList(GraphVector &graph, int firstNode, int secondNode,
		list<PathPiece>::iterator iterIn, int *piLocal);

	int checkPath(GraphVector &graph);//check correctness of path

	//pi is array saved the father of points in our searched path
	//this func is used when path is empty,we use this func to create a path by pi
	//int createPath(GraphVector &graph, ViolentRoute &violentRoute);

	//connect two nodes
	int connectTwoNodes(GraphVector &graph, int firstNode, int secondNode, int enableUsed);

	int crackOnePiece(GraphVector &graph, int nodeId);
	int crackTwoPiece(GraphVector &graph, int nodeId);

	//check used property
	int checkUsedProperty(GraphVector &graph);

	//compute num of piece need to be cracked in pi
	int cmNumToBeCracked(GraphVector &graph, int firstNode, int lastNode, int *piLocal);

	//bfs from point vStart,if we can not visit point xx,we delete xx
	int cutGraph(GraphVector &graph);

	//deal with used points and pathpiece associated with, restore:used/pathpieceone/pathpiecetwo 
	int dealUsedNodes(GraphVector &graph, int firstNode, int secondNode);
	int dealUsedNodes(GraphVector &graph, int firstNode, int secondNode, int *piLocal);

	//remove a vprime point in path, delete two piece,such as:a->b->c,we delete b
	//attention: after we use this func,we must add a piece to a->c, or the path will be wrong
	//return iter of piece that after which you deleted
	list<PathPiece>::iterator deleteOneVPrime(GraphVector &graph, int vPrimeId);

	//delete one piece, we should set points unused, then delete this piece,such as:m->a->b->n,we delete a->b
	//attention,after we use this func, we must add a piece of a~b, or the path will get wrong
	//return iter of piece that after which you deleted
	list<PathPiece>::iterator deleteOnePiece(GraphVector &graph, int firstNode){};
	list<PathPiece>::iterator deleteOnePiece(GraphVector &graph, list<PathPiece>::iterator iterPiece, int enableSetFalse);

	int getValidNodesNum();

	//insert a vprime point to a piece,such as a->b, we add to a->c->b
	//we should delete piece a->b,then add piece a->c->b
	int insertVPrimeToPiece(GraphVector &graph, list<PathPiece>::iterator iterPiece, int vPrimeId, int enableUsed);

	//insert vprime to approprite position
	int insertVPrimeToPath(GraphVector &graph, int vPrimeId, int enableUsed);

	//insert crack piece for two vprime points before iterIn,return iter of the inserted piece
	list<PathPiece>::iterator insertCrackPiece(GraphVector &graph, int firstNode, int secondNode,
		list<PathPiece>::iterator iterIn);

	//bfs graph to find a path for two point, we can use this func to find more than 1 path, prevent of using same points
	int multiBfsPathPiece(GraphVector &graph, int firstNode, int secondNode, int *commonColor, int *piLocal, bool enableUsed);

	//output to screen
	int printPath();//output vprime point in order
	int printNodeWeight();//output every vprime points' weight
	int printAllNodeInPath();//print all node in path

	//clear path,set empty
	int resetSelf();

	//set used property of piece's v points
	inline void setPieceNodeFalse(GraphVector &graph, const list<PathPiece>::iterator iterPiece)
	{
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();
		for (auto iter = linkNodesVector.begin(); iter != linkNodesVector.end(); ++iter)
		{
			setUsed((*iter), false);
		}
	}
	inline void setPieceNodeTrue(GraphVector &graph, const list<PathPiece>::iterator iterPiece)
	{
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();
		for (auto iter = linkNodesVector.begin(); iter != linkNodesVector.end(); ++iter)
		{
			setUsed((*iter), true);
		}
	}

	//set points in common color black prevent of reusing these points
	inline void setCommonColor(int firstNode, int lastNode, int *commonColor, int *piLocal)
	{
		if (piLocal[lastNode] != lastNode)
		{
			int uu = piLocal[lastNode];
			int vv = lastNode;

			while (uu != firstNode)
			{
				commonColor[uu] = BLACK;

				vv = uu;
				uu = piLocal[uu];
			}
		}
	}

	//save piece need to be cracked in vector,just save iter, not piece itself
	int savePiecesToBeCrackedInSet(GraphVector &graph, set<list<PathPiece>::iterator, compareListPieceIter> &iterCrackPieceSet,
		int firstNode, int lastNode, int *piLocal);

	//get value
	inline list<PathPiece>& getPathList() { return pathList; }
	inline int getLastNode(){ return (*(--(pathList.end()))).getLastNode(); }
	inline int **getCrackTimes() const { return crackTimes; }
	inline const int* getPropertyArray() const { return propertyArray; }
	inline const int* getValidityArray() const { return validityArray; }
	inline const int* getUsedArray() const { return usedArray; }
	inline int getProperty(int index) const { return propertyArray[index]; }
	inline int getValidity(int index) const{ return validityArray[index]; }
	inline int getUsed(int index) const { return usedArray[index]; }
	inline list<PathPiece>::iterator getPathPieceOne(int index) const { return pathPieceOne[index]; }
	inline list<PathPiece>::iterator getPathPieceTwo(int index) const { return pathPieceTwo[index]; }
	inline const int getVStart() const { return vStart; }
	inline const int getVEnd() const { return vEnd; }
	inline const int getVPrimeSize() const { return vPrimeSize; }
	inline const int getVPrime(int index) const { return vPrimeIndex[index]; }
	inline const PATH_ID getPathId() const { return pathId; }
	int getValidVPrimeNum();//compute number of valid vprime points,some vprime point may be setted POINT_INVALID

	//set value
	inline void setProperty(int index, int xx) { propertyArray[index] = xx; }
	inline void setValidity(int index, int xx) { validityArray[index] = xx; }
	inline void setUsed(int index, int xx) { usedArray[index] = xx; }
	inline void setPathPieceOne(int index, list<PathPiece>::iterator xx) { pathPieceOne[index] = xx; }
	inline void setPathPieceTwo(int index, list<PathPiece>::iterator xx) { pathPieceTwo[index] = xx; }
	inline void setAllNodesUnused(){ memset(usedArray, 0, sizeof(int)*MAXPOINTNUM); }

private:
	list<PathPiece> pathList;
	vector<list<PathPiece>::iterator> pathPieceOne;//路径片段1
	vector<list<PathPiece>::iterator> pathPieceTwo;//路径片段2，只有V’点才有片段2

	PATH_ID pathId;

	int pi[MAXPOINTNUM];//for search and can be used in many member func
	int distance[MAXPOINTNUM];//same as above
	
	//crackTimes[MAXPOINTNUM][MAXPOINTNUM],记录两个点之间的建立连接的不稳定性,由于比较大，采用danymic memory
	int **crackTimes;

	//for path
	int vStart = INVALID;
	int vEnd = INVALID;
	int vPrimeIndex[MAXVPRIME];//save the id of vprime point
	int vPrimeSize = INVALID;//if we have 3 v' points, this should be 3

	//point's property of path
	int propertyArray[MAXPOINTNUM];//path中点的属性:起点、终点、V’、 V 或者是没有这个点(无效)
	int validityArray[MAXPOINTNUM];//path中点的有效性:有效、无效（图中没有这个点）、被删除（针对当前path，该点被删除）
	int usedArray[MAXPOINTNUM];//当前path中，点是否已经被使用,false 或者 true
};

#endif
