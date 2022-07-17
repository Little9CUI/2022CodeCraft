#pragma once
/*
1.本头文件存放customer和server的struct结构
	避免循环依赖
2.存放全局define，所有头文件都要包含这个

*/
#pragma once
#ifndef _STRUCTFILE_H_
#define _STRUCTFILE_H_
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
#include <complex>
using namespace std;
#define TEST
//#define SUBMIT

typedef pair<int, int> ElementType;
typedef struct HeapStruct* MaxHeap;
struct HeapStruct {
	ElementType* Elements;//存储堆元素的数组
	int Size;//堆的当前元素个数
	int Capacity;//堆的最大容量
	//~HeapStruct() {
	//	delete[] Elements;
	//}
};

//递减
bool static cmp(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second > b.second;
}
//递增
bool static cmpUp(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second < b.second;
}
//递减
bool static customerStreamDown(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}

//递减
bool static cmpCenterDown(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}



struct streamInfo {
	string cusName;//客户端的名字
	string streamID;//流的ID
	int streamNum;//流的大小
	int time;//所属的天
	int index;//在数组里的第几个位置
};

struct serStruct {
	int nodeNum;
	double cost = 0.0;//记录当前时刻的开销
	pair<int, int> historyConstrain = make_pair(0, 0);//95上限,第几天，值是多少
	int freeDays = 0;//自由日天数，一开始初始化为0.05总天数
	int constrain = 0;
	bool open = false;//记录是否开启
	int sortTimes = 0;

	unordered_map<string, int> cusConnected;//记录连接的客户端以及流量
	vector < pair<int, int> > dataAllocate;//每一时刻的流量分配
	vector < pair<int, int> > cache;//每一时刻的流量缓存
	vector < pair<int, int> > dataAllocateOrigin;//每一时刻的流量分配
	vector < pair<int, int> > cacheOrigin;//每一时刻的流量缓存
	vector < pair<int, int> > sortDataAllocate;//每一时刻的流量分配
	vector<pair<int, int>> dailyNeeds;//预估每天的请求,<天，需求>
	unordered_map<int, int> dayIndex;//用来存储天在堆数组的下标位置，加快查找
	MaxHeap dailyNeedsMaxHeap;
	MaxHeap historyConstrainHeap;//记录虚上限的最小堆，堆顶是虚上限
	unordered_map<int, int> historyConstrainHeapIndex;//记录虚上限的最小堆中，每一个时刻的位置
	unordered_set<int> freeDaySet;//记录他是freeday的时刻
	unordered_set<int> unableDecrease;//不能下降的时刻
	//unordered_map<string, vector<int>> centerAllocate;//流的ID，时间序列（对应时刻的流的大小）
	//unordered_map<string, unordered_map<int,int>> centerAllocate;//流的ID，时间序列（对应时刻的流的大小）
	vector < unordered_set<string> > centerAllocateSet;//每一时刻的流量分配
	unordered_map<string, int> centerAllocatePerDay;//流的ID，d


};

struct cusStruct {
	int nodeNum;
	int num = 0;
	unordered_map<string, int> serConnected;//记录连接的边缘节点以及流量
	vector<vector<pair<string, int>>> dataNeeds;//每天的需求，行是第几天，列是每个种类的需求<种类，大小>
	vector<vector<bool>> dataNeedsIfSatisfied;//每天的需求，是否被满足
	vector<int> unSatisfiedNum;//每天没有被满足的需求个数
	vector<vector<bool>> dataNeedsIfSatisfiedOrigin;//每天的需求，是否被满足
	vector<int> unSatisfiedNumOrigin;//每天没有被满足的需求个数

};



#endif