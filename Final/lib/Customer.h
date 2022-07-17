#pragma once
#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_


#include <StructFile.h>

using namespace std;




class customerClass {
public:
	unordered_map<string, cusStruct> cusMap;


	//重新初始化
	void initCus();

	//对所有流进行排序
	void sortStream(int timeLen);

	//把需求是零的流都写成true
	void setZeroStreamTrue(int timeLen);






};
#endif