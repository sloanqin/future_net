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
#ifndef _ASSIST_H
#define _ASSIST_H
//in this file, we define some funcs for using in other cpp files

#include <stdio.h>
#include "environment.h"
#include "path.h"
#include "graphVector.h"
#include "pathEdge.h"
#include <list>

template<class T>
inline T assistMax(const T& a, const T& b)
{
	return (a > b) ? a : b;
}

template<class T>
inline T assistMin(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

typedef struct timeMeasurexx
{
	float timeSwitchNode = 0;
	float timeDfs = 0;
	float timeAll = 0;
	float timeAllDijkstra = 0;

	long long int exeTimesSwitchNode = 0;
	long long int startTimeDijkstra = 0;
	long long int timesDijkstra = 0;
	long long int exeTimesDfs = 0;
	long long int startTime = 0;
	long long int timesUsefulDijkstra = 0;

}TimeMeasure;

extern long long int startTimeAll;
extern TimeMeasure timeMeasure;//for time measure

//check whether have solution, here vPTopoMax is the max topo order in vprime points
int judgeSolution(GraphVector &graph, Path &path, int vPTopoMax, int vPTopoMin);

//根据topo顺序的范围限制，删除不满足path要求的点
int deleteNodesByTopo(GraphVector &graph, Path &path, int topoMax, int topoMin);

//get current time, for time measure,just cpu time, not used time
long long int getTime();

//compute weight, accoording to graph, path, and common edge
int computeVPrimeNodeWeight(GraphVector &graph, Path &path, int nodeId, PathEdge &otherPathEdge);
int computePathPieceWeight(GraphVector &graph, Path &path, list<PathPiece>::iterator iterPathPiece, PathEdge &otherPathEdge);

//according to whether other path has used, whether have two edge (uu,vv), return differ weight of (uu,vv)
int computeDanymicEdgeWeight(GraphVector &graph, PathEdge &otherPathEdge, int uu, int vv);

//output to file buffer,as huawei requires
int outputAns(Path &path, PathEdge &selfPathEdge);

int printAns(Path &path, PathEdge &selfPathEdge);//output answer as huawei requires

#endif


