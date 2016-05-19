/*
* BetaCome: betacome of future network routing
*
* Code to accompany the competition:
*   huawei software challenge : future network routing,
*   Yuanyuan Qin, Song Tang,2016
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
#ifndef _BETAOPTIMIZE_H
#define _BETAOPTIMIZE_H

#include "lib_time.h"
#include "graphVector.h"
#include "path.h"
#include "point.h"
#include "heap.h"
#include "assist.h"
#include "pathEdge.h"
#include <queue>
#include <iostream>
#include <unordered_set>
#include <set>

using namespace std;

#define SWITCHTIMESLIMITS 50
//switchAlgorithmKind
#define SWITCHNODEPIECE 11
#define SWITCHNODENODE 12

class BetaOptimize
{
public:
	BetaOptimize();

	//use node switch (same scc) to optimize
	int optimizeNodeSwitch(GraphVector &graph, Path &path);

	//optimize path
	int optimizePath(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);

	//use single source shortest path algorithm to optimize,but not change vprime order
	int optimizeSssp(){};//sssp:single source shortest path

private:
	int nodeFilter = INT_MAX;//ie the weight limit of node
	int pieceOrNodeFilter = INT_MAX;//ie the weight limit of edge
	int dfsFlag = 0;//flag when found path,flag=1
	int vPrimeColor[MAXVPRIME];//for func:findMaxNode,filter nodes have been visited
	int pieceColor[MAXVPRIME + 1];//for func:findMaxPieceOrSecNode,filter pieces have been visited
	int secondNodeColor[MAXVPRIME];//for func:findSecondNode,filter nodes have been visited
	int searchPieceColor[MAXPOINTNUM + 1];//for func:searchPathPiece, filter nodes have been searched in func before, add 1 for heap
	int convergenceTimes = 0;//times of convergence
	int switchAlgorithmKind = SWITCHNODENODE;
	int sAKArray[2];//switchAlgorithmKindsArray

	int findMaxNode(Path &path, int weightLimit);
	list<PathPiece>::iterator findMaxPieceOrSecNode(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit);
	list<PathPiece>::iterator findMaxPiece(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit);
	list<PathPiece>::iterator findSecondNode(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit);

	//check whether have common node of two pi[]
	bool haveCommonNode(int firstNode1, int lastNode1, int *pi1, int firstNode2, int lastNode2, int *pi2);
	//check whether have other path's edge
	bool piHaveOtherPathEdge(GraphVector &graph, int firstNode, int lastNode, int *pi, PathEdge &otherPathEdge);

	//firstNode and lastNode is for inPi, we reverse it to outPi
	inline void reversePi(int firstNode, int lastNode, int *piIn, int *piOut)
	{
		int uu;
		int vv;

		vv = lastNode;
		uu = piIn[vv];

		while (uu != firstNode)
		{
			piOut[uu] = vv;

			vv = uu;
			uu = piIn[vv];
		}

		piOut[uu] = vv;
	}

	//switch node,create new piece see wether we can reduce weight of path 
	//if can not,return 0
	int switchAlgorithm(GraphVector &graph, Path &path, int curMaxNodeId, list<PathPiece>::iterator iterPiece, int secMaxNode);
	int switchNodePiece(GraphVector &graph, Path &path, int curMaxNodeId, list<PathPiece>::iterator iterPiece);
	int switchNodeNode(GraphVector &graph, Path &path, int curMaxNodeId, int secMaxNode);

	int threeOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);
	int threeOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);
	int fourOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);
	int fourOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);
	int fiveOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);
	int fiveOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge);

	//elastic algorithm,use dijkstra to make path weight less
	int elasticOnePiece(GraphVector &graph, PathEdge &otherPathEdge, Path &path, int enableUsed);

	//set used property of piece's v points
	inline void setPieceNodeFalse(Path &path, list<PathPiece>::iterator iterPiece)
	{
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();
		for (auto iter = linkNodesVector.begin(); iter != linkNodesVector.end(); ++iter)
		{
			path.setUsed((*iter), false);
		}
	}
	inline void setPieceNodeTrue(Path &path, list<PathPiece>::iterator iterPiece)
	{
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();
		for (auto iter = linkNodesVector.begin(); iter != linkNodesVector.end(); ++iter)
		{
			path.setUsed((*iter), true);
		}
	}

	//different ways to find a path
	int searchPathPieceDfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi);
	int searchPathPieceBfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi);
	int dfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi, int *color);//three dfs not use same color,need change!!
	int bfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi);
	int searchPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int lastNode, int *pi, int enableUsed);
	int searchMultiPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int lastNode, int* commonColor, int *pi, int enableUsed);
	int searchMultiPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int *distance, int* commonColor, int *pi, int enableUsed);

	//将piece iter换成piL和piR记录的左右两个piece,新piece的中间点是middleNode
	int emplaceOne2Two(GraphVector &graph, Path &path, list<PathPiece>::iterator iter, int middleNode, int *piL, int *piR);
	//将iter和++iter两个piece，换成pi中记录的一个piece
	int emplaceTwo2One(GraphVector &graph, Path &path, list<PathPiece>::iterator iter1, int *pi);
	int emplaceDoubleTwo2Two(GraphVector &graph, Path &path, int leftNode, int rightNode, int *piLL, int *piLR, int *piRL, int *piRR);
	//将iter和++iter两个piece，换成piL和piR记录的左右两个piece,新piece的中间点是middleNode
	int emplaceTwo2Two(GraphVector &graph, Path &path, list<PathPiece>::iterator iter, int middleNode, int *piL, int *piR);
	int emplaceOne2One(GraphVector &graph, Path &path, list<PathPiece>::iterator iter, int *pi);//将某条pathpiece换成另外一条pathpiece

	//for time measure
	long long int startTimeSwitchAlgorithm = 0;
	long long int startTimeDfs = 0;

	//set points in SearchPieceColor black prevent of reusing these points
	inline void setSearchPieceColor(int firstNode, int lastNode, int *pi)
	{
		if (pi[lastNode] != lastNode)
		{
			int uu = pi[lastNode];
			int vv = lastNode;

			while (uu != firstNode)
			{
				searchPieceColor[uu] = BLACK;

				vv = uu;
				uu = pi[uu];
			}
		}
	}

	inline void setCommonColor(int firstNode, int lastNode, int *pi, int *commonColor)
	{
		if (pi[lastNode] != lastNode)
		{
			int uu = pi[lastNode];
			int vv = lastNode;

			while (uu != firstNode)
			{
				commonColor[uu] = BLACK;

				vv = uu;
				uu = pi[uu];
			}
		}
	}

	//cout time measure answer
	inline void printTimeMeasure(void)
	{
		cout << endl;
		cout << "switchnode timecount:" << timeMeasure.timeSwitchNode << " exe times:" << timeMeasure.exeTimesSwitchNode
			<< " average time:" << (timeMeasure.timeSwitchNode / timeMeasure.exeTimesSwitchNode) << endl;
		cout << "dfs timecount:" << timeMeasure.timeDfs << " exe times:" << timeMeasure.exeTimesDfs
			<< " average time:" << (timeMeasure.timeDfs / timeMeasure.exeTimesDfs) << endl;
	}

};


#endif

