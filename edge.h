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
#ifndef _EDGE_H
#define _EDGE_H

#include "environment.h"
#include <stdio.h>

class Edge
{
public:
	Edge() :nodeStart(INVALID), nodeEnd(INVALID){ edgeId[0] = INVALID; edgeId[1] = INVALID; edgeWeight[0] = INVALID; edgeWeight[1] = INVALID; };//default constructor

	//get value
	inline int getEdgeId(int index) const { return edgeId[index]; }
	inline int getNodeOne() const { return nodeStart; }
	inline int getNodeTwo() const { return nodeEnd; }
	inline int getEdgeWeight(int index) const { return edgeWeight[index]; }
	inline Edge *getNextEdge() const { return nextEdge; }

	//set value
	inline void setEdgeId(int index, int xx){ edgeId[index] = xx; }
	inline void setNodeOne(int xx){ nodeStart = xx; }
	inline void setNodeTwo(int xx){ nodeEnd = xx; }
	inline void setEdgeWeight(int index, int xx){ edgeWeight[index] = xx; }
	inline void setNextEdge(Edge *xx){ nextEdge = xx; }

private:
	int nodeStart = INVALID;//边的起点
	int nodeEnd = INVALID;//边的终点
	int edgeId[2];//不可能达到INT_MAX，用来表示是无效值
	int edgeWeight[2];//边的权重

	Edge *nextEdge = NULL;//指向下一条边
};


#endif

