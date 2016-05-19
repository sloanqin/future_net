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
#ifndef _INOUT_H_
#define _INOUT_H_


//读取文件并按行输出到buff。
//buff为一个指针数组，每一个元素是一个字符指针，对应文件中一行的内容。
//spec为允许解析的最大行数。
int read_file(char ** const buff, const unsigned int spec, const char * const filename);

//将result缓冲区中的内容写入文件，写入方式为覆盖写入
void write_result(const char * const filename);

//释放读文件的缓冲区
void release_buff(char ** const buff, const int valid_item_num);


#endif




