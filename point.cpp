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
#include "point.h"
#include "assist.h"

Point::Point() :pointId(POINT_INVALID), sccNum(INVALID),topoOrder(INVALID), inoutDegree(INVALID)
{

}


//�Ƚ�����point�Ĵ�С��sort�㷨���õ��������
//���ȱȽ�topoֵ��topoֵ���С
//��ͬtopoֵ�£�inoutdegree С��С
//bool Point::operator<(const Point &rPoint)
//{
//	//if (topoOrder>rPoint.getTopoOrder())
//	//{
//	//	return true;
//	//}
//	//else if (topoOrder < rPoint.getTopoOrder())
//	//{
//	//	return false;
//	//}
//	//else if (inoutDegree <= rPoint.getInoutDegree())
//	//{
//	//	return true;
//	//}
//	//else 
//	//{
//	//	return false;
//	//}
//	//����������߼�������ȥif�жϣ�������ˮ��
//	return ((topoOrder>rPoint.getTopoOrder()) || 
//		((topoOrder == rPoint.getTopoOrder()) && (inoutDegree < rPoint.getInoutDegree())));
//}

bool operator<(const Point &lPoint, const Point &rPoint)//�Ƚ�����point�Ĵ�С��sort�㷨���õ��������
{
	return ((lPoint.getTopoOrder()>rPoint.getTopoOrder()) ||
		((lPoint.getTopoOrder() == rPoint.getTopoOrder()) && (lPoint.getInoutDegree() < rPoint.getInoutDegree())));
}
