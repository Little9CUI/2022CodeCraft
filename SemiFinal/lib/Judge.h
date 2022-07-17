#pragma once
#ifndef _JUDGE_
#define _JUDGE_
#include <StructFile.h>
#include "Customer.h"
#include "Server.h"
class JudgeClass {
public:
	//����������
	void testHisConstrain(serverClass& myServer, uint16_t timeLen);

	//�������Ľڵ�cost
	void testCenterCost(serverClass& myServer, uint16_t timeLen, double A);

	//���Ի���
	void testCache(serverClass& myServer, uint16_t timeLen);


	void testOneCache(serverClass& myServer, string serName, uint16_t timeLen);

	//��֤��������û�г���������
	void JudgeServerConstrain(serverClass& myServer, uint16_t timeLen);

	void JudgeCustomerNeeds(uint16_t timeLen, const serverClass& myServer, const customerClass& myCustomer, const vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate);

	//����������
	double calCostTotal(unordered_map < string, serStruct >& server, int timeLen, int V, double A);

};
#endif
