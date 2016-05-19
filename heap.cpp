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
#include "heap.h"
#include <assert.h>

inline void Heap::exchange(int indexA, int indexB)
{
	//exchange key
	int temp = aa[indexA];
	aa[indexA] = aa[indexB];
	aa[indexB] = temp;

	//exchange position of key
	temp = handlePosition[aa[indexA]];
	handlePosition[aa[indexA]] = handlePosition[aa[indexB]];
	handlePosition[aa[indexB]] = temp;
}

Heap::Heap()
{

}

//heap operation
void Heap::buildHeap(int *aaIn, int *handleValueIn, int *handlePositionIn, int sizeIn)
{
	aa = aaIn;
	handleValue = handleValueIn;
	handlePosition = handlePositionIn;
	heapSize = sizeIn;

	//from size/2 to 1
	for (int i = (sizeIn >> 1); i>0; i--)
	{
		minHeap(i);
	}
}

int Heap::extractMin()
{
#ifdef DEBUG
	assert(heapSize >= 1);
#endif

	int minimum = aa[1];

	exchange(1,heapSize);
	heapSize--;
	minHeap(1);

	return minimum;//return the minimum key
}

//decrease the value of key vv ,not key itself
void Heap::decreaseKey(int key, int newValue)
{
#ifdef DEBUG
	assert(handleValue[key] >= newValue);
#endif

	//change value of key
	handleValue[key] = newValue;

	//maintain heap
	int i = handlePosition[key];
	while ((i > 1) && (handleValue[aa[(i >> 1)]] > handleValue[aa[i]]))
	{
		exchange(i, (i >> 1));

		i = (i >> 1);//the parent of i=i/2
	}

}

void Heap::minHeap(int index)
{
	int minimum = index;
	int left = (index << 1);
	int right = (index << 1) + 1;

	if (left <= heapSize && handleValue[aa[left]] < handleValue[aa[index]])//与左孩子比较  
	{
		minimum = left;
	}
	if (right <= heapSize && handleValue[aa[right]] < handleValue[aa[minimum]])//与右孩子比较  
	{
		minimum = right;
	}
	if (minimum != index)
	{
		exchange(minimum, index);
		minHeap(minimum);
	}
}

