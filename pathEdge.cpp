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
#include "pathEdge.h"
#include "edge.h"
#include <vector>

PathEdge::PathEdge(PathEdge &path1Edge, PathEdge &path2Edge)
{
	for (auto iter = path1Edge.pathUsedEdges.begin(); iter != path1Edge.pathUsedEdges.end(); ++iter)
	{
		if (path2Edge.pathUsedEdges.count((*iter).first) >= 1)
		{
			if ((path2Edge.pathUsedEdges.at((*iter).first)) == ((*iter).second))
			{
				pathUsedEdges[(*iter).first] = (*iter).second;
			}
		}
	}
}

int PathEdge::refreshPathEdge(GraphVector &graph, Path &path)
{
	pathUsedEdges.clear();

	int nodeLeft = 0;
	int nodeRight = 0;
	auto pathList = path.getPathList();
	auto iterPiece = pathList.begin();

	for (iterPiece = pathList.begin(); iterPiece != (pathList.end()); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);

			//save edge to map
			Edge &edgeTemp = graph.getEdge(nodeLeft, nodeRight);

			//if we have two edge in graph
			//insert the smaller weight edge of (uu,vv) to pathEdge
			if ((edgeTemp.getEdgeId(1) != INVALID) && (edgeTemp.getEdgeWeight(1) < edgeTemp.getEdgeWeight(0)))
			{

				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
			}
			else
			{
				//if we only have one edge in graph, this path use the same edge of (uu,vv)
				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
			}

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();

		//save edge to map
		Edge &edgeTemp = graph.getEdge(nodeLeft, nodeRight);

		//if we have two edge in graph
		//insert the smaller weight edge of (uu,vv) to pathEdge
		if ((edgeTemp.getEdgeId(1) != INVALID) && (edgeTemp.getEdgeWeight(1) < edgeTemp.getEdgeWeight(0)))
		{

			pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
		}
		else
		{
			//if we only have one edge in graph, this path use the same edge of (uu,vv)
			pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
		}
	}

	return EXIT_SUCCESS;
}

int PathEdge::refreshPathEdge(GraphVector &graph, Path &path, PathEdge &otherPathEdge)
{
	pathUsedEdges.clear();

	int nodeLeft = 0;
	int nodeRight = 0;
	auto pathList = path.getPathList();
	auto iterPiece = pathList.begin();

	for (iterPiece = pathList.begin(); iterPiece != (pathList.end()); ++iterPiece)
	{
		nodeLeft = (*iterPiece).getFirstNode();
		vector<int> &linkNodesVector = (*iterPiece).getLinkNodesVector();

		for (auto iterLinkNodes = linkNodesVector.rbegin(); iterLinkNodes != linkNodesVector.rend(); ++iterLinkNodes)
		{
			nodeRight = (*iterLinkNodes);

			//save edge to map
			Edge &edgeTemp = graph.getEdge(nodeLeft, nodeRight);

			//if otherpath has used (uu,vv)
			if (otherPathEdge.hasEdge(nodeLeft, nodeRight))
			{
				//if we have two edge in graph
				//insert the edge of (uu,vv) that has not been used to pathEdge
				if ((edgeTemp.getEdgeId(1) != INVALID) && (otherPathEdge.getEdgeId(nodeLeft, nodeRight) == edgeTemp.getEdgeId(0)))
				{
					//insert the second edge of (uu,vv) to pathEdge
					pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
				}
				else
				{
					//if we only have one edge in graph, this path use the same edge of (uu,vv)
					pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
				}

			}
			else// if other path do not use (uu,vv)
			{
				//if we have two edge in graph
				//insert the smaller weight edge of (uu,vv) to pathEdge
				if ((edgeTemp.getEdgeId(1) != INVALID) && (edgeTemp.getEdgeWeight(1) < edgeTemp.getEdgeWeight(0)))
				{

					pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
				}
				else
				{
					//if we only have one edge in graph, this path use the same edge of (uu,vv)
					pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
				}
			}

			nodeLeft = nodeRight;
		}

		nodeRight = (*iterPiece).getLastNode();

		//save edge to map
		Edge &edgeTemp = graph.getEdge(nodeLeft, nodeRight);

		//if otherpath has used (uu,vv)
		if (otherPathEdge.hasEdge(nodeLeft, nodeRight))
		{
			//if we have two edge in graph
			//insert the edge of (uu,vv) that has not been used to pathEdge
			if ((edgeTemp.getEdgeId(1) != INVALID) && (otherPathEdge.getEdgeId(nodeLeft, nodeRight) == edgeTemp.getEdgeId(0)))
			{
				//insert the second edge of (uu,vv) to pathEdge
				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
			}
			else
			{
				//if we only have one edge in graph, this path use the same edge of (uu,vv)
				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
			}

		}
		else// if other path do not use (uu,vv)
		{
			//if we have two edge in graph
			//insert the smaller weight edge of (uu,vv) to pathEdge
			if ((edgeTemp.getEdgeId(1) != INVALID) && (edgeTemp.getEdgeWeight(1) < edgeTemp.getEdgeWeight(0)))
			{

				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(1), edgeTemp.getEdgeWeight(1) };
			}
			else
			{
				//if we only have one edge in graph, this path use the same edge of (uu,vv)
				pathUsedEdges[((nodeLeft << SHIFTSIXTEEN) + nodeRight)] = { edgeTemp.getEdgeId(0), edgeTemp.getEdgeWeight(0) };
			}
		}
	}

	return EXIT_SUCCESS;
}

int PathEdge::getEdgeWeightSum()
{
	int weightSum = 0;

	for (auto iter = pathUsedEdges.begin(); iter != pathUsedEdges.end(); ++iter)
	{
		weightSum += ((*iter).second).second;
	}

	return weightSum;
}