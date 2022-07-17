#pragma once
/*
1.��ͷ�ļ����customer��server��struct�ṹ
	����ѭ������
2.���ȫ��define������ͷ�ļ���Ҫ�������

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
	ElementType* Elements;//�洢��Ԫ�ص�����
	int Size;//�ѵĵ�ǰԪ�ظ���
	int Capacity;//�ѵ��������
	//~HeapStruct() {
	//	delete[] Elements;
	//}
};

//�ݼ�
bool static cmp(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second > b.second;
}
//����
bool static cmpUp(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second < b.second;
}
//�ݼ�
bool static customerStreamDown(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}

//�ݼ�
bool static cmpCenterDown(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}



struct streamInfo {
	string cusName;//�ͻ��˵�����
	string streamID;//����ID
	int streamNum;//���Ĵ�С
	int time;//��������
	int index;//��������ĵڼ���λ��
};

struct serStruct {
	int nodeNum;
	double cost = 0.0;//��¼��ǰʱ�̵Ŀ���
	pair<int, int> historyConstrain = make_pair(0, 0);//95����,�ڼ��죬ֵ�Ƕ���
	int freeDays = 0;//������������һ��ʼ��ʼ��Ϊ0.05������
	int constrain = 0;
	bool open = false;//��¼�Ƿ���
	int sortTimes = 0;

	unordered_map<string, int> cusConnected;//��¼���ӵĿͻ����Լ�����
	vector < pair<int, int> > dataAllocate;//ÿһʱ�̵���������
	vector < pair<int, int> > cache;//ÿһʱ�̵���������
	vector < pair<int, int> > dataAllocateOrigin;//ÿһʱ�̵���������
	vector < pair<int, int> > cacheOrigin;//ÿһʱ�̵���������
	vector < pair<int, int> > sortDataAllocate;//ÿһʱ�̵���������
	vector<pair<int, int>> dailyNeeds;//Ԥ��ÿ�������,<�죬����>
	unordered_map<int, int> dayIndex;//�����洢���ڶ�������±�λ�ã��ӿ����
	MaxHeap dailyNeedsMaxHeap;
	MaxHeap historyConstrainHeap;//��¼�����޵���С�ѣ��Ѷ���������
	unordered_map<int, int> historyConstrainHeapIndex;//��¼�����޵���С���У�ÿһ��ʱ�̵�λ��
	unordered_set<int> freeDaySet;//��¼����freeday��ʱ��
	unordered_set<int> unableDecrease;//�����½���ʱ��
	//unordered_map<string, vector<int>> centerAllocate;//����ID��ʱ�����У���Ӧʱ�̵����Ĵ�С��
	//unordered_map<string, unordered_map<int,int>> centerAllocate;//����ID��ʱ�����У���Ӧʱ�̵����Ĵ�С��
	vector < unordered_set<string> > centerAllocateSet;//ÿһʱ�̵���������
	unordered_map<string, int> centerAllocatePerDay;//����ID��d


};

struct cusStruct {
	int nodeNum;
	int num = 0;
	unordered_map<string, int> serConnected;//��¼���ӵı�Ե�ڵ��Լ�����
	vector<vector<pair<string, int>>> dataNeeds;//ÿ����������ǵڼ��죬����ÿ�����������<���࣬��С>
	vector<vector<bool>> dataNeedsIfSatisfied;//ÿ��������Ƿ�����
	vector<int> unSatisfiedNum;//ÿ��û�б�������������
	vector<vector<bool>> dataNeedsIfSatisfiedOrigin;//ÿ��������Ƿ�����
	vector<int> unSatisfiedNumOrigin;//ÿ��û�б�������������

};



#endif