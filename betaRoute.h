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
#ifndef _BETAROUTE_H
#define _BETAROUTE_H

#include "environment.h"
#include "graphVector.h"
#include "vPrimeToUse.h"
#include "path.h"
#include "pathPiece.h"

#define CRACKLIMITS 3

class BetaRoute
{
public:
	BetaRoute();
	~BetaRoute();

	//find a path of graph,not the best
	int cmPath(GraphVector &graph, VPrimeToUse &vPrime, Path &path);

	//add super link in path piece
	int addSuperLink(PathPiece &pPiece);

private:


};

#endif/*_BETAROUTE_H*/