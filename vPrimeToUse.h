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
#ifndef _VPRIMETOUSE_H
#define _VPRIMETOUSE_H

#include "environment.h"
#include <list>
#include "point.h"
#include "graphBase.h"
#include "path.h"
#include <algorithm>
#include <iostream>

using namespace std;

class VPrimeToUse
{
public:
	VPrimeToUse(){};
	VPrimeToUse(GraphBase &graph, Path &path);

	//return the iter after last point of current scc num
	list<Point>::iterator curSccEnd(GraphBase &graph, Path &path);

	//insert vprime point to list(the position is according to scc),return iter of it
	list<Point>::iterator insertVPrime(GraphBase &graph, Path &path, int vPrimeId);

	//delete a vprime,and return the lter after it
	list<Point>::iterator deleteVPrime(list<Point>::iterator iter);

	void sortVPrime();//sort vprimevector

	//copy valid vprime point to vprimetouse
	int refreshVPrime(GraphBase &graph, Path &path);
	int refreshAgainVPrime(GraphBase &graph, Path &path);//random order of vprime

	//iter of list
	inline list<Point>::iterator begin(){ return vPrime2Use.begin(); }
	inline list<Point>::iterator end(){ return vPrime2Use.end(); }

	inline int empty(){ return vPrime2Use.empty(); }

	int deleteUsedVPrime(GraphBase &graph, Path &path);//delete vprime that has been used

	//get value 
	inline int getTopoOrderMax() const { return topoOrderMax; }
	inline int getTopoOrderMin() const { return topoOrderMin; }

	void printVprime();//print all vprime points in vprime2use

	int randomOrder();//random the order of vprime belong to the same scc

private:
	list<Point> vPrime2Use;
	int topoOrderMax = 0;
	int topoOrderMin = 0;

	//exchange 2 elements of vprime
	inline int exchange(int ii, int jj)
	{
		Point pointTemp;

		list<Point>::iterator iterIi = vPrime2Use.begin();
		list<Point>::iterator iterJj = vPrime2Use.begin();

		for (int i = 0; i < ii; ++i)
		{
			++iterIi;
		}

		for (int i = 0; i < jj; ++i)
		{
			++iterJj;
		}

		pointTemp = (*iterIi);
		(*iterIi) = (*iterJj);
		(*iterJj) = pointTemp;

		return EXIT_SUCCESS;
	}
};

#endif

