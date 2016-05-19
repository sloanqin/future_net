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
#include "route.h"
#include "assist.h"
#include "scc.h"
#include "vPrimeToUse.h"
#include "betaRoute.h"
#include "pathEdge.h"
#include "betaOptimize.h"
#include "graphVector.h"

using namespace std;
TimeMeasure timeMeasure;//for time measure

void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
{

	timeMeasure.startTime = getTime();

	int timeRunOneTime = 0;
	int minimumCommonEdge = INT_MAX;
	int minimumSumWeight = INT_MAX;
	int loopTimesCnt = 0;

	int solutionKind1 = 0;
	int solutionKind2 = 0;

	//initial graph
	GraphVector graph(topo, edge_num);
	GraphVector graphReverse(topo, edge_num, true);

	//initial path
	Path bestPath1;
	Path bestPath2;

	PathEdge bestPath1Edge;
	PathEdge bestPath2Edge;

	//if have time, run once again
	while ((TIMELIMITNUM - (getTime() - timeMeasure.startTime)) > timeRunOneTime)
	{
		//cout << "(TIMELIMITNUM - (getTime() - timeMeasure.startTime)): " << (TIMELIMITNUM - (getTime() - timeMeasure.startTime)) << endl;
		//cout << "getTime(): " << getTime() << endl;
		//cout << "timeMeasure.startTime: " << timeMeasure.startTime << endl;
		//cout << "timeRunOneTime: " << timeRunOneTime << endl << endl;

		//initial path
		Path path1(graph, demand[0], WORK_PATH);
		Path path2(graph, demand[1], BACK_PATH);

		//add scc num to graph
		Scc scc(graph);

#ifdef DEBUG
		cout << "before cutting valid node num for path1 is: " << path1.getValidNodesNum() << endl;
		cout << "before cutting valid node num for path2 is: " << path2.getValidNodesNum() << endl;
#endif

		//according to path, delete nodes by topo order
		deleteNodesByTopo(graph, path1, graph.getNode(path1.getVStart()).getSccNum(), graph.getNode(path1.getVEnd()).getSccNum());
		deleteNodesByTopo(graph, path2, graph.getNode(path2.getVStart()).getSccNum(), graph.getNode(path2.getVEnd()).getSccNum());

#ifdef DEBUG
		cout << "after cutting valid node num for path1 is: " << path1.getValidNodesNum() << endl;
		cout << "after cutting valid node num for path2 is: " << path2.getValidNodesNum() << endl;
#endif

		VPrimeToUse vPrime1;
		VPrimeToUse vPrime2;

		//initial vprimetouse
		if (loopTimesCnt == 0)
		{
			vPrime1.refreshVPrime(graph, path1);
			vPrime2.refreshVPrime(graph, path2);
		}
		else
		{
			//initial with random order
			vPrime1.refreshAgainVPrime(graph, path1);
			vPrime2.refreshAgainVPrime(graph, path2);
		}

		//judge whether have solution
		//if after cutgraph and delete by topo,vprime is same as before,we think may have solution
		solutionKind1 = judgeSolution(graph, path1, vPrime1.getTopoOrderMax(), vPrime1.getTopoOrderMin());
		solutionKind2 = judgeSolution(graph, path2, vPrime2.getTopoOrderMax(), vPrime2.getTopoOrderMin());

		BetaRoute betaRoute1;
		BetaRoute betaRoute2;

		if ((solutionKind1 != NOSOLUTION) && (solutionKind2 != NOSOLUTION))
		{
			//find path for path1 and path2
			solutionKind1 = betaRoute1.cmPath(graph, vPrime1, path1);
			solutionKind2 = betaRoute2.cmPath(graph, vPrime2, path2);

			//path1.printPath();//print vprime node in order
			//path1.printAllNodeInPath();
		}
		else break;//NA

		//initial edge been used in path
		PathEdge path1Edge(graph, path1);
		PathEdge path2Edge(graph, path2, path1Edge);

		//optimize path
		BetaOptimize betaOptimize;

		//if path1 and path2 have solution
		if ((solutionKind1 != NOSOLUTION) && (solutionKind2 != NOSOLUTION))
		{
			cout << "@_@: beta optimize: " << endl;
			betaOptimize.optimizePath(graph, graphReverse, path1, path2, path1Edge, path2Edge);

		}
		else break;//NA

#ifdef DEBUG
		cout << endl << endl << "@_@: we optimize path1: " << endl;
		path1.printPath();//print vprime node in order
		path1.printAllNodeInPath();//print all node in path
		printAns(path1, path1Edge);//print edge id as huawei requires

		cout << endl << endl << "@_@: we optimize path2: " << endl;
		path2.printPath();//print vprime node in order
		path2.printAllNodeInPath();//print all node in path
		printAns(path2, path2Edge);//print edge id as huawei requires
#endif

		PathEdge commonEdge(path1Edge, path2Edge);

		//compute commonedge num and weightSum,if smaller refresh the best
		int commonEdgeNum = commonEdge.getEdgeNum();
		int weightSum = path1Edge.getEdgeWeightSum() + path2Edge.getEdgeWeightSum();

		if ((commonEdgeNum < minimumCommonEdge) ||
			((commonEdgeNum == minimumCommonEdge) && (weightSum < minimumSumWeight)))
		{
			bestPath1 = path1;
			bestPath2 = path2;

			bestPath1Edge = path1Edge;
			bestPath2Edge = path2Edge;

			//save the best for comparison
			minimumCommonEdge = commonEdgeNum;
			minimumSumWeight = weightSum;

			cout << "    new best path"<< endl;
		}

		//refresh runone time, only once
		if (timeRunOneTime == 0) timeRunOneTime = (getTime() - timeMeasure.startTime);		

		loopTimesCnt++;
		cout << "loopTimesCnt:" << loopTimesCnt << endl;

	}//while

	//output path to buffer
	outputAns(bestPath1, bestPath1Edge);
	outputAns(bestPath2, bestPath2Edge);

	timeMeasure.timeAll = float(getTime() - timeMeasure.startTime);
	cout << "cost time: " << timeMeasure.timeAll << " ms" << endl;

	cout << "timeRunOneTime:" << timeRunOneTime << endl;
	cout << "route over..." << endl;
}