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

#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include <limits.h>

#if defined WIN32
#include <windows.h>  
#else

#endif/*WIN32*/

//graph input property
#define MAXPOINTNUM 2001 //点数限制
#define MAXOUTGEDREE 21  //点的最大出度
#define MAXVPRIME 101  //ie num of V'
#define INFINITEWEIGHT (0x01010101)//16843009
#define MAXPATHORDINARYWEIGHT (100*2000)

//pattern
#define _cplusplus
//#define DEBUG  //调试模式
//#define RELEASE
#define TIMELIMIT//时间限制
//#define MEASURE//测量分析

//
extern int MAXNODEID;//比如图中node最大的ID是19，则该值=19,!!!注意该变量的定义改了，新加入的老文件，首先要在for循环中改掉条件
extern int TIMELIMITNUM;

//graph kind
#define GRAPHLIST 2
#define GRAPHVECTOR 3
#define GRAPHVECTOR_REVERSE 5
#define GRAPHARRAY 4

//solution kind
#define NOSOLUTION 10
#define MAYBESOLUTION 11
#define HAVESOLUTION 12

//color,for dfs or bfs
#define WHITE 0
#define GRAY 1
#define BLACK 2

//使用一些代码优化的方法，加快程序的运行速度
//#define CODEOPTIMIZE

//for bfs search result,if can find a path,return FOUND
#define FOUND 3
#define NOTFOUND 4
#define NA 5 //no answer

//choose which method to search a path in optimization
//#define SEARCHPATHBFS
#define SEARCHPATHDIJKSTRA

//
#define CONVERGENCETIMESLIMITS 2

//limit of opt
#define VIOLENTNUMLIMIT 31
#define THREE_OPTBOUNDARIES 500 //[0,THREE_OPTBOUNDARIES) slow
#define FOUR_OPTBOUNDARIES 400
#define FIVE_OPTBOUNDARIES 200

//when out of time,func optimizeNodeSwitch return outoftime
#define OUTOFTIME 18

//flag for the result of an algorithm
#define ALGORITHMSUCCESS 1
#define ALGORITHMFAILURE 2

//save time to result.csv
//#define SAVETIME

//valid and invalid
#define INVALID INT_MIN
#define VALID INT_MAX

//macro define for point
//表示点的种类，或者此点不存在
#define POINT_START 1
#define POINT_END 2
#define POINT_V 3
#define POINT_VPRIME 4

//point validity，有的点是无效的（由于点的编号不一定连续）
//有的点处于肯定不符合要求，不被考虑，用POINT_DELETE表示
#define POINT_VALID 1
#define POINT_DELETE 2
#define POINT_INVALID INVALID

#endif/*_ENVIRONMENT_H*/


