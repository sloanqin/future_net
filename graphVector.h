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
#ifndef _GRAPH_VECTOR_H_
#define _GRAPH_VECTOR_H_

#include "graphBase.h"
#include "environment.h"
#include "edge.h"
#include "point.h"
#include <vector>
#include <iostream>

using namespace std;

//graph base is designed as abstract base class 
class GraphVector :public GraphBase
{
public:
	GraphVector() :GraphBase(){};

	GraphVector(char *topo[], int edge_num);//using topo and demand to initialize graph
	GraphVector(char *topo[], int edge_num, bool reverse);//reverse graph

	~GraphVector(){ };

	//int reverseGraph(GraphBase &dstGraph) override;//reverse edge of srcGraph,get a new dstGraph

	Edge& getEdge(int uu, int vv) override;//返回某条边

	int insertEdge(int edgeId, int uu, int vv, int edgeWeight) override;//插入某条边

	//返回节点u
	inline Point &getNode(int uu) override
	{
		if (uu > MAXNODEID)
		{
#ifdef DEBUG
			cout << "get node out of graph" << endl;
#endif
			static Point pointInvalid;
			return pointInvalid;
		}

		auto iter = graphNode.begin();
		iter += uu;
		return (*iter);
	}

	int checkNode(int uu) override;//检查节点是否存在

	int refreshInoutDegree() override;//刷新每个有效点的出入度属性

	//from firstnode, bfs all the nodes(can be visited) in graph, save visited node in color
	int bfs(int firstNode, int *color) override;

	const vector<Point>& getGraphNode() const override{ return graphNode; };//get reference of graphNode

protected:

private:
	std::vector<Point> graphNode;//保存图的节点
};



#endif