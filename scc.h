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

	int scc[MAXPOINTNUM];///�洢ÿ�����������ǿ��ͨ�����������±��ǽ���ţ������ֵ�ǽ��������ǿ��ͨ������ֵ  
	int color[MAXPOINTNUM];///�洢ÿ��������ɫ����ɫ��ʾû�з��ʣ���ɫ��ʾ����stack�У���ɫ��ʾ�Ѿ�������ǿ��ͨ����  
	int low[MAXPOINTNUM];///ÿ������lowֵ  
	int dfn[MAXPOINTNUM];///ÿ������dfnֵ  

	int lowcnt = 0, dfncnt = 0;
	int scccnt = 0;///ǿ��ͨ�����ı�ţ���0��ʼ,�������ǵĶ��壬��õ�scc�ı�ž�������˳��

	int tarjan(int i, GraphBase &graph);//algorthim of tarjan
	int cmscc(GraphBase &graph);//use tarjan to compute scc
	int addSccTopo(GraphBase &graph);//add scc and topo to graph
};

#endif/**/