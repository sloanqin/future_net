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
#include "vPrimeToUse.h"
#include "assist.h"
#include <stdlib.h>

//copy valid vprime points to vprimetouse 
int VPrimeToUse::refreshVPrime(GraphBase &graph, Path &path)
{
	//if size is empty, return exit_success
	if (path.getVPrimeSize() == 0) return EXIT_SUCCESS;

	Point xx;
	vPrime2Use.resize(path.getVPrimeSize(), xx);

	//copy v' point to vPrime2Use
	list<Point>::iterator pIter = vPrime2Use.begin();

	*pIter = graph.getNode(path.getVPrime(0));//copy V¡¯Point
	topoOrderMax = (*pIter).getTopoOrder();//initialize
	topoOrderMin = (*pIter).getTopoOrder();//this is important,or will get wrong
	for (int i = 0; pIter != vPrime2Use.end(); ++pIter, ++i)
	{
		//must check, vprime points may be setted POINT_DELETE
		if (path.getValidity(path.getVPrime(i)) == POINT_VALID)
		{
			*pIter = graph.getNode(path.getVPrime(i));//copy V¡¯Point

			//get max and min of topo order
			topoOrderMax = assistMax(topoOrderMax, (*pIter).getTopoOrder());
			topoOrderMin = assistMin(topoOrderMin, (*pIter).getTopoOrder());
		}
	}

	return EXIT_SUCCESS;
}

VPrimeToUse::VPrimeToUse(GraphBase &graph, Path &path) :topoOrderMax(0), topoOrderMin(0)
{
	refreshVPrime(graph, path);
}

int VPrimeToUse::refreshAgainVPrime(GraphBase &graph, Path &path)
{
	refreshVPrime(graph, path);

	randomOrder();

	return EXIT_SUCCESS;
}

//return the iter after last vprime of current scc num
//such as 6(3) 7(3) 5(2) 4(1) 2(0) in vprime2use,we should return iter of 5, not iter of 7
//when no vprime satisfied, return vprim2use.end()
list<Point>::iterator VPrimeToUse::curSccEnd(GraphBase &graph, Path &path)
{
	int curSccNum = 0;
	int oneTimeFlag = false;

	for (auto iterTemp = vPrime2Use.begin(); iterTemp != vPrime2Use.end(); ++iterTemp)
	{
		int pointId = (*iterTemp).getPointId();
		Point &pointTemp = graph.getNode(pointId);

		if ((path.getUsed(pointId) == false) && (oneTimeFlag == false))
		{
			curSccNum = pointTemp.getSccNum();

			oneTimeFlag = true;
		}

		if ((oneTimeFlag == true) && (curSccNum != pointTemp.getSccNum()))
		{
			return iterTemp;
		}
	}

	//if all vprime are invalid, or all vprime are the same scc,return end
	return vPrime2Use.end();
}


list<Point>::iterator VPrimeToUse::insertVPrime(GraphBase &graph, Path &path, int vPrimeId)
{
	Point &pointTemp = graph.getNode(vPrimeId);
	path.setUsed(vPrimeId, false);

	for (auto iterTemp = vPrime2Use.begin(); iterTemp != vPrime2Use.end(); ++iterTemp)
	{
		if (pointTemp.getSccNum() >= (*iterTemp).getSccNum())
		{
			return vPrime2Use.insert(iterTemp, pointTemp);
		}
	}

	return vPrime2Use.insert(vPrime2Use.end(), pointTemp);
}

list<Point>::iterator VPrimeToUse::deleteVPrime(list<Point>::iterator iter)
{
	return vPrime2Use.erase(iter);
}

void VPrimeToUse::sortVPrime()//sort vprimevector
{
	//stable_sort(vPrime2Use.begin(), vPrime2Use.end());
	vPrime2Use.sort();
	//sort(vPrime2Use.begin(), vPrime2Use.end());
}

int VPrimeToUse::deleteUsedVPrime(GraphBase &graph, Path &path)
{
	list<Point>::iterator iterBefore;

	for (auto iterTemp = vPrime2Use.begin(); iterTemp != vPrime2Use.end(); )
	{
		
		if (path.getUsed((*iterTemp).getPointId()) == true)
		{
			//if we erase, this iter will be invalid
			iterBefore = iterTemp;
			++iterTemp;

			vPrime2Use.erase(iterBefore);
		}
		else
		{
			++iterTemp;
		}
	}

	return EXIT_SUCCESS;
}

void VPrimeToUse::printVprime()
{
	for (auto iter = vPrime2Use.begin(); iter != vPrime2Use.end(); ++iter)
	{
		cout << (*iter).getPointId() << " " << "topoOrder:" << (*iter).getTopoOrder()
			<< " " << "inoutdegree:" << (*iter).getInoutDegree() << endl;
	}
}

//random the order of vprime belong to the same scc
int VPrimeToUse::randomOrder()
{
	auto iterVPrime = vPrime2Use.begin();
	int startIndex = 0;
	int endIndex = 0;
	int curSccNum = 0;
	static unsigned int seedCnt = 0;

	curSccNum = (*iterVPrime).getSccNum();

	for (; iterVPrime != vPrime2Use.end(); ++iterVPrime)
	{
		if (curSccNum != (*iterVPrime).getSccNum())
		{
			//update cursccnum
			curSccNum = (*iterVPrime).getSccNum();

			//if more than 3 node in one scc
			if ((endIndex - startIndex) >= 3)
			{
				//exchange from startIndex to endIndex-1,[startIndex,endIndex)
				for (int i = 0; i < (endIndex - startIndex); i++)
				{
					srand((unsigned int)(seedCnt));
					int ii = (rand()) % (endIndex - startIndex) + startIndex;//[startIndex,endIndex)
					srand((unsigned int)(seedCnt + 1000));
					int jj = (rand()) % (endIndex - startIndex) + startIndex;//[startIndex,endIndex)

					exchange(ii, jj);
					seedCnt++;//add seed
					//cout << "ii: " << ii << " jj: " << jj << endl;
				}
			}

			//uapdate index
			startIndex = endIndex;
		}
		endIndex++;
	}

	//for the rest
	//if more than 3 node in one scc
	if ((endIndex - startIndex) >= 3)
	{
		//exchange from startIndex to endIndex-1,[startIndex,endIndex)
		for (int i = 0; i < (endIndex - startIndex); i++)
		{
			srand((unsigned int)(seedCnt));
			int ii = (rand()) % (endIndex - startIndex) + startIndex;//[startIndex,endIndex)
			srand((unsigned int)(seedCnt + 1000));
			int jj = (rand()) % (endIndex - startIndex) + startIndex;//[startIndex,endIndex)

			exchange(ii, jj);
			seedCnt++;//add seed
			//cout << "ii: " << ii << " jj: " << jj << endl;
		}
	}

	return EXIT_SUCCESS;
}

