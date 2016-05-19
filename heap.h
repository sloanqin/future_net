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
//this class is designed for using in searchpathpiece dijsktra
#ifndef _HEAP_H
#define _HEAP_H

#include "environment.h"
#include <stdio.h>

using namespace std;

class Heap
{
public:
	Heap();

	void exchange(int a, int b);//inline func,exchange key and the position of key

	//heap operation
	void buildHeap(int *aaIn, int *handleValueIn, int *handlePositionIn, int sizeIn);//create a heap
	int extractMin();//return the minimum key,and delete it from heap
	void decreaseKey(int key, int newValue);//change the value of key,and maintain heap
	void minHeap(int index);//maintain heap in index i and it's pi index

	inline int isEmpty(){ return (heapSize == 0); }

private:
	int *aa;//input index of aa,output key
	int *handleValue;//input key,output value of key
	int *handlePosition;//input key,output index of aa
	int heapSize = 0;//size is the element number of heap,such as aa[1...n],size==n

};

#endif