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
#include "graphVector.h"
#include "assist.h"
#include <queue>
#include <cassert>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace std;

int MAXNODEID = 0;
int TIMELIMITNUM = 0;

//using topo and demand to initialize graph
GraphVector::GraphVector(char *topo[], int edge_num) :GraphBase()
{
	graphKind = GRAPHVECTOR;

	int i = 0;
	int edgeId = 0;
	int uu = 0;
	int vv = 0;
	int edgeWeight = 0;
	char *pLine;
	int maxNodeIdOld = 0;
	unordered_set<int> nodeIdSet;

	//input edge and node of graph
	while (i<edge_num)
	{
		//change ',' to ' ' then we can use cin to get num 
		pLine = topo[i];
		while (*pLine != '\0')
		{
			if (*pLine == ',') *pLine = ' ';
			pLine++;
		}

		string strLine(topo[i]);

		istringstream istr(strLine);
		i++;
		if (!(istr >> edgeId >> uu >> vv >> edgeWeight))//get in num
		{
			continue;
		}

		nodeIdSet.insert(uu);
		nodeIdSet.insert(vv);

		MAXNODEID = assistMax(MAXNODEID, uu);
		MAXNODEID = assistMax(MAXNODEID, vv);

		if (MAXNODEID > maxNodeIdOld)
		{
			graphNode.resize(MAXNODEID + 1);

			maxNodeIdOld = MAXNODEID;
		}

		insertEdge(edgeId, uu, vv, edgeWeight);//insert edge
#ifdef DEBUG
		cout << edgeId << " " << uu << " " << vv << " " << edgeWeight << " " << endl;
#endif

	}

	nodeNum = nodeIdSet.size();

	TIMELIMITNUM = assistMin(((nodeNum / 100 + 1) * 1000), 9000);
}

GraphVector::GraphVector(char *topo[], int edge_num, bool reverse)
{
	if (reverse)
	{
		graphKind = GRAPHVECTOR_REVERSE;

		int i = 0;
		int edgeId = 0;
		int uu = 0;
		int vv = 0;
		int edgeWeight = 0;
		char *pLine;
		int maxNodeIdOld = 0;
		unordered_set<int> nodeIdSet;

		//input edge and node of graph
		while (i<edge_num)
		{
			//change ',' to ' ' then we can use cin to get num 
			pLine = topo[i];
			while (*pLine != '\0')
			{
				if (*pLine == ',') *pLine = ' ';
				pLine++;
			}

			string strLine(topo[i]);

			istringstream istr(strLine);
			i++;
			if (!(istr >> edgeId >> vv >> uu >> edgeWeight))//get in num
			{
				//here, we reversed uu and vv
				continue;
			}

			nodeIdSet.insert(uu);
			nodeIdSet.insert(vv);

			MAXNODEID = assistMax(MAXNODEID, uu);
			MAXNODEID = assistMax(MAXNODEID, vv);

			if (MAXNODEID > maxNodeIdOld)
			{
				graphNode.resize(MAXNODEID + 1);

				maxNodeIdOld = MAXNODEID;
			}

			insertEdge(edgeId, uu, vv, edgeWeight);//insert edge
#ifdef DEBUG
			cout << edgeId << " " << uu << " " << vv << " " << edgeWeight << " " << endl;
#endif

		}

		nodeNum = nodeIdSet.size();
	}

}
//reverse edge of srcGraph,get a new dstGraph
//just copy valid point, point invalid and delete, do not copy
//int GraphVector::reverseGraph(GraphBase &dstGraph)
//{
//	int dstGraphNodeIndex = 0;
//
//	//copy graph information
//	dstGraph.setGraphKind(graphKind);
//
//	for (auto iterPoint = graphNode.begin(); iterPoint != graphNode.end(); ++iterPoint)
//	{
//		Point &pointTemp = (*iterPoint);
//
//		//if in srcgraph, this point is valid
//		if (pointTemp.getPointId() != POINT_INVALID)
//		{
//			//copy point
//			Point &dstPointTemp = dstGraph.getNode(dstGraphNodeIndex);
//			dstPointTemp.setInoutDegree(pointTemp.getInoutDegree());
//			dstPointTemp.setPointId(pointTemp.getPointId());
//			dstPointTemp.setSccNum(pointTemp.getSccNum());
//			dstPointTemp.setTopoOrder(pointTemp.getTopoOrder());
//
//			//insert reverse edge
//			vector<Edge> &srcEdgeVectorTemp = pointTemp.getEdgeVector();
//			for (auto iterEdgeTemp = srcEdgeVectorTemp.begin(); iterEdgeTemp != srcEdgeVectorTemp.end(); ++iterEdgeTemp)
//			{
//				Edge &edgeTemp = (*iterEdgeTemp);
//
//				dstGraph.insertEdge(edgeTemp.getEdgeId(), edgeTemp.getNodeTwo(), edgeTemp.getNodeOne(), edgeTemp.getEdgeWeight());
//			}
//
//		}
//
//		dstGraphNodeIndex++;
//	}
//
//	return EXIT_SUCCESS;
//}

Edge& GraphVector::getEdge(int uu, int vv)//返回某条边
{
	static Edge xx;
	xx.setEdgeWeight(0, INFINITEWEIGHT);

	Point &node = getNode(uu);
	if (checkNode(uu) == INVALID)//point uu not exist
	{
		return xx;//invalid edge
	}
	else//point uu exist
	{
		vector<Edge> &edgeVector = node.getEdgeVector();
		char flag = 0;
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)
		{
			if (((*iter).getNodeTwo() == vv))//edge (uu,vv) have existed
			{
				return (*iter);
			}
		}

		//edge (uu,vv) not exist
		{
			return xx;//invalid edge
		}
	}
}

int GraphVector::insertEdge(int edgeId, int uu, int vv, int edgeWeight)//time complexity: O(8)
{
	Point &node = getNode(uu);
	if ((checkNode(uu) == INVALID) || (node.getEdgeVector().empty()))//point uu not exist or don't have edge out
	{
		node.setPointId(uu);//设置点的属性

		//建立新边，设置新边的属性
		Edge edge;
		edge.setEdgeId(0, edgeId);
		edge.setNodeOne(uu);
		edge.setNodeTwo(vv);
		edge.setEdgeWeight(0, edgeWeight);

		node.insertEdge(edge);//insert new edge at the end of edgevector
	}
	else//point uu exist
	{
#if 1
		//save edge without repeated (uu,vv)
		char flag = 0;
		vector<Edge> &edgeVector = node.getEdgeVector();

		//search, see whether edge (uu,vv) exists
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)
		{
			//edge (uu,vv) have existed
			if (((*iter).getNodeTwo() == vv))
			{
				//if have existed 2 edge (uu,vv)
				if ((*iter).getEdgeId(1) != INVALID)
				{
					//if edge weight is smaller, replace
					if (edgeWeight < ((*iter).getEdgeWeight(0)))
					{
						(*iter).setEdgeWeight(0, edgeWeight);
						(*iter).setEdgeId(0, edgeId);
					}
					else if (edgeWeight < ((*iter).getEdgeWeight(1)))
					{
						(*iter).setEdgeWeight(1, edgeWeight);
						(*iter).setEdgeId(1, edgeId);
					}
				}
				else//only existed 1 edge (uu,vv),insert to edge
				{
					(*iter).setEdgeWeight(1, edgeWeight);
					(*iter).setEdgeId(1, edgeId);
				}
	
				flag = 1;
				break;
			}
		}

		if (!flag)//edge (uu,vv) not exist
		{
			Edge edge;
			edge.setEdgeId(0,edgeId);
			edge.setNodeOne(uu);
			edge.setNodeTwo(vv);
			edge.setEdgeWeight(0,edgeWeight);

			node.insertEdge(edge);//insert new edge at the end of edgevector
		}
#else
		//save edge with repeated (uu,vv)
		Edge edge;
		edge.setEdgeId(edgeId);
		edge.setNodeOne(uu);
		edge.setNodeTwo(vv);
		edge.setEdgeWeight(edgeWeight);

		node.insertEdge(edge);//insert new edge at the end of edgevector
#endif

	}

	if (checkNode(vv) == INVALID)//point vv not exist
	{
		Point &nodeVv = getNode(vv);

		//设置点的属性
		nodeVv.setPointId(vv);
	}

	return EXIT_SUCCESS;
}

//检查node是否存在
inline int GraphVector::checkNode(int uu)
{
	if (getNode(uu).getPointId() == POINT_INVALID) return INVALID;
	else return VALID;
}

// time complexity : O(V + E)，so : O(4800)
int GraphVector::bfs(int firstNode, int *color)
{
	queue<int> pointQueue;

	//initial start point
	color[firstNode] = GRAY;

	//push first point to queue
	pointQueue.push(firstNode);

	while (!pointQueue.empty())//time complexity:O(V+E), actually less than this
	{
		int uu = pointQueue.front();
		pointQueue.pop();

		vector<Edge> &edgeVector = getNode(uu).getEdgeVector();
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)//traverse every edge of point uu
		{
			int vv = (*iter).getNodeTwo();
			Point &pointTemp = getNode(vv);

			//(not valid) || (has visited)
			if ((color[vv] != WHITE) || (pointTemp.getPointId() == POINT_INVALID)) continue;

			//the rest can satisfy needs
			color[vv] = GRAY;
			pointQueue.push(vv);
		}

		color[uu] = BLACK;
	}

	return EXIT_SUCCESS;
}

int GraphVector::refreshInoutDegree()//刷新每个有效点的出入度属性,time complexity:O(V+E)
{
	int degreeCnt[MAXPOINTNUM];

	//initial memory
	memset(degreeCnt, 0, MAXPOINTNUM*sizeof(int));

	//traverse every edge of graph
	for (int i = 0; i <= MAXNODEID; ++i)
	{
		vector<Edge> &edgeVector = getNode(i).getEdgeVector();
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)
		{
			degreeCnt[(*iter).getNodeOne()]++;
			degreeCnt[(*iter).getNodeTwo()]++;
		}
	}

	//traverse every node of graph,set inoutdegree
	for (int i = 0; i <= MAXNODEID; ++i)
	{
		getNode(i).setInoutDegree(degreeCnt[i]);
	}

	return EXIT_SUCCESS;
}
