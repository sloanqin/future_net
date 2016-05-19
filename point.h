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

#ifndef _POINT_H
#define _POINT_H
#include "environment.h"
#include "edge.h"
#include <vector>

using namespace std;

class PathPiece;

class Point
{
	friend bool operator<(const Point &lPoint, const Point &rPoint);//�Ƚ�����point�Ĵ�С��sort�㷨���õ��������
public:
	Point();

	//ʹ�úϳɿ�����ֵ�����

	//bool operator<(const Point &rPoint);//�Ƚ�����point�Ĵ�С��sort�㷨���õ��������

	//get value
	inline int getPointId() const { return pointId; }
	inline int getSccNum() const { return sccNum; }
	inline int getTopoOrder() const { return topoOrder; }
	inline int getInoutDegree() const { return inoutDegree; }
	inline vector<Edge>& getEdgeVector() { return edgeVector; }

	//set value
	inline void setPointId(int xx) { pointId = xx; }
	inline void setSccNum(int xx) { sccNum = xx; }
	inline void setTopoOrder(int xx) { topoOrder = xx; }
	inline void setInoutDegree(int xx) { inoutDegree = xx; }

	inline void insertEdge(Edge &xx){ edgeVector.push_back(xx); }

private:
	int pointId = POINT_INVALID;//��ı�ţ���==POINT_INVALID����ʾ�������ͼ�иõ㲢������
	int sccNum = INVALID;//id of scc,strongly connected component
	int topoOrder = INVALID;//topological order
	int inoutDegree = INVALID;//=(indegree + outdegree)>>1
	vector<Edge> edgeVector;//�Ӹõ�����ı߹��ɵ�����

};

bool operator<(const Point &lPoint, const Point &rPoint);//�Ƚ�����point�Ĵ�С��sort�㷨���õ��������
#endif
