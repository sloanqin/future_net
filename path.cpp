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

#include "path.h"
#include "assist.h"
#include "lib_record.h"
#include "pathEdge.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <cassert>
#include <memory.h>

using namespace std;

Path::Path(GraphVector &graph, char *demand, PATH_ID pathIdIn)
{
	pathId = pathIdIn;

	list<PathPiece>::iterator xx;
	pathPieceOne.resize(MAXNODEID + 1 , xx);//resize
	pathPieceTwo.resize(MAXNODEID + 1, xx);

	memset(pi, 0, MAXPOINTNUM*sizeof(int));
	memset(distance, 0, MAXPOINTNUM*sizeof(int));

	crackTimes = new int*[MAXPOINTNUM];
	for (int i = 0; i < MAXPOINTNUM; i++)
	{
		crackTimes[i] = new int[MAXPOINTNUM];
		memset(crackTimes[i], 0, MAXPOINTNUM*sizeof(int));
	}

	memset(propertyArray, 0xff, sizeof(int)*MAXPOINTNUM);//initial to be invalid
	memset(validityArray, 0xff, sizeof(int)*MAXPOINTNUM);
	memset(usedArray, 0, sizeof(int)*MAXPOINTNUM);

	//get information from graph, set array
	auto iterGraphNode = graph.getGraphNode().begin();
	auto iterGraphNodeEnd = graph.getGraphNode().end();
	for (; iterGraphNode != iterGraphNodeEnd; ++iterGraphNode)
	{
		int pointId = (*iterGraphNode).getPointId();
		if (pointId != POINT_INVALID)
		{
			propertyArray[pointId] = POINT_V;
			validityArray[pointId] = POINT_VALID;
		}
	}

	//set demand:start end vPrimeIndex vPrimeSize
	string strLine(demand);
	string::size_type j = 0;
	for (; j != strLine.size(); ++j)//change character to space
	{
		if (isdigit(strLine[j])) continue;//if is digit continue
		else strLine[j] = ' ';
	}
	istringstream istr(strLine);
	istr >> vStart;//first num is pathid
	istr >> vStart >> vEnd;
	vPrimeSize = 0;
	while (istr >> vPrimeIndex[vPrimeSize])//input vPrime point
	{
		vPrimeSize++;
	}

	//according to demand,set property of vprime, start and end point 
	propertyArray[vStart] = POINT_START;
	propertyArray[vEnd] = POINT_END;
	for (int i = 0; i < vPrimeSize; i++)
	{
		propertyArray[vPrimeIndex[i]] = POINT_VPRIME;
	}
}

Path::~Path()
{
	for (int i = 0; i < MAXPOINTNUM; i++)
	{
		delete[] crackTimes[i];
	}
	delete[] crackTimes;
}

Path::Path()
{
	crackTimes = new int*[MAXPOINTNUM];
	for (int i = 0; i < MAXPOINTNUM; i++)
	{
		crackTimes[i] = new int[MAXPOINTNUM];
	}
}

Path& Path::operator=(const Path &otherPath)
{
	pathList = otherPath.pathList;
	pathPieceOne = otherPath.pathPieceOne;//路径片段1
	pathPieceTwo = otherPath.pathPieceTwo;//路径片段2，只有V’点才有片段2

	pathId = otherPath.pathId;

	memcpy(pi, otherPath.pi, MAXPOINTNUM*sizeof(int));
	memcpy(distance, otherPath.distance, MAXPOINTNUM*sizeof(int));

	//attention, do not copy this: int **crackTimes;

	//for path
	vStart = otherPath.vStart;
	vEnd = otherPath.vEnd;
	memcpy(vPrimeIndex, otherPath.vPrimeIndex, MAXVPRIME*sizeof(int));
	vPrimeSize = otherPath.vPrimeSize;//if we have 3 v' points, this should be 3

	//point's property of path
	memcpy(propertyArray, otherPath.propertyArray, MAXPOINTNUM*sizeof(int));
	memcpy(validityArray, otherPath.validityArray, MAXPOINTNUM*sizeof(int));
	memcpy(usedArray, otherPath.usedArray, MAXPOINTNUM*sizeof(int));

	return *this;
}

//delete one piece, we set points unused, then delete this piece,such as:m->a->b->n,we delete a->b
//attention: after we use this func, we must add a piece of a~b, or the path will get wrong
//return iter of piece that after which you deleted,support for continuous deletion
//attention: this may delete VStart and VEnd, so be carefully
list<PathPiece>::iterator Path::deleteOnePiece(GraphVector &graph, list<PathPiece>::iterator iterPiece, int enableSetFalse)
{
	//set points unused,just set ordinary, do not set endpoint ie vprime 
	if (enableSetFalse) setPieceNodeFalse(graph, iterPiece);

	//check vprime before and after the deleted piece,if it is not continuous,set vprime unused
	list<PathPiece>::iterator iterTemp;

	iterTemp = iterPiece;
	if (iterPiece == pathList.begin())
	{
		//iterTemp is the first piece
		setUsed((*iterPiece).getFirstNode(), false);
	}
	else
	{
		--iterTemp;
		if ((*iterTemp).getLastNode() != (*iterPiece).getFirstNode()) setUsed((*iterPiece).getFirstNode(), false);
	}

	iterTemp = iterPiece;
	if (iterPiece == (--(pathList.end())))
	{
		//iterTemp is the last piece
		setUsed((*iterPiece).getLastNode(), false);
	}
	else
	{
		++iterTemp;
		if ((*iterTemp).getFirstNode() != (*iterPiece).getLastNode()) setUsed((*iterPiece).getLastNode(), false);
	}

	//delete piece
	return pathList.erase(iterPiece);
}

//this func use edge to compute weight
//pair<int, int>  Path::getPathWeight(GraphVector &graph)
//{
//	int nodeLeft = 0;
//	int nodeRight = 0;
//	int weight = 0;
//	auto iterPiece = pathList.begin();
//
//	for (iterPiece = pathList.begin(); iterPiece != pathList.end(); ++iterPiece)
//	{
//		nodeLeft = (*iterPiece).getFirstNode();
//		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();
//
//		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
//		{
//			nodeRight = (*iterLinkNodes);
//			weight += graph.getEdge(nodeLeft, nodeRight).getEdgeWeight();
//
//			nodeLeft = nodeRight;
//		}
//
//		nodeRight = (*iterPiece).getLastNode();
//		weight += graph.getEdge(nodeLeft, nodeRight).getEdgeWeight();
//	}
//	return weight;
//}

//return a pair<int,int>, pair.first=weight  pair.second=pieceNum
pair<int, int>  Path::getPathWeight(GraphVector &graph)
{
	int weight = 0;
	int pieceNum = 0;

	for (auto iterPiece = pathList.begin(); iterPiece != pathList.end(); ++iterPiece)
	{
		weight += (*iterPiece).getWeight();

		pieceNum++;
	}

	return pair<int, int>(weight, pieceNum);
}

//insert crack piece for two vprime points before iterIn,return iter of the inserted piece
list<PathPiece>::iterator Path::insertCrackPiece(GraphVector &graph, int firstNode, int secondNode,
	list<PathPiece>::iterator iterIn)
{
	int curFirstNode = secondNode;
	int curLastNode = secondNode;

	PathPiece pieceTemp;
	list<PathPiece>::iterator iter;

	iter = pathList.insert(iterIn, pieceTemp);

	//set points properties
	setUsed(firstNode, true);
	setPathPieceTwo(firstNode, iter);

	setUsed(secondNode, true);
	setPathPieceOne(secondNode, iter);

	//set piece properties
	(*iter).setFirstNode(firstNode);
	(*iter).setLastNode(secondNode);
	(*iter).setLinkProperty(CRACKED);
	(*iter).setWeight(INFINITEWEIGHT);

	return iter;
}

//check all pieces in path, call func insertVPrimeToPiece
//insert one vPrime to one piece
int Path::insertVPrimeToPath(GraphVector &graph, int vPrimeId, int enableUsed)
{
	int firstNode = 0;
	int lastNode = 0;
	int firstNodeSccNum = 0;
	int lastNodeSccNum = 0;
	int curVPrimeSccNum = graph.getNode(vPrimeId).getSccNum();
	int ansInsertVPrimeToPiece = 0;

	//for loop,check pieces satisfied scc needs
	for (auto iterTemp = pathList.begin(); iterTemp != pathList.end(); ++iterTemp)
	{
		firstNode = (*iterTemp).getFirstNode();
		lastNode = (*iterTemp).getLastNode();
		firstNodeSccNum = graph.getNode(firstNode).getSccNum();
		lastNodeSccNum = graph.getNode(lastNode).getSccNum();

		//if sccnum do not satisfied needs
		if (lastNodeSccNum > curVPrimeSccNum) continue;
		if (firstNodeSccNum < curVPrimeSccNum) break;//have passed the range of curVPrimeSccNum

		//the rest are ok
		ansInsertVPrimeToPiece = insertVPrimeToPiece(graph, iterTemp, vPrimeId, enableUsed);

		//check result
		if (ansInsertVPrimeToPiece == ALGORITHMFAILURE) continue;
		else return ALGORITHMSUCCESS;
	}

	//if can not insert to all pieces,return failure
	return ALGORITHMFAILURE;
}

//insert a vprime point to a piece
//such as we have a piece a->b, now we want a->c->b
int Path::insertVPrimeToPiece(GraphVector &graph, list<PathPiece>::iterator iterPiece, int vPrimeId, int enableUsed)
{
	int piAC[MAXPOINTNUM];
	int piCB[MAXPOINTNUM];
	int commonColor[MAXPOINTNUM];

	memset(piAC, 0, sizeof(int)*MAXPOINTNUM);
	memset(piCB, 0, sizeof(int)*MAXPOINTNUM);
	memset(commonColor, 0, sizeof(int)*MAXPOINTNUM);

	int nodeA = (*iterPiece).getFirstNode();
	int nodeB = (*iterPiece).getLastNode();
	int nodeC = vPrimeId;

	int resultMultiBfs = 0;
	list<PathPiece>::iterator iterTemp;
	
	if (enableUsed == false)
	{
		//set points in a->b unused
		setPieceNodeFalse(graph, iterPiece);

		//two bfs,multi-bfs,need common color array
		resultMultiBfs = multiBfsPathPiece(graph, nodeA, nodeC, commonColor, piAC, false);

		if (resultMultiBfs == FOUND)
		{
			setCommonColor(nodeA, nodeC, commonColor, piAC);
			resultMultiBfs = multiBfsPathPiece(graph, nodeC, nodeB, commonColor, piCB, false);

			if (resultMultiBfs == FOUND)//if can find, delete piece a-b,add a->c->b
			{
				//delete old piece
				iterTemp = deleteOnePiece(graph, iterPiece, true);

				//insert 2 new pieces,insert c->b first, then a->c
				iterTemp = createPieceList(graph, nodeC, nodeB, iterTemp, piCB);
				iterTemp = createPieceList(graph, nodeA, nodeC, iterTemp, piAC);

				return ALGORITHMSUCCESS;
			}
			else//if can not, return ALGORITHMFAILURE
			{
				//set points in a->b used
				setPieceNodeTrue(graph, iterPiece);

				return ALGORITHMFAILURE;
			}
		}
		else
		{
			//set points in a->b used
			setPieceNodeTrue(graph, iterPiece);

			return ALGORITHMFAILURE;
		}

	}
	else//can use used points
	{
		//set points in a->b unused
		setPieceNodeFalse(graph, iterPiece);

		//two bfs,multi-bfs,need common color array
		resultMultiBfs = multiBfsPathPiece(graph, nodeA, nodeC, commonColor, piAC, true);

		if (resultMultiBfs == FOUND)
		{
			setCommonColor(nodeA, nodeC, commonColor, piAC);
			resultMultiBfs = multiBfsPathPiece(graph, nodeC, nodeB, commonColor, piCB, true);

			if (resultMultiBfs == FOUND)//if can find, deal with used points, delete piece a-b,add a->c->b
			{
				//deal with used piece
				dealUsedNodes(graph, nodeC, nodeB, piCB);
				dealUsedNodes(graph, nodeA, nodeC, piAC);

				//delete old piece
				iterTemp = deleteOnePiece(graph, iterPiece, true);

				//insert 2 new pieces,insert c->b first, then a->c
				iterTemp = createPieceList(graph, nodeC, nodeB, iterTemp, piCB);
				iterTemp = createPieceList(graph, nodeA, nodeC, iterTemp, piAC);

				return ALGORITHMSUCCESS;

			}
			else
			{
				//set points in a->b used
				setPieceNodeTrue(graph, iterPiece);

				return ALGORITHMFAILURE;
			}
		}
		else
		{
			//set points in a->b used
			setPieceNodeTrue(graph, iterPiece);

			return ALGORITHMFAILURE;
		}

	}

	return ALGORITHMFAILURE;
}

//connect two nodes
//when enableUsed == false,we only use unused points to find a path
//when enableUsed == true, we ensure to find a path 
int Path::connectTwoNodes(GraphVector &graph, int firstNode, int secondNode, int enableUsed)
{
	int searchResult = 0;

	if (enableUsed == false)
	{
		//search without used point, see wether we can get a pathpiece
		searchResult = bfsPathPiece(graph, firstNode, secondNode, false);

		if (searchResult == FOUND)
		{
			addPathPiece(graph, firstNode, secondNode);
		}

		return searchResult;
	}
	else
	{
		//search without used point fisrt,see wether we can get a pathpiece
		searchResult = bfsPathPiece(graph, firstNode, secondNode, false);

		if (searchResult == FOUND)
		{
			addPathPiece(graph, firstNode, secondNode);
		}
		else
		{
			//use used points to search pathpiece
			if (bfsPathPiece(graph, firstNode, secondNode, true) == NOTFOUND)
			{
				return NOSOLUTION;
			}

			//deal with used points and pathpiece associated with 
			//restore:used/pathpieceone/pathpiecetwo 
			dealUsedNodes(graph, firstNode, secondNode);

			addPathPiece(graph, firstNode, secondNode);
		}

		return searchResult;
	}
}

//bfs graph to find a path for two point
//bfs graph to find a path for two point
//bfs的算法可以参考 算法导论 上的伪代码，大师确实写得很好
int Path::bfsPathPiece(GraphVector &graph, int firstNode, int secondNode, bool enableUsed)
{
	queue<int> pointQueue;
	int flag = 0;
	int color[MAXPOINTNUM];

	//initial，需要的话，这种初始化操作可以优化掉
	for (int i = 0; i <= MAXNODEID; ++i)
	{
		color[i] = WHITE;
		pi[i] = INVALID;
		distance[i] = INVALID;
	}

	//initial start point
	color[firstNode] = GRAY;
	distance[firstNode] = 0;
	pi[firstNode] = INVALID;

	pointQueue.push(firstNode);

	while (!pointQueue.empty() && !flag)//time complexity:O(V+E), actually less than this
	{
		int uu = pointQueue.front();
		pointQueue.pop();

		vector<Edge> &edgeVector = graph.getNode(uu).getEdgeVector();
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)//traverse every edge of point uu
		{
			int vv = (*iter).getNodeTwo();
			Point &pointTemp = graph.getNode(vv);

			if (vv == secondNode)//meet point we need 
			{
				color[vv] = GRAY;
				distance[vv] = distance[uu] + 1;
				pi[vv] = uu;
				pointQueue.push(vv);

				flag = 1;
				break;//jump out of loop
			}

			if ((color[vv] != WHITE) || (getValidity(vv) != POINT_VALID) || ((getUsed(vv) != false) && !enableUsed)
				|| (getProperty(vv) == POINT_START) || (getProperty(vv) == POINT_END)) continue;
				
			//the rest can satisfy needs
			color[vv] = GRAY;
			distance[vv] = distance[uu] + 1;
			pi[vv] = uu;
			pointQueue.push(vv);
		}

		color[uu] = BLACK;
	}

	if (flag) return FOUND;
	else return NOTFOUND;
}

//this bfs is used for finding more than one path, so we need commoncolor and unique pi
int Path::multiBfsPathPiece(GraphVector &graph, int firstNode, int secondNode, int *commonColor, int *pi, bool enableUsed)
{
	queue<int> pointQueue;
	int flag = 0;

	int color[MAXPOINTNUM];
	memset(color, 0, sizeof(int)*MAXPOINTNUM);

	//initial start point
	color[firstNode] = GRAY;
	pi[firstNode] = INVALID;

	pointQueue.push(firstNode);

	while (!pointQueue.empty() && !flag)//time complexity:O(V+E), actually less than this
	{
		int uu = pointQueue.front();
		pointQueue.pop();

		vector<Edge> &edgeVector = graph.getNode(uu).getEdgeVector();
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)//traverse every edge of point uu
		{
			int vv = (*iter).getNodeTwo();
			Point &pointTemp = graph.getNode(vv);

			if (vv == secondNode)//meet point we need 
			{
				color[vv] = GRAY;
				pi[vv] = uu;
				pointQueue.push(vv);

				flag = 1;
				break;//jump out of loop
			}

			// (used) || (the point has been deleted by me) || (has visited) || (used by other bfs)
			if ((!enableUsed  && (getUsed(vv) != false)) || (getValidity(vv) != POINT_VALID)
				|| (color[vv] != WHITE) || (commonColor[vv] != WHITE)) continue;

			//the rest can satisfy needs
			color[vv] = GRAY;
			pi[vv] = uu;
			pointQueue.push(vv);
		}

		color[uu] = BLACK;
	}

	if (flag) return FOUND;
	else return NOTFOUND;
}


int Path::dealUsedNodes(GraphVector &graph, int firstNode, int secondNode)
{
	int nodeId = pi[secondNode];//从secondNode的父节点开始

	for (; nodeId != firstNode; nodeId = pi[nodeId])
	{
		Point &pointTemp = graph.getNode(nodeId);

		if (getUsed(nodeId) == false) continue;
		else
		{
			int xx = getProperty(nodeId);
			if (xx == POINT_V)//ordinary point
			{
				crackOnePiece(graph, nodeId);
			}
			else if (xx == POINT_VPRIME)//vprime point
			{
				crackTwoPiece(graph, nodeId);
			}
		}
	}

	if (nodeId == firstNode) return EXIT_SUCCESS;
	else
	{
#ifdef DEBUG
		assert(0);//ie,wrong
#endif
		return EXIT_FAILURE;
	}
}

int Path::dealUsedNodes(GraphVector &graph, int firstNode, int secondNode, int *piLocal)
{
	int nodeId = piLocal[secondNode];//从secondNode的父节点开始

	for (; nodeId != firstNode; nodeId = piLocal[nodeId])
	{
		Point &pointTemp = graph.getNode(nodeId);

		if (getUsed(nodeId) == false) continue;
		else
		{
			int xx = getProperty(nodeId);
			if (xx == POINT_V)//ordinary point
			{
				crackOnePiece(graph, nodeId);
			}
			else if (xx == POINT_VPRIME)//vprime point
			{
				crackTwoPiece(graph, nodeId);
			}
		}
	}

	if (nodeId == firstNode) return EXIT_SUCCESS;
	else
	{
#ifdef DEBUG
		assert(0);//ie,wrong
#endif
		return EXIT_FAILURE;
	}
}

int Path::crackOnePiece(GraphVector &graph, int nodeId)
{
#ifdef DEBUG
	assert(getProperty(nodeId) == POINT_V);
#endif
	list<PathPiece>::iterator iterPieceTemp = getPathPieceOne(nodeId);

	//restore properties of points
	list<PathPiece>::iterator pathPieceNull;
	auto &linkNodesVector = (*iterPieceTemp).getLinkNodesVector();
	auto iterLinkNodesVectorBegin = linkNodesVector.begin();
	auto iterLinkNodesVectorEnd = linkNodesVector.end();
	for (auto iter = iterLinkNodesVectorBegin; iter != iterLinkNodesVectorEnd; ++iter)
	{
		Point &xx = graph.getNode(*iter);
		setUsed((*iter), false);
		setPathPieceOne((*iter), pathPieceNull);
	}

	//set properties of path piece
	(*iterPieceTemp).getLinkNodesVector().clear();
	(*iterPieceTemp).setLinkProperty(CRACKED);
	(*iterPieceTemp).setWeight(INFINITEWEIGHT);

	//cracktimes ++
	crackTimes[(*iterPieceTemp).getFirstNode()][(*iterPieceTemp).getLastNode()]++;

	return EXIT_SUCCESS;
}

int Path::crackTwoPiece(GraphVector &graph, int nodeId)
{
#ifdef DEBUG
	assert(getProperty(nodeId) == POINT_VPRIME);
#endif
	Point &node = graph.getNode(nodeId);
	list<PathPiece>::iterator iterPieceOne = getPathPieceOne(nodeId);
	list<PathPiece>::iterator iterPieceTwo = getPathPieceTwo(nodeId);

	int firstNode = (*iterPieceOne).getFirstNode();
	int lastNode = (*iterPieceTwo).getLastNode();

	//restore properties of points
	list<PathPiece>::iterator pathPieceNull;
	for (auto iter = (*iterPieceOne).getLinkNodesVector().begin(); iter != (*iterPieceOne).getLinkNodesVector().end(); ++iter)
	{
		Point &xx = graph.getNode(*iter);
		setUsed((*iter), false);
		setPathPieceOne((*iter), pathPieceNull);
	}
	for (auto iter = (*iterPieceTwo).getLinkNodesVector().begin(); iter != (*iterPieceTwo).getLinkNodesVector().end(); ++iter)
	{
		Point &xx = graph.getNode(*iter);
		setUsed((*iter), false);
		setPathPieceOne((*iter), pathPieceNull);
	}
	setUsed(nodeId, false);
	setPathPieceOne(nodeId, pathPieceNull);
	setPathPieceTwo(nodeId, pathPieceNull);

	//crack times ++
	crackTimes[firstNode][nodeId]++;//不知道这个crack增加合不合适
	crackTimes[nodeId][lastNode]++;//不知道这个crack增加合不合适
	crackTimes[firstNode][lastNode]++;

	//create cracked pathpiece,set new piece,insert new piece
	PathPiece pieceNew;
	pieceNew.setFirstNode(firstNode);
	pieceNew.setLastNode(lastNode);
	pieceNew.setLinkProperty(CRACKED);
	auto iter = pathList.insert(iterPieceOne, pieceNew);

	//modify point's PathPiece,and delete unused pathpiece
	setPathPieceTwo(firstNode, iter);
	setPathPieceOne(lastNode, iter);
	iter = pathList.erase(++iter);//erase old pieceOne
	pathList.erase(iter);//erase old pieceTwo

	return EXIT_SUCCESS;
}


//add created pathpiece list to path
//this func can add piece to empty path, to the tail of path
//or can repair cracked piece
int Path::addPathPiece(GraphVector &graph, int firstNode, int secondNode)
{
	//if path is empty,create a pathpiece(or pathpiece list),and add to path
	if (pathList.empty())
	{
		//here,pathList.begin()==pathList.end()
		createPieceList(graph, firstNode, secondNode, pathList.begin());
		return EXIT_SUCCESS;
	}

	list<PathPiece>::iterator iter = pathList.end();
	if (firstNode == (*(--iter)).getLastNode())//firstnode is lastnode of path
	{
		createPieceList(graph, firstNode, secondNode, pathList.end());//add to tail of path
	}
	else
	{
		//locate to piece,for path a->b x c,we locate to a->b
		//for path a->b c->d,we locate to a->b
		for (iter = pathList.begin(); ((iter != pathList.end()) && ((*iter).getFirstNode() != firstNode)); ++iter);

		//delete cracked piece b x c,return iter of c->d
		iter = pathList.erase(iter);

		//create path list for b x c
		createPieceList(graph, firstNode, secondNode, iter);
	}

	return EXIT_SUCCESS;
}

//set points used/pathpieceone/pathpiecetwo,create pathpiece
//firstNode and secondNode are endpoints of our piece,we insert piece before iterIn
list<PathPiece>::iterator Path::createPieceList(GraphVector &graph, int firstNode, int secondNode,
	list<PathPiece>::iterator iterIn)
{
#ifdef DEBUG
	assert((getProperty(firstNode) != POINT_V)
		&& (getProperty(secondNode) != POINT_V));
#endif

	int uu = pi[secondNode];
	int vv = secondNode;
	int weight = 0;
	int curFirstNode = secondNode;
	int curLastNode = secondNode;

	PathPiece pieceTemp;

	list<PathPiece>::iterator iter;
	auto iterEnd = pathList.end();
	if (pathList.empty())//1:pathlist is empty
	{
		iter = pathList.insert(iterEnd, pieceTemp);
	}
	else if (iterIn == iterEnd)//2:add pathpiece to the tail of pathlist
	{
		iter = pathList.insert(iterEnd, pieceTemp);
	}
	else//3:repair cracked piece, should arase old piece
	{
		iter = pathList.insert(iterIn, pieceTemp);
		//!!!!!remember delete old piece!!!
	}

	//set point vv property
	setUsed(vv, true);
	setPathPieceOne(vv, iter);

	while (uu != firstNode)//deal with all points in searched path except firstnode
	{
		if (getProperty(uu) != POINT_VPRIME)//uu is ordinary v point
		{
			//set piece
			(*iter).getLinkNodesVector().push_back(uu);
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;

			//set node of graph
			setUsed(uu, true);
			setPathPieceOne(uu, iter);//for v point,only have piece one
		}
		else//uu is vprime point
		{
#ifdef DEBUG
			assert(getProperty(uu) == POINT_VPRIME);
#endif
			//get current piece's first and second node
			curFirstNode = uu;

			//set piece
			(*iter).setFirstNode(curFirstNode);
			(*iter).setLastNode(curLastNode);
			(*iter).setLinkProperty(LINKED);
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;
			(*iter).setWeight(weight);

			//set point
			setUsed(uu, true);
			setPathPieceTwo(uu, iter);//for vprime point,set piece two
			setPathPieceOne(curLastNode, iter);//for last vprime point,set piece one

			//add new path piece,last piece setted over
			weight = 0;
			iter = pathList.insert(iter, pieceTemp);

			//get current piece's first and second node
			curLastNode = curFirstNode;
		}

		//refresh uu and vv
		vv = uu;
		uu = pi[uu];
	}

	if (uu == firstNode)
	{
		//get current piece's first and second node
		curFirstNode = uu;

		//set piece
		(*iter).setFirstNode(curFirstNode);
		(*iter).setLastNode(curLastNode);
		(*iter).setLinkProperty(LINKED);
		//weight += graph.getEdge(uu, vv).getEdgeWeight();
		weight = INVALID;
		(*iter).setWeight(weight);

		//set point
		setUsed(uu, true);
		setPathPieceTwo(uu, iter);//for vprime point,set piece two
		setPathPieceOne(curLastNode, iter);
	}

	return iter;
}

//set points used/pathpieceone/pathpiecetwo,create pathpiece
//firstNode and secondNode are endpoints of our piece,we insert piece before iterIn
list<PathPiece>::iterator Path::createPieceList(GraphVector &graph, int firstNode, int secondNode,
	list<PathPiece>::iterator iterIn, int *piLocal)
{
#ifdef DEBUG
	assert((getProperty(firstNode) != POINT_V)
		&& (getProperty(secondNode) != POINT_V));
#endif

	int uu = piLocal[secondNode];
	int vv = secondNode;
	int weight = 0;
	int curFirstNode = secondNode;
	int curLastNode = secondNode;

	PathPiece pieceTemp;

	list<PathPiece>::iterator iter;
	auto iterEnd = pathList.end();
	if (pathList.empty())//1:pathlist is empty
	{
		iter = pathList.insert(iterEnd, pieceTemp);
	}
	else if (iterIn == iterEnd)//2:add pathpiece to the tail of pathlist
	{
		iter = pathList.insert(iterEnd, pieceTemp);
	}
	else//3:repair cracked piece, should arase old piece
	{
		iter = pathList.insert(iterIn, pieceTemp);
		//!!!!!remember delete old piece!!!
	}

	//set point vv property
	setUsed(vv, true);
	setPathPieceOne(vv, iter);

	while (uu != firstNode)//deal with all points in searched path except firstnode
	{
		if (getProperty(uu) != POINT_VPRIME)//uu is ordinary v point
		{
			//set piece
			(*iter).getLinkNodesVector().push_back(uu);
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;

			//set node of graph
			setUsed(uu, true);
			setPathPieceOne(uu, iter);//for v point,only have piece one
		}
		else//uu is vprime point
		{
#ifdef DEBUG
			assert(getProperty(uu) == POINT_VPRIME);
#endif
			//get current piece's first and second node
			curFirstNode = uu;

			//set piece
			(*iter).setFirstNode(curFirstNode);
			(*iter).setLastNode(curLastNode);
			(*iter).setLinkProperty(LINKED);
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;
			(*iter).setWeight(weight);

			//set point
			setUsed(uu, true);
			setPathPieceTwo(uu, iter);//for vprime point,set piece two
			setPathPieceOne(curLastNode, iter);//for last vprime point,set piece one

			//add new path piece,last piece setted over
			weight = 0;
			iter = pathList.insert(iter, pieceTemp);

			//get current piece's first and second node
			curLastNode = curFirstNode;
		}

		//refresh uu and vv
		vv = uu;
		uu = piLocal[uu];
	}

	if (uu == firstNode)
	{
		//get current piece's first and second node
		curFirstNode = uu;

		//set piece
		(*iter).setFirstNode(curFirstNode);
		(*iter).setLastNode(curLastNode);
		(*iter).setLinkProperty(LINKED);
		//weight += graph.getEdge(uu, vv).getEdgeWeight();
		weight = INVALID;
		(*iter).setWeight(weight);

		//set point
		setUsed(uu, true);
		setPathPieceTwo(uu, iter);//for vprime point,set piece two
		setPathPieceOne(curLastNode, iter);
	}

	return iter;
}

//such as: a->b->c,a->b x c,a x b->c
//we should set b false,set points in pieces a->b,b->c false
//delete pieces a->b,b->c
list<PathPiece>::iterator Path::deleteOneVPrime(GraphVector &graph, int vPrimeId)
{
	list<PathPiece>::iterator iter;

	for (auto iterTemp = pathList.begin(); iterTemp != pathList.end(); ++iterTemp)
	{
		if ((*iterTemp).getLastNode() == vPrimeId)
		{
			//set v point property used false
			setPieceNodeFalse(graph, iterTemp);
			iter = iterTemp; ++iter;
			setPieceNodeFalse(graph, iter);

			//set vprime property used false
			setUsed(vPrimeId, false);

			//delete piece 
			iter = pathList.erase(iterTemp);
			iter = pathList.erase(iter);

			return iter;
		}
	}

#ifdef DEBUG
	assert(0);//shouldn't happen
#endif
	return iter;
}

int Path::printPath()
{
	int weight = 0;
	list<PathPiece>::iterator iter;
	cout << endl;
	cout << "the path is: ";

	if (pathList.empty())
	{
		cout << "NA" << endl;
		return INVALID;
	}

	for (iter = pathList.begin(); iter != pathList.end();++iter)
	{
		cout << (*iter).getFirstNode();
		weight += (*iter).getWeight();

		if ((*iter).getLinkProperty() == LINKED)
		{
			cout << " ~ ";
		}
		else if ((*iter).getLinkProperty() == CRACKED)//if it is cracked piece,print x
		{
			cout << " x ";
		}
		else if ((*iter).getLinkProperty() == SUPERLINK)
		{
			cout << " ^ ";
		}
	}

	cout << (*(--iter)).getLastNode() << "  weight:  " << weight << endl << endl;
	return weight;//this weight is valid in 2 path, i do not use it yet
}

int Path::printNodeWeight()
{
	list<PathPiece>::iterator iterR;
	list<PathPiece>::iterator iterL;
	cout << endl;
	cout << "the node weight is: ";

	iterL = pathList.begin();
	iterR = pathList.begin();
	++iterR;
	for (; iterR != pathList.end(); ++iterR,++iterL)
	{
		cout << ((*iterR).getWeight() + (*iterL).getWeight())<<"  ";
	}

	cout<< endl;
	return EXIT_SUCCESS;
}

//compute num of piece need to be cracked in pi
int Path::cmNumToBeCracked(GraphVector &graph, int firstNode, int lastNode, int *piLocal)
{
	int uu = piLocal[lastNode];
	int vv = lastNode;
	int crackPieceFirstNode = 0;

	unordered_set<int> hashSet;

	while (uu != firstNode)
	{
		Point &pointTemp = graph.getNode(uu);

		if (getUsed(uu) == true)
		{
			crackPieceFirstNode = (*(getPathPieceOne(uu))).getFirstNode();

			hashSet.insert(crackPieceFirstNode);
		}

		vv = uu;
		uu = piLocal[uu];
	}

	//uu == firstNode, is the vprime point

	return hashSet.size();
}

//bfs from point vStart,if we can not visit point xx,we delete xx
int Path::cutGraph(GraphVector &graph)
{
	int color[MAXPOINTNUM];
	memset(color, 0, sizeof(int)*MAXPOINTNUM);

	//bfs from start point
	graph.bfs(getVStart(), color);

	//traverse every point in graph
	for (int i = 0; i <= MAXNODEID; i++)
	{
		//if color==WHITE and this point is a valid point,we delete it:set validity==POINT_DELETE
		if ((getValidity(i) == POINT_VALID) && (color[i] == WHITE))
		{
			setValidity(i, POINT_DELETE);
		}
	}

	return EXIT_SUCCESS;
}

//save piece need to be cracked in vector,just save iter, not piece itself
int Path::savePiecesToBeCrackedInSet(GraphVector &graph, set<list<PathPiece>::iterator, compareListPieceIter> &iterCrackPieceSet,
	int firstNode, int lastNode, int *piLocal)
{
	int uu = piLocal[lastNode];
	int vv = lastNode;
	list<PathPiece>::iterator iterTemp;

	while (uu != firstNode)
	{
		Point &pointTemp = graph.getNode(uu);

		if (getUsed(uu) == true)
		{
			iterTemp = getPathPieceOne(uu);

			iterCrackPieceSet.insert(iterTemp);
		}

		vv = uu;
		uu = piLocal[uu];
	}

	return EXIT_SUCCESS;
}

//check used property
int Path::checkUsedProperty(GraphVector &graph)
{
	int nodeLeft = 0;
	int nodeRight = 0;
	auto iterPiece = pathList.begin();

	for (iterPiece = pathList.begin(); iterPiece != (pathList.end()); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);

			if (getUsed(nodeLeft) != true)
			{
				cout << "node: "<<nodeLeft<<" used property wrong." << endl;
				return WRONG;
			}

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();

		if (getUsed(nodeLeft) != true)
		{
			cout << "node: " << nodeLeft << " used property wrong." << endl;
			return WRONG;
		}
	}

	nodeLeft = nodeRight;
	if (getUsed(nodeLeft) != true)
	{
		cout << "node: " << nodeLeft << " used property wrong." << endl;
		return WRONG;
	}

	return	RIGHT;
}

//print all node in path
int Path::printAllNodeInPath()
{
	int nodeLeft = 0;
	int nodeRight = 0;
	auto iterPiece = pathList.begin();

	cout << "all node in path: ";

	for (iterPiece = pathList.begin(); iterPiece != (pathList.end()); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);
			cout << nodeLeft<<" ";

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();
		cout << nodeLeft << " ";
	}

	nodeLeft = nodeRight;
	cout << nodeLeft << " " << endl << endl;;

	return EXIT_SUCCESS;
}

//这个函数需要写，没有写还
int Path::checkPath(GraphVector &graph)
{

#ifdef DEBUG
	assert(0);
#endif

	return EXIT_SUCCESS;
}

//set points used/pathpieceone/pathpiecetwo
//create pathpiece
//int Path::createPath(GraphVector &graph, ViolentRoute &violentRoute)
//{
//	int firstNode = graph.getVStart();
//	int lastNode = graph.getVEnd();
//	const int *pi = violentRoute.getPiBest();
//	int weight = 0;//this weight is for every piece
//
//#ifdef DEBUG
//	assert((graph.getNode(firstNode).getProperty() != POINT_V)
//		&& (graph.getNode(lastNode).getProperty() != POINT_V));
//#endif
//
//	int uu = pi[lastNode];
//	int vv = lastNode;
//	int curFirstNode = lastNode;
//	int curLastNode = lastNode;
//
//	PathPiece pieceTemp;
//
//	list<PathPiece>::iterator iter;
//	auto iterEnd = pathList.end();
//	if (pathList.empty())//pathlist should be empty
//	{
//		iter = pathList.insert(iterEnd, pieceTemp);
//	}
//	else
//	{
//#ifdef DEBUG
//		assert(1);
//#endif
//	}
//
//	//set point vv property
//	setUsed(vv, true);
//	setPathPieceOne(vv, iter);
//
//	while (uu != firstNode)//deal with all points in searched path except firstnode
//	{
//		if (getProperty(uu) != POINT_VPRIME)//uu is ordinary v point
//		{
//			//set piece
//			(*iter).getLinkNodesVector().push_back(uu);
//			weight += graph.getEdge(uu, vv).getEdgeWeight();
//
//			//set node of graph
//			setUsed(uu, true);
//			setPathPieceOne(uu, iter);//for v point,only have piece one
//		}
//		else//uu is vprime point
//		{
//#ifdef DEBUG
//			assert(graph.getNode(uu).getProperty() == POINT_VPRIME);
//#endif
//			//get current piece's first and second node
//			curFirstNode = uu;
//
//			//set piece
//			(*iter).setFirstNode(curFirstNode);
//			(*iter).setLastNode(curLastNode);
//			(*iter).setLinkProperty(LINKED);
//			weight += graph.getEdge(uu, vv).getEdgeWeight();
//			(*iter).setWeight(weight);
//
//			//set point
//			setUsed(uu, true);
//			setPathPieceTwo(uu, iter);//for vprime point,set piece two
//			setPathPieceOne(curLastNode, iter);//for last vprime point,set piece one
//
//			//add new path piece,last piece setted over
//			weight = 0;
//			iter = pathList.insert(iter, pieceTemp);
//
//			//get current piece's first and second node
//			curLastNode = curFirstNode;
//		}
//
//		//refresh uu and vv
//		vv = uu;
//		uu = pi[uu];
//	}
//
//	if (uu == firstNode)
//	{
//		//get current piece's first and second node
//		curFirstNode = uu;
//
//		//set piece
//		(*iter).setFirstNode(curFirstNode);
//		(*iter).setLastNode(curLastNode);
//		(*iter).setLinkProperty(LINKED);
//		weight += graph.getEdge(uu, vv).getEdgeWeight();
//		(*iter).setWeight(weight);
//
//		//set point
//		setUsed(uu, true);
//		setPathPieceTwo(uu, iter);//for vprime point,set piece two
//		setPathPieceOne(curLastNode, iter);
//	}
//
//	return EXIT_SUCCESS;
//}

//reset path itself
int Path::resetSelf()
{
	pathList.clear();

	return EXIT_SUCCESS;
}

//compute number of valid vprime point in graph
//time complexity: O(50)
int Path::getValidVPrimeNum()
{
	int ans = 0;

	//traverse every vprime point
	for (int i = 0; i < vPrimeSize; i++)
	{
		ans += (getValidity(vPrimeIndex[i]) == POINT_VALID);
	}

	return ans;
}

int Path::getValidNodesNum()
{
	int ans = 0;

	//traverse every point
	for (int i = 0; i <= MAXNODEID; i++)
	{
		ans += (getValidity(i) == POINT_VALID);
	}

	return ans;
}

