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
#include "betaRoute.h"
#include "assist.h"
#include <iostream>
#include <vector>
#include <list>
#include <cassert>
#include <queue>

using namespace std;

BetaRoute::BetaRoute()
{

}

BetaRoute::~BetaRoute()
{

}

//new algorithm of compute path
int BetaRoute::cmPath(GraphVector &graph, VPrimeToUse &vPrime, Path &path)
{

	//reset path,if path isn't empty,we random the order of vprime
	if (!(path.getPathList().empty()))
	{
		//reset path to empty
		path.resetSelf();

		//reset graph node to unused
		path.setAllNodesUnused();

		//re-arrange in random inoutdegree order,still meet scc order
		vPrime.refreshVPrime(graph, path);
		//vPrime.randomOrder();//////////////??????????here i do not write this func
	}

#ifdef DEBUG
	vPrime.printVprime();
#endif

	int repairPieceFlag = false;
	int firstNode = 0;//used for search of bfsPathPiece
	int secondNode = 0;//used for search of bfsPathPiece
	int algorithmFlag;//result of algorithm
	int crackFlag = NOCRACK;//flag index that whether we bringing in cracked piece
	int pieceFirstNode = 0;
	int pieceBeforeFirstNode = 0;
	int pieceLastNode = 0;
	int pieceAfterLastNode = 0;
	int timeCnt = 0;
	list<PathPiece> &pathList = path.getPathList();

	//when there is no vprime in path
	if (path.getVPrimeSize() == 0)
	{
		//find a path from start to end
		path.bfsPathPiece(graph, path.getVStart(), path.getVEnd(), false);

		//according to pathpiece we searched, add path piece or repair path piece
		path.createPieceList(graph, path.getVStart(), path.getVEnd(), path.getPathList().end());

#ifdef DEBUG
		cout << "@_@ cmpath find a path: "  << endl;
		path.printPath();
#endif

		return HAVESOLUTION;//we find a path
	}

	//when there is no piece in path
	if (path.getPathList().empty())
	{
		firstNode = path.getVStart();

		secondNode = vPrime.begin()->getPointId();

		//because this is the first node,not point has been used
		path.bfsPathPiece(graph, firstNode, secondNode, false);

		//according to pathpiece we searched, add path piece or repair path piece
		path.createPieceList(graph, firstNode, secondNode, path.getPathList().end());

#ifdef DEBUG
		cout << "add first piece, firstNode: " << firstNode << " ,secondNode: " << secondNode << endl;
		path.printPath();
#endif

	}

	while (1)
	{

#ifdef DEBUG
		if (path.checkUsedProperty(graph) == WRONG)
		{
			path.printAllNodeInPath();
			assert(0);
		}
#endif

#ifndef DEBUG
		timeCnt++;
		if (!(timeCnt & 0x0000000f))//every xx times,check time
		{
			timeMeasure.timeAll = getTime();
			if (timeMeasure.timeAll>9200)
			{
				cout << "cmpath end for time out............................." << endl;
				return NOSOLUTION;
			}
		}
#endif

		//when maybe have cracked piece
		if (crackFlag != NOCRACK)
		{
			//deal with cracked pieces first
			list<PathPiece>::iterator iterTemp;
			for (iterTemp = pathList.begin(); iterTemp != pathList.end(); ++iterTemp)
			{
				pieceFirstNode = (*iterTemp).getFirstNode();
				pieceLastNode = (*iterTemp).getLastNode();

				if ((*iterTemp).getLinkProperty() == CRACKED)
				{
					if ((path.getCrackTimes()[pieceFirstNode][pieceLastNode]<CRACKLIMITS))
					{
						//由于如果删除了piece，当前的迭代器会失效，所以需要从头开始，无法避免
						path.connectTwoNodes(graph, pieceFirstNode, pieceLastNode,true);

#ifdef DEBUG
						cout << "repair cracked piece<CRACKLIMITS may with new crack, firstNode: " << pieceFirstNode << " ,secondNode: " << pieceLastNode << endl;
						path.printPath();
						path.printAllNodeInPath();
#endif

						repairPieceFlag = true;//flag==1表示对crack边进行了修复
						break;//jump out of for loop,still in while loop
					}
					else if ((iterTemp == pathList.begin()) || (iterTemp == (--(pathList.end()))))
					{
						//for: Vstart x b->n
						if (iterTemp == pathList.begin())
						{
							auto iter = iterTemp; ++iter;
							pieceAfterLastNode = (*iter).getLastNode();

							auto iterPiece = iterTemp;

							iterPiece = path.deleteOnePiece(graph, iterPiece, true);
							iterPiece = path.deleteOnePiece(graph, iterPiece, true);

							vPrime.insertVPrime(graph, path, pieceLastNode);

							//here firstnode is vstart
							path.insertCrackPiece(graph, pieceFirstNode, pieceAfterLastNode, iterPiece);

#ifdef DEBUG
							cout << "delete 1 point to vprime2use and new crack happen, VStart: "
								<< pieceFirstNode << " ,secondNode: " << pieceAfterLastNode << endl;
							path.printPath();
							path.printAllNodeInPath();
#endif

							repairPieceFlag = true;//because of new crack,we should check from begining
							break;//jump out of for loop,still in while loop
						}
						else//for: m->a x Vend
						{
							auto iter = iterTemp; --iter;
							pieceBeforeFirstNode = (*iter).getFirstNode();

							auto iterPiece = iter;

							iterPiece = path.deleteOnePiece(graph, iterPiece, true);
							iterPiece = path.deleteOnePiece(graph, iterPiece, true);

							vPrime.insertVPrime(graph, path, pieceFirstNode);

							//here pieceLastNode is vend
							path.insertCrackPiece(graph, pieceBeforeFirstNode, pieceLastNode, iterPiece);

#ifdef DEBUG
							cout << "delete 1 point to vprime2use and new crack happen, firstNode: "
								<< pieceBeforeFirstNode << " ,VEnd: " << pieceLastNode << endl;
							path.printPath();
							path.printAllNodeInPath();
#endif

							repairPieceFlag = true;//because of new crack,we should check from begining
							break;//jump out of for loop,still in while loop
						}
						
					}
					else//bigger than cracklimits 
					{
						//we should remove a or b vprime point,and add it to list vprimeToUse
						//the cracked piece is like: m->a x b->n
						auto iter = iterTemp; --iter;
						pieceBeforeFirstNode = (*iter).getFirstNode();

						path.setPieceNodeFalse(graph, iter);//set v points in piece m->a false

						//if find a piecelist of m~b
						if (path.bfsPathPiece(graph, pieceBeforeFirstNode, pieceLastNode, false) == FOUND)
						{
							//remove vprime point and add to vprimeToUse
							auto iterIn = path.deleteOneVPrime(graph, pieceFirstNode);//iterIn should points to b->n 
							vPrime.insertVPrime(graph, path, pieceFirstNode);

							//attention: must prevent invalid of iterTemp
							iterTemp = path.createPieceList(graph, pieceBeforeFirstNode, pieceLastNode, iterIn);				

#ifdef DEBUG
							cout << "repair cracked piece>CRACKLIMITS(1), firstNode: " << pieceBeforeFirstNode << " ,secondNode: " << pieceLastNode << endl;
							path.printPath();
							path.printAllNodeInPath();
#endif

							continue;
						}
						else//for: m->a x b->n, can not find a path of m->b,find path a->n
						{
							path.setPieceNodeTrue(graph, iter);//set v points in piece m->a true

							iter = iterTemp; ++iter;
							pieceAfterLastNode = (*iter).getLastNode();

							path.setPieceNodeFalse(graph, iter);//set v points in piece b->n false

							//if find a piecelist of a~n
							if (path.bfsPathPiece(graph, pieceFirstNode, pieceAfterLastNode, false) == FOUND)
							{
								//remove vprime point and add to vprimeToUse
								auto iterIn = path.deleteOneVPrime(graph, pieceLastNode);
								vPrime.insertVPrime(graph, path, pieceLastNode);

								//iterIn add after sub for preventing invalid of iterTemp
								iterTemp = path.createPieceList(graph, pieceFirstNode, pieceAfterLastNode, iterIn);

#ifdef DEBUG
								cout << "repair cracked piece>CRACKLIMITS(2), firstNode: " << pieceFirstNode << " ,secondNode: " << pieceAfterLastNode << endl;
								path.printPath();
								path.printAllNodeInPath();
#endif

								continue;
							}
							else // m->a x b->n,find piecelist of a~n
							{
								iter = iterTemp; --iter;

								path.setPieceNodeFalse(graph, iter);//set v points in piece m->a false

								//if find a piecelist of m~n
								if (path.bfsPathPiece(graph, pieceBeforeFirstNode, pieceAfterLastNode, false) == FOUND)
								{
									auto iterIn = iterTemp; --iterIn;

									//remove vprime point and add to vprimeToUse
									iterIn = path.deleteOnePiece(graph, iterIn, true);
									iterIn = path.deleteOnePiece(graph, iterIn, true);
									iterIn = path.deleteOnePiece(graph, iterIn, true);

									vPrime.insertVPrime(graph, path, pieceFirstNode);
									vPrime.insertVPrime(graph, path, pieceLastNode);

									iterTemp = path.createPieceList(graph, pieceBeforeFirstNode, pieceAfterLastNode, iterIn);

#ifdef DEBUG
									cout << "repair cracked piece>CRACKLIMITS(3), firstNode: " << pieceBeforeFirstNode << " ,secondNode: " << pieceAfterLastNode << endl;
									path.printPath();
									path.printAllNodeInPath();
#endif

									continue;

								}
								else//if can not,insert a crack piece of m x n,delete  m->a x b->n,and add a b to vprime
								{
									auto iterPiece = iterTemp; --iterPiece;
									
									iterPiece = path.deleteOnePiece(graph, iterPiece, true);
									iterPiece = path.deleteOnePiece(graph, iterPiece, true);
									iterPiece = path.deleteOnePiece(graph, iterPiece, true);

									vPrime.insertVPrime(graph, path, pieceFirstNode);
									vPrime.insertVPrime(graph, path, pieceLastNode);

									path.insertCrackPiece(graph, pieceBeforeFirstNode, pieceAfterLastNode, iterPiece);

#ifdef DEBUG
									cout << "delete 2 point to vprime2use and new crack happen, firstNode: "
										<< pieceFirstNode << " ,secondNode: " << pieceAfterLastNode << endl;
									path.printPath();
									path.printAllNodeInPath();
#endif

									repairPieceFlag = true;//because of new crack,we should check from begining
									break;//jump out of for loop,still in while loop
								}

							}
						}

					}
				}//if cracked

			}//for loop
		}

		if (repairPieceFlag == true)//如果对cracked边进行了修复，可能还有其他cracked边，需要返回重新用for loop检查
		{
			repairPieceFlag = false;
			continue;
		}
		else//if there are no cracked pieces in path
		{
			crackFlag = NOCRACK;
			firstNode = path.getLastNode();

			//delete all used vprime points, these vprime points may still in vprime
			//time complexity O(50)
			vPrime.deleteUsedVPrime(graph, path);

			//if vprime is empty
			if (vPrime.empty())
			{
				//if lastnode is vend, over
				if (path.getLastNode() == path.getVEnd())
				{
					return HAVESOLUTION;//we find a path
				}
				else
				{
					//set lastnode to be vend
					if (path.connectTwoNodes(graph, path.getLastNode(), path.getVEnd(), true) == FOUND)
					{

#ifdef DEBUG
						cout << "@_@ cmpath find a path, link to end: " << path.getVEnd() << endl;
						path.printPath();
						path.printAllNodeInPath();
#endif

						return HAVESOLUTION;//we find a path
					}
					else
					{

#ifdef DEBUG
						cout << "@_@ cmpath find a path with crack, link to end: " << path.getVEnd() << endl;
						path.printPath();
						path.printAllNodeInPath();
#endif

						//crack piece,continue
						crackFlag = HAVECRACK;
						continue;
					}

				}
			}

			//2: if begin's scc is biger than lastnode of path,or have add vend to path
			//use "vprime insert" algorithm
			if ((path.getLastNode() == path.getVEnd()) || (vPrime.begin()->getSccNum() > graph.getNode(path.getLastNode()).getSccNum()))
			{
				algorithmFlag = ALGORITHMFAILURE;

				auto iterEnd = vPrime.curSccEnd(graph, path);
				for (auto iterTemp = vPrime.begin(); iterTemp != iterEnd; ++iterTemp)
				{
					if (path.insertVPrimeToPath(graph, (*iterTemp).getPointId(),false) == ALGORITHMSUCCESS)
					{

#ifdef DEBUG
						cout << "insertVPrimeToPath no crack happen(up),vprime: " << (*iterTemp).getPointId() << endl;
						path.printPath();
						path.printAllNodeInPath();
#endif

						//delete vprime we have used
						vPrime.deleteVPrime(iterTemp);

						//flag = success,we do not need to run other algorithm
						algorithmFlag = ALGORITHMSUCCESS;

						//get out of for loop
						break;
					}
				}

				//if algorthim 1 and 2, both are useless
				//4: we have to use violent insert
				if (algorithmFlag == ALGORITHMFAILURE)
				{
					auto iterEnd = vPrime.curSccEnd(graph, path);
					for (auto iterTemp = vPrime.begin(); iterTemp != iterEnd; ++iterTemp)
					{
						if (path.insertVPrimeToPath(graph, (*iterTemp).getPointId(),true) == ALGORITHMSUCCESS)
						{

#ifdef DEBUG
							cout << "insertVPrimeToPath crack happen,vprime: " << (*iterTemp).getPointId() << endl;
							path.printPath();
							path.printAllNodeInPath();
#endif

							//delete vprime we have used
							vPrime.deleteVPrime(iterTemp);

							//flag = success,we do not need to run other algorithm
							algorithmFlag = ALGORITHMSUCCESS;
							crackFlag = HAVECRACK;

							//get out of for loop
							break;
						}
					}
#ifdef DEBUG
					assert(algorithmFlag == ALGORITHMSUCCESS);//this must happen,but may not
#endif
				}
			}
			else
			{
				//1: if begin's scc is not biger than lastnode of path
				//use "later vprime in advance" algorithm
				algorithmFlag = ALGORITHMFAILURE;

				auto iterEnd = vPrime.curSccEnd(graph, path);
				for (auto iterTemp = vPrime.begin(); iterTemp != iterEnd; ++iterTemp)
				{
					secondNode = (*iterTemp).getPointId();

					if (path.bfsPathPiece(graph, firstNode, secondNode, false) == FOUND)
					{
						path.createPieceList(graph, firstNode, secondNode, path.getPathList().end());

						//delete vprime we have used
						vPrime.deleteVPrime(iterTemp);

						//flag = success,we do not need to run other algorithm
						algorithmFlag = ALGORITHMSUCCESS;

#ifdef DEBUG
						cout << "later vprime in advance, firstNode: " << firstNode << " ,secondNode: " << secondNode << endl;
						path.printPath();
						path.printAllNodeInPath();
#endif

						//get out of for loop
						break;
					}
				}

				//2: if "later vprime in advance" algorithm is useless
				//use "vprime insert" algorithm instead
				if (algorithmFlag == ALGORITHMFAILURE)
				{
					auto iterEnd = vPrime.curSccEnd(graph, path);
					for (auto iterTemp = vPrime.begin(); iterTemp != iterEnd; ++iterTemp)
					{
						if (path.insertVPrimeToPath(graph, (*iterTemp).getPointId(),false) == ALGORITHMSUCCESS)
						{

#ifdef DEBUG
							cout << "insertVPrimeToPath no crack happen(down),vprime: " << (*iterTemp).getPointId() << endl;
							path.printPath();
							path.printAllNodeInPath();
#endif

							//delete vprime we have used
							vPrime.deleteVPrime(iterTemp);

							//flag = success,we do not need to run other algorithm
							algorithmFlag = ALGORITHMSUCCESS;

							//get out of for loop
							break;
						}
					}
				}

				//if algorthim 1 and 2, both are useless
				//3: here we have to use violent bfs
				if (algorithmFlag == ALGORITHMFAILURE)
				{
					//use used points to search pathpiece
					if (path.bfsPathPiece(graph, firstNode, secondNode, true) == NOTFOUND)
					{
						return NOSOLUTION;
					}

					//deal with used points and pathpiece associated with 
					//restore:used/pathpieceone/pathpiecetwo 
					path.dealUsedNodes(graph, firstNode, secondNode);

					path.createPieceList(graph, firstNode, secondNode, path.getPathList().end());

					crackFlag = HAVECRACK;

#ifdef DEBUG
					cout << "bfs with crack happening,firstNode: " << firstNode << " ,secondNode: " << secondNode << endl;
					path.printPath();
					path.printAllNodeInPath();
#endif
				}
			}

		}//if there are no cracked pieces in path
	}//while

	//return MAYBESOLUTION;
}

int BetaRoute::addSuperLink(PathPiece &pPiece)
{
	pPiece.setLinkProperty(SUPERLINK);
	pPiece.setWeight(INFINITEWEIGHT);
	pPiece.getLinkNodesVector().clear();

	return EXIT_SUCCESS;
}





