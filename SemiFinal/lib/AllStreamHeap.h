#pragma once
#pragma once

#ifndef _ALLSTREAMHEAP_H_
#define _ALLSTREAMHEAP_H_

#include <StructFile.h>

bool streamInfoDown(const streamInfo& a, streamInfo& b);
bool kindCmp(const pair<string, int>& a, const pair<string, int>& b);


typedef struct AllStreamHeapStruct* AllStreamHeap;
struct AllStreamHeapStruct {
	streamInfo* Elements;//存储堆元素的数组，
	int Size;//堆的当前元素个数
	int Capacity;//堆的最大容量
	//~AllStreamHeapStruct() {
	//	delete Elements;
	//}
};

class AllStreamHeapClass {
public:
	AllStreamHeap dataHeap;


	~AllStreamHeapClass();


	//把一个数组在线性时间内变成堆
	void MakeHeap(const vector<streamInfo>& streamData);

	//获取堆顶元素
	streamInfo GetMax();

	//是否满
	bool IsFull();

	//是否为空
	bool IsEmpty();


	//删除堆顶元素
	streamInfo DeleteMax();


	//调整数组中第index个节点，返回调整到了哪个节点
	void adjustHeap(int index);

};



#endif