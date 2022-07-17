#pragma once
#ifndef _JUDGE_
#define _JUDGE_
#include <StructFile.h>
#include "Customer.h"
#include "Server.h"
class JudgeClass {
public:
	//测试虚上线
	void testHisConstrain(serverClass& myServer, uint16_t timeLen);

	//测试中心节点cost
	void testCenterCost(serverClass& myServer, uint16_t timeLen, double A);

	//测试缓存
	void testCache(serverClass& myServer, uint16_t timeLen);


	void testOneCache(serverClass& myServer, string serName, uint16_t timeLen);

	//验证服务器有没有超过总上限
	void JudgeServerConstrain(serverClass& myServer, uint16_t timeLen);

	void JudgeCustomerNeeds(uint16_t timeLen, const serverClass& myServer, const customerClass& myCustomer, const vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate);

	//计算总需求
	double calCostTotal(unordered_map < string, serStruct >& server, int timeLen, int V, double A);

};
#endif
