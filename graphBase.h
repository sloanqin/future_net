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
#ifndef _GRAPH_BASE_H_
#define _GRAPH_BASE_H_

#include "environment.h"
#include "point.h"
#include "lib_io.h"
#include <string>
#include <vector>

//graph base is designed as abstract base class 
class GraphBase
{
public:
	GraphBase() :graphKind(INVALID), nodeNum(0){};

	GraphBase(char *topo[], int edge_num) :graphKind(INVALID){ };//using topo and demand to initialize graph

	virtual ~GraphBase(){ };

	//virtual int reverseGraph(GraphBase &dstGraph) = 0;//reverse edge of srcGraph,get a new dstGraph

	virtual Edge& getEdge(int uu, int vv) = 0;//返回某条边

	virtual int insertEdge(int edgeId, int uu, int vv, int edgeWeight) = 0;//插入某条边

	virtual Point &getNode(int uu) = 0;//返回节点u

	virtual int checkNode(int uu) = 0;//检查节点是否存在

	virtual int refreshInoutDegree() = 0;//刷新每个有效点的出入度属性

	virtual const vector<Point>& getGraphNode() const = 0;//get reference of graphNode

	//from firstnode, bfs all the nodes(can be visited) in graph, save visited node in color
	virtual int bfs(int firstNode, int *color) = 0;

	//inline function:get value
	inline const int getGraphKind() const { return graphKind; }
	inline const int getNodeNum() const { return nodeNum; }//count num of graph, can not change after graph has been  initialized

	//inline func: set value
	inline void setGraphKind(int xx) { graphKind = xx; }

protected:
	inline void setNodeNum(int xx) { nodeNum = xx; }

	int graphKind = INVALID;
	int nodeNum = 0;

private:

};

#endif
