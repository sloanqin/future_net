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
#ifndef _SCC_H
#define _SCC_H

#include "environment.h"
#include "graphVector.h"
#include "graphBase.h"
#include <cstdio>  
#include <cstring>  
#include <iostream>  
#include <algorithm>  
#include <vector>  
#include <stack>  

using namespace std;

//statement:this class compute scc and topological sort of graph
//for example: a->b->c is a graph,here are 3 scc, 
//and we think the topological sort of a b c is 2 1 0
class Scc
{
public:
	Scc();//default constructor
	Scc(GraphBase &graph);
private:
	stack<int> nodeStack;

	int scc[MAXPOINTNUM];///存储每个结点所属的强连通分量，数组下标是结点编号，数组的值是结点所属的强连通分量的值  
	int color[MAXPOINTNUM];///存储每个结点的颜色，白色表示没有访问，灰色表示还在stack中，黑色表示已经生成了强连通分量  
	int low[MAXPOINTNUM];///每个结点的low值  
	int dfn[MAXPOINTNUM];///每个结点的dfn值  

	int lowcnt = 0, dfncnt = 0;
	int scccnt = 0;///强连通分量的编号，从0开始,根据我们的定义，求得的scc的编号就是拓扑顺序

	int tarjan(int i, GraphBase &graph);//algorthim of tarjan
	int cmscc(GraphBase &graph);//use tarjan to compute scc
	int addSccTopo(GraphBase &graph);//add scc and topo to graph
};

#endif/**/