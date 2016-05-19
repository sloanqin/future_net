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

#ifndef _PATHPIECE_H
#define _PATHPIECE_H
#include "environment.h"
#include "point.h"

using namespace std;

// set linkProperty, LINKED CRACKED
#define LINKED 1
#define CRACKED 2
#define SUPERLINK 3

class PathPiece
{
public:
	PathPiece(){ linkNodesVector.resize(0); };

	//如果用到了动态内存，该类的拷贝函数，赋值函数，析构函数需要自己定义下

	//析构函数

	//get value
	inline int getFirstNode() const{ return firstNode; }
	inline int getLastNode() const{ return lastNode; }
	inline int getLinkProperty() const{ return linkProperty; }
	inline int getWeight() const{ return weight; }
	inline vector<int>& getLinkNodesVector() { return linkNodesVector; }
	//inline PathPiece* getPNextPiece() const { return pNextPiece; }//qyymodify

	//set value
	inline void setFirstNode(int xx) { firstNode = xx; }
	inline void setLastNode(int xx) { lastNode = xx; }
	inline void setLinkProperty(int xx) { linkProperty = xx; }
	inline void setWeight(int xx) { weight = xx; }
	//inline void setPNextPiece(PathPiece *xx) { pNextPiece = xx; }//qyymodify

private:
	int firstNode = INVALID;
	int lastNode = INVALID;
	int linkProperty = INVALID;
	int weight = INFINITEWEIGHT;
	vector<int> linkNodesVector;
	//PathPiece *pNextPiece = NULL;//qyymodify
};

#endif