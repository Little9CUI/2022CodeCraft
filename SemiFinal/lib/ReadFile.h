#pragma once
#ifndef _READFILE_
#define _READFILE_

#include <StructFile.h>

#ifdef SUBMIT
const string site_bandwidthPath("/data/site_bandwidth.csv");
const string demandPath("/data/demand.csv");
const string OutputPath("/output/solution.txt");
const string qosPath("/data/qos.csv");
const string configPath("/data/config.ini");

#endif // SUBMIT

#ifdef TEST
const string site_bandwidthPath("site_bandwidth.csv");
const string demandPath("demand.csv");
const string OutputPath("solution.txt");
const string qosPath("qos.csv");
const string configPath("config.ini");
#endif // SUBMIT


class ReadFileClass {
public:
	void readQosConstrain(int& qosConstrain, int& V, double& A);

	void readQos(int qosConstrain, unordered_map < string, cusStruct >& customer, unordered_map < string, serStruct >& serverMap);
	//返回总共几个时刻
	int readDemand(unordered_map < string, cusStruct >& customer, vector<pair<int, int>>& demands);
	void readServerData(unordered_map < string, serStruct >& server, int timeLen);
	//给服务器和客户端编号
	void setNode(vector <string>& nodeIndex, vector<string>& cusNodeIndex, vector<string>& serNodeIndex, unordered_map < string, serStruct >& server, unordered_map < string, cusStruct >& customer);

	void OutputFile(vector<string>& ans);

	void OutputData(vector<string>& cusNodeIndex, vector<string>& serNodeIndex, const vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int timeLen, int cusNum);
};


#endif