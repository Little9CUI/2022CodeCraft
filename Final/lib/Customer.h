#pragma once
#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_


#include <StructFile.h>

using namespace std;




class customerClass {
public:
	unordered_map<string, cusStruct> cusMap;


	//���³�ʼ��
	void initCus();

	//����������������
	void sortStream(int timeLen);

	//���������������д��true
	void setZeroStreamTrue(int timeLen);






};
#endif