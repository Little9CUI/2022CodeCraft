#pragma once
#ifndef _SERVER_H_
#define _SERVER_H_

#include <StructFile.h>

class serverClass {
public:
	unordered_map<string, serStruct> serMap;

	MaxHeap CenterCiteHeap;
	unordered_map<int, int> centerHeapIndex;
	vector < pair<int, int> > centerDataAllocate;//每一时刻的流量分配
	unordered_set<int> centerFreeDaySet;
	pair<int, int> centerCistHistoryConstrain = make_pair(0, 0);
	double centerCost = 0.0;
	vector<unordered_map<string, unordered_map<string,int>>> streamSource;//记录每一个时刻，每一个流的来源是哪些服务器


	void MakeCenterHeap(int timeLen);

	//把一个数组在线性时间内变成堆
	MaxHeap MakeHeap(vector<pair<int, int>>& target);
	//获取堆顶元素
	ElementType GetMax(MaxHeap H);
	//删除堆顶元素
	ElementType DeleteMax(MaxHeap H, string serName);
	bool IsEmpty(MaxHeap H);
	//调整堆
	void adjustHeap(MaxHeap H, int index, string serName);


	//给每个服务器进行白嫖天数初始化
	void ServerFreeDaysInit(int timeLen, vector<pair<int, int>>& demands);

	//初始化所有服务器的总需求
	void RecordCusNeeds(unordered_map<string, cusStruct>& cusMap, int timeLen);

	//选一个需求最大的服务器，返回名字;取服务器上限和需求最小值
	string FindMaxServer(vector<vector<string>>& freeServerInDay);

	//判断一个流是否能装进去
	bool IfCanPut(string serName, int time, int streamNum, int constrain);

	//最大白嫖填满服务器
	void FullOneServer(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time);

	//最大白嫖那一步更新cache
	void UpdataCacheForFirstStep(int time, string serName, int V);
	//第二步第三步更新cache
	void UpdataCacheSecondStep(int time, string serName, int V);

	//把一个流放进一个服务器里
	void putOneStreamToServer(bool toV, int index, int needs, string streamID,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, string cusName, int time, int V, double A);

	//给对应客户端分配资源到V
	void AllocateToV(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, int time, int V, double A);
	//给对应客户端分配资源到V
	void AllocateToVNew(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, int time, int V, double A);

	//在白嫖之后，有可能由于缓存，导致有一些服务器产生了cost,更新这些cost/虚上限
	void UpdataHisConstrainAfterFreeCost(int timeLen, int V);

	//判断这个服务器是否满了
	bool ifFullServer(const unordered_map<string, serStruct>& serMap, string serName, int time);

	//计算边缘节点新的开销，如果把这个流放进去，cost变成了多少
	double newCitecostCal(string serName, int time, int streamNum, int V);

	//找如果放进去开销最小的
	string findMinCostServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A);
	//找当前容量最小的
	string findMinServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A);




	//返回一个服务器在某一天真正的流量，需要加上前一个时刻的cache
	uint16_t GetOneServerCurrentDayActualData(string serName, uint16_t t);


	void FullOneServerNew(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time);

	//调整中心节点的虚上限堆
	void adjustCenterCiteHistoryHeap(int index);

	//更新中心节点的虚上限
	void UpdateCenterCiteHisConstrain(string serName, int time, string streamId, int streamNum, double A);

	//更新中心节点的虚上限，之前那个有bug，linux下虚上限判断不准
	void UpdataHisConstrainAfterFreeCostNew(int timeLen, int V);

	////建立中心节点和边缘节点的最小堆，堆顶元素是虚上限
	//void MakeCenterCiteANDCiteHistoryConstrainHeap();
	//计算中心节点开销
	double costCalCenter(string serName, int time, int streamNum, int V, string streamID, double A);

	void sortSteamMaxOrder(unordered_map<string, vector<streamInfo>>& streamVecMap, vector<pair<string, int>>& streamKinds);




	/**动态调整***/


	//给所有服务器的data排序
	void sortDataAllocate();

	//动态调整
	void AdjustAllocate(clock_t startTime, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct> cusMap, int V, vector < pair<int, int> >& centerDataAllocate);
	//降低一个服务器的一个时刻
//outSerName在time时刻想要降低到target，目标是全降低，最后返回的时候如果能降到target就够了
	bool DownOneServer(int time, int downTime, int virTime, string outSerName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, int V, int virCenterLim, unordered_set<int>&centerNodeFree);
	//只排一个服务器
	void sortOneServerDataAllocate(string serName);

	//下降白嫖的天
	bool DownFreeDayServer(int indexVirTime, string serName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, int V, unordered_set<int>&centerNodeFree, int virCenterLim);

	//降低一个服务器的一个时刻，cost小于value可以降
	bool DownOneServerCostValue(int time, int downTime, int virTime, string outSerName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, unordered_map<string, cusStruct>& cusMap);

	//判断能不能降
	bool AdjustOneStreamToOneServerCostValue(string streamId, int streamNum, string outSerName, string toSerName, int time, int virTime,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V);
	//尝试把一个流放到一个服务器里，返回值是还需要多少空间
	int AdjustOneStreamToOneServer(string streamId, int streamNum, string serName, int time, int virTime,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, int virCenterLim,unordered_set<int>&centerNodeFree);
	//获取新的上限，需要考虑之前时刻和之后时刻
	uint16_t getNewConstrain(string serName, uint16_t time);
	//用于sortdataallocate，把cache加上
	void UpdataActualDataPlusCache(vector<pair<int, int>>& data, string serName);

	//动态调整里的更新缓存
	void UpdataCacheUingInAllocate(int time, string serName, int V);
	//获取新的上限，需要考虑之前时刻和之后时刻
	uint16_t getNewConstrainNew(string serName, uint16_t time);
	//判断一个流是否能装进去，动态调整用
	bool IfCanPutAdjust(string serName, int time, int streamNum, int constrain, const string streamID, const int virCenterLim, unordered_set<int>&centerNodeFree);
	//得到服务器新的上限
	int getNewConstrain(string serName, int time, int virLimit);

};

#endif