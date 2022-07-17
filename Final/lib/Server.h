#pragma once
#ifndef _SERVER_H_
#define _SERVER_H_

#include <StructFile.h>

class serverClass {
public:
	unordered_map<string, serStruct> serMap;

	MaxHeap CenterCiteHeap;
	unordered_map<int, int> centerHeapIndex;
	vector < pair<int, int> > centerDataAllocate;//ÿһʱ�̵���������
	unordered_set<int> centerFreeDaySet;
	pair<int, int> centerCistHistoryConstrain = make_pair(0, 0);
	double centerCost = 0.0;
	vector<unordered_map<string, unordered_map<string,int>>> streamSource;//��¼ÿһ��ʱ�̣�ÿһ��������Դ����Щ������


	void MakeCenterHeap(int timeLen);

	//��һ������������ʱ���ڱ�ɶ�
	MaxHeap MakeHeap(vector<pair<int, int>>& target);
	//��ȡ�Ѷ�Ԫ��
	ElementType GetMax(MaxHeap H);
	//ɾ���Ѷ�Ԫ��
	ElementType DeleteMax(MaxHeap H, string serName);
	bool IsEmpty(MaxHeap H);
	//������
	void adjustHeap(MaxHeap H, int index, string serName);


	//��ÿ�����������а���������ʼ��
	void ServerFreeDaysInit(int timeLen, vector<pair<int, int>>& demands);

	//��ʼ�����з�������������
	void RecordCusNeeds(unordered_map<string, cusStruct>& cusMap, int timeLen);

	//ѡһ���������ķ���������������;ȡ���������޺�������Сֵ
	string FindMaxServer(vector<vector<string>>& freeServerInDay);

	//�ж�һ�����Ƿ���װ��ȥ
	bool IfCanPut(string serName, int time, int streamNum, int constrain);

	//����������������
	void FullOneServer(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time);

	//��������һ������cache
	void UpdataCacheForFirstStep(int time, string serName, int V);
	//�ڶ�������������cache
	void UpdataCacheSecondStep(int time, string serName, int V);

	//��һ�����Ž�һ����������
	void putOneStreamToServer(bool toV, int index, int needs, string streamID,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, string cusName, int time, int V, double A);

	//����Ӧ�ͻ��˷�����Դ��V
	void AllocateToV(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, int time, int V, double A);
	//����Ӧ�ͻ��˷�����Դ��V
	void AllocateToVNew(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
		string serName, int time, int V, double A);

	//�ڰ���֮���п������ڻ��棬������һЩ������������cost,������Щcost/������
	void UpdataHisConstrainAfterFreeCost(int timeLen, int V);

	//�ж�����������Ƿ�����
	bool ifFullServer(const unordered_map<string, serStruct>& serMap, string serName, int time);

	//�����Ե�ڵ��µĿ����������������Ž�ȥ��cost����˶���
	double newCitecostCal(string serName, int time, int streamNum, int V);

	//������Ž�ȥ������С��
	string findMinCostServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A);
	//�ҵ�ǰ������С��
	string findMinServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A);




	//����һ����������ĳһ����������������Ҫ����ǰһ��ʱ�̵�cache
	uint16_t GetOneServerCurrentDayActualData(string serName, uint16_t t);


	void FullOneServerNew(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time);

	//�������Ľڵ�������޶�
	void adjustCenterCiteHistoryHeap(int index);

	//�������Ľڵ��������
	void UpdateCenterCiteHisConstrain(string serName, int time, string streamId, int streamNum, double A);

	//�������Ľڵ�������ޣ�֮ǰ�Ǹ���bug��linux���������жϲ�׼
	void UpdataHisConstrainAfterFreeCostNew(int timeLen, int V);

	////�������Ľڵ�ͱ�Ե�ڵ����С�ѣ��Ѷ�Ԫ����������
	//void MakeCenterCiteANDCiteHistoryConstrainHeap();
	//�������Ľڵ㿪��
	double costCalCenter(string serName, int time, int streamNum, int V, string streamID, double A);

	void sortSteamMaxOrder(unordered_map<string, vector<streamInfo>>& streamVecMap, vector<pair<string, int>>& streamKinds);




	/**��̬����***/


	//�����з�������data����
	void sortDataAllocate();

	//��̬����
	void AdjustAllocate(clock_t startTime, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct> cusMap, int V, vector < pair<int, int> >& centerDataAllocate);
	//����һ����������һ��ʱ��
//outSerName��timeʱ����Ҫ���͵�target��Ŀ����ȫ���ͣ���󷵻ص�ʱ������ܽ���target�͹���
	bool DownOneServer(int time, int downTime, int virTime, string outSerName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, int V, int virCenterLim, unordered_set<int>&centerNodeFree);
	//ֻ��һ��������
	void sortOneServerDataAllocate(string serName);

	//�½����ε���
	bool DownFreeDayServer(int indexVirTime, string serName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, int V, unordered_set<int>&centerNodeFree, int virCenterLim);

	//����һ����������һ��ʱ�̣�costС��value���Խ�
	bool DownOneServerCostValue(int time, int downTime, int virTime, string outSerName, int target,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, unordered_map<string, cusStruct>& cusMap);

	//�ж��ܲ��ܽ�
	bool AdjustOneStreamToOneServerCostValue(string streamId, int streamNum, string outSerName, string toSerName, int time, int virTime,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V);
	//���԰�һ�����ŵ�һ�������������ֵ�ǻ���Ҫ���ٿռ�
	int AdjustOneStreamToOneServer(string streamId, int streamNum, string serName, int time, int virTime,
		vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, int virCenterLim,unordered_set<int>&centerNodeFree);
	//��ȡ�µ����ޣ���Ҫ����֮ǰʱ�̺�֮��ʱ��
	uint16_t getNewConstrain(string serName, uint16_t time);
	//����sortdataallocate����cache����
	void UpdataActualDataPlusCache(vector<pair<int, int>>& data, string serName);

	//��̬������ĸ��»���
	void UpdataCacheUingInAllocate(int time, string serName, int V);
	//��ȡ�µ����ޣ���Ҫ����֮ǰʱ�̺�֮��ʱ��
	uint16_t getNewConstrainNew(string serName, uint16_t time);
	//�ж�һ�����Ƿ���װ��ȥ����̬������
	bool IfCanPutAdjust(string serName, int time, int streamNum, int constrain, const string streamID, const int virCenterLim, unordered_set<int>&centerNodeFree);
	//�õ��������µ�����
	int getNewConstrain(string serName, int time, int virLimit);

};

#endif