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
#include "scc.h"
#include "environment.h"

using namespace std;

Scc::Scc() :lowcnt(0), dfncnt(0), scccnt(0)
{

}

Scc::Scc(GraphBase &graph)
{
	//initial array
	memset(scc, 0, sizeof(int)*MAXPOINTNUM);
	memset(color, 0, sizeof(int)*MAXPOINTNUM);
	memset(low, 0, sizeof(int)*MAXPOINTNUM);
	memset(dfn, 0, sizeof(int)*MAXPOINTNUM);

	cmscc(graph);
#ifdef DEBUG//show scc num of every node
	//for (int i = 0; i <= MAXNODEID; i++)
	//{
	//	cout << "node " << i << "  scc&topoOrder:" << scc[i] << endl;
	//}
#endif/*DEBUG*/

	//add scc and topo to graph node
	addSccTopo(graph);
}

int Scc::tarjan(int i, GraphBase &graph)///i是结点编号
{
	int temp = 0;
	Point &graphNode = graph.getNode(i);
	vector<Edge> &edgeVector = graphNode.getEdgeVector();

	nodeStack.push(i);
	color[i] = GRAY;
	lowcnt++; dfncnt++;
	low[i] = lowcnt; dfn[i] = dfncnt;//lowcnt和dfncnt从1开始

	if (graph.getGraphKind() == GRAPHVECTOR)
	{
		int vv = 0;
		for (auto iter = edgeVector.begin(); iter != edgeVector.end(); ++iter)
		{
			vv = (*iter).getNodeTwo();

			//if vv does not exist, do not call tarjan func
			if (graph.getNode(vv).getPointId() == POINT_INVALID)
			{
				continue;
			}

			//the rests are satified
			if (color[vv] == WHITE)//if this node has not been visited
			{
				tarjan(vv, graph);
				low[i] = min(low[i], low[vv]);
			}
			else if (color[vv] == GRAY)//if it is in the stack
			{
				low[i] = min(low[i], dfn[vv]);
			}
		}
	}

	if (low[i] == dfn[i])///产生一个SCC（强连通分量） 
	{ 
		do
		{
			temp = nodeStack.top();
			scc[temp] = scccnt;///存储该节点对应的SCC  
			nodeStack.pop();
			color[temp] = BLACK;///出栈后为black，表示已经为该节点分配SCC  
		} while (temp != i);

		scccnt++;///SCC的编号值+1  
	}
	
	return EXIT_SUCCESS;
}

int Scc::cmscc(GraphBase &graph)
{
	for (int i = 0; i <= MAXNODEID; i++)//traverse every node of graph
	{
		Point &graphNode = graph.getNode(i);
		if ((graphNode.getPointId() != POINT_INVALID) && (color[i] == WHITE))//point exists and color should be white
		{
			tarjan(i,graph);//tarjan is like dfs
		}
	}

	return EXIT_SUCCESS;
}

int Scc::addSccTopo(GraphBase &graph)
{
	for (int i = 0; i <= MAXNODEID; i++)
	{
		Point &xx = graph.getNode(i);
		if (xx.getPointId() != POINT_INVALID)//the point should be valid: or may not exist, or may has been deleted
		{
			xx.setSccNum(scc[i]);
			xx.setTopoOrder(scc[i]);
		}
	}

	return EXIT_SUCCESS;
}