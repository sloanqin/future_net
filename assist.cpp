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
#include "assist.h"
#include <time.h>
#include <sys/timeb.h>
#include "pathPiece.h"
#include <iostream>

using namespace std;

//check whether have solution
int judgeSolution(GraphVector &graph, Path &path, int vPTopoMax, int vPTopoMin)
{
	//if vstart or vend is invalid
	if ((graph.getNode(path.getVStart()).getPointId() == INVALID) || (graph.getNode(path.getVEnd()).getPointId() == INVALID))
	{
		return NOSOLUTION;
	}

	int topoStart = graph.getNode(path.getVStart()).getTopoOrder();
	int topoEnd = graph.getNode(path.getVEnd()).getTopoOrder();

	//if vprime is empty
	if (path.getVPrimeSize() == 0)
	{
		if (topoStart < topoEnd)
		{
			return NOSOLUTION;
		}
		else
		{
			return MAYBESOLUTION;
		}
		
	}

	//if have solution,vprime point should not be cutted
	if (path.getVPrimeSize() > path.getValidVPrimeNum())
	{
		return NOSOLUTION;
	}

	//if have solution ,topo order need meet: t <= min <= max <= s
	//actually,now i put deleteby topo before this func,so we only copy valid vprime
	//points to vprimevector,so the rest judge is absolutely true
	if (topoEnd <= vPTopoMin && vPTopoMin <= vPTopoMax && vPTopoMax <= topoStart)
	{
		return MAYBESOLUTION;
	}
	else
	{
		return NOSOLUTION;
	}
}

//根据topo顺序的范围限制，删除不满足path要求的点
int deleteNodesByTopo(GraphVector &graph, Path &path, int topoMax, int topoMin)
{
	//traverse every point of graph
	for (int i = 0; i <= MAXNODEID; ++i)
	{
		int j = graph.getNode(i).getTopoOrder();
		if (j>topoMax || j < topoMin)
		{
			path.setValidity(i, POINT_DELETE);//this flag means this point has been delete in path
		}
	}

	return EXIT_SUCCESS;
}

//return current time, in the unit of ms
long long int getTime()
{
	struct timeb rawtime;
	struct tm * timeinfo;
	ftime(&rawtime);
	timeinfo = localtime(&rawtime.time);

	static int ms = rawtime.millitm;
	static unsigned long s = rawtime.time;
	int out_ms = rawtime.millitm - ms;
	unsigned long out_s = rawtime.time - s;
	//ms = rawtime.millitm;
	//s = rawtime.time;

	if (out_ms < 0)
	{
		out_ms += 1000;
		out_s -= 1;
	}

	return (out_s * 1000 + out_ms);
}

//compute weight, accoording to common edge
//time complexity < O(40 * pieceNodesNum)
int computeVPrimeNodeWeight(GraphVector &graph, Path &path, int nodeId, PathEdge &otherPathEdge)
{
	list<PathPiece>::iterator iterPathPieceOne = path.getPathPieceOne(nodeId);
	list<PathPiece>::iterator iterPathPieceTwo = path.getPathPieceTwo(nodeId);

	int vPrimeNodeWeight = computePathPieceWeight(graph, path, iterPathPieceOne, otherPathEdge);
	vPrimeNodeWeight += computePathPieceWeight(graph, path, iterPathPieceTwo, otherPathEdge);

	return vPrimeNodeWeight;
}

//compute weight, accoording to common edge
//time complexity < O(20 * pieceNodesNum)
int computePathPieceWeight(GraphVector &graph, Path &path, list<PathPiece>::iterator iterPathPiece, PathEdge &otherPathEdge)
{
	int pathPieceWeight = 0;
	int nodeLeft = 0;
	int nodeRight = 0;

	PathPiece &pieceTemp = (*iterPathPiece);

	nodeLeft = (*iterPathPiece).getFirstNode();
	vector<int> &linkNodesVector = (*iterPathPiece).getLinkNodesVector();

	for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
	{
		nodeRight = (*iterLinkNodes);
		pathPieceWeight += computeDanymicEdgeWeight(graph, otherPathEdge, nodeLeft, nodeRight);//get danymic weight of (uu,vv)

		nodeLeft = nodeRight;
	}

	nodeRight = (*iterPathPiece).getLastNode();
	pathPieceWeight += computeDanymicEdgeWeight(graph, otherPathEdge, nodeLeft, nodeRight);

	return pathPieceWeight;
}

//according to whether other path has used, whether have two edge (uu,vv), return differ weight of (uu,vv)
int computeDanymicEdgeWeight(GraphVector &graph, PathEdge &otherPathEdge, int uu, int vv)
{
	//i think this func can be simplified

	Edge &edgeTemp = graph.getEdge(uu, vv);

	if (graph.getGraphKind() == GRAPHVECTOR)
	{
		if (otherPathEdge.hasEdge(uu, vv))
		{
			//if we have 2 edge of (uu,vv)
			if (edgeTemp.getEdgeId(1) != INVALID)
			{
				if (edgeTemp.getEdgeId(1) == otherPathEdge.getEdgeId(uu, vv))
				{
					return edgeTemp.getEdgeWeight(0);
				}
				else
				{
					return edgeTemp.getEdgeWeight(1);
				}
			}
			else
			{
				return (edgeTemp.getEdgeWeight(0) + MAXPATHORDINARYWEIGHT);
			}
		}
		else
		{
			return edgeTemp.getEdgeWeight(0);
		}
	}
	else if (graph.getGraphKind() == GRAPHVECTOR_REVERSE)
	{
		if (otherPathEdge.hasEdge(vv, uu))
		{
			//if we have 2 edge of (uu,vv)
			if (edgeTemp.getEdgeId(1) != INVALID)
			{
				if (edgeTemp.getEdgeId(1) == otherPathEdge.getEdgeId(vv, uu))
				{
					return edgeTemp.getEdgeWeight(0);
				}
				else
				{
					return edgeTemp.getEdgeWeight(1);
				}
			}
			else
			{
				return (edgeTemp.getEdgeWeight(0) + MAXPATHORDINARYWEIGHT);
			}
		}
		else
		{
			return edgeTemp.getEdgeWeight(0);
		}
	}
	
}

//output to buffer,for write to file
//output as huawei requires
int outputAns(Path &path, PathEdge &selfPathEdge)
{
	int nodeLeft = 0;
	int nodeRight = 0;
	list<PathPiece> &pathList = path.getPathList();
	auto iterPiece = pathList.begin();

	//if path is empty
	if (pathList.empty())
	{
		clear_result();//output NA
		return EXIT_SUCCESS;
	}

	for (iterPiece = pathList.begin(); iterPiece != (pathList.end()); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);
			record_result(path.getPathId(), selfPathEdge.getEdgeId(nodeLeft, nodeRight));//output to buffer

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();
		record_result(path.getPathId(), selfPathEdge.getEdgeId(nodeLeft, nodeRight));//output to buffer
	}

	return EXIT_SUCCESS;
}

//print to screen
//print answer as huawei requires
int printAns(Path &path, PathEdge &selfPathEdge)
{
	cout << "ans: ";
	int nodeLeft = 0;
	int nodeRight = 0;
	int weight = 0;
	list<PathPiece> &pathList = path.getPathList();
	auto iterPiece = pathList.begin();

	if (pathList.empty())
	{
		cout << "NA" << endl;
		return INVALID;
	}

	for (iterPiece = pathList.begin(); iterPiece != (--(pathList.end())); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);
			cout << selfPathEdge.getEdgeId(nodeLeft, nodeRight) << "|";
			weight += selfPathEdge.getEdgeWeight(nodeLeft, nodeRight);

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();
		cout << selfPathEdge.getEdgeId(nodeLeft, nodeRight) << "|";
		weight += selfPathEdge.getEdgeWeight(nodeLeft, nodeRight);
	}

	//为了最后一个不输出|，把最后一个循环单独拿出来了
	nodeLeft = (*iterPiece).getFirstNode();
	vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

	for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
	{
		nodeRight = (*iterLinkNodes);
		cout << selfPathEdge.getEdgeId(nodeLeft, nodeRight) << "|";
		weight += selfPathEdge.getEdgeWeight(nodeLeft, nodeRight);

		nodeLeft = nodeRight;
	}

	nodeRight = (*iterPiece).getLastNode();
	cout << selfPathEdge.getEdgeId(nodeLeft, nodeRight) << "|";
	weight += selfPathEdge.getEdgeWeight(nodeLeft, nodeRight);

	cout << "  weight: " << weight << endl;
	return weight;
}