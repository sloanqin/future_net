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
#ifndef _PATH_EDGE_H
#define _PATH_EDGE_H

#include "path.h"
#include "graphVector.h"
#include <utility>
#include <unordered_map>

using namespace std;

class PathEdge
{
public:
	PathEdge(){};

	PathEdge(GraphVector &graph, Path &path){ refreshPathEdge(graph, path); };
	PathEdge(GraphVector &graph, Path &path, PathEdge &otherPathEdge){ refreshPathEdge(graph, path, otherPathEdge); };
	PathEdge(PathEdge &path1Edge, PathEdge &path2Edge);

	~PathEdge(){};

	int refreshPathEdge(GraphVector &graph, Path &path);
	int refreshPathEdge(GraphVector &graph, Path &path, PathEdge &otherPathEdge);//concern other pathEdge's influence

	int getEdgeWeightSum();

	inline bool hasEdge(int uu, int vv) const { return pathUsedEdges.count(((uu << SHIFTSIXTEEN) + vv)); }
	inline int getEdgeId(int uu, int vv) const { return pathUsedEdges.at(((uu << SHIFTSIXTEEN) + vv)).first; }
	inline int getEdgeWeight(int uu, int vv) const { return pathUsedEdges.at(((uu << SHIFTSIXTEEN) + vv)).second; }
	inline int getEdgeNum() const { return pathUsedEdges.size(); }

protected:
	unordered_map<int, pair<int, int>> pathUsedEdges;//pair<edgeId,weight>
	enum SHIFTNUM { SHIFTSIXTEEN = 16 };

private:

};


#endif
