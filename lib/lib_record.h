#ifndef _LIB_RECORD_H_
#define _LIB_RECORD_H_

enum PATH_ID
{
	WORK_PATH = 1,
	BACK_PATH = 2
};

void record_result(const PATH_ID, const unsigned short edge);
void clear_result();

#endif