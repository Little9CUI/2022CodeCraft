#pragma once
#pragma once

#ifndef _ALLSTREAMHEAP_H_
#define _ALLSTREAMHEAP_H_

#include <StructFile.h>

bool streamInfoDown(const streamInfo& a, streamInfo& b);
bool kindCmp(const pair<string, int>& a, const pair<string, int>& b);


typedef struct AllStreamHeapStruct* AllStreamHeap;
struct AllStreamHeapStruct {
	streamInfo* Elements;//�洢��Ԫ�ص����飬
	int Size;//�ѵĵ�ǰԪ�ظ���
	int Capacity;//�ѵ��������
	//~AllStreamHeapStruct() {
	//	delete Elements;
	//}
};

class AllStreamHeapClass {
public:
	AllStreamHeap dataHeap;


	~AllStreamHeapClass();


	//��һ������������ʱ���ڱ�ɶ�
	void MakeHeap(const vector<streamInfo>& streamData);

	//��ȡ�Ѷ�Ԫ��
	streamInfo GetMax();

	//�Ƿ���
	bool IsFull();

	//�Ƿ�Ϊ��
	bool IsEmpty();


	//ɾ���Ѷ�Ԫ��
	streamInfo DeleteMax();


	//���������е�index���ڵ㣬���ص��������ĸ��ڵ�
	void adjustHeap(int index);

};



#endif