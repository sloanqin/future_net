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

#include "betaOptimize.h"
#include "lib_time.h"
#include <memory.h>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

using namespace std;

BetaOptimize::BetaOptimize() :nodeFilter(INT_MAX), pieceOrNodeFilter(INT_MAX), dfsFlag(0), convergenceTimes(0),
switchAlgorithmKind(SWITCHNODEPIECE)
{

}

inline int BetaOptimize::switchAlgorithm(GraphVector &graph, Path &path, int curMaxNodeId, list<PathPiece>::iterator iterOldPiece45, int secMaxNode)
{
	if (switchAlgorithmKind == SWITCHNODEPIECE)
	{
		return (switchNodePiece(graph, path, curMaxNodeId, iterOldPiece45));
	}
	else if (switchAlgorithmKind == SWITCHNODENODE)
	{
		return (switchNodeNode(graph, path, curMaxNodeId, secMaxNode));
	}
}

//optimize path
int BetaOptimize::optimizePath(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	//three opt to optmize
	threeOptSlow(graph, path1, path2, path1Edge, path2Edge);

	//three opt to optmize
	threeOptFast(graph, graphReverse, path1, path2, path1Edge, path2Edge);

	//four opt to optmize
	fourOptSlow(graph, path1, path2, path1Edge, path2Edge);

	//four opt to optmize
	//fourOptFast(graph, graphReverse, path1, path2, path1Edge, path2Edge);

	//four opt to optmize
	fiveOptSlow(graph, path1, path2, path1Edge, path2Edge);

	//four opt to optmize
	//fiveOptFast(graph, graphReverse, path1, path2, path1Edge, path2Edge);

	return EXIT_SUCCESS;
}

//here we have 4 nodes and 3 pieces named 
//path as follows:
//old path: node1 iterOldPiece12 node2 iterOldPiece23 node3     node4 iterOldPiece45 node5
//new path: node1 iterNewPiece13 node3     node4 iterNewPiece42 node2 iterNewPiece25 node5
int BetaOptimize::threeOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() >= THREE_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5
	};

	//for search path,save in pi[]
	int pi13[MAXPOINTNUM];
	int pi42[MAXPOINTNUM];
	int pi25[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************
	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();

	//for flag
	int loopFlagPath1 = true;
	int changeFlagPath1 = PATH1NOTCHANGE;

	//for new path
	list<PathPiece>::iterator iterNewPiece13Path1;
	list<PathPiece>::iterator iterNewPiece42Path1;
	list<PathPiece>::iterator iterNewPiece25Path1;
	int weightNewPath1 = 0;

	//for old path
	list<PathPiece>::iterator iterOldPiece12Path1 = pathListPath1.begin();
	list<PathPiece>::iterator iterOldPiece23Path1 = (++(pathListPath1.begin()));
	list<PathPiece>::iterator iterOldPiece45Path1;
	int weightOldPath1;

	//node id num
	int loopEndNodeIdPath1 = (*(--(pathListPath1.end()))).getFirstNode();//initial to be the last vprime point in path

	//old node id
	int node1Path1 = (*iterOldPiece12Path1).getFirstNode();
	int node2Path1 = (*iterOldPiece12Path1).getLastNode();
	int node3Path1 = (*iterOldPiece23Path1).getLastNode();
	int node4Path1;
	int node5Path1;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************
	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//for flag
	int loopFlagPath2 = true;
	int changeFlagPath2 = PATH2NOTCHANGE;

	//for new path
	list<PathPiece>::iterator iterNewPiece13Path2;
	list<PathPiece>::iterator iterNewPiece42Path2;
	list<PathPiece>::iterator iterNewPiece25Path2;
	int weightNewPath2 = 0;

	//for old path
	list<PathPiece>::iterator iterOldPiece12Path2 = pathListPath2.begin();
	list<PathPiece>::iterator iterOldPiece23Path2 = (++(pathListPath2.begin()));
	list<PathPiece>::iterator iterOldPiece45Path2;
	int weightOldPath2;

	//node id num
	int loopEndNodeIdPath2 = (*(--(pathListPath2.end()))).getFirstNode();//initial to be the last vprime point in path

	//old node id
	int node1Path2 = (*iterOldPiece12Path2).getFirstNode();
	int node2Path2 = (*iterOldPiece12Path2).getLastNode();
	int node3Path2 = (*iterOldPiece23Path2).getLastNode();
	int node4Path2;
	int node5Path2;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************

		//reset change flag
		changeFlagPath1 = PATH1NOTCHANGE;

		loopCnt++;

		while (loopFlagPath1)//time complexity:O(100 * 100)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "3optSlow1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the weight decrease of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			//string: node1 node2 node3
			string searchedNodes123StringPath1 = to_string(node1Path1) + to_string(node2Path1) + to_string(node3Path1);

			for (iterOldPiece45Path1 = pathListPath1.begin(); iterOldPiece45Path1 != pathListPath1.end(); ++iterOldPiece45Path1)
			{
				//get node4 and node5
				node4Path1 = (*iterOldPiece45Path1).getFirstNode();
				node5Path1 = (*iterOldPiece45Path1).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath1 = searchedNodes123StringPath1 + to_string(node4Path1) + to_string(node5Path1);
				static int searchedCntPath1 = 0;
				if (searchedRecordPath1.count(searchedNodesStringPath1) >= 1)
				{
					searchedCntPath1++;
					//cout << "searchedCntPath1:" << searchedCntPath1 << endl; 
					continue;
				}
				else searchedRecordPath1.insert(searchedNodesStringPath1);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node4Path1).getSccNum())
				{
					iterOldPiece45Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node4 or node5, pass
				if ((node2Path1 == node4Path1) || (node2Path1 == node5Path1)) continue;

				//old weight sum of piece 12 23 45
				weightOldPath1 = node2WeightOldPath1 + computePathPieceWeight(graph, path1, iterOldPiece45Path1, path2Edge);

				//set used property false of oldpiece, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece45Path1);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				//when we use MultiPathPieceDijkstra, we must initial commoncolor first
				weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node3Path1, commonColor, pi13, false);

				if (weightNewPath1 < weightOldPath1)
				{
					setCommonColor(node1Path1, node3Path1, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node4Path1, node2Path1, commonColor, pi42, false);

					if (weightNewPath1 < weightOldPath1)
					{
						setCommonColor(node4Path1, node2Path1, pi42, commonColor);
						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node5Path1, commonColor, pi25, false);

					}
				}
#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath1 >= weightOldPath1))
				{
					path1.setPieceNodeTrue(graph, iterOldPiece45Path1);

					continue;//go to next iterOldPiece45Path1
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath1 = PATH1CHANGE;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceTwo2One(graph, path1, iterOldPiece12Path1, pi13);
					emplaceOne2Two(graph, path1, iterOldPiece45Path1, node2Path1, pi42, pi25);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 3;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt3 we optimize path1: insert node: "
						<< node2Path1 << " between node: " << node4Path1 << " and node: " << node5Path1 << endl;
					path1.printPath();//print vprime node in order
					path1.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath1.begin()
					iterOldPiece45Path1 = pathListPath1.begin();

					//next loopEndNodeId and node2
					if (node3Path1 == path1.getVEnd()) node2Path1 = (*(path1.getPathList().begin())).getLastNode();
					else node2Path1 = node3Path1;

					if (node1Path1 == path1.getVStart()) loopEndNodeIdPath1 = (*(--(path1.getPathList().end()))).getFirstNode();
					else loopEndNodeIdPath1 = node1Path1;

					break;//go to next node2
				}

			}//for loop

#ifdef MEASURE
			cout << "3optSlow1 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece45Path1 == pathListPath1.end())
			{
				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//next new node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//compute loopflag
			loopFlagPath1 = (node2Path1 != loopEndNodeIdPath1);
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************

		//reset change flag
		changeFlagPath2 = PATH2NOTCHANGE;

		loopCnt++;

		while (loopFlagPath2)//time complexity:O(100 * 100)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "3optSlow2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the weight decrease of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			string searchedNodes123StringPath2 = to_string(node1Path2) + to_string(node2Path2) + to_string(node3Path2);

			for (iterOldPiece45Path2 = pathListPath2.begin(); iterOldPiece45Path2 != pathListPath2.end(); ++iterOldPiece45Path2)
			{
				//get node4 and node5
				node4Path2 = (*iterOldPiece45Path2).getFirstNode();
				node5Path2 = (*iterOldPiece45Path2).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath2 = searchedNodes123StringPath2 + to_string(node4Path2) + to_string(node5Path2);
				static int searchedCntPath2 = 0;
				if (searchedRecordPath2.count(searchedNodesStringPath2) >= 1)
				{
					searchedCntPath2++;
					//cout << "searchedCntPath2:" << searchedCntPath2 << endl; 
					continue;
				}
				else searchedRecordPath2.insert(searchedNodesStringPath2);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node4Path2).getSccNum())
				{
					iterOldPiece45Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node4 or node5, pass
				if ((node2Path2 == node4Path2) || (node2Path2 == node5Path2)) continue;

				//old weight sum of piece 12 23 45
				weightOldPath2 = node2WeightOldPath2 + computePathPieceWeight(graph, path2, iterOldPiece45Path2, path1Edge);

				//set used property false of oldpiece, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece45Path2);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				//when we use MultiPathPieceDijkstra, we must initial commoncolor first
				weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node3Path2, commonColor, pi13, false);

				if (weightNewPath2 < weightOldPath2)
				{
					setCommonColor(node1Path2, node3Path2, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node4Path2, node2Path2, commonColor, pi42, false);

					if (weightNewPath2 < weightOldPath2)
					{
						setCommonColor(node4Path2, node2Path2, pi42, commonColor);
						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node5Path2, commonColor, pi25, false);

					}
				}
#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath2 >= weightOldPath2))
				{
					path2.setPieceNodeTrue(graph, iterOldPiece45Path2);

					continue;//go to next iterOldPiece45
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath2 = PATH2CHANGE;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceTwo2One(graph, path2, iterOldPiece12Path2, pi13);
					emplaceOne2Two(graph, path2, iterOldPiece45Path2, node2Path2, pi42, pi25);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 3;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt3 we optimize path2: insert node: "
						<< node2Path2 << " between node: " << node4Path2 << " and node: " << node5Path2 << endl;
					path2.printPath();//print vprime node in order
					path2.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath2.begin()
					iterOldPiece45Path2 = pathListPath2.begin();

					//next loopEndNodeId and node2
					if (node3Path2 == path2.getVEnd()) node2Path2 = (*(path2.getPathList().begin())).getLastNode();
					else node2Path2 = node3Path2;

					if (node1Path2 == path2.getVStart()) loopEndNodeIdPath2 = (*(--(path2.getPathList().end()))).getFirstNode();
					else loopEndNodeIdPath2 = node1Path2;

					break;//go to next node2
				}

			}//for loop

#ifdef MEASURE
			cout << "3optSlow2 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece45Path2 == pathListPath2.end())
			{
				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//next new node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//compute loopflag
			loopFlagPath2 = (node2Path2 != loopEndNodeIdPath2);
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		if (changeFlagPath2 == PATH2NOTCHANGE) break;

	}




	return EXIT_SUCCESS;
}

//here we have 4 nodes and 3 pieces named 
//path as follows:
//old path: node1 iterOldPiece12 node2 iterOldPiece23 node3     node4 iterOldPiece45 node5
//new path: node1 iterNewPiece13 node3     node4 iterNewPiece42 node2 iterNewPiece25 node5
int BetaOptimize::threeOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() < THREE_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5,
		PATHLISTSMINIMUM = 3
	};

	//for search path,save in pi[]
	int pi13[MAXPOINTNUM];
	int pi42[MAXPOINTNUM];
	int pi42Reverse[MAXPOINTNUM];
	int pi25[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];
	int commonColorPiece13[MAXPOINTNUM + 1];

	//distance for dijkstra,this array will be initialized in dijkstra
	int distance25[MAXPOINTNUM + 1];
	int distance42Reverse[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************
	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();
	
	//for flag
	bool chooseFlagPath1 = (pathListPath1.size() >= PATHLISTSMINIMUM);
	int loopFlagPath1 = chooseFlagPath1 ? true : false;
	int changeFlagPath1 = PATH1NOTCHANGE;

	//for new path
	list<PathPiece>::iterator iterNewPiece13Path1;
	list<PathPiece>::iterator iterNewPiece42Path1;
	list<PathPiece>::iterator iterNewPiece25Path1;
	int weightNewPath1 = 0;

	//for old path
	list<PathPiece>::iterator iterPieceNull;
	list<PathPiece>::iterator iterOldPiece12Path1 = chooseFlagPath1 ? pathListPath1.begin() : iterPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path1 = chooseFlagPath1 ? (++(pathListPath1.begin())) : iterPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path1;
	int weightOldPath1;

	//node id num,initial to be the last vprime point in path
	int loopEndNodeIdPath1 = chooseFlagPath1 ? (*(--(pathListPath1.end()))).getFirstNode() : INVALID;

	//old node id
	int node1Path1 = chooseFlagPath1 ? (*iterOldPiece12Path1).getFirstNode() : INVALID;
	int node2Path1 = chooseFlagPath1 ? (*iterOldPiece12Path1).getLastNode() : INVALID;
	int node3Path1 = chooseFlagPath1 ? (*iterOldPiece23Path1).getLastNode() : INVALID;
	int node4Path1;
	int node5Path1;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************
	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//for flag
	bool chooseFlagPath2 = (pathListPath2.size() >= PATHLISTSMINIMUM);
	int loopFlagPath2 = chooseFlagPath2 ? true : false;
	int changeFlagPath2 = PATH2NOTCHANGE;

	//for new path
	list<PathPiece>::iterator iterNewPiece13Path2;
	list<PathPiece>::iterator iterNewPiece42Path2;
	list<PathPiece>::iterator iterNewPiece25Path2;
	int weightNewPath2 = 0;

	//for old path
	list<PathPiece>::iterator iterOldPiece12Path2 = chooseFlagPath2 ? pathListPath2.begin() : iterPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path2 = chooseFlagPath2 ? (++(pathListPath2.begin())) : iterPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path2;
	int weightOldPath2;

	//node id num,initial to be the last vprime point in path
	int loopEndNodeIdPath2 = chooseFlagPath2 ? (*(--(pathListPath2.end()))).getFirstNode() : INVALID;

	//old node id
	int node1Path2 = chooseFlagPath2 ? (*iterOldPiece12Path2).getFirstNode() : INVALID;
	int node2Path2 = chooseFlagPath2 ? (*iterOldPiece12Path2).getLastNode() : INVALID;
	int node3Path2 = chooseFlagPath2 ? (*iterOldPiece23Path2).getLastNode() : INVALID;
	int node4Path2;
	int node5Path2;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************

		//reset change flag
		changeFlagPath1 = PATH1NOTCHANGE;

		loopCnt++;

		while (loopFlagPath1)//time complexity:O(100 * 100)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "3optFast1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the weight decrease of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			//string: node1 node2 node3
			string searchedNodes123StringPath1 = to_string(node1Path1) + to_string(node2Path1) + to_string(node3Path1);

			//find new piece13
			memset(commonColorPiece13, 0, sizeof(int)*(MAXPOINTNUM + 1));
			int piece13WeightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node3Path1, commonColorPiece13, pi13, false);
			
			//if we can not find a pathpiece13
			if (piece13WeightNewPath1 >= INFINITEWEIGHT)
			{
				iterOldPiece45Path1 = pathListPath1.end();
				goto endForLoopPath1;
			}
			else
			{
				//attention:must can find a pathpiece of dijkstra, if we can not find a pathpiece,this set func will get wrong
				setCommonColor(node1Path1, node3Path1, pi13, commonColorPiece13);
			}

			//note: here has a problem, when newpiece42 and newpiece25 has common vpoint, we need to redijkstra again in for loop!!!
			//find all piece25, attention: all !
			searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, distance25, commonColorPiece13, pi25, false);

			//find all piece42, attention: all !; actually we use reverse graph to find all dij path from node2 
			memset(pi42Reverse, 0xff, sizeof(int)*(MAXPOINTNUM));
			searchMultiPathPieceDijkstra(graphReverse, path1, path2Edge, node2Path1, distance42Reverse, commonColorPiece13, pi42Reverse, false);

			for (iterOldPiece45Path1 = pathListPath1.begin(); iterOldPiece45Path1 != pathListPath1.end(); ++iterOldPiece45Path1)
			{
				//get node4 and node5
				node4Path1 = (*iterOldPiece45Path1).getFirstNode();
				node5Path1 = (*iterOldPiece45Path1).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath1 = searchedNodes123StringPath1 + to_string(node4Path1) + to_string(node5Path1);
				static int searchedCntPath1 = 0;
				if (searchedRecordPath1.count(searchedNodesStringPath1) >= 1) 
				{ 
					searchedCntPath1++; 
					//cout << "searchedCntPath1:" << searchedCntPath1 << endl; 
					continue; 
				}
				else searchedRecordPath1.insert(searchedNodesStringPath1);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node4Path1).getSccNum())
				{
					iterOldPiece45Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node4 or node5, pass
				if ((node2Path1 == node4Path1) || (node2Path1 == node5Path1)) continue;

				//old weight sum of piece 12 23 45
				weightOldPath1 = node2WeightOldPath1 + computePathPieceWeight(graph, path1, iterOldPiece45Path1, path2Edge);

				//set used property false of oldpiece, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece45Path1);

				//because we have memcpy,we do not need to memset now
				//memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				//when can not find a pathpiece from node2 to node5, the distance will be very large
				weightNewPath1 = piece13WeightNewPath1 + distance25[node5Path1];

				if ((piece13WeightNewPath1 + distance42Reverse[node4Path1]) > weightOldPath1)
					weightNewPath1 = weightOldPath1 + 10000;

				if (weightNewPath1 < weightOldPath1)
				{
					//check whether newpiece42 and newpiece25 have common vpoint
					if (haveCommonNode(node2Path1, node5Path1, pi25, node2Path1, node4Path1, pi42Reverse))
					{
						memcpy(commonColor, commonColorPiece13, sizeof(int)*(MAXPOINTNUM + 1));
						setCommonColor(node2Path1, node5Path1, pi25, commonColor);

						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node4Path1, node2Path1, commonColor, pi42, false);
					}
					else
					{
						weightNewPath1 += distance42Reverse[node4Path1];

						//reverse pi42Reverse to pi42
						if (weightNewPath1 < weightOldPath1)
						{
							reversePi(node2Path1, node4Path1, pi42Reverse, pi42);
						}

					}
				}
#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath1 >= weightOldPath1))
				{
					path1.setPieceNodeTrue(graph, iterOldPiece45Path1);

					continue;//go to next iterOldPiece45Path1
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath1 = PATH1CHANGE;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceTwo2One(graph, path1, iterOldPiece12Path1, pi13);
					emplaceOne2Two(graph, path1, iterOldPiece45Path1, node2Path1, pi42, pi25);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 3;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt3 we optimize path1: insert node: " 
						<< node2Path1 << " between node: " << node4Path1 << " and node: " << node5Path1 << endl;
					path1.printPath();//print vprime node in order
					path1.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath1.begin()
					iterOldPiece45Path1 = pathListPath1.begin();

					//next loopEndNodeId and node2
					if (node3Path1 == path1.getVEnd()) node2Path1 = (*(path1.getPathList().begin())).getLastNode();
					else node2Path1 = node3Path1;				

					if (node1Path1 == path1.getVStart()) loopEndNodeIdPath1 = (*(--(path1.getPathList().end()))).getFirstNode();
					else loopEndNodeIdPath1 = node1Path1;			

					break;//go to next node2
				}

			}//for loop

#ifdef MEASURE
			cout << "3optFast1 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" <<endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			endForLoopPath1:

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece45Path1 == pathListPath1.end())
			{
				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//next new node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//compute loopflag
			loopFlagPath1 = (node2Path1 != loopEndNodeIdPath1);

		}//while (loopFlagPath1)

		//if only have 0 or 1 vprime point
		if (path1.getVPrimeSize() == 0)
		{
			weightOldPath1 = computePathPieceWeight(graph, path1, pathListPath1.begin(), path2Edge);

			path1.setPieceNodeFalse(graph, pathListPath1.begin());

			weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, path1.getVStart(), path1.getVEnd(), commonColor, pi13, false);

			if (weightNewPath1 < weightOldPath1)
			{
				emplaceOne2One(graph, path1, pathListPath1.begin(), pi13);
			}
			else
			{
				path1.setPieceNodeTrue(graph, pathListPath1.begin());
			}
		}
		else if (path1.getVPrimeSize() == 1)
		{
			for (auto iterOldPieceTempPath1 = pathListPath1.begin(); iterOldPieceTempPath1 != pathListPath1.end(); ++iterOldPieceTempPath1)
			{
				path1.setPieceNodeFalse(graph, iterOldPieceTempPath1);
			}

			node2Path1 = (*(pathListPath1.begin())).getLastNode();
			node4Path1 = path1.getVStart();
			node5Path1 = path1.getVEnd();
			weightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

			weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node4Path1, node2Path1, commonColor, pi42, false);

			if (weightNewPath1 < weightOldPath1)
			{
				setCommonColor(node4Path1, node2Path1, pi42, commonColor);
				weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node5Path1, commonColor, pi25, false);

			}
			
			//if weight do not reduce,restore used property of points
			if ((weightNewPath1 >= weightOldPath1))
			{
				path1.setPieceNodeTrue(graph, pathListPath1.begin());
				path1.setPieceNodeTrue(graph, (++(pathListPath1.begin())));

			}
			else//if weight reduce,create new piece and erase old piece,return true
			{
				//path change flag
				changeFlagPath1 = PATH1CHANGE;

				//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
				emplaceTwo2Two(graph, path1, pathListPath1.begin(), node2Path1, pi42, pi25);
			}
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		loopFlagPath1 = chooseFlagPath1 ? true : false;
		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************

		//reset change flag
		changeFlagPath2 = PATH2NOTCHANGE;

		loopCnt++;

		while (loopFlagPath2)//time complexity:O(100 * 100)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "3optFast2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the weight decrease of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			string searchedNodes123StringPath2 = to_string(node1Path2) + to_string(node2Path2) + to_string(node3Path2);

			//find new piece13
			memset(commonColorPiece13, 0, sizeof(int)*(MAXPOINTNUM + 1));
			int piece13WeightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node3Path2, commonColorPiece13, pi13, false);

			//if we can not find a pathpiece13
			if (piece13WeightNewPath2 >= INFINITEWEIGHT)
			{
				iterOldPiece45Path2 = pathListPath2.end();
				goto endForLoopPath2;
			}
			else
			{
				//attention:must can find a pathpiece of dijkstra, if we can not find a pathpiece,this set func will get wrong
				setCommonColor(node1Path2, node3Path2, pi13, commonColorPiece13);
			}

			//note: here has a problem, when newpiece42 and newpiece25 has common vpoint, we need to redijkstra again in for loop!!!
			//find all piece25, attention: all !
			searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, distance25, commonColorPiece13, pi25, false);

			//find all piece42, attention: all !; actually we use reverse graph to find all dij path from node2 
			memset(pi42Reverse, 0xff, sizeof(int)*(MAXPOINTNUM));
			searchMultiPathPieceDijkstra(graphReverse, path2, path1Edge, node2Path2, distance42Reverse, commonColorPiece13, pi42Reverse, false);

			for (iterOldPiece45Path2 = pathListPath2.begin(); iterOldPiece45Path2 != pathListPath2.end(); ++iterOldPiece45Path2)
			{
				//get node4 and node5
				node4Path2 = (*iterOldPiece45Path2).getFirstNode();
				node5Path2 = (*iterOldPiece45Path2).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath2 = searchedNodes123StringPath2 + to_string(node4Path2) + to_string(node5Path2);
				static int searchedCntPath2 = 0;
				if (searchedRecordPath2.count(searchedNodesStringPath2) >= 1) 
				{ 
					searchedCntPath2++; 
					//cout << "searchedCntPath2:" << searchedCntPath2 << endl; 
					continue; 
				}
				else searchedRecordPath2.insert(searchedNodesStringPath2);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node4Path2).getSccNum())
				{
					iterOldPiece45Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node4 or node5, pass
				if ((node2Path2 == node4Path2) || (node2Path2 == node5Path2)) continue;

				//old weight sum of piece 12 23 45
				weightOldPath2 = node2WeightOldPath2 + computePathPieceWeight(graph, path2, iterOldPiece45Path2, path1Edge);

				//set used property false of oldpiece, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece45Path2);

				//because we have memcpy,we do not need to memset now
				//memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				//when can not find a pathpiece from node2 to node5, the distance will be very large
				weightNewPath2 = piece13WeightNewPath2 + distance25[node5Path2];

				if ((piece13WeightNewPath2 + distance42Reverse[node4Path2]) > weightOldPath2)
					weightNewPath2 = weightOldPath2 + 10000;

				if (weightNewPath2 < weightOldPath2)
				{
					//check whether newpiece42 and newpiece25 have common vpoint
					if (haveCommonNode(node2Path2, node5Path2, pi25, node2Path2, node4Path2, pi42Reverse))
					{
						memcpy(commonColor, commonColorPiece13, sizeof(int)*(MAXPOINTNUM + 1));
						setCommonColor(node2Path2, node5Path2, pi25, commonColor);

						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node4Path2, node2Path2, commonColor, pi42, false);
					}
					else
					{
						weightNewPath2 += distance42Reverse[node4Path2];

						//reverse pi42Reverse to pi42
						if (weightNewPath2 < weightOldPath2)
						{
							reversePi(node2Path2, node4Path2, pi42Reverse, pi42);
						}

					}
				}
#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath2 >= weightOldPath2))
				{
					path2.setPieceNodeTrue(graph, iterOldPiece45Path2);

					continue;//go to next iterOldPiece45
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath2 = PATH2CHANGE;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceTwo2One(graph, path2, iterOldPiece12Path2, pi13);
					emplaceOne2Two(graph, path2, iterOldPiece45Path2, node2Path2, pi42, pi25);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 3;
					//assert(!(piHaveOtherPathEdge(graph, node1Path2, node3Path2, pi13, path1Edge)));
					//assert(!(piHaveOtherPathEdge(graph, node4Path2, node2Path2, pi42, path1Edge)));
					//assert(!(piHaveOtherPathEdge(graph, node2Path2, node5Path2, pi25, path1Edge)));
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt3 we optimize path2: insert node: "
						<< node2Path2 << " between node: " << node4Path2 << " and node: " << node5Path2 << endl;
					path2.printPath();//print vprime node in order
					path2.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath2.begin()
					iterOldPiece45Path2 = pathListPath2.begin();

					//next loopEndNodeId and node2
					if (node3Path2 == path2.getVEnd()) node2Path2 = (*(path2.getPathList().begin())).getLastNode();
					else node2Path2 = node3Path2;

					if (node1Path2 == path2.getVStart()) loopEndNodeIdPath2 = (*(--(path2.getPathList().end()))).getFirstNode();
					else loopEndNodeIdPath2 = node1Path2;

					break;//go to next node2
				}

			}

#ifdef MEASURE
			cout << "3optFast2 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

		endForLoopPath2:

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece45Path2 == pathListPath2.end())
			{
				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//next new node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//compute loopflag
			loopFlagPath2 = (node2Path2 != loopEndNodeIdPath2);

		}//while(loopFlagPath2)

		//if only have 0 or 1 vprime point
		if (path2.getVPrimeSize() == 0)
		{
			weightOldPath2 = computePathPieceWeight(graph, path2, pathListPath2.begin(), path1Edge);

			path2.setPieceNodeFalse(graph, pathListPath2.begin());

			weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, path2.getVStart(), path2.getVEnd(), commonColor, pi13, false);

			if (weightNewPath2 < weightOldPath2)
			{
				emplaceOne2One(graph, path2, pathListPath2.begin(), pi13);
			}
			else
			{
				path2.setPieceNodeTrue(graph, pathListPath2.begin());
			}
		}
		else if (path2.getVPrimeSize() == 1)
		{
			for (auto iterOldPieceTempPath2 = pathListPath2.begin(); iterOldPieceTempPath2 != pathListPath2.end(); ++iterOldPieceTempPath2)
			{
				path2.setPieceNodeFalse(graph, iterOldPieceTempPath2);
			}

			node2Path2 = (*(pathListPath2.begin())).getLastNode();
			node4Path2 = path2.getVStart();
			node5Path2 = path2.getVEnd();
			weightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

			weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node4Path2, node2Path2, commonColor, pi42, false);

			if (weightNewPath2 < weightOldPath2)
			{
				setCommonColor(node4Path2, node2Path2, pi42, commonColor);
				weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node5Path2, commonColor, pi25, false);

			}

			//if weight do not reduce,restore used property of points
			if ((weightNewPath2 >= weightOldPath2))
			{
				path2.setPieceNodeTrue(graph, pathListPath2.begin());
				path2.setPieceNodeTrue(graph, (++(pathListPath2.begin())));

			}
			else//if weight reduce,create new piece and erase old piece,return true
			{
				//path change flag
				changeFlagPath2 = PATH2CHANGE;

				//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
				emplaceTwo2Two(graph, path2, pathListPath2.begin(), node2Path2, pi42, pi25);
			}
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		loopFlagPath2 = chooseFlagPath2 ? true : false;
		if (changeFlagPath2 == PATH2NOTCHANGE) break;

	}

	return EXIT_SUCCESS;
}

//for example:
//node1 node2 node3     node7 node8 node9
//we exchange node2 and node8 in path
//node1 node8 node3     node7 node2 node9
//attention:because neighbour node switch is same as node piece switch,so we not consider it yet
int BetaOptimize::fourOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() >= FOUR_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5,
		PATHLISTSMINIMUM = 4
	};

	//for search path,save pi[]
	int pi18[MAXPOINTNUM];
	int pi83[MAXPOINTNUM];
	int pi72[MAXPOINTNUM];
	int pi29[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************

	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();

	//flag for loop
	bool chooseFlagPath1 = (pathListPath1.size() >= PATHLISTSMINIMUM);
	int loopFlagPath1 = chooseFlagPath1 ? true : false;
	int changeFlagPath1 = PATH1NOTCHANGE;
	int loopContinUnchgCntPath1 = 0;

	//node initialization
	int node1Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getFirstNode() : INVALID;
	int node2Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getLastNode() : INVALID;
	int node3Path1 = chooseFlagPath1 ? (*(++(pathListPath1.begin()))).getLastNode() : INVALID;
	int node7Path1 = INVALID;
	int node8Path1 = INVALID;
	int node9Path1 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece12Path1 = chooseFlagPath1 ? path1.getPathPieceOne(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path1 = chooseFlagPath1 ? path1.getPathPieceTwo(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece78Path1;
	list<PathPiece>::iterator iterOldPiece89Path1;
	int weightOldPath1;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece18Path1;
	list<PathPiece>::iterator iterNewPiece83Path1;
	list<PathPiece>::iterator iterNewPiece72Path1;
	list<PathPiece>::iterator iterNewPiece29Path1;
	int weightNewPath1 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************

	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//flag for loop
	bool chooseFlagPath2 = (pathListPath2.size() >= PATHLISTSMINIMUM);
	int loopFlagPath2 = chooseFlagPath2 ? true : false;
	int changeFlagPath2 = PATH2NOTCHANGE;
	int loopContinUnchgCntPath2 = 0;

	//node initialization
	int node1Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getFirstNode() : INVALID;
	int node2Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getLastNode() : INVALID;
	int node3Path2 = chooseFlagPath2 ? (*(++(pathListPath2.begin()))).getLastNode() : INVALID;
	int node7Path2 = INVALID;
	int node8Path2 = INVALID;
	int node9Path2 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterOldPiece12Path2 = chooseFlagPath2 ? path2.getPathPieceOne(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path2 = chooseFlagPath2 ? path2.getPathPieceTwo(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece78Path2;
	list<PathPiece>::iterator iterOldPiece89Path2;
	int weightOldPath2;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece18Path2;
	list<PathPiece>::iterator iterNewPiece83Path2;
	list<PathPiece>::iterator iterNewPiece72Path2;
	list<PathPiece>::iterator iterNewPiece29Path2;
	int weightNewPath2 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************
		//reset
		changeFlagPath1 = PATH1NOTCHANGE;
		loopContinUnchgCntPath1 = 0;

		loopCnt++;

		while (loopFlagPath1)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "4optSlow1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			string searchedNodes123StringPath1 = to_string(node1Path1) + to_string(node2Path1) + to_string(node3Path1);

			//traverse pathpiece to find node8
			for (iterOldPiece89Path1 = (++(pathListPath1.begin())); iterOldPiece89Path1 != pathListPath1.end(); ++iterOldPiece89Path1)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece78Path1 = iterOldPiece89Path1;
				--iterOldPiece78Path1;
				node7Path1 = (*iterOldPiece78Path1).getFirstNode();
				node8Path1 = (*iterOldPiece78Path1).getLastNode();
				node9Path1 = (*iterOldPiece89Path1).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath1 = searchedNodes123StringPath1 + to_string(node7Path1) + to_string(node8Path1) + to_string(node9Path1);
				if (searchedRecordPath1.count(searchedNodesStringPath1) >= 1) continue;
				else searchedRecordPath1.insert(searchedNodesStringPath1);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node8Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node8Path1).getSccNum())
				{
					iterOldPiece89Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node8 or node8 is neighboor of node2, pass
				if ((node8Path1 == node2Path1) || (node7Path1 == node2Path1) || (node9Path1 == node2Path1)) continue;

				//old weight sum of piece 12 23 78 89
				weightOldPath1 = node2WeightOldPath1 + computeVPrimeNodeWeight(graph, path1, node8Path1, path2Edge);

				//set used property false of oldpiece 78 89, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece78Path1);
				path1.setPieceNodeFalse(graph, iterOldPiece89Path1);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node8Path1, commonColor, pi18, false);

				if (weightNewPath1 < weightOldPath1)
				{
					setCommonColor(node1Path1, node8Path1, pi18, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node8Path1, node3Path1, commonColor, pi83, false);

					if (weightNewPath1 < weightOldPath1)
					{
						setCommonColor(node8Path1, node3Path1, pi83, commonColor);
						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node7Path1, node2Path1, commonColor, pi72, false);

						if (weightNewPath1 < weightOldPath1)
						{
							setCommonColor(node7Path1, node2Path1, pi72, commonColor);
							weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node9Path1, commonColor, pi29, false);
						}
					}
				}

#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath1 >= weightOldPath1))
				{
					setPieceNodeTrue(path1, iterOldPiece78Path1);
					setPieceNodeTrue(path1, iterOldPiece89Path1);

					continue;//go to next iterOldPiece78
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath1 = PATH1CHANGE;

					//change path, so reset to 0
					loopContinUnchgCntPath1 = 0;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceDoubleTwo2Two(graph, path1, node2Path1, node8Path1, pi18, pi83, pi72, pi29);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 4;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt4 we optimize path1: switch node: "
						<< node2Path1 << " node: " << node8Path1 << endl;
					path1.printPath();//print vprime node in order
					path1.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath1.begin()
					iterOldPiece89Path1 = pathListPath1.begin();

					//next node2
					if (node3Path1 == path1.getVEnd())
					{
						node2Path1 = (*(path1.getPathList().begin())).getLastNode();
					}
					else
					{
						node2Path1 = node3Path1;
					}

					break;//go to next node2
				}
			}//for loop

#ifdef MEASURE
			cout << "4optSlow1 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece89Path1 == pathListPath1.end())
			{
				loopContinUnchgCntPath1++;

				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//next new node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//judge whether continue or endloop
			loopFlagPath1 = (loopContinUnchgCntPath1 < path1.getVPrimeSize());
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		loopFlagPath1 = chooseFlagPath1 ? true : false;
		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************

		//reset
		changeFlagPath2 = PATH2NOTCHANGE;
		loopContinUnchgCntPath2 = 0;

		loopCnt++;

		while (loopFlagPath2)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "4optSlow2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			string searchedNodes123StringPath2 = to_string(node1Path2) + to_string(node2Path2) + to_string(node3Path2);

			//traverse pathpiece to find node8
			for (iterOldPiece89Path2 = (++(pathListPath2.begin())); iterOldPiece89Path2 != pathListPath2.end(); ++iterOldPiece89Path2)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece78Path2 = iterOldPiece89Path2;
				--iterOldPiece78Path2;
				node7Path2 = (*iterOldPiece78Path2).getFirstNode();
				node8Path2 = (*iterOldPiece78Path2).getLastNode();
				node9Path2 = (*iterOldPiece89Path2).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath2 = searchedNodes123StringPath2 + to_string(node7Path2) + to_string(node8Path2) + to_string(node9Path2);
				if (searchedRecordPath2.count(searchedNodesStringPath2) >= 1) continue;
				else searchedRecordPath2.insert(searchedNodesStringPath2);

				if (graph.getNode(node8Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;//don't get into to the range of sccNum,continue

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node8Path2).getSccNum())
				{
					iterOldPiece89Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node8 or node8 is neighboor of node2, pass
				if ((node8Path2 == node2Path2) || (node7Path2 == node2Path2) || (node9Path2 == node2Path2)) continue;

				//old weight sum of piece 12 23 78 89
				weightOldPath2 = node2WeightOldPath2 + computeVPrimeNodeWeight(graph, path2, node8Path2, path1Edge);

				//set used property false of oldpiece 78 89, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece78Path2);
				path2.setPieceNodeFalse(graph, iterOldPiece89Path2);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node8Path2, commonColor, pi18, false);

				if (weightNewPath2 < weightOldPath2)
				{
					setCommonColor(node1Path2, node8Path2, pi18, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node8Path2, node3Path2, commonColor, pi83, false);

					if (weightNewPath2 < weightOldPath2)
					{
						setCommonColor(node8Path2, node3Path2, pi83, commonColor);
						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node7Path2, node2Path2, commonColor, pi72, false);

						if (weightNewPath2 < weightOldPath2)
						{
							setCommonColor(node7Path2, node2Path2, pi72, commonColor);
							weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node9Path2, commonColor, pi29, false);
						}
					}
				}

#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath2 >= weightOldPath2))
				{
					setPieceNodeTrue(path2, iterOldPiece78Path2);
					setPieceNodeTrue(path2, iterOldPiece89Path2);

					continue;//go to next iterOldPiece78
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath2 = PATH2CHANGE;

					//change path, so reset to 0
					loopContinUnchgCntPath2 = 0;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceDoubleTwo2Two(graph, path2, node2Path2, node8Path2, pi18, pi83, pi72, pi29);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 4;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt4 we optimize path2: switch node: "
						<< node2Path2 << " node: " << node8Path2 << endl;
					path2.printPath();//print vprime node in order
					path2.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath2.begin()
					iterOldPiece89Path2 = pathListPath2.begin();

					//next node2
					if (node3Path2 == path2.getVEnd())
					{
						node2Path2 = (*(path2.getPathList().begin())).getLastNode();
					}
					else
					{
						node2Path2 = node3Path2;
					}

					break;//go to next node2
				}
			}//for loop

#ifdef MEASURE
			cout << "4optSlow2 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece89Path2 == pathListPath2.end())
			{
				loopContinUnchgCntPath2++;

				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//next new node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//judge whether continue or endloop
			loopFlagPath2 = (loopContinUnchgCntPath2 < path2.getVPrimeSize());
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		loopFlagPath2 = chooseFlagPath2 ? true : false;
		if ((changeFlagPath2 == PATH2NOTCHANGE) && (loopCnt > 1)) break;


	}

	return EXIT_SUCCESS;
}

//for example:
//node1 node2 node3     node7 node8 node9
//we exchange node2 and node8 in path
//node1 node8 node3     node7 node2 node9
//attention:because neighbour node switch is same as node piece switch,so we not consider it yet
int BetaOptimize::fourOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() < FOUR_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5,
		PATHLISTSMINIMUM = 4
	};

	//for search path,save pi[]
	int pi18[MAXPOINTNUM];
	int pi83[MAXPOINTNUM];
	int pi72[MAXPOINTNUM];
	int pi29[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************

	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();

	//flag for loop
	bool chooseFlagPath1 = (pathListPath1.size() >= PATHLISTSMINIMUM);
	int loopFlagPath1 = chooseFlagPath1 ? true : false;
	int changeFlagPath1 = PATH1NOTCHANGE;
	int loopContinUnchgCntPath1 = 0;

	//node initialization
	int node1Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getFirstNode() : INVALID;
	int node2Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getLastNode() : INVALID;
	int node3Path1 = chooseFlagPath1 ? (*(++(pathListPath1.begin()))).getLastNode() : INVALID;
	int node7Path1 = INVALID;
	int node8Path1 = INVALID;
	int node9Path1 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece12Path1 = chooseFlagPath1 ? path1.getPathPieceOne(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path1 = chooseFlagPath1 ? path1.getPathPieceTwo(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece78Path1;
	list<PathPiece>::iterator iterOldPiece89Path1;
	int weightOldPath1;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece18Path1;
	list<PathPiece>::iterator iterNewPiece83Path1;
	list<PathPiece>::iterator iterNewPiece72Path1;
	list<PathPiece>::iterator iterNewPiece29Path1;
	int weightNewPath1 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************

	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//flag for loop
	bool chooseFlagPath2 = (pathListPath2.size() >= PATHLISTSMINIMUM);
	int loopFlagPath2 = chooseFlagPath2 ? true : false;
	int changeFlagPath2 = PATH2NOTCHANGE;
	int loopContinUnchgCntPath2 = 0;

	//node initialization
	int node1Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getFirstNode() : INVALID;
	int node2Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getLastNode() : INVALID;
	int node3Path2 = chooseFlagPath2 ? (*(++(pathListPath2.begin()))).getLastNode() : INVALID;
	int node7Path2 = INVALID;
	int node8Path2 = INVALID;
	int node9Path2 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterOldPiece12Path2 = chooseFlagPath2 ? path2.getPathPieceOne(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path2 = chooseFlagPath2 ? path2.getPathPieceTwo(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece78Path2;
	list<PathPiece>::iterator iterOldPiece89Path2;
	int weightOldPath2;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece18Path2;
	list<PathPiece>::iterator iterNewPiece83Path2;
	list<PathPiece>::iterator iterNewPiece72Path2;
	list<PathPiece>::iterator iterNewPiece29Path2;
	int weightNewPath2 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************
		//reset
		changeFlagPath1 = PATH1NOTCHANGE;
		loopContinUnchgCntPath1 = 0;

		loopCnt++;

		while (loopFlagPath1)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "4optFast1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			string searchedNodes123StringPath1 = to_string(node1Path1) + to_string(node2Path1) + to_string(node3Path1);

			//traverse pathpiece to find node8
			for (iterOldPiece89Path1 = (++(pathListPath1.begin())); iterOldPiece89Path1 != pathListPath1.end(); ++iterOldPiece89Path1)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece78Path1 = iterOldPiece89Path1;
				--iterOldPiece78Path1;
				node7Path1 = (*iterOldPiece78Path1).getFirstNode();
				node8Path1 = (*iterOldPiece78Path1).getLastNode();
				node9Path1 = (*iterOldPiece89Path1).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath1 = searchedNodes123StringPath1 + to_string(node7Path1) + to_string(node8Path1) + to_string(node9Path1);
				if (searchedRecordPath1.count(searchedNodesStringPath1) >= 1) continue;
				else searchedRecordPath1.insert(searchedNodesStringPath1);

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node8Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node8Path1).getSccNum())
				{
					iterOldPiece89Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node8 or node8 is neighboor of node2, pass
				if ((node8Path1 == node2Path1) || (node7Path1 == node2Path1) || (node9Path1 == node2Path1)) continue;

				//old weight sum of piece 12 23 78 89
				weightOldPath1 = node2WeightOldPath1 + computeVPrimeNodeWeight(graph, path1, node8Path1, path2Edge);

				//set used property false of oldpiece 78 89, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece78Path1);
				path1.setPieceNodeFalse(graph, iterOldPiece89Path1);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node8Path1, commonColor, pi18, false);

				if (weightNewPath1 < weightOldPath1)
				{
					setCommonColor(node1Path1, node8Path1, pi18, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node8Path1, node3Path1, commonColor, pi83, false);

					if (weightNewPath1 < weightOldPath1)
					{
						setCommonColor(node8Path1, node3Path1, pi83, commonColor);
						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node7Path1, node2Path1, commonColor, pi72, false);

						if (weightNewPath1 < weightOldPath1)
						{
							setCommonColor(node7Path1, node2Path1, pi72, commonColor);
							weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node9Path1, commonColor, pi29, false);
						}
					}
				}

#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath1 >= weightOldPath1))
				{
					setPieceNodeTrue(path1, iterOldPiece78Path1);
					setPieceNodeTrue(path1, iterOldPiece89Path1);

					continue;//go to next iterOldPiece78
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath1 = PATH1CHANGE;

					//change path, so reset to 0
					loopContinUnchgCntPath1 = 0;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceDoubleTwo2Two(graph, path1, node2Path1, node8Path1, pi18, pi83, pi72, pi29);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 4;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt4 we optimize path1: switch node: "
						<< node2Path1 << " node: " << node8Path1 << endl;
					path1.printPath();//print vprime node in order
					path1.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath1.begin()
					iterOldPiece89Path1 = pathListPath1.begin();

					//next node2
					if (node3Path1 == path1.getVEnd())
					{
						node2Path1 = (*(path1.getPathList().begin())).getLastNode();
					}
					else
					{
						node2Path1 = node3Path1;
					}

					break;//go to next node2
				}
			}//for loop

#ifdef MEASURE
			cout << "4optFast1 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece89Path1 == pathListPath1.end())
			{
				loopContinUnchgCntPath1++;

				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//next new node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//judge whether continue or endloop
			loopFlagPath1 = (loopContinUnchgCntPath1 < path1.getVPrimeSize());
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		loopFlagPath1 = chooseFlagPath1 ? true : false;
		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************
		//reset
		changeFlagPath2 = PATH2NOTCHANGE;
		loopContinUnchgCntPath2 = 0;

		loopCnt++;

		while (loopFlagPath2)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "4optFast2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			string searchedNodes123StringPath2 = to_string(node1Path2) + to_string(node2Path2) + to_string(node3Path2);

			//traverse pathpiece to find node8
			for (iterOldPiece89Path2 = (++(pathListPath2.begin())); iterOldPiece89Path2 != pathListPath2.end(); ++iterOldPiece89Path2)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece78Path2 = iterOldPiece89Path2;
				--iterOldPiece78Path2;
				node7Path2 = (*iterOldPiece78Path2).getFirstNode();
				node8Path2 = (*iterOldPiece78Path2).getLastNode();
				node9Path2 = (*iterOldPiece89Path2).getLastNode();

				//check whether have been searched before
				string searchedNodesStringPath2 = searchedNodes123StringPath2 + to_string(node7Path2) + to_string(node8Path2) + to_string(node9Path2);
				if (searchedRecordPath2.count(searchedNodesStringPath2) >= 1) continue;
				else searchedRecordPath2.insert(searchedNodesStringPath2);

				if (graph.getNode(node8Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;//don't get into to the range of sccNum,continue

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node8Path2).getSccNum())
				{
					iterOldPiece89Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node8 or node8 is neighboor of node2, pass
				if ((node8Path2 == node2Path2) || (node7Path2 == node2Path2) || (node9Path2 == node2Path2)) continue;

				//old weight sum of piece 12 23 78 89
				weightOldPath2 = node2WeightOldPath2 + computeVPrimeNodeWeight(graph, path2, node8Path2, path1Edge);

				//set used property false of oldpiece 78 89, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece78Path2);
				path2.setPieceNodeFalse(graph, iterOldPiece89Path2);

				memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

				weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node8Path2, commonColor, pi18, false);

				if (weightNewPath2 < weightOldPath2)
				{
					setCommonColor(node1Path2, node8Path2, pi18, commonColor);//if we can not find a pathpiece,this set func will get wrong
					weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node8Path2, node3Path2, commonColor, pi83, false);

					if (weightNewPath2 < weightOldPath2)
					{
						setCommonColor(node8Path2, node3Path2, pi83, commonColor);
						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node7Path2, node2Path2, commonColor, pi72, false);

						if (weightNewPath2 < weightOldPath2)
						{
							setCommonColor(node7Path2, node2Path2, pi72, commonColor);
							weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node9Path2, commonColor, pi29, false);
						}
					}
				}

#endif//SEARCHPATHDIJKSTRA

				//if weight do not reduce,restore used property of points
				if ((weightNewPath2 >= weightOldPath2))
				{
					setPieceNodeTrue(path2, iterOldPiece78Path2);
					setPieceNodeTrue(path2, iterOldPiece89Path2);

					continue;//go to next iterOldPiece78
				}
				else//if weight reduce,create new piece and erase old piece,return true
				{
					//path change flag
					changeFlagPath2 = PATH2CHANGE;

					//change path, so reset to 0
					loopContinUnchgCntPath2 = 0;

					//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
					emplaceDoubleTwo2Two(graph, path2, node2Path2, node8Path2, pi18, pi83, pi72, pi29);

#ifdef MEASURE
					timeMeasure.timesUsefulDijkstra += 4;
#endif

#ifdef DEBUG
					cout << endl << endl << "@_@:opt4 we optimize path2: switch node: "
						<< node2Path2 << " node: " << node8Path2 << endl;
					path2.printPath();//print vprime node in order
					path2.printAllNodeInPath();//print all node in path
#endif

					//because iterOldPiece45 will be invalid,we set it to pathListPath2.begin()
					iterOldPiece89Path2 = pathListPath2.begin();

					//next node2
					if (node3Path2 == path2.getVEnd())
					{
						node2Path2 = (*(path2.getPathList().begin())).getLastNode();
					}
					else
					{
						node2Path2 = node3Path2;
					}

					break;//go to next node2
				}


			}//for loop

#ifdef MEASURE
			cout << "4optFast2 dijkstra time all: " << timeMeasure.timeAllDijkstra << "   cnt: " << timeMeasure.timesDijkstra << "   average time: " <<
				float((timeMeasure.timeAllDijkstra) / (float(timeMeasure.timesDijkstra))) << " ms" << endl;
			cout << "useful dijkstra times: " << timeMeasure.timesUsefulDijkstra << endl;
#endif

			//if node2 can not be inserted to other piece,restore to true
			if (iterOldPiece89Path2 == pathListPath2.end())
			{
				loopContinUnchgCntPath2++;

				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//next new node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}
			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//judge whether continue or endloop
			loopFlagPath2 = (loopContinUnchgCntPath2 < path2.getVPrimeSize());
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		loopFlagPath2 = chooseFlagPath2 ? true : false;
		if ((changeFlagPath2 == PATH2NOTCHANGE) && (loopCnt > 1)) break;


	}

	return EXIT_SUCCESS;
}

//we have a path like: s xx node1 node2 node3 xx xx node4 node5 node6 xx xx node7 node8 xx xx t
//after optimization : s xx node1 node3 xx xx node4 ndoe2 node6 xx xx node7 node5 node8 xx xx t
int BetaOptimize::fiveOptSlow(GraphVector &graph, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() >= FIVE_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5,
		PATHLISTSMINIMUM=5
	};

	//for search path,save pi[]
	int pi13[MAXPOINTNUM];
	int pi42[MAXPOINTNUM];
	int pi26[MAXPOINTNUM];
	int pi75[MAXPOINTNUM];
	int pi58[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************

	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();

	//flag for loop
	bool chooseFlagPath1 = (pathListPath1.size() >= PATHLISTSMINIMUM);
	int loopFlagPath1 = chooseFlagPath1 ? true : false;
	int changeFlagPath1 = PATH1NOTCHANGE;//show whether the path was optimized
	int loopContinUnchgCntPath1 = 0;
	bool FlagGoToNextNode2Path1 = false;//flag help us to jump out of two for loop

	//node initialization
	int node1Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getFirstNode() : INVALID;
	int node2Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getLastNode() : INVALID;
	int node3Path1 = chooseFlagPath1 ? (*(++(pathListPath1.begin()))).getLastNode() : INVALID;
	int node4Path1 = INVALID;
	int node5Path1 = INVALID;
	int node6Path1 = INVALID;
	int node7Path1 = INVALID;
	int node8Path1 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece12Path1 = chooseFlagPath1 ? path1.getPathPieceOne(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path1 = chooseFlagPath1 ? path1.getPathPieceTwo(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path1;
	list<PathPiece>::iterator iterOldPiece56Path1;
	list<PathPiece>::iterator iterOldPiece78Path1;
	int weightOldPath1 = 0;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece13Path1;
	list<PathPiece>::iterator iterNewPiece42Path1;
	list<PathPiece>::iterator iterNewPiece26Path1;
	list<PathPiece>::iterator iterNewPiece75Path1;
	list<PathPiece>::iterator iterNewPiece58Path1;
	int weightNewPath1 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************

	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//flag for loop
	bool chooseFlagPath2 = (pathListPath2.size() >= PATHLISTSMINIMUM);
	int loopFlagPath2 = chooseFlagPath2 ? true : false;
	int changeFlagPath2 = PATH2NOTCHANGE;
	int loopContinUnchgCntPath2 = 0;
	bool FlagGoToNextNode2Path2 = false;

	//node initialization
	int node1Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getFirstNode() : INVALID;
	int node2Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getLastNode() : INVALID;
	int node3Path2 = chooseFlagPath2 ? (*(++(pathListPath2.begin()))).getLastNode() : INVALID;
	int node4Path2 = INVALID;
	int node5Path2 = INVALID;
	int node6Path2 = INVALID;
	int node7Path2 = INVALID;
	int node8Path2 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterOldPiece12Path2 = chooseFlagPath2 ? path2.getPathPieceOne(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path2 = chooseFlagPath2 ? path2.getPathPieceTwo(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path2;
	list<PathPiece>::iterator iterOldPiece56Path2;
	list<PathPiece>::iterator iterOldPiece78Path2;
	int weightOldPath2 = 0;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece13Path2;
	list<PathPiece>::iterator iterNewPiece42Path2;
	list<PathPiece>::iterator iterNewPiece26Path2;
	list<PathPiece>::iterator iterNewPiece75Path2;
	list<PathPiece>::iterator iterNewPiece58Path2;
	int weightNewPath2 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************
		//reset
		changeFlagPath1 = PATH1NOTCHANGE;
		loopContinUnchgCntPath1 = 0;

		loopCnt++;

		while (loopFlagPath1)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "5optSlow1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			FlagGoToNextNode2Path1 = false;

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			//traverse pathpiece to find node5
			for (iterOldPiece56Path1 = (++(pathListPath1.begin())); iterOldPiece56Path1 != pathListPath1.end(); ++iterOldPiece56Path1)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece45Path1 = iterOldPiece56Path1;
				--iterOldPiece45Path1;
				node4Path1 = (*iterOldPiece45Path1).getFirstNode();
				node5Path1 = (*iterOldPiece45Path1).getLastNode();
				node6Path1 = (*iterOldPiece56Path1).getLastNode();

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node5Path1).getSccNum())
				{
					iterOldPiece56Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node5 or node5 is neighboor of node2, pass
				if ((node4Path1 == node2Path1) || (node5Path1 == node2Path1) || (node6Path1 == node2Path1)) continue;

				//compute the old weight of iterOldPiece12 and iterOldPiece23
				int node5WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node5Path1, path2Edge);

				//set used property false of oldpiece 45 56, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece45Path1);
				path1.setPieceNodeFalse(graph, iterOldPiece56Path1);

				//traverse pathpiece to find piece78
				for (iterOldPiece78Path1 = (pathListPath1.begin()); iterOldPiece78Path1 != pathListPath1.end(); ++iterOldPiece78Path1)
				{
					node7Path1 = (*iterOldPiece78Path1).getFirstNode();
					node8Path1 = (*iterOldPiece78Path1).getLastNode();

					//don't get into to the range of sccNum,continue
					if (graph.getNode(node8Path1).getSccNum() > graph.getNode(node5Path1).getSccNum()) continue;

					//exceeded the range of sccNum,break, should go to next node
					if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node7Path1).getSccNum())
					{
						iterOldPiece78Path1 = pathListPath1.end();
						break;
					}

					//if node2 == node5 or node5 is neighboor of node2, pass
					if ((node7Path1 == node1Path1) || (node7Path1 == node2Path1) || (node7Path1 == node4Path1) || (node7Path1 == node5Path1)) continue;

					//old weight sum of piece 12 23 45 56 78
					weightOldPath1 = node2WeightOldPath1 + node5WeightOldPath1 + computePathPieceWeight(graph, path1, iterOldPiece78Path1, path2Edge);

					//set used property false of oldpiece 78, for search path
					path1.setPieceNodeFalse(graph, iterOldPiece78Path1);

					//reset commonColor
					memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

					weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node3Path1, commonColor, pi13, false);

					if (weightNewPath1 < weightOldPath1)
					{
						setCommonColor(node1Path1, node3Path1, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node4Path1, node2Path1, commonColor, pi42, false);

						if (weightNewPath1 < weightOldPath1)
						{
							setCommonColor(node4Path1, node2Path1, pi42, commonColor);
							weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node6Path1, commonColor, pi26, false);

							if (weightNewPath1 < weightOldPath1)
							{
								setCommonColor(node2Path1, node6Path1, pi26, commonColor);
								weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node7Path1, node5Path1, commonColor, pi75, false);

								if (weightNewPath1 < weightOldPath1)
								{
									setCommonColor(node7Path1, node5Path1, pi75, commonColor);
									weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node5Path1, node8Path1, commonColor, pi58, false);
								}
							}
						}
					}

#endif//SEARCHPATHDIJKSTRA

					//if weight do not reduce,restore used property of points
					if ((weightNewPath1 >= weightOldPath1))
					{
						setPieceNodeTrue(path1, iterOldPiece78Path1);

						continue;//go to next iterOldPiece78
					}
					else//success, find a new path
					{
						//path change flag
						changeFlagPath1 = PATH1CHANGE;

						//change path, so reset to 0
						loopContinUnchgCntPath1 = 0;

						//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
						emplaceTwo2One(graph, path1, iterOldPiece12Path1, pi13);
						emplaceTwo2Two(graph, path1, iterOldPiece45Path1, node2Path1, pi42, pi26);
						emplaceOne2Two(graph, path1, iterOldPiece78Path1, node5Path1, pi75, pi58);

#ifdef DEBUG
						cout << endl << endl << "@_@:opt5 we optimize path1: replace node: "<< node5Path1 
							<< " with node: " << node2Path1 << " and insert between node: " << node7Path1 << " and " << node8Path1 << endl;
						cout << "weightOldPath1: " << weightOldPath1 << " weightNewPath1: " << weightNewPath1 << endl;
						path1.printPath();//print vprime node in order
						path1.printAllNodeInPath();//print all node in path
#endif

						//because iterOldPiece78  and iterOldPiece56Path1 will be invalid,we set it to pathListPath1.begin()
						iterOldPiece78Path1 = pathListPath1.begin();
						iterOldPiece56Path1 = pathListPath1.begin();

						//next node2
						if (node3Path1 == path1.getVEnd())
						{
							node2Path1 = (*(path1.getPathList().begin())).getLastNode();
						}
						else
						{
							node2Path1 = node3Path1;
						}

						FlagGoToNextNode2Path1 = true;
						break;//go to next node2
					}

				}//for loop oldPiece78

				if (FlagGoToNextNode2Path1) break;//go to next node2

				//if this node5 is not appropriate,restore to true
				if (iterOldPiece78Path1 == pathListPath1.end())
				{
					path1.setPieceNodeTrue(graph, iterOldPiece45Path1);
					path1.setPieceNodeTrue(graph, iterOldPiece56Path1);

					continue;//go to next node5
				}
			
			}//for loop oldPiece56

			//if this node2 is not appropriate,restore to true
			if (iterOldPiece56Path1 == pathListPath1.end())
			{
				loopContinUnchgCntPath1++;//@@@@@@@@@@@@@@@@@@@@@@@@

				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//go to next node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}

			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//judge whether continue or endloop
			loopFlagPath1 = (loopContinUnchgCntPath1 < path1.getVPrimeSize());//@@maybe time is too long,want to change this
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		loopFlagPath1 = chooseFlagPath1 ? true : false;
		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************

		//reset
		changeFlagPath2 = PATH2NOTCHANGE;
		loopContinUnchgCntPath2 = 0;

		loopCnt++;

		while (loopFlagPath2)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "5optSlow2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			FlagGoToNextNode2Path2 = false;

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			//traverse pathpiece to find node5
			for (iterOldPiece56Path2 = (++(pathListPath2.begin())); iterOldPiece56Path2 != pathListPath2.end(); ++iterOldPiece56Path2)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece45Path2 = iterOldPiece56Path2;
				--iterOldPiece45Path2;
				node4Path2 = (*iterOldPiece45Path2).getFirstNode();
				node5Path2 = (*iterOldPiece45Path2).getLastNode();
				node6Path2 = (*iterOldPiece56Path2).getLastNode();

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node5Path2).getSccNum())
				{
					iterOldPiece56Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node5 or node5 is neighboor of node2, pass
				if ((node4Path2 == node2Path2) || (node5Path2 == node2Path2) || (node6Path2 == node2Path2)) continue;

				//compute the old weight of iterOldPiece12 and iterOldPiece23
				int node5WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node5Path2, path1Edge);

				//set used property false of oldpiece 45 56, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece45Path2);
				path2.setPieceNodeFalse(graph, iterOldPiece56Path2);

				//traverse pathpiece to find piece78
				for (iterOldPiece78Path2 = (pathListPath2.begin()); iterOldPiece78Path2 != pathListPath2.end(); ++iterOldPiece78Path2)
				{
					node7Path2 = (*iterOldPiece78Path2).getFirstNode();
					node8Path2 = (*iterOldPiece78Path2).getLastNode();

					//don't get into to the range of sccNum,continue
					if (graph.getNode(node8Path2).getSccNum() > graph.getNode(node5Path2).getSccNum()) continue;

					//exceeded the range of sccNum,break, should go to next node
					if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node7Path2).getSccNum())
					{
						iterOldPiece78Path2 = pathListPath2.end();
						break;
					}

					//if node2 == node5 or node5 is neighboor of node2, pass
					if ((node7Path2 == node1Path2) || (node7Path2 == node2Path2) || (node7Path2 == node4Path2) || (node7Path2 == node5Path2)) continue;

					//old weight sum of piece 12 23 45 56 78
					weightOldPath2 = node2WeightOldPath2 + node5WeightOldPath2 + computePathPieceWeight(graph, path2, iterOldPiece78Path2, path1Edge);

					//set used property false of oldpiece 78, for search path
					path2.setPieceNodeFalse(graph, iterOldPiece78Path2);

					//reset commonColor
					memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

					weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node3Path2, commonColor, pi13, false);

					if (weightNewPath2 < weightOldPath2)
					{
						setCommonColor(node1Path2, node3Path2, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node4Path2, node2Path2, commonColor, pi42, false);

						if (weightNewPath2 < weightOldPath2)
						{
							setCommonColor(node4Path2, node2Path2, pi42, commonColor);
							weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node6Path2, commonColor, pi26, false);

							if (weightNewPath2 < weightOldPath2)
							{
								setCommonColor(node2Path2, node6Path2, pi26, commonColor);
								weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node7Path2, node5Path2, commonColor, pi75, false);

								if (weightNewPath2 < weightOldPath2)
								{
									setCommonColor(node7Path2, node5Path2, pi75, commonColor);
									weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node5Path2, node8Path2, commonColor, pi58, false);
								}
							}
						}
					}

#endif//SEARCHPATHDIJKSTRA

					//if weight do not reduce,restore used property of points
					if ((weightNewPath2 >= weightOldPath2))
					{
						setPieceNodeTrue(path2, iterOldPiece78Path2);

						continue;//go to next iterOldPiece78
					}
					else//success, find a new path
					{
						//path change flag
						changeFlagPath2 = PATH2CHANGE;

						//change path, so reset to 0
						loopContinUnchgCntPath2 = 0;

						//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
						emplaceTwo2One(graph, path2, iterOldPiece12Path2, pi13);
						emplaceTwo2Two(graph, path2, iterOldPiece45Path2, node2Path2, pi42, pi26);
						emplaceOne2Two(graph, path2, iterOldPiece78Path2, node5Path2, pi75, pi58);

#ifdef DEBUG
						cout << endl << endl << "@_@:opt5 we optimize path2: replace node: " << node5Path2
							<< " with node: " << node2Path2 << " and insert between node: " << node7Path2 << " and " << node8Path2 << endl;
						cout << "weightOldPath2: " << weightOldPath2 << " weightNewPath2: " << weightNewPath2 << endl;
						path2.printPath();//print vprime node in order
						path2.printAllNodeInPath();//print all node in path
#endif

						//because iterOldPiece78 and iterOldPiece56 will be invalid,we set it to pathListPath2.begin()
						iterOldPiece78Path2 = pathListPath2.begin();
						iterOldPiece56Path2 = pathListPath2.begin();

						//next node2
						if (node3Path2 == path2.getVEnd())
						{
							node2Path2 = (*(path2.getPathList().begin())).getLastNode();
						}
						else
						{
							node2Path2 = node3Path2;
						}

						FlagGoToNextNode2Path2 = true;
						break;//go to next node2
					}

				}//for loop oldPiece78

				if (FlagGoToNextNode2Path2) break;//go to next node2

				//if this node5 is not appropriate,restore to true
				if (iterOldPiece78Path2 == pathListPath2.end())
				{
					path2.setPieceNodeTrue(graph, iterOldPiece45Path2);
					path2.setPieceNodeTrue(graph, iterOldPiece56Path2);

					continue;//go to next node5
				}

			}//for loop oldPiece56

			//if this node2 is not appropriate,restore to true
			if (iterOldPiece56Path2 == pathListPath2.end())
			{
				loopContinUnchgCntPath2++;//@@@@@@@@@@@@@@@@@@@@@@@@

				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//go to next node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}

			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//judge whether continue or endloop
			loopFlagPath2 = (loopContinUnchgCntPath2 < path2.getVPrimeSize());//@@maybe time is too long,want to change this
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		loopFlagPath2 = chooseFlagPath2 ? true : false;
		if ((changeFlagPath2 == PATH2NOTCHANGE) && (loopCnt > 1)) break;

	}



	return EXIT_SUCCESS;
}

//we have a path like: s xx node1 node2 node3 xx xx node4 node5 node6 xx xx node7 node8 xx xx t
//after optimization : s xx node1 node3 xx xx node4 ndoe2 node6 xx xx node7 node5 node8 xx xx t
int BetaOptimize::fiveOptFast(GraphVector &graph, GraphVector &graphReverse, Path &path1, Path &path2, PathEdge &path1Edge, PathEdge &path2Edge)
{
	if (graph.getNodeNum() < FIVE_OPTBOUNDARIES) return EXIT_SUCCESS;

	//enum for flag
	enum enumFlag
	{
		PATH1NOTCHANGE = 2,
		PATH1CHANGE = 3,
		PATH2NOTCHANGE = 4,
		PATH2CHANGE = 5,
		PATHLISTSMINIMUM = 5
	};

	//for search path,save pi[]
	int pi13[MAXPOINTNUM];
	int pi42[MAXPOINTNUM];
	int pi26[MAXPOINTNUM];
	int pi75[MAXPOINTNUM];
	int pi58[MAXPOINTNUM];

	//common color in search path
	int commonColor[MAXPOINTNUM + 1];

	//times: sum of while(loopPath1) and  while(loopPath2)
	int loopCnt = 0;

	//************************for path1**********************

	//pathlist for path
	list<PathPiece>& pathListPath1 = path1.getPathList();

	//flag for loop
	bool chooseFlagPath1 = (pathListPath1.size() >= PATHLISTSMINIMUM);
	int loopFlagPath1 = chooseFlagPath1 ? true : false;
	int changeFlagPath1 = PATH1NOTCHANGE;//show whether the path was optimized
	int loopContinUnchgCntPath1 = 0;
	bool FlagGoToNextNode2Path1 = false;//flag help us to jump out of two for loop

	//node initialization
	int node1Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getFirstNode() : INVALID;
	int node2Path1 = chooseFlagPath1 ? (*(pathListPath1.begin())).getLastNode() : INVALID;
	int node3Path1 = chooseFlagPath1 ? (*(++(pathListPath1.begin()))).getLastNode() : INVALID;
	int node4Path1 = INVALID;
	int node5Path1 = INVALID;
	int node6Path1 = INVALID;
	int node7Path1 = INVALID;
	int node8Path1 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece12Path1 = chooseFlagPath1 ? path1.getPathPieceOne(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path1 = chooseFlagPath1 ? path1.getPathPieceTwo(node2Path1) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path1;
	list<PathPiece>::iterator iterOldPiece56Path1;
	list<PathPiece>::iterator iterOldPiece78Path1;
	int weightOldPath1 = 0;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece13Path1;
	list<PathPiece>::iterator iterNewPiece42Path1;
	list<PathPiece>::iterator iterNewPiece26Path1;
	list<PathPiece>::iterator iterNewPiece75Path1;
	list<PathPiece>::iterator iterNewPiece58Path1;
	int weightNewPath1 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath1;

	//************************for path2**********************

	//pathlist for path
	list<PathPiece>& pathListPath2 = path2.getPathList();

	//flag for loop
	bool chooseFlagPath2 = (pathListPath2.size() >= PATHLISTSMINIMUM);
	int loopFlagPath2 = chooseFlagPath2 ? true : false;
	int changeFlagPath2 = PATH2NOTCHANGE;
	int loopContinUnchgCntPath2 = 0;
	bool FlagGoToNextNode2Path2 = false;

	//node initialization
	int node1Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getFirstNode() : INVALID;
	int node2Path2 = chooseFlagPath2 ? (*(pathListPath2.begin())).getLastNode() : INVALID;
	int node3Path2 = chooseFlagPath2 ? (*(++(pathListPath2.begin()))).getLastNode() : INVALID;
	int node4Path2 = INVALID;
	int node5Path2 = INVALID;
	int node6Path2 = INVALID;
	int node7Path2 = INVALID;
	int node8Path2 = INVALID;

	//for old pathpiece
	list<PathPiece>::iterator iterOldPiece12Path2 = chooseFlagPath2 ? path2.getPathPieceOne(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece23Path2 = chooseFlagPath2 ? path2.getPathPieceTwo(node2Path2) : iterPathPieceNull;
	list<PathPiece>::iterator iterOldPiece45Path2;
	list<PathPiece>::iterator iterOldPiece56Path2;
	list<PathPiece>::iterator iterOldPiece78Path2;
	int weightOldPath2 = 0;

	//for new pathpiece
	list<PathPiece>::iterator iterNewPiece13Path2;
	list<PathPiece>::iterator iterNewPiece42Path2;
	list<PathPiece>::iterator iterNewPiece26Path2;
	list<PathPiece>::iterator iterNewPiece75Path2;
	list<PathPiece>::iterator iterNewPiece58Path2;
	int weightNewPath2 = 0;

	//save points that have been searched 
	unordered_set<string> searchedRecordPath2;

	while (true)
	{
		//************************for path1**********************
		//reset
		changeFlagPath1 = PATH1NOTCHANGE;
		loopContinUnchgCntPath1 = 0;

		loopCnt++;

		while (loopFlagPath1)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "5optFast1 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			FlagGoToNextNode2Path1 = false;

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node2Path1, path2Edge);

			//set used property false of old piece, for search path
			path1.setPieceNodeFalse(graph, iterOldPiece12Path1);
			path1.setPieceNodeFalse(graph, iterOldPiece23Path1);

			//traverse pathpiece to find node5
			for (iterOldPiece56Path1 = (++(pathListPath1.begin())); iterOldPiece56Path1 != pathListPath1.end(); ++iterOldPiece56Path1)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece45Path1 = iterOldPiece56Path1;
				--iterOldPiece45Path1;
				node4Path1 = (*iterOldPiece45Path1).getFirstNode();
				node5Path1 = (*iterOldPiece45Path1).getLastNode();
				node6Path1 = (*iterOldPiece56Path1).getLastNode();

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node2Path1).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path1).getSccNum() > graph.getNode(node5Path1).getSccNum())
				{
					iterOldPiece56Path1 = pathListPath1.end();
					break;
				}

				//if node2 == node5 or node5 is neighboor of node2, pass
				if ((node4Path1 == node2Path1) || (node5Path1 == node2Path1) || (node6Path1 == node2Path1)) continue;

				//compute the old weight of iterOldPiece12 and iterOldPiece23
				int node5WeightOldPath1 = computeVPrimeNodeWeight(graph, path1, node5Path1, path2Edge);

				//set used property false of oldpiece 45 56, for search path
				path1.setPieceNodeFalse(graph, iterOldPiece45Path1);
				path1.setPieceNodeFalse(graph, iterOldPiece56Path1);

				//traverse pathpiece to find piece78
				for (iterOldPiece78Path1 = (pathListPath1.begin()); iterOldPiece78Path1 != pathListPath1.end(); ++iterOldPiece78Path1)
				{
					node7Path1 = (*iterOldPiece78Path1).getFirstNode();
					node8Path1 = (*iterOldPiece78Path1).getLastNode();

					//don't get into to the range of sccNum,continue
					if (graph.getNode(node8Path1).getSccNum() > graph.getNode(node5Path1).getSccNum()) continue;

					//exceeded the range of sccNum,break, should go to next node
					if (graph.getNode(node5Path1).getSccNum() > graph.getNode(node7Path1).getSccNum())
					{
						iterOldPiece78Path1 = pathListPath1.end();
						break;
					}

					//if node2 == node5 or node5 is neighboor of node2, pass
					if ((node7Path1 == node1Path1) || (node7Path1 == node2Path1) || (node7Path1 == node4Path1) || (node7Path1 == node5Path1)) continue;

					//old weight sum of piece 12 23 45 56 78
					weightOldPath1 = node2WeightOldPath1 + node5WeightOldPath1 + computePathPieceWeight(graph, path1, iterOldPiece78Path1, path2Edge);

					//set used property false of oldpiece 78, for search path
					path1.setPieceNodeFalse(graph, iterOldPiece78Path1);

					//reset commonColor
					memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

					weightNewPath1 = searchMultiPathPieceDijkstra(graph, path1, path2Edge, node1Path1, node3Path1, commonColor, pi13, false);

					if (weightNewPath1 < weightOldPath1)
					{
						setCommonColor(node1Path1, node3Path1, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
						weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node4Path1, node2Path1, commonColor, pi42, false);

						if (weightNewPath1 < weightOldPath1)
						{
							setCommonColor(node4Path1, node2Path1, pi42, commonColor);
							weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node2Path1, node6Path1, commonColor, pi26, false);

							if (weightNewPath1 < weightOldPath1)
							{
								setCommonColor(node2Path1, node6Path1, pi26, commonColor);
								weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node7Path1, node5Path1, commonColor, pi75, false);

								if (weightNewPath1 < weightOldPath1)
								{
									setCommonColor(node7Path1, node5Path1, pi75, commonColor);
									weightNewPath1 += searchMultiPathPieceDijkstra(graph, path1, path2Edge, node5Path1, node8Path1, commonColor, pi58, false);
								}
							}
						}
					}

#endif//SEARCHPATHDIJKSTRA

					//if weight do not reduce,restore used property of points
					if ((weightNewPath1 >= weightOldPath1))
					{
						setPieceNodeTrue(path1, iterOldPiece78Path1);

						continue;//go to next iterOldPiece78
					}
					else//success, find a new path
					{
						//path change flag
						changeFlagPath1 = PATH1CHANGE;

						//change path, so reset to 0
						loopContinUnchgCntPath1 = 0;

						//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
						emplaceTwo2One(graph, path1, iterOldPiece12Path1, pi13);
						emplaceTwo2Two(graph, path1, iterOldPiece45Path1, node2Path1, pi42, pi26);
						emplaceOne2Two(graph, path1, iterOldPiece78Path1, node5Path1, pi75, pi58);

#ifdef DEBUG
						cout << endl << endl << "@_@:opt5 we optimize path1: replace node: " << node5Path1
							<< " with node: " << node2Path1 << " and insert between node: " << node7Path1 << " and " << node8Path1 << endl;
						cout << "weightOldPath1: " << weightOldPath1 << " weightNewPath1: " << weightNewPath1 << endl;
						path1.printPath();//print vprime node in order
						path1.printAllNodeInPath();//print all node in path
#endif

						//because iterOldPiece78  and iterOldPiece56Path1 will be invalid,we set it to pathListPath1.begin()
						iterOldPiece78Path1 = pathListPath1.begin();
						iterOldPiece56Path1 = pathListPath1.begin();

						//next node2
						if (node3Path1 == path1.getVEnd())
						{
							node2Path1 = (*(path1.getPathList().begin())).getLastNode();
						}
						else
						{
							node2Path1 = node3Path1;
						}

						FlagGoToNextNode2Path1 = true;
						break;//go to next node2
					}

				}//for loop oldPiece78

				if (FlagGoToNextNode2Path1) break;//go to next node2

				//if this node5 is not appropriate,restore to true
				if (iterOldPiece78Path1 == pathListPath1.end())
				{
					path1.setPieceNodeTrue(graph, iterOldPiece45Path1);
					path1.setPieceNodeTrue(graph, iterOldPiece56Path1);

					continue;//go to next node5
				}

			}//for loop oldPiece56

			//if this node2 is not appropriate,restore to true
			if (iterOldPiece56Path1 == pathListPath1.end())
			{
				loopContinUnchgCntPath1++;//@@@@@@@@@@@@@@@@@@@@@@@@

				path1.setPieceNodeTrue(graph, iterOldPiece12Path1);
				path1.setPieceNodeTrue(graph, iterOldPiece23Path1);

				//go to next node2
				if (node3Path1 == path1.getVEnd())
				{
					node2Path1 = (*(path1.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path1 = node3Path1;
				}

			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path1 = path1.getPathPieceOne(node2Path1);
			iterOldPiece23Path1 = path1.getPathPieceTwo(node2Path1);

			node1Path1 = (*iterOldPiece12Path1).getFirstNode();
			node3Path1 = (*iterOldPiece23Path1).getLastNode();

			//judge whether continue or endloop
			loopFlagPath1 = (loopContinUnchgCntPath1 < path1.getVPrimeSize());//@@maybe time is too long,want to change this
		}

		//refresh self path's pathedge
		path1Edge.refreshPathEdge(graph, path1, path2Edge);

		loopFlagPath1 = chooseFlagPath1 ? true : false;
		if ((changeFlagPath1 == PATH1NOTCHANGE) && (loopCnt > 1)) break;

		//************************for path2**********************
		//reset
		changeFlagPath2 = PATH2NOTCHANGE;
		loopContinUnchgCntPath2 = 0;

		loopCnt++;

		while (loopFlagPath2)
		{

#ifdef TIMELIMIT
			if ((getTime() - timeMeasure.startTime)>TIMELIMITNUM)
			{
				cout << "5optFast2 end for time out......" << (getTime() - timeMeasure.startTime) << endl;
				break;
			}
#endif

			FlagGoToNextNode2Path2 = false;

			//compute the old weight of iterOldPiece12 and iterOldPiece23
			int node2WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node2Path2, path1Edge);

			//set used property false of old piece, for search path
			path2.setPieceNodeFalse(graph, iterOldPiece12Path2);
			path2.setPieceNodeFalse(graph, iterOldPiece23Path2);

			//traverse pathpiece to find node5
			for (iterOldPiece56Path2 = (++(pathListPath2.begin())); iterOldPiece56Path2 != pathListPath2.end(); ++iterOldPiece56Path2)
			{
				//get node7 8 9 and pathpiece89
				iterOldPiece45Path2 = iterOldPiece56Path2;
				--iterOldPiece45Path2;
				node4Path2 = (*iterOldPiece45Path2).getFirstNode();
				node5Path2 = (*iterOldPiece45Path2).getLastNode();
				node6Path2 = (*iterOldPiece56Path2).getLastNode();

				//don't get into to the range of sccNum,continue
				if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node2Path2).getSccNum()) continue;

				//exceeded the range of sccNum,break, should go to next node
				if (graph.getNode(node2Path2).getSccNum() > graph.getNode(node5Path2).getSccNum())
				{
					iterOldPiece56Path2 = pathListPath2.end();
					break;
				}

				//if node2 == node5 or node5 is neighboor of node2, pass
				if ((node4Path2 == node2Path2) || (node5Path2 == node2Path2) || (node6Path2 == node2Path2)) continue;

				//compute the old weight of iterOldPiece12 and iterOldPiece23
				int node5WeightOldPath2 = computeVPrimeNodeWeight(graph, path2, node5Path2, path1Edge);

				//set used property false of oldpiece 45 56, for search path
				path2.setPieceNodeFalse(graph, iterOldPiece45Path2);
				path2.setPieceNodeFalse(graph, iterOldPiece56Path2);

				//traverse pathpiece to find piece78
				for (iterOldPiece78Path2 = (pathListPath2.begin()); iterOldPiece78Path2 != pathListPath2.end(); ++iterOldPiece78Path2)
				{
					node7Path2 = (*iterOldPiece78Path2).getFirstNode();
					node8Path2 = (*iterOldPiece78Path2).getLastNode();

					//don't get into to the range of sccNum,continue
					if (graph.getNode(node8Path2).getSccNum() > graph.getNode(node5Path2).getSccNum()) continue;

					//exceeded the range of sccNum,break, should go to next node
					if (graph.getNode(node5Path2).getSccNum() > graph.getNode(node7Path2).getSccNum())
					{
						iterOldPiece78Path2 = pathListPath2.end();
						break;
					}

					//if node2 == node5 or node5 is neighboor of node2, pass
					if ((node7Path2 == node1Path2) || (node7Path2 == node2Path2) || (node7Path2 == node4Path2) || (node7Path2 == node5Path2)) continue;

					//old weight sum of piece 12 23 45 56 78
					weightOldPath2 = node2WeightOldPath2 + node5WeightOldPath2 + computePathPieceWeight(graph, path2, iterOldPiece78Path2, path1Edge);

					//set used property false of oldpiece 78, for search path
					path2.setPieceNodeFalse(graph, iterOldPiece78Path2);

					//reset commonColor
					memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHDIJKSTRA

					weightNewPath2 = searchMultiPathPieceDijkstra(graph, path2, path1Edge, node1Path2, node3Path2, commonColor, pi13, false);

					if (weightNewPath2 < weightOldPath2)
					{
						setCommonColor(node1Path2, node3Path2, pi13, commonColor);//if we can not find a pathpiece,this set func will get wrong
						weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node4Path2, node2Path2, commonColor, pi42, false);

						if (weightNewPath2 < weightOldPath2)
						{
							setCommonColor(node4Path2, node2Path2, pi42, commonColor);
							weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node2Path2, node6Path2, commonColor, pi26, false);

							if (weightNewPath2 < weightOldPath2)
							{
								setCommonColor(node2Path2, node6Path2, pi26, commonColor);
								weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node7Path2, node5Path2, commonColor, pi75, false);

								if (weightNewPath2 < weightOldPath2)
								{
									setCommonColor(node7Path2, node5Path2, pi75, commonColor);
									weightNewPath2 += searchMultiPathPieceDijkstra(graph, path2, path1Edge, node5Path2, node8Path2, commonColor, pi58, false);
								}
							}
						}
					}

#endif//SEARCHPATHDIJKSTRA

					//if weight do not reduce,restore used property of points
					if ((weightNewPath2 >= weightOldPath2))
					{
						setPieceNodeTrue(path2, iterOldPiece78Path2);

						continue;//go to next iterOldPiece78
					}
					else//success, find a new path
					{
						//path change flag
						changeFlagPath2 = PATH2CHANGE;

						//change path, so reset to 0
						loopContinUnchgCntPath2 = 0;

						//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
						emplaceTwo2One(graph, path2, iterOldPiece12Path2, pi13);
						emplaceTwo2Two(graph, path2, iterOldPiece45Path2, node2Path2, pi42, pi26);
						emplaceOne2Two(graph, path2, iterOldPiece78Path2, node5Path2, pi75, pi58);

#ifdef DEBUG
						cout << endl << endl << "@_@:opt5 we optimize path2: replace node: " << node5Path2
							<< " with node: " << node2Path2 << " and insert between node: " << node7Path2 << " and " << node8Path2 << endl;
						cout << "weightOldPath2: " << weightOldPath2 << " weightNewPath2: " << weightNewPath2 << endl;
						path2.printPath();//print vprime node in order
						path2.printAllNodeInPath();//print all node in path
#endif

						//because iterOldPiece78 and iterOldPiece56 will be invalid,we set it to pathListPath2.begin()
						iterOldPiece78Path2 = pathListPath2.begin();
						iterOldPiece56Path2 = pathListPath2.begin();

						//next node2
						if (node3Path2 == path2.getVEnd())
						{
							node2Path2 = (*(path2.getPathList().begin())).getLastNode();
						}
						else
						{
							node2Path2 = node3Path2;
						}

						FlagGoToNextNode2Path2 = true;
						break;//go to next node2
					}

				}//for loop oldPiece78

				if (FlagGoToNextNode2Path2) break;//go to next node2

				//if this node5 is not appropriate,restore to true
				if (iterOldPiece78Path2 == pathListPath2.end())
				{
					path2.setPieceNodeTrue(graph, iterOldPiece45Path2);
					path2.setPieceNodeTrue(graph, iterOldPiece56Path2);

					continue;//go to next node5
				}

			}//for loop oldPiece56

			//if this node2 is not appropriate,restore to true
			if (iterOldPiece56Path2 == pathListPath2.end())
			{
				loopContinUnchgCntPath2++;//@@@@@@@@@@@@@@@@@@@@@@@@

				path2.setPieceNodeTrue(graph, iterOldPiece12Path2);
				path2.setPieceNodeTrue(graph, iterOldPiece23Path2);

				//go to next node2
				if (node3Path2 == path2.getVEnd())
				{
					node2Path2 = (*(path2.getPathList().begin())).getLastNode();
				}
				else
				{
					node2Path2 = node3Path2;
				}

			}

			//next node 1 3 and iterOldPiece12 and iterOldPiece23
			iterOldPiece12Path2 = path2.getPathPieceOne(node2Path2);
			iterOldPiece23Path2 = path2.getPathPieceTwo(node2Path2);

			node1Path2 = (*iterOldPiece12Path2).getFirstNode();
			node3Path2 = (*iterOldPiece23Path2).getLastNode();

			//judge whether continue or endloop
			loopFlagPath2 = (loopContinUnchgCntPath2 < path2.getVPrimeSize());//@@maybe time is too long,want to change this
		}

		//refresh self path's pathedge
		path2Edge.refreshPathEdge(graph, path2, path1Edge);

		loopFlagPath2 = chooseFlagPath2 ? true : false;
		if ((changeFlagPath2 == PATH2NOTCHANGE) && (loopCnt > 1)) break;

	}



	return EXIT_SUCCESS;
}

int BetaOptimize::optimizeNodeSwitch(GraphVector &graph,Path &path)
{
	int convergence = 0;//when convergence equals vprimesize,we get the optimal path
	int optimalFlag = 0;
	int timeUpFlag = 0;
	int timeCnt = 0;
	int curMaxNodeId = 0;
	int vPrimeSize = path.getVPrimeSize();

	//appoint algorthim
	//sAKArray[0] = SWITCHNODENODE;
	//sAKArray[1] = SWITCHNODEPIECE;
	sAKArray[0] = SWITCHNODEPIECE;
	sAKArray[1] = SWITCHNODENODE;
	switchAlgorithmKind = sAKArray[convergenceTimes];

	//reset nodefilter and pieceOrNodeFilter
	nodeFilter = INT_MAX;
	pieceOrNodeFilter = INT_MAX;
	memset(vPrimeColor, 0, sizeof(int)*MAXVPRIME);
	memset(secondNodeColor, 0, sizeof(int)*MAXVPRIME);
	memset(pieceColor, 0, sizeof(int)*(MAXVPRIME + 1));
	
	PathEdge otherPathEdgeNull;

	//use elastic algorithm to decrease the weight of path
	elasticOnePiece(graph, otherPathEdgeNull, path, false);

	while (1)
	{
		//the effect is as same as curMaxNodeId == INVALID
		//!!!!we can simplify this
		if (convergence == vPrimeSize)//we get optimal path
		{
			optimalFlag = 1;

			//convergenceTimes add one, and choose another algorthim to optimize
			convergenceTimes++;
			switchAlgorithmKind = sAKArray[convergenceTimes];

			//reset nodefilter and pieceOrNodeFilter
			//and also reset the corresponding color[]
			nodeFilter = INT_MAX;
			pieceOrNodeFilter = INT_MAX;
			memset(vPrimeColor, 0, sizeof(int)*MAXVPRIME);
			if (switchAlgorithmKind == SWITCHNODEPIECE) memset(pieceColor, 0, sizeof(int)*(MAXVPRIME + 1));
			else if (switchAlgorithmKind == SWITCHNODENODE) memset(secondNodeColor, 0, sizeof(int)*MAXVPRIME);
		}

		if (convergenceTimes == CONVERGENCETIMESLIMITS)
		{
			break;
		}

#ifndef DEBUG
		timeCnt++;
		if (!(timeCnt & 0x00000003))//every xx times,check time
		{
			timeMeasure.timeAll = getTime();
			if (timeMeasure.timeAll>9200)
			{
				timeUpFlag = 1;
				cout << "end for time out............................." << endl;
				break;
			}
		}
#endif
		//find max vprime point:weight of pieceone and piecetwo of this point is max
		//same path,a vprime point shouldn't be found twice,so we use filterNode
		curMaxNodeId = findMaxNode(path, nodeFilter);//可以加一个判断，当curmaxnodeid 为invalid 时退出循环
		if (curMaxNodeId == INVALID) break;

#ifdef DEBUG
		path.printPath();
		path.printNodeWeight();
		cout << "curMaxNodeId:" << curMaxNodeId << "  nodeFilter: " << nodeFilter;
		cout << endl;
#endif

		int switchTimes = 0;
		while (1)
		{
			list<PathPiece>::iterator iter = findMaxPieceOrSecNode(graph, path, curMaxNodeId, pieceOrNodeFilter);

#ifdef DEBUG
			if ((switchAlgorithmKind == SWITCHNODENODE) && (iter != path.getPathList().end()))
			{
				cout << "SecNode:" << (*iter).getLastNode() << "  pieceOrNodeFilter: " << pieceOrNodeFilter;
				cout << endl;
			}
#endif

			//time complexity:O(50)*O(4800*3)=(720,000)
			//if iter is valid: can find a pathpiece meetting requirements
			//if iter == path.getPathList().end(),means we can not find a piece
			if ((iter != path.getPathList().end()) && switchTimes < SWITCHTIMESLIMITS)
			{
				switchTimes++;
#ifdef DEBUG
				startTimeSwitchAlgorithm = getTime();
#endif
				int xx = switchAlgorithm(graph, path, curMaxNodeId, iter, (*iter).getLastNode());
#ifdef DEBUG
				timeMeasure.timeSwitchNode += (getTime()-startTimeSwitchAlgorithm);
				timeMeasure.exeTimesSwitchNode++;
#endif
				if (xx)
				{
					//reset nodefilter and pieceOrNodeFilter
					//and also reset the corresponding color[]
					nodeFilter = INT_MAX;
					pieceOrNodeFilter = INT_MAX;
					memset(vPrimeColor, 0, sizeof(int)*MAXVPRIME);
					if (switchAlgorithmKind == SWITCHNODEPIECE) memset(pieceColor, 0, sizeof(int)*(MAXVPRIME + 1));
					else if (switchAlgorithmKind == SWITCHNODENODE) memset(secondNodeColor, 0, sizeof(int)*MAXVPRIME);			
#ifdef DEBUG
					cout << "switchAlgorithm, get new path:" << endl;
					int weight = path.printPath();
#endif
					//reset convergence
					convergence = 0;
					break;
				}
				else
				{
					//refresh filter of pieceOrNodeFilter
					if (switchAlgorithmKind == SWITCHNODEPIECE)
					{
						pieceOrNodeFilter = (*iter).getWeight();
					}
					else if (switchAlgorithmKind == SWITCHNODENODE)
					{
						pieceOrNodeFilter = (*iter).getWeight() + (*(++iter)).getWeight();
#ifdef DEBUG
						cout << "not found, new pieceOrNodeFilter:" << pieceOrNodeFilter << endl;
#endif
					}
					continue;
				}
			}
			else//iter invalid: we should change a curMaxNodeId
			{
#ifdef DEBUG
				printTimeMeasure();
				cout << "convergence: " << convergence;
				cout << "   convergenceTimes: " << convergenceTimes << endl;
#endif
				//refresh filter of node
				auto iterTemp = path.getPathPieceOne(curMaxNodeId);
				nodeFilter = (*iterTemp).getWeight() + (*(++iterTemp)).getWeight();

				//change a curMaxNodeId,we should reset pieceOrNodeFilter
				//and also,we should reset piececolor[]
				pieceOrNodeFilter = INT_MAX;
				if (switchAlgorithmKind == SWITCHNODEPIECE) memset(pieceColor, 0, sizeof(int)*(MAXVPRIME + 1));
				else if (switchAlgorithmKind == SWITCHNODENODE) memset(secondNodeColor, 0, sizeof(int)*MAXVPRIME);

				convergence++;
				break;
			}
		}//inner while
	}//outer while

	//use elastic algorithm to decrease the weight of path
	//elasticOnePiece(graph, path, false);
	elasticOnePiece(graph, otherPathEdgeNull, path, true);

	if (timeUpFlag == 1)
	{
		return OUTOFTIME;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}

//here we have 4 nodes and 3 pieces named 
//path as follows:
//old path: node1 iterOldPiece12 node2 iterOldPiece23 node3     node4 iterOldPiece45 node5
//new path: node1 iterNewPiece13 node3     node4 iterNewPiece42 node2 iterNewPiece25 node5
int BetaOptimize::switchNodePiece(GraphVector &graph, Path &path, int node2, list<PathPiece>::iterator iterOldPiece45)
{
	//for old path
	Point point2 = graph.getNode(node2);
	list<PathPiece>::iterator iterOldPiece12 = path.getPathPieceOne(node2);
	list<PathPiece>::iterator iterOldPiece23 = path.getPathPieceTwo(node2);
	int node1 = (*iterOldPiece12).getFirstNode();
	int node3 = (*iterOldPiece23).getLastNode();
	int node4 = (*iterOldPiece45).getFirstNode();
	int node5 = (*iterOldPiece45).getLastNode();
	int weightOld = (*iterOldPiece12).getWeight() + (*iterOldPiece23).getWeight() + (*iterOldPiece45).getWeight();

	//for new path
	list<PathPiece>::iterator iterNewPiece13;
	list<PathPiece>::iterator iterNewPiece42;
	list<PathPiece>::iterator iterNewPiece25;
	int weightNew = 0;

	//for search path,save pi[]
	int pi13[MAXPOINTNUM];
	int pi42[MAXPOINTNUM];
	int pi25[MAXPOINTNUM];

	//set used property false of 3 oldpiece, for search path
	setPieceNodeFalse(path, iterOldPiece12);
	setPieceNodeFalse(path, iterOldPiece23);
	setPieceNodeFalse(path, iterOldPiece45);

	//有可能找不到path，这个时候回返回很大的权重值
	//O(4800)*3=O(14400)
#ifdef DEBUG
	startTimeDfs = getTime();
#endif
	//!!!very important,must initial color first
	memset(searchPieceColor, 0, sizeof(int)*(MAXPOINTNUM + 1));
	/*******************************************************************************************/
#ifdef SEARCHPATHBFS

	weightNew += searchPathPieceBfs(graph, node1, node3, pi13);

#ifdef DEBUG
	timeMeasure.exeTimesDfs++;//times add 1
#endif

	if (weightNew < weightOld)
	{
		setSearchPieceColor(node1, node3, pi13);//if we can not find a pathpiece,this set func will get wrong
		weightNew += searchPathPieceBfs(graph, node4, node2, pi42);

#ifdef DEBUG
		timeMeasure.exeTimesDfs++;//times add 1
#endif

		if (weightNew < weightOld)
		{
			setSearchPieceColor(node4, node2, pi42);
			weightNew += searchPathPieceBfs(graph, node2, node5, pi25);//do not need to set color of pi now

#ifdef DEBUG
			timeMeasure.exeTimesDfs++;//times add 1
#endif

		}
	}
	

#endif//SEARCHPATHBFS
	/*******************************************************************************************/

#ifdef SEARCHPATHDIJKSTRA

	//when we use MultiPathPieceDijkstra, we must initial commoncolor first
	PathEdge otherPathEdgeNull;
	weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node1, node3, searchPieceColor, pi13, false);

#ifdef DEBUG
	timeMeasure.exeTimesDfs++;//times add 1
#endif

	if (weightNew < weightOld)
	{
		setSearchPieceColor(node1, node3, pi13);//if we can not find a pathpiece,this set func will get wrong
		weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node4, node2, searchPieceColor, pi42, false);

#ifdef DEBUG
		timeMeasure.exeTimesDfs++;//times add 1
#endif

		if (weightNew < weightOld)
		{
			setSearchPieceColor(node4, node2, pi42);
			weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node2, node5, searchPieceColor, pi25, false);

#ifdef DEBUG
			timeMeasure.exeTimesDfs++;//times add 1
#endif

		}
	}
#endif//SEARCHPATHDIJKSTRA

#ifdef DEBUG
	timeMeasure.timeDfs += (getTime() - startTimeDfs);
#endif

	//if weight do not reduce,restore used property of points, return false
	if ((weightNew >= weightOld))
	{
		setPieceNodeTrue(path, iterOldPiece12);
		setPieceNodeTrue(path, iterOldPiece23);
		setPieceNodeTrue(path, iterOldPiece45);

		return false;
	}
	else//if weight reduce,create new piece and erase old piece,return true
	{
		//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
		emplaceTwo2One(graph, path, iterOldPiece12, pi13);
		emplaceOne2Two(graph, path, iterOldPiece45, node2, pi42, pi25);

		return true;
	}
}

//elastic algorithm,use dijkstra to make path weight less
//when enableused is true, we can use used points and repair cracked piece
int BetaOptimize::elasticOnePiece(GraphVector &graph, PathEdge &otherPathEdge, Path &path, int enableUsed)
{
	int weightNew = 0;
	int weightOld = 0;
	int firstNode = 0;
	int lastNode = 0;

	//if enable == true
	if (enableUsed == true)
	{
		int commonColor[MAXPOINTNUM+1];
		int piLocal[6][MAXPOINTNUM];
		int crackPieceLimits = 6;

		int numCrack = 0;//num of pieces need to be cracked in pi
		int updateSuccessFlag = false;
		float alpha = 0.1;//to decide whether to use this pi

		//traverse every piece
		auto iterEnd = path.getPathList().end();
		for (auto iterTemp = path.getPathList().begin(); iterTemp != iterEnd; ++iterTemp)
		{
			firstNode = (*iterTemp).getFirstNode();
			lastNode = (*iterTemp).getLastNode();
			weightOld = (*iterTemp).getWeight();
			updateSuccessFlag = true;

			//get pieceAverWeight,pair<weight,pieceNum>, can reduce this if need!!!
			auto pairTemp = path.getPathWeight(graph);
			float pieceAverWeight = (float)(pairTemp.first) / (float)(pairTemp.second);

			//for every piece, initial color
			memset(commonColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

			//set this piece used false for temporary
			path.setPieceNodeFalse(graph, iterTemp);

			//dijkstra to see whether weight can decrease, can use used Vpoints, not Vprime points
			//so this dijkstra will be more slowly than enableUsed==false
			weightNew = searchMultiPathPieceDijkstra(graph, path, otherPathEdge, firstNode, lastNode, commonColor, piLocal[0], true);

			//compute num of piece that will be cracked: numCrack, use an unsingned long long num is ok
			numCrack = path.cmNumToBeCracked(graph, firstNode, lastNode, piLocal[0]);

#ifdef DEBUG
			cout << "piece ,firstNode: " << firstNode << " lastNode: " << lastNode << " numCrack: " << numCrack << endl;
			cout << "weightOld - weightNew: " << weightOld - weightNew << endl;
			cout << "numCrack*pieceAverWeight*alpha: " << (((float)numCrack)*pieceAverWeight*alpha) << endl << endl;
#endif

			//compute weight decrease , oldWeight-newWeight
			//if (oldWeight-newWeight) > numCrack*pieceAverWeight*alpha and numcrack<6, means more likely to find new path
			if ((numCrack < crackPieceLimits) && ((float)(weightOld - weightNew) > (((float)numCrack)*pieceAverWeight*alpha)))
			{
				//get pieces need to be cracked, save iter to a set
				set<list<PathPiece>::iterator, compareListPieceIter> iterCrackPieceSet;
				path.savePiecesToBeCrackedInSet(graph, iterCrackPieceSet, firstNode, lastNode, piLocal[0]);

				//set these pieces nodes used false
				//compute weightOld, weight sum of associated piece
				for (auto iterXx = iterCrackPieceSet.begin(); iterXx != iterCrackPieceSet.end(); ++iterXx)
				{
					path.setPieceNodeFalse(graph, *iterXx);
					weightOld += (*(*iterXx)).getWeight();
				}

				//for loop, multi-dijkstra-noUsed to find multi-piece, remeber using commonColor and set commomColor,pi00-55
				int cntTemp = 0;
				path.setCommonColor(firstNode, lastNode, commonColor, piLocal[0]);

				//if all the new weight together decrease, update these pieces
				for (auto iterCrackPiece = iterCrackPieceSet.begin(); iterCrackPiece != iterCrackPieceSet.end(); ++iterCrackPiece)
				{
					firstNode = (*(*iterCrackPiece)).getFirstNode();
					lastNode = (*(*iterCrackPiece)).getLastNode();
					cntTemp++;

					weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdge, firstNode, lastNode, commonColor, piLocal[cntTemp], false);
					if (weightNew >= weightOld)
					{
						updateSuccessFlag = false;
						break;
					}
					
					//remember to set color,this is important
					path.setCommonColor(firstNode, lastNode, commonColor, piLocal[cntTemp]);
					
				}

				//if == true, means we can update pieces
				if (updateSuccessFlag == true)
				{
					//update iterTemp,delete old then create new
					firstNode = (*iterTemp).getFirstNode();
					lastNode = (*iterTemp).getLastNode();
					iterTemp = path.deleteOnePiece(graph, iterTemp, false);
					iterTemp = path.createPieceList(graph, firstNode, lastNode, iterTemp, piLocal[0]);

					//update other been cracked pieces
					int xx = 0;
					list<PathPiece>::iterator iterUpdateTemp;
					for (auto iterXx = iterCrackPieceSet.begin(); iterXx != iterCrackPieceSet.end(); ++iterXx)
					{
						xx++;
						firstNode = (*(*iterXx)).getFirstNode();
						lastNode = (*(*iterXx)).getLastNode();
						iterUpdateTemp = path.deleteOnePiece(graph, (*iterXx), false);
						iterUpdateTemp = path.createPieceList(graph, firstNode, lastNode, iterUpdateTemp, piLocal[xx]);
					}

#ifdef DEBUG
					cout << "update piece ,firstNode: " << firstNode << " lastNode: " << lastNode << endl;
#endif

				}
				else
				{
					//set this piece node used true 
					path.setPieceNodeTrue(graph, iterTemp);

					//or not, set these pieces used node true, go on to next piece
					for (auto iterSetUsedTrue = iterCrackPieceSet.begin(); iterSetUsedTrue != iterCrackPieceSet.end(); ++iterSetUsedTrue)
					{
						path.setPieceNodeTrue(graph, *iterSetUsedTrue);
					}

#ifdef DEBUG
					cout << "do not update piece ,firstNode: " << firstNode << " lastNode: " << lastNode << endl;
#endif

				}
				
			}
			else
			{
				//set this piece node used true 
				path.setPieceNodeTrue(graph, iterTemp);

				//go on to next piece
				continue;
			}
			
		}//for loop

		return EXIT_SUCCESS;

	}
	else
	{
		int piLocal[MAXPOINTNUM];

		//traverse every piece
		auto iterEnd = path.getPathList().end();
		for (auto iterTemp = path.getPathList().begin(); iterTemp != iterEnd; ++iterTemp)
		{
			weightOld = (*iterTemp).getWeight();
			firstNode = (*iterTemp).getFirstNode();
			lastNode = (*iterTemp).getLastNode();

			//set piece vpoint use false
			path.setPieceNodeFalse(graph, iterTemp);

			//dijkstra to see whether the weight decrease
			weightNew = searchPathPieceDijkstra(graph, path, otherPathEdge, firstNode, lastNode, piLocal, false);

			if (weightNew < weightOld)
			{
				//if decrease update piece
				iterTemp = path.deleteOnePiece(graph, iterTemp, true);

				//insert new piece
				iterTemp = path.createPieceList(graph, firstNode, lastNode, iterTemp, piLocal);
			}
			else
			{
				//if not, set piece vpoint true, go on
				path.setPieceNodeTrue(graph, iterTemp);

				continue;
			}

		}

		return EXIT_SUCCESS;
	}

}

//for example:
//node1 node2 node3     node7 node8 node9
//we exchange node2 and node8 in path
//node1 node8 node3     node7 node2 node9
//attention:because neighbour node switch is same as node piece switch,so we not consider it yet
int BetaOptimize::switchNodeNode(GraphVector &graph, Path &path, int curMaxNodeId, int secMaxNode)
{
	//for old path
	Point &pointNode2 = graph.getNode(curMaxNodeId);
	Point &pointNode8 = graph.getNode(secMaxNode);
	list<PathPiece>::iterator iterOldPiece12 = path.getPathPieceOne(curMaxNodeId);
	list<PathPiece>::iterator iterOldPiece23 = path.getPathPieceTwo(curMaxNodeId);
	list<PathPiece>::iterator iterOldPiece78 = path.getPathPieceOne(secMaxNode);
	list<PathPiece>::iterator iterOldPiece89 = path.getPathPieceTwo(secMaxNode);
	int node1 = (*iterOldPiece12).getFirstNode();
	int node2 = curMaxNodeId;
	int node3 = (*iterOldPiece23).getLastNode();
	int node7 = (*iterOldPiece78).getFirstNode();
	int node8 = secMaxNode;
	int node9 = (*iterOldPiece89).getLastNode();
	int weightOld = (*iterOldPiece12).getWeight() + (*iterOldPiece23).getWeight() 
		+ (*iterOldPiece78).getWeight() + (*iterOldPiece89).getWeight();

	//for new path
	list<PathPiece>::iterator iterNewPiece18;
	list<PathPiece>::iterator iterNewPiece83;
	list<PathPiece>::iterator iterNewPiece72;
	list<PathPiece>::iterator iterNewPiece29;
	int weightNew = 0;

	//for search path,save pi[]
	int pi18[MAXPOINTNUM];
	int pi83[MAXPOINTNUM];
	int pi72[MAXPOINTNUM];
	int pi29[MAXPOINTNUM];

	//set used property false of 4 oldpiece, for search path
	setPieceNodeFalse(path, iterOldPiece12);
	setPieceNodeFalse(path, iterOldPiece23);
	setPieceNodeFalse(path, iterOldPiece78);
	setPieceNodeFalse(path, iterOldPiece89);

	//有可能找不到path，这个时候回返回很大的权重值
	//O(4800)*4=O(19200)
	//!!!very important,must initial color first
	memset(searchPieceColor, 0, sizeof(int)*(MAXPOINTNUM + 1));

#ifdef SEARCHPATHBFS

	weightNew += searchPathPieceBfs(graph, node1, node8, pi18);

	if (weightNew < weightOld)
	{
		setSearchPieceColor(node1, node8, pi18);//if we can not find a pathpiece,this set func will get wrong
		weightNew += searchPathPieceBfs(graph, node8, node3, pi83);

		if (weightNew < weightOld)
		{
			setSearchPieceColor(node8, node3, pi83);
			weightNew += searchPathPieceBfs(graph, node7, node2, pi72);//do not need to set color of pi now

			if (weightNew < weightOld)
			{
				setSearchPieceColor(node7, node2, pi72);
				weightNew += searchPathPieceBfs(graph, node2, node9, pi29);//do not need to set color of pi now
			}
		}
	}

#endif//SEARCHPATHBFS

#ifdef SEARCHPATHDIJKSTRA

	PathEdge otherPathEdgeNull;
	weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node1, node8, searchPieceColor, pi18, false);

	if (weightNew < weightOld)
	{
		setSearchPieceColor(node1, node8, pi18);//if we can not find a pathpiece,this set func will get wrong
		weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node8, node3, searchPieceColor, pi83, false);

		if (weightNew < weightOld)
		{
			setSearchPieceColor(node8, node3, pi83);
			weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node7, node2, searchPieceColor, pi72, false);//do not need to set color of pi now

			if (weightNew < weightOld)
			{
				setSearchPieceColor(node7, node2, pi72);
				weightNew += searchMultiPathPieceDijkstra(graph, path, otherPathEdgeNull, node2, node9, searchPieceColor, pi29, false);//do not need to set color of pi now
			}
		}
	}

#endif//SEARCHPATHBFS

#ifdef DEBUG
	cout << "weightNew: " << weightNew << "  weightOld: " << weightOld << endl;
#endif

	//if weight do not reduce,restore used property of points, return false
	if ((weightNew >= weightOld))
	{
		setPieceNodeTrue(path, iterOldPiece12);
		setPieceNodeTrue(path, iterOldPiece23);
		setPieceNodeTrue(path, iterOldPiece78);
		setPieceNodeTrue(path, iterOldPiece89);

		return false;
	}
	else//if weight reduce,create new piece and erase old piece,return true
	{
		//attention 1 set points' properties(used/pieceone/piecetwo);2 set pieces' properties;3 delete unused old pieces 
		emplaceDoubleTwo2Two(graph, path, node2, node8, pi18, pi83, pi72, pi29);

		return true;
	}
}

//check whether have common node of two pi[]
bool BetaOptimize::haveCommonNode(int firstNode1, int lastNode1, int *pi1, int firstNode2, int lastNode2, int *pi2)
{
	unordered_set<int> usedNodesInPi1;

	int uu1;
	int vv1;
	int uu2;
	int vv2;

	vv1 = lastNode1;
	uu1 = pi1[vv1];

	while (uu1 != firstNode1)
	{
		//insert to set, we donot need to insert firstnode and lastnode
		usedNodesInPi1.insert(uu1);

		vv1 = uu1;
		uu1 = pi1[vv1];
	}

	vv2 = lastNode2;
	uu2 = pi2[vv2];

	while (uu2 != firstNode2)
	{
		if (usedNodesInPi1.count(uu2) >= 1) return true;

		vv2 = uu2;
		uu2 = pi2[vv2];
	}

	return false;
}

bool BetaOptimize::piHaveOtherPathEdge(GraphVector &graph, int firstNode, int lastNode, int *pi, PathEdge &otherPathEdge)
{
	int uu;
	int vv;

	vv = lastNode;
	uu = pi[vv];

	while (uu != firstNode)
	{
		//if otherpath has (uu,vv) and graph only have one edge of (uu,vv) 
		if ((otherPathEdge.hasEdge(uu, vv)) && (graph.getEdge(uu, vv).getEdgeId(1) == INVALID))
		{
			cout << "have common edge in piece firstnode:"<<firstNode<<" laseNode:"<<lastNode<<" edge uu,vv of:"<< uu << " " << vv << endl;
			return true;
		}
			

		vv = uu;
		uu = pi[vv];
	}

	if ((otherPathEdge.hasEdge(uu, vv)) && (graph.getEdge(uu, vv).getEdgeId(1) == INVALID))
	{
		cout << "have common edge in piece firstnode:" << firstNode << " laseNode:" << lastNode << " edge uu,vv of:" << uu << " " << vv << endl;
		return true;
	}
	else return false;
}

//将某条pathpiece换成另外一条pathpiece
int BetaOptimize::emplaceOne2One(GraphVector &graph, Path &path, list<PathPiece>::iterator iter, int *pi)
{
	int firstNode = (*iter).getFirstNode();
	int lastNode = (*iter).getLastNode();

	//create piece and insert
	//set pathpiece
	PathPiece pathPiece;
	pathPiece.setFirstNode(firstNode);
	pathPiece.setLastNode(lastNode);
	if (pi[lastNode] == lastNode) pathPiece.setLinkProperty(CRACKED);
	else pathPiece.setLinkProperty(LINKED);
	int weight = 0;

	auto iterNewPiece = path.getPathList().insert(iter, pathPiece);

	//set point used,set pathpieceone
	//compute weight
	if (pi[lastNode] == lastNode)//cracked path
	{
		(*iterNewPiece).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uu = pi[lastNode];
		int vv = lastNode;
		auto &linkNodesVector = (*iterNewPiece).getLinkNodesVector();
		while (uu != firstNode)
		{
			//for piece
			//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
			weight = INVALID;//do not comput weight,do not have static weight
			linkNodesVector.push_back(uu);

			//for point
			path.setUsed(uu, true);
			path.setPathPieceOne(uu, iterNewPiece);

			//next
			vv = uu;
			uu = pi[uu];
		}
		//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
		weight = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece).setWeight(weight);//set weight
	}

	//set point:for lastNode,set pathpiece one
	path.setUsed(lastNode, true);
	path.setPathPieceOne(lastNode, iterNewPiece);

	//set point:for firstNode,set pathpiece two
	path.setPathPieceTwo(firstNode, iterNewPiece);

	//delete the replaced piece
	path.getPathList().erase(iter);

	return EXIT_SUCCESS;
}

//将iter和++iter两个piece，换成piL和piR记录的左右两个piece,新piece的中间点是middleNode
//for example,we have path        : s node2 xx xx node4 node5 node6 xx xx t
//after empalce,we  have new path : s xx xx node4 node2 node6 xx xx t
//attention: in this func, we do not need to concern where node5 goes
int BetaOptimize::emplaceTwo2Two(GraphVector &graph, Path &path, list<PathPiece>::iterator iter, int node2, int *pi42, int *pi26)
{
	list<PathPiece>::iterator iterOldPiece45 = iter;
	list<PathPiece>::iterator iterOldPiece56 = iter;
	++iterOldPiece56;

	int node4 = (*iterOldPiece45).getFirstNode();
	int node5 = (*iterOldPiece45).getLastNode();
	int node6 = (*iterOldPiece56).getLastNode();

	//create piece and insert
	//set pathpiece
	PathPiece pathPiece42;
	pathPiece42.setFirstNode(node4);
	pathPiece42.setLastNode(node2);
	if (pi42[node2] == node2) pathPiece42.setLinkProperty(CRACKED);
	else pathPiece42.setLinkProperty(LINKED);
	int weight42 = 0;

	PathPiece pathPiece26;
	pathPiece26.setFirstNode(node2);
	pathPiece26.setLastNode(node6);
	if (pi26[node6] == node6) pathPiece26.setLinkProperty(CRACKED);
	else pathPiece26.setLinkProperty(LINKED);
	int weight26 = 0;

	auto iterNewPiece26 = path.getPathList().insert(iterOldPiece45, pathPiece26);
	auto iterNewPiece42 = path.getPathList().insert(iterNewPiece26, pathPiece42);

	//set point used,set pathpieceone
	//compute weight
	if (pi42[node2] == node2)//cracked path
	{
		(*iterNewPiece42).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uu42 = pi42[node2];
		int vv42 = node2;
		auto &linkNodesVector42 = (*iterNewPiece42).getLinkNodesVector();
		while (uu42 != node4)
		{
			//for piece
			//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
			weight42 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector42.push_back(uu42);

			//for point
			path.setUsed(uu42, true);
			path.setPathPieceOne(uu42, iterNewPiece42);

			//next
			vv42 = uu42;
			uu42 = pi42[uu42];
		}
		//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
		weight42 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece42).setWeight(weight42);//set weight
	}

	if (pi26[node6] == node6)//cracked path
	{
		(*iterNewPiece26).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uu26 = pi26[node6];
		int vv26 = node6;
		auto &linkNodesVector26 = (*iterNewPiece26).getLinkNodesVector();
		while (uu26 != node2)
		{
			//for piece
			//weightR += graph.getEdge(uuR, vvR).getEdgeWeight();
			weight26 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector26.push_back(uu26);

			//for point
			path.setUsed(uu26, true);
			path.setPathPieceOne(uu26, iterNewPiece26);

			//next
			vv26 = uu26;
			uu26 = pi26[uu26];
		}
		//weightR += graph.getEdge(uuR, vvR).getEdgeWeight();
		weight26 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece26).setWeight(weight26);//set weight
	}

	//set point:for node2,set pathpiece one and pathpiecetwo
	path.setUsed(node2, true);
	path.setPathPieceOne(node2, iterNewPiece42);
	path.setPathPieceTwo(node2, iterNewPiece26);

	//set point:set node4 and node6 pathpiece
	path.setPathPieceTwo(node4, iterNewPiece42);
	path.setPathPieceOne(node6, iterNewPiece26);

	//delete the replaced two piece
	path.getPathList().erase(iterOldPiece45);
	path.getPathList().erase(iterOldPiece56);

	return EXIT_SUCCESS;

}

//exchange position of leftnode and rightnode in path
//for example:
//node1 node2 node3     node7 node8 node9
//we exchange node2 and node8 in path
//node1 node8 node3     node7 node2 node9
int BetaOptimize::emplaceDoubleTwo2Two(GraphVector &graph, Path &path, int node2, int node8, int *pi18, int *pi83, int *pi72, int *pi29)
{

	Point &pointNode2 = graph.getNode(node2);
	Point &pointNode8 = graph.getNode(node8);
	list<PathPiece>::iterator iterOldPiece12 = path.getPathPieceOne(node2);
	list<PathPiece>::iterator iterOldPiece23 = path.getPathPieceTwo(node2);
	list<PathPiece>::iterator iterOldPiece78 = path.getPathPieceOne(node8);
	list<PathPiece>::iterator iterOldPiece89 = path.getPathPieceTwo(node8);
	int node1 = (*iterOldPiece12).getFirstNode();
	int node3 = (*iterOldPiece23).getLastNode();
	int node7 = (*iterOldPiece78).getFirstNode();
	int node9 = (*iterOldPiece89).getLastNode();

	//reset point,four piece have been setted false
	//actually,if do not set vprime points,it does not matter
	path.setUsed(node2, false);
	path.setUsed(node8, false);

	//create 4 new piece ,and insert it
	//set piece
	PathPiece pathPiece18;
	pathPiece18.setFirstNode(node1);
	pathPiece18.setLastNode(node8);
	if (pi18[node8] == node8) pathPiece18.setLinkProperty(CRACKED);
	else pathPiece18.setLinkProperty(LINKED);
	int weight18 = 0;

	PathPiece pathPiece83;
	pathPiece83.setFirstNode(node8);
	pathPiece83.setLastNode(node3);
	if (pi83[node3] == node3) pathPiece83.setLinkProperty(CRACKED);
	else pathPiece83.setLinkProperty(LINKED);
	int weight83 = 0;

	PathPiece pathPiece72;
	pathPiece72.setFirstNode(node7);
	pathPiece72.setLastNode(node2);
	if (pi72[node2] == node2) pathPiece72.setLinkProperty(CRACKED);
	else pathPiece72.setLinkProperty(LINKED);
	int weight72 = 0;

	PathPiece pathPiece29;
	pathPiece29.setFirstNode(node2);
	pathPiece29.setLastNode(node9);
	if (pi29[node9] == node9) pathPiece29.setLinkProperty(CRACKED);
	else pathPiece29.setLinkProperty(LINKED);
	int weight29 = 0;

	auto iterNewPiece83 = path.getPathList().insert(iterOldPiece12, pathPiece83);
	auto iterNewPiece18 = path.getPathList().insert(iterNewPiece83, pathPiece18);

	auto iterNewPiece29 = path.getPathList().insert(iterOldPiece78, pathPiece29);
	auto iterNewPiece72 = path.getPathList().insert(iterNewPiece29, pathPiece72);

	//set point used,set pathpieceone
	//compute weight
	if (pi18[node8] == node8) (*iterNewPiece18).setWeight(INFINITEWEIGHT);//cracked path，set weight
	else
	{
		int uu18 = pi18[node8];
		int vv18 = node8;
		auto &linkNodesVector18 = (*iterNewPiece18).getLinkNodesVector();
		while (uu18 != node1)
		{
			//for piece
			//weight18 += graph.getEdge(uu18, vv18).getEdgeWeight();
			weight18 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector18.push_back(uu18);

			//for point
			path.setUsed(uu18, true);
			path.setPathPieceOne(uu18, iterNewPiece18);

			//next
			vv18 = uu18;
			uu18 = pi18[uu18];
		}
		//weight18 += graph.getEdge(uu18, vv18).getEdgeWeight();
		weight18 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece18).setWeight(weight18);//set weight
	}

	if (pi83[node3] == node3) (*iterNewPiece83).setWeight(INFINITEWEIGHT);//cracked path，set weight
	else
	{
		int uu83 = pi83[node3];
		int vv83 = node3;
		auto &linkNodesVector83 = (*iterNewPiece83).getLinkNodesVector();
		while (uu83 != node8)
		{
			//for piece
			//weight83 += graph.getEdge(uu83, vv83).getEdgeWeight();
			weight83 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector83.push_back(uu83);

			//for point
			path.setUsed(uu83, true);
			path.setPathPieceOne(uu83, iterNewPiece83);

			//next
			vv83 = uu83;
			uu83 = pi83[uu83];
		}
		//weight83 += graph.getEdge(uu83, vv83).getEdgeWeight();
		weight83 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece83).setWeight(weight83);//set weight
	}

	if (pi72[node2] == node2) (*iterNewPiece72).setWeight(INFINITEWEIGHT);//cracked path，set weight
	else
	{
		int uu72 = pi72[node2];
		int vv72 = node2;
		auto &linkNodesVector72 = (*iterNewPiece72).getLinkNodesVector();
		while (uu72 != node7)
		{
			//for piece
			//weight72 += graph.getEdge(uu72, vv72).getEdgeWeight();
			weight72 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector72.push_back(uu72);

			//for point
			path.setUsed(uu72, true);
			path.setPathPieceOne(uu72, iterNewPiece72);

			//next
			vv72 = uu72;
			uu72 = pi72[uu72];
		}
		//weight72 += graph.getEdge(uu72, vv72).getEdgeWeight();
		weight72 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece72).setWeight(weight72);//set weight
	}

	if (pi29[node9] == node9) (*iterNewPiece29).setWeight(INFINITEWEIGHT);//cracked path，set weight
	else
	{
		int uu29 = pi29[node9];
		int vv29 = node9;
		auto &linkNodesVector29 = (*iterNewPiece29).getLinkNodesVector();
		while (uu29 != node2)
		{
			//for piece
			//weight29 += graph.getEdge(uu29, vv29).getEdgeWeight();
			weight29 = INVALID;//do not comput weight,do not have static weight
			linkNodesVector29.push_back(uu29);

			//for point
			path.setUsed(uu29, true);
			path.setPathPieceOne(uu29, iterNewPiece29);

			//next
			vv29 = uu29;
			uu29 = pi29[uu29];
		}
		//weight29 += graph.getEdge(uu29, vv29).getEdgeWeight();
		weight29 = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece29).setWeight(weight29);//set weight
	}

	//set point:for node2 and node 8,set pathpiece one and pathpiecetwo
	path.setUsed(node2, true);
	path.setPathPieceOne(node2, iterNewPiece72);
	path.setPathPieceTwo(node2, iterNewPiece29);

	path.setUsed(node8, true);
	path.setPathPieceOne(node8, iterNewPiece18);
	path.setPathPieceTwo(node8, iterNewPiece83);

	//set point:set node1 node3 node7 node9 ,set pathpiece one or pathpiecetwo
	path.setPathPieceTwo(node1, iterNewPiece18);
	path.setPathPieceTwo(node7, iterNewPiece72);
	path.setPathPieceOne(node3, iterNewPiece83);
	path.setPathPieceOne(node9, iterNewPiece29);

	//delete the replaced one piece
	auto iterDeletePiece = path.getPathList().erase(iterOldPiece12);
	path.getPathList().erase(iterDeletePiece);

	iterDeletePiece = path.getPathList().erase(iterOldPiece78);
	path.getPathList().erase(iterDeletePiece);
	
	return EXIT_SUCCESS;
}

//将piece iter换成piL和piR记录的左右两个piece
int BetaOptimize::emplaceOne2Two(GraphVector &graph, Path &path, list<PathPiece>::iterator iter
	, int middleNode, int *piL, int *piR)
{
	int firstNode = (*iter).getFirstNode();
	int lastNode = (*iter).getLastNode();

	//create piece and insert
	//set pathpiece
	PathPiece pathPieceL;
	pathPieceL.setFirstNode(firstNode);
	pathPieceL.setLastNode(middleNode);
	if (piL[middleNode] == middleNode) pathPieceL.setLinkProperty(CRACKED);
	else pathPieceL.setLinkProperty(LINKED);
	int weightL = 0;

	PathPiece pathPieceR;
	pathPieceR.setFirstNode(middleNode);
	pathPieceR.setLastNode(lastNode);
	if (piR[lastNode] == lastNode) pathPieceR.setLinkProperty(CRACKED);
	else pathPieceR.setLinkProperty(LINKED);
	int weightR = 0;

	auto iterNewPieceR = path.getPathList().insert(iter, pathPieceR);
	auto iterNewPieceL = path.getPathList().insert(iterNewPieceR, pathPieceL);

	//set point used,set pathpieceone
	//compute weight
	if (piL[middleNode] == middleNode)//cracked path
	{
		(*iterNewPieceL).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uuL = piL[middleNode];
		int vvL = middleNode;
		auto &linkNodesVectorL = (*iterNewPieceL).getLinkNodesVector();
		while (uuL != firstNode)
		{
			//for piece
			//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
			weightL = INVALID;//do not comput weight,do not have static weight
			linkNodesVectorL.push_back(uuL);

			//for point
			path.setUsed(uuL, true);
			path.setPathPieceOne(uuL, iterNewPieceL);

			//next
			vvL = uuL;
			uuL = piL[uuL];
		}
		//weightL += graph.getEdge(uuL, vvL).getEdgeWeight();
		weightL = INVALID;//do not comput weight,do not have static weight
		(*iterNewPieceL).setWeight(weightL);//set weight
	}
	
	if (piR[lastNode] == lastNode)//cracked path
	{
		(*iterNewPieceR).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uuR = piR[lastNode];
		int vvR = lastNode;
		auto &linkNodesVectorR = (*iterNewPieceR).getLinkNodesVector();
		while (uuR != middleNode)
		{
			//for piece
			//weightR += graph.getEdge(uuR, vvR).getEdgeWeight();
			weightR = INVALID;//do not comput weight,do not have static weight
			linkNodesVectorR.push_back(uuR);

			//for point
			path.setUsed(uuR, true);
			path.setPathPieceOne(uuR, iterNewPieceR);

			//next
			vvR = uuR;
			uuR = piR[uuR];
		}
		//weightR += graph.getEdge(uuR, vvR).getEdgeWeight();
		weightR = INVALID;//do not comput weight,do not have static weight
		(*iterNewPieceR).setWeight(weightR);//set weight
	}

	//set point:for middlenode,set pathpiece one and pathpiecetwo
	path.setUsed(middleNode, true);
	path.setPathPieceOne(middleNode, iterNewPieceL);
	path.setPathPieceTwo(middleNode, iterNewPieceR);

	//set point:set firstnode and lastnode pathpiece
	path.setPathPieceTwo(firstNode, iterNewPieceL);
	path.setPathPieceOne(lastNode, iterNewPieceR);

	//delete the replaced one piece
	auto iterDeletePiece = iterNewPieceR;
	path.getPathList().erase(++iterDeletePiece);

	return EXIT_SUCCESS;
}

//将iter和++iter两个piece，换成pi中记录的一个piece
int BetaOptimize::emplaceTwo2One(GraphVector &graph, Path &path, list<PathPiece>::iterator iter1, int *pi)
{
	auto iter2 = iter1;
	++iter2;

	int firstNode = (*iter1).getFirstNode();
	int lastNode = (*iter2).getLastNode();
	int middleNode = (*iter1).getLastNode();

	//reset point
	path.setUsed(middleNode, false);//这里设置与否其实并不影响，设置符合操作规范

	//create piece,insert it
	//set piece properties
	PathPiece pathPiece;
	pathPiece.setFirstNode(firstNode);
	pathPiece.setLastNode(lastNode);
	if (pi[lastNode] == lastNode) pathPiece.setLinkProperty(CRACKED);
	else pathPiece.setLinkProperty(LINKED);
	int weight = 0;
	auto iterNewPiece = path.getPathList().insert(iter1, pathPiece);

	//compute weight,push point to vector
	//set point used,set pathpieceone
	if (pi[lastNode] == lastNode)//creacked path
	{
		(*iterNewPiece).setWeight(INFINITEWEIGHT);//set weight
	}
	else
	{
		int uu = pi[lastNode];
		int vv = lastNode;
		auto &linkNodesVector = (*iterNewPiece).getLinkNodesVector();
		while (uu != firstNode)
		{
			//for piece
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;//do not comput weight,do not have static weight
			linkNodesVector.push_back(uu);

			//for point
			path.setUsed(uu, true);
			path.setPathPieceOne(uu, iterNewPiece);

			vv = uu;
			uu = pi[uu];
		}

		//set piece weight
		//weight += graph.getEdge(uu, vv).getEdgeWeight();
		weight = INVALID;//do not comput weight,do not have static weight
		(*iterNewPiece).setWeight(weight);
	}
	
	//set firstnode and lastnode pathpiece property
	path.setPathPieceTwo(firstNode, iterNewPiece);
	path.setPathPieceOne(lastNode, iterNewPiece);

	//delete the replaced two piece
	auto iterDeletePiece = iterNewPiece;
	iterDeletePiece = path.getPathList().erase(++iterDeletePiece);
	path.getPathList().erase(iterDeletePiece);

	return EXIT_SUCCESS;
}

//we should use dijkstra algorithm to find minimum weight piece
int BetaOptimize::searchPathPieceDfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi)
{
	//这里我先用dfs写一个简单的用着，应该要使用dijkstra 算法的
	int color[MAXPOINTNUM];
	memset(color, 0, sizeof(int)*MAXPOINTNUM);
	dfsFlag = 0;

	dfs(graph, path, firstNode, lastNode, pi, color);

	//compute weight of path
	if (dfsFlag)
	{
		int uu = pi[lastNode];
		int vv = lastNode;
		int weight = 0;
		while (uu != firstNode)
		{
			//weight += graph.getEdge(uu,vv).getEdgeWeight();
			weight = INVALID;//do not comput weight,do not have static weight
			vv = uu;
			uu = pi[uu];
		}
		//weight += graph.getEdge(uu, vv).getEdgeWeight();
		weight = INVALID;//do not comput weight,do not have static weight

		return weight;
	}
	else
	{
		return INFINITEWEIGHT;
	}
}

//time complexity:O(V+E)，so :O(4800)
int BetaOptimize::dfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi, int *color)
{
	int uu = firstNode;
	color[uu] = GRAY;
	auto &edgeVector = graph.getNode(uu).getEdgeVector();
	for (auto iterTemp = edgeVector.begin(); iterTemp != edgeVector.end(); ++iterTemp)
	{
		int vv = (*iterTemp).getNodeTwo();

		if (vv == lastNode)//find the node we need
		{
			dfsFlag = 1;
			pi[vv] = uu;
			return EXIT_SUCCESS;
		}

		Point &pointVv = graph.getNode(vv);
		if ((path.getProperty(vv) == POINT_V) && (path.getValidity(vv) == POINT_VALID) 
			&& (path.getUsed(vv)==false) && (color[vv] == WHITE))
		{
			pi[vv] = uu;
			dfs(graph, path, vv, lastNode, pi, color);
			if (dfsFlag) return EXIT_SUCCESS;//make dfs jump out of recursive
		}
	}

	color[uu] = BLACK;

	return EXIT_SUCCESS;
}


//find a path, using dijkstra algorithm
//read book 《introduction to algorithm》 p383
//if can not find a path, return INFINITEWEIGHT
//when enableUsed==true, we can use used vpoints but not include vprime points
int BetaOptimize::searchPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int lastNode, int *pi, int enableUsed)
{
	//attention: for every point,it's id must +1,for we do not use array[0]

	//some array for statement save
	//for heap[0] we do not use ,so +1
	int set[MAXPOINTNUM + 1];//if node has been add to set,set[nodeid+1]==black
	int distance[MAXPOINTNUM + 1];//distance of every point,like handleValue:input key,output value of key
	int aa[MAXPOINTNUM + 1];//input index of aa,output key
	int position[MAXPOINTNUM + 1];//like handlePosition,input key,output index of aa
	int firstNodeSccNum = graph.getNode(firstNode).getSccNum();

	//for dijkstra, set firstNode and lastNode unused and POINT_Vprime temporary
	path.setUsed(firstNode, false);
	path.setProperty(firstNode, POINT_V);
	path.setUsed(lastNode, false);
	path.setProperty(lastNode, POINT_V);

	//add unused and valid points to aa ,then set position and value(distance)
	int index = 1;
	for (int i = 0; i <= MAXNODEID; i++)
	{
		Point &pointTemp = graph.getNode(i);

		if ((!enableUsed && (path.getUsed(i) != false)) || (path.getValidity(i) != POINT_VALID)
			|| (path.getProperty(i) != POINT_V) || (pointTemp.getSccNum() > firstNodeSccNum))
		{
			continue;
		}

		//the rest is satisfied
		{
			aa[index] = i;
			position[i] = index;

			index++;
		}
	}

	//initial every point
	memset(set, 0, sizeof(int)*(MAXPOINTNUM + 1));//set is empty
	//for (int i = 0; i <= MAXNODEID; i++)
	//{
	//	distance[i] = INFINITEWEIGHT;//infinite weight
	//}
	//use memset to seed up
	memset(distance, 0x1, sizeof(int)*(MAXPOINTNUM + 1));
	distance[firstNode] = 0;

	//build heap
	Heap heap;
	heap.buildHeap(aa, distance, position, index - 1);

	int uu = 0;
	int vv = 0;
	int weightTemp = 0;
	while (!heap.isEmpty())//time complexity:O((V+E)lgV),so we need ruduce v and e
	{
		uu = heap.extractMin();//uu is the key in aa,ie point id
		set[uu] = BLACK;//add uu to set

		//find node we need, break
		if (uu == lastNode)
		{
			break;
		}

		//traverse every edge of point uu
		vector<Edge> &edgeVector = graph.getNode(uu).getEdgeVector();
		for (auto iterTemp = edgeVector.begin(); iterTemp != edgeVector.end(); ++iterTemp)
		{
			vv = (*iterTemp).getNodeTwo();
			Point &pointTemp = graph.getNode(vv);

			//if vv do not satisify,continue
			// (used) || (the point has been deleted by me) || (not point v) || (used by searchPathPieceDijkstra before)
			if ((!enableUsed && (path.getUsed(vv) != false)) || (set[vv] != WHITE) || (path.getProperty(vv) != POINT_V) //这个地方判断太多，得想办法简化下
				|| (path.getValidity(vv) != POINT_VALID)) continue;

			//relax operation
			//weightTemp = graph.getEdge(uu, vv).getEdgeWeight();
			weightTemp = computeDanymicEdgeWeight(graph, otherPathEdge, uu, vv);//now weight is influenced by otherPath

			if (distance[vv] > (distance[uu] + weightTemp))
			{
				distance[vv] = (distance[uu] + weightTemp);
				pi[vv] = uu;//save pi, by this we can get the path

				//decrease key's value of heap
				heap.decreaseKey(vv, distance[vv]);
			}
		}
	}

	//after dijkstra, restore firstNode and lastNode unused and POINT_Vprime properties
	path.setUsed(firstNode, true);
	path.setProperty(firstNode, POINT_VPRIME);
	path.setUsed(lastNode, true);
	path.setProperty(lastNode, POINT_VPRIME);

	if (distance[lastNode] == 0x01010101)
	{
		pi[lastNode] = lastNode;//by this we can konw, whether exist path in other func
		return INFINITEWEIGHT;//if can not find a path, return INFINITEWEIGHT
	}
	else
	{
		return distance[lastNode];
	}

	//if (distance[lastNode] == INFINITEWEIGHT) pi[lastNode] = lastNode;//by this we can konw, whether exist path in other func
	//return distance[lastNode];//if can not find a path, return INFINITEWEIGHT
}

int BetaOptimize::searchMultiPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int lastNode, int* commonColor, int *pi, int enableUsed)
{

#ifdef MEASURE
	timeMeasure.startTimeDijkstra = getTime();
#endif

	//attention: for every point,it's id must +1,for we do not use array[0]

	//some array for statement save
	//for heap[0] we do not use ,so +1
	int set[MAXPOINTNUM + 1];//if node has been add to set,set[nodeid+1]==black
	int distance[MAXPOINTNUM + 1];//distance of every point,like handleValue:input key,output value of key
	int aa[MAXPOINTNUM + 1];//input index of aa,output key
	int position[MAXPOINTNUM + 1];//like handlePosition,input key,output index of aa
	int firstNodeSccNum = graph.getNode(firstNode).getSccNum();

	//for dijkstra, set firstNode and lastNode unused and POINT_Vprime temporary
	path.setUsed(firstNode, false);
	path.setProperty(firstNode, POINT_V);
	path.setUsed(lastNode, false);
	path.setProperty(lastNode, POINT_V);

	//add unused and valid points to aa ,then set position and value(distance)
	int index = 1;
	for (int i = 0; i <= MAXNODEID; i++)
	{
		Point &pointTemp = graph.getNode(i);

		if ((!enableUsed && (path.getUsed(i) != false)) || (path.getValidity(i) != POINT_VALID)
			|| (path.getProperty(i) != POINT_V) || (commonColor[i] != WHITE) || (pointTemp.getSccNum() > firstNodeSccNum))
		{
			continue;
		}

		//the rest is satisfied
		{
			aa[index] = i;
			position[i] = index;

			index++;
		}
	}

	//initial every point
	memset(set, 0, sizeof(int)*(MAXPOINTNUM + 1));//set is empty

	//use memset to seed up
	memset(distance, 0x1, sizeof(int)*(MAXPOINTNUM + 1));
	distance[firstNode] = 0;

	//build heap
	Heap heap;
	heap.buildHeap(aa, distance, position, index - 1);

	int uu = 0;
	int vv = 0;
	int weightTemp = 0;
	while (!heap.isEmpty())//time complexity:O((V+E)lgV),so we need ruduce v and e
	{
		uu = heap.extractMin();//uu is the key in aa,ie point id
		set[uu] = BLACK;//add uu to set

		//find node we need, break
		if (uu == lastNode)
		{
			break;
		}

		//traverse every edge of point uu
		vector<Edge> &edgeVector = graph.getNode(uu).getEdgeVector();
		for (auto iterTemp = edgeVector.begin(); iterTemp != edgeVector.end(); ++iterTemp)
		{
			vv = (*iterTemp).getNodeTwo();
			Point &pointTemp = graph.getNode(vv);

			//if vv do not satisify,continue
			// (used) || (the point has been deleted by me) || (not point v) || (used by searchPathPieceDijkstra before)
			//after test, we arrange these in descending order
			if ((!enableUsed && (path.getUsed(vv) != false)) || (set[vv] != WHITE) || (path.getProperty(vv) != POINT_V)
				|| (commonColor[vv] != WHITE) || (path.getValidity(vv) != POINT_VALID)) continue;//这个地方判断太多，得想办法简化下

			//relax operation
			//weightTemp = graph.getEdge(uu, vv).getEdgeWeight();
			weightTemp = computeDanymicEdgeWeight(graph, otherPathEdge, uu, vv);//now weight is influenced by otherPath

			if (distance[vv] > (distance[uu] + weightTemp))
			{
				distance[vv] = (distance[uu] + weightTemp);
				pi[vv] = uu;//save pi, by this we can get the path

				//decrease key's value of heap
				heap.decreaseKey(vv, distance[vv]);
			}
		}
	}

	//after dijkstra, restore firstNode and lastNode unused and POINT_Vprime properties
	path.setUsed(firstNode, true);
	path.setProperty(firstNode, POINT_VPRIME);
	path.setUsed(lastNode, true);
	path.setProperty(lastNode, POINT_VPRIME);

#ifdef MEASURE
	timeMeasure.timeAllDijkstra += float(getTime()-timeMeasure.startTimeDijkstra);
	timeMeasure.timesDijkstra++;
#endif

	if (distance[lastNode] == 0x01010101)
	{
		pi[lastNode] = lastNode;//by this we can konw, whether exist path in other func
		return INFINITEWEIGHT;//if can not find a path, return INFINITEWEIGHT
	}
	else
	{
		return distance[lastNode];
	}
}

//distance:int distance[MAXPOINTNUM + 1], distance of every point,like handleValue:input key,output value of key
int BetaOptimize::searchMultiPathPieceDijkstra(GraphVector &graph, Path &path, PathEdge &otherPathEdge, int firstNode, int *distance, int* commonColor, int *pi, int enableUsed)
{

#ifdef MEASURE
	timeMeasure.startTimeDijkstra = getTime();
#endif

	//attention: for every point,it's id must +1,for we do not use array[0]

	//some array for statement save
	//for heap[0] we do not use ,so +1
	int set[MAXPOINTNUM + 1];//if node has been add to set,set[nodeid+1]==black
	int aa[MAXPOINTNUM + 1];//input index of aa,output key
	int position[MAXPOINTNUM + 1];//like handlePosition,input key,output index of aa
	int firstNodeSccNum = graph.getNode(firstNode).getSccNum();

	//for dijkstra, set firstNode unused and POINT_Vprime temporary
	path.setUsed(firstNode, false);
	path.setProperty(firstNode, POINT_V);

	//add unused and valid points to aa ,then set position and value(distance)
	int index = 1;
	for (int i = 0; i <= MAXNODEID; i++)
	{
		Point &pointTemp = graph.getNode(i);

		if ((!enableUsed && (path.getUsed(i) != false)) || (path.getValidity(i) != POINT_VALID)
			|| (path.getProperty(i) != POINT_V) || (commonColor[i] != WHITE) || (pointTemp.getSccNum() > firstNodeSccNum))
		{
			continue;
		}

		//the rest is satisfied
		{
			aa[index] = i;
			position[i] = index;

			index++;
		}
	}

	//initial every point
	memset(set, 0, sizeof(int)*(MAXPOINTNUM + 1));//set is empty

	//use memset to seed up
	memset(distance, 0x1, sizeof(int)*(MAXPOINTNUM + 1));
	distance[firstNode] = 0;

	//build heap
	Heap heap;
	heap.buildHeap(aa, distance, position, index - 1);

	int uu = 0;
	int vv = 0;
	int weightTemp = 0;
	while (!heap.isEmpty())//time complexity:O((V+E)lgV),so we need ruduce v and e
	{
		uu = heap.extractMin();//uu is the key in aa,ie point id
		set[uu] = BLACK;//add uu to set

		//traverse every edge of point uu
		vector<Edge> &edgeVector = graph.getNode(uu).getEdgeVector();
		for (auto iterTemp = edgeVector.begin(); iterTemp != edgeVector.end(); ++iterTemp)
		{
			vv = (*iterTemp).getNodeTwo();
			Point &pointTemp = graph.getNode(vv);

			if ((path.getProperty(vv) == POINT_VPRIME) || (path.getProperty(vv) == POINT_END))
			{
				weightTemp = computeDanymicEdgeWeight(graph, otherPathEdge, uu, vv);//now weight is influenced by otherPath

				if (distance[vv] > (distance[uu] + weightTemp))
				{
					distance[vv] = (distance[uu] + weightTemp);
					pi[vv] = uu;//save pi, by this we can get the path
				}

				//attention: just save distance, do not decrease key's value of heap, because this point is not in the heap
				continue;
			}

			//if vv do not satisify,continue
			// (used) || (the point has been deleted by me) || (not point v) || (used by searchPathPieceDijkstra before)
			//after test, we arrange these in descending order
			if ((!enableUsed && (path.getUsed(vv) != false)) || (set[vv] != WHITE) || (path.getProperty(vv) != POINT_V)
				|| (commonColor[vv] != WHITE) || (path.getValidity(vv) != POINT_VALID)) continue;//这个地方判断太多，得想办法简化下

			//relax operation
			//weightTemp = graph.getEdge(uu, vv).getEdgeWeight();
			weightTemp = computeDanymicEdgeWeight(graph, otherPathEdge, uu, vv);//now weight is influenced by otherPath

			if (distance[vv] > (distance[uu] + weightTemp))
			{
				distance[vv] = (distance[uu] + weightTemp);
				pi[vv] = uu;//save pi, by this we can get the path

				//decrease key's value of heap
				heap.decreaseKey(vv, distance[vv]);
			}
		}
	}

	//after dijkstra, restore firstNode unused and POINT_Vprime properties
	path.setUsed(firstNode, true);
	path.setProperty(firstNode, POINT_VPRIME);

#ifdef MEASURE
	timeMeasure.timeAllDijkstra += float(getTime() - timeMeasure.startTimeDijkstra);
	timeMeasure.timesDijkstra++;
#endif

	return EXIT_SUCCESS;
}

//we should use dijkstra algorithm to find minimum weight piece
int BetaOptimize::searchPathPieceBfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi)
{
	int bfsFlag = bfs(graph, path, firstNode, lastNode, pi);

	//compute weight of path
	if (bfsFlag==FOUND)
	{
		int uu = pi[lastNode];
		int vv = lastNode;
		int weight = 0;
		while (uu != firstNode)
		{
			//weight += graph.getEdge(uu, vv).getEdgeWeight();
			weight = INVALID;//do not comput weight,do not have static weight
			vv = uu;
			uu = pi[uu];
		}
		//weight += graph.getEdge(uu, vv).getEdgeWeight();
		weight = INVALID;//do not comput weight,do not have static weight

		return weight;
	}
	else
	{
		pi[lastNode] = lastNode;//by this we can konw, wthether exist path in other func
		return INFINITEWEIGHT;
	}
}
// time complexity : O(V + E)，so : O(4800)
int BetaOptimize::bfs(GraphVector &graph, Path &path, int firstNode, int lastNode, int *pi)
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

			if (vv == lastNode)//meet point we need 
			{
				color[vv] = GRAY;
				pi[vv] = uu;
				pointQueue.push(vv);

				flag = 1;
				break;//jump out of loop
			}

			// (used) || (not point v) || (the point has been deleted by me) || (has visited) || (used by searchPathPiece before)
			if ((path.getUsed(vv) != false) || (path.getProperty(vv) != POINT_V) || (path.getValidity(vv) != POINT_VALID)
				|| (color[vv] != WHITE) || (searchPieceColor[vv] != WHITE)) continue;

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

//time complexity:<O(50)
int BetaOptimize::findMaxNode(Path &path, int weightLimit)
{
	//traverse every pathpiece,compute weight of piece, ignore piece which weight greater than weightlimit
	int maxNodeId = INVALID;
	int weightMax = 0;
	int weight = 0;
	int temp=0;//for counting the order of current maxnodeid in path

	list<PathPiece>::iterator iterLeft = path.getPathList().begin();
	list<PathPiece>::iterator iterRight = iterLeft;
	++iterRight;

	for (int cnt=0;iterRight!=path.getPathList().end();++iterRight,++iterLeft,cnt++)
	{
		weight = ((*iterLeft).getWeight() + (*iterRight).getWeight());

		//in for loop, remove the judge, maybe run faster
		if ((weight > weightMax) && (weight <= weightLimit) && (vPrimeColor[cnt] == WHITE))
		{
			weightMax = weight;
			maxNodeId = (*iterLeft).getLastNode();

			temp = cnt;//save the  order of maxnodeid
		}
	}

	//set choosed maxnode color black
	vPrimeColor[temp] = BLACK;

#ifdef DEBUG
	//assert(maxNodeId != INVALID);//now we are allowed to return INVALID,as we get optimal ans 
#endif
	return maxNodeId;
}

//time complexity:<O(50)
//attention: this weightlimit maybe limit of piece or limit of second node
//when we need find second node,we return iter, secondNode=(*iter).getlastnode
inline list<PathPiece>::iterator BetaOptimize::findMaxPieceOrSecNode(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit)
{
	if (switchAlgorithmKind == SWITCHNODEPIECE)
	{
		return (findMaxPiece(graph, path, curMaxNodeId, weightLimit));
	}
	else if (switchAlgorithmKind == SWITCHNODENODE)
	{
		return (findSecondNode(graph, path, curMaxNodeId, weightLimit));
	}
}

//attention need to be in the same scc
//when we need find second node,we return iter: secondNode=(*iter).getlastnode
//when there is no secnode satisfied,we return path.getPathList().end() as flag
list<PathPiece>::iterator BetaOptimize::findSecondNode(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit)
{
	list<PathPiece>::iterator secNodeIter = path.getPathList().end();
	int secNodeTemp = 0;
	int weightMax = 0;
	int weight = 0;
	int temp = 0; //for counting the order of current secnode in path
	int sccOfSecNodeTemp = 0;//scc num of secNodeTemp
	int sccNum = graph.getNode(curMaxNodeId).getSccNum();//scc of max node

	list<PathPiece>::iterator iterLeft = path.getPathList().begin();
	list<PathPiece>::iterator iterRight = iterLeft;
	++iterRight;

	for (int cnt = 0; iterRight != path.getPathList().end(); ++iterRight, ++iterLeft, cnt++)
	{
		secNodeTemp = (*iterLeft).getLastNode();
		sccOfSecNodeTemp = graph.getNode(secNodeTemp).getSccNum();

		if (sccOfSecNodeTemp>sccNum) continue;//don't get into to the range of sccNum,continue

		if (sccNum>sccOfSecNodeTemp) break;//exceeded the range of sccNum,break

		//secnode is neighbour of maxnode,continue
		//secnode == maxnode,continue
		//secnode has visited,continue
		if ((secondNodeColor[cnt] == BLACK) || (secNodeTemp == curMaxNodeId) || ((*iterRight).getLastNode() == curMaxNodeId)
			|| ((*iterLeft).getFirstNode() == curMaxNodeId)) continue;

		//the rest is satisfied
		weight = ((*iterLeft).getWeight() + (*iterRight).getWeight());

		//in for loop, remove the judge, maybe run faster
		if ((weight > weightMax) && (weight <= weightLimit))
		{
			weightMax = weight;
			secNodeIter = iterLeft;

			temp = cnt;//save the  order of secnode choosed
		}
	}

	//set choosed node's color black
	secondNodeColor[temp] = BLACK;

	//when there is no secnode satisfied,we return path.getPathList().end() as flag
	return secNodeIter;
}

//attention need to be in the same scc
list<PathPiece>::iterator BetaOptimize::findMaxPiece(GraphVector &graph, Path &path, int curMaxNodeId, int weightLimit)
{
	int firstNode = 0;
	int lastNode = 0;
	int sccOfFirstNode = 0;
	int sccOfLastNode = 0;
	int sccNum = graph.getNode(curMaxNodeId).getSccNum();
	int weightMax = 0;
	int weight = 0;
	int temp = 0;//for save the order of choosed piece
	list<PathPiece>::iterator ansIter = path.getPathList().end();

	list<PathPiece>::iterator iter = path.getPathList().begin();
	for (int cnt = 0; iter != path.getPathList().end(); ++iter, cnt++)
	{
		firstNode = (*iter).getFirstNode();
		lastNode = (*iter).getLastNode();

		sccOfFirstNode = graph.getNode(firstNode).getSccNum();
		sccOfLastNode = graph.getNode(lastNode).getSccNum();

		if (sccOfLastNode>sccNum) continue;//don't get into to the range of sccNum,continue

		if (sccNum>sccOfFirstNode) break;//exceeded the range of sccNum,break

		//piece have node curMaxNodeId,continue
		//piece have been visited,continue
		if ((pieceColor[cnt] == BLACK) || (firstNode == curMaxNodeId) || (lastNode == curMaxNodeId)) continue;

		//the rest is satisified
		weight = (*iter).getWeight();
		if ((weight > weightMax) && (weight<weightLimit))
		{
			weightMax = weight;
			ansIter = iter;

			//save order
			temp = cnt;
		}
	}

	//set color black
	pieceColor[temp] = BLACK;

	return ansIter;//if not found,ansIter=path.getPathList().end()

}
