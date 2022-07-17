#include "Customer.h"
#include "Server.h"
#include "ReadFile.h"
#include "AllStreamHeap.h"
#include "Judge.h"


int main() {

	clock_t startTime, endTime;
	startTime = clock();//��ʱ��ʼ
	ReadFileClass readOpe;
	customerClass customer;
	serverClass server;
	int qosConstrain, V, timeLen;
	double A;
	pair<int, int> dataConfig;
	vector<pair<int, int>> sortDemands;


	///**************************************��ȡ����***************************************************************/
	readOpe.readQosConstrain(qosConstrain, V, A);
	readOpe.readQos(qosConstrain, customer.cusMap, server.serMap);
	timeLen = readOpe.readDemand(customer.cusMap, sortDemands);
	readOpe.readServerData(server.serMap, timeLen);


	///**************************************������Ԥ����***************************************************************/

	vector<vector<string>> freeServerInDay(timeLen);//ÿһ���Ӧ����ѷ���������
	unordered_set<string> emptySet;
	vector<streamInfo> streamVec, streamVec2;

	vector<vector<vector<vector<pair<string, int>>>>> sourceAllocate(timeLen, vector<vector<vector<pair<string, int>>>>(customer.cusMap.size(), vector<vector<pair<string, int>>>(server.serMap.size())));//�������
	vector<vector<vector<vector<pair<string, int>>>>> sourceAllocateorigin(timeLen, vector<vector<vector<pair<string, int>>>>(customer.cusMap.size(), vector<vector<pair<string, int>>>(server.serMap.size())));//�������

	vector<string> cusNodeIndex(customer.cusMap.size());
	vector<string> serNodeIndex(server.serMap.size());
	vector <string> nodeIndex(customer.cusMap.size() + server.serMap.size() + 2);
	server.MakeCenterHeap(timeLen);//�������Ľڵ����С�ѣ��Ѷ�Ԫ����������

	readOpe.setNode(nodeIndex, cusNodeIndex, serNodeIndex, server.serMap, customer.cusMap);
	server.ServerFreeDaysInit(timeLen, sortDemands);

	server.RecordCusNeeds(customer.cusMap, timeLen);
	//�Ȱѿͻ�ÿ��ʱ�̵������������
	//customer.sortStream(timeLen);
	customer.setZeroStreamTrue(timeLen);


	//ofstream ofs;
	//ofs.open(OutputPath, ios::trunc); 
	//for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
	//	ofs << '"'<< it->first << '"' << ",";
	//}
	//ofs.close();


	while (1) {
		//ѡһ���������ķ���������һ�죬��һ��
		string serName;

		serName = server.FindMaxServer(freeServerInDay);
		if (serName == "") {
			//�������
			break;
		}
		int time = server.GetMax(server.serMap.find(serName)->second.dailyNeedsMaxHeap).first;//�ڼ����

		server.serMap.find(serName)->second.cost = V;
		//server.serMap.find(serName)->second.historyConstrain = V;
		//�����������Ŀͻ���
		server.DeleteMax(server.serMap.find(serName)->second.dailyNeedsMaxHeap, serName);//ɾ����һ��ģ��ٲ�ѡ��
		//server.Allocate(sourceAllocate, customer, serName, time);
		//server.FullOneServer(A, V, sourceAllocate, customer.cusMap, serName, time);
		server.FullOneServerNew(A, V, sourceAllocate, customer.cusMap, serName, time);
	}
#ifdef TEST
	cout << server.centerCost << endl;
#endif // TEST

	server.sortDataAllocate();
	//�ε�V
	for (int t = 0; t < timeLen; ++t) {
		//�������з�����
		for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
			//���������ӵĿͻ��˵�������
			if (it->second.cusConnected.size() == 0)
				continue;
			//���û���ι�������
			if (it->second.freeDaySet.size() == 0)
				continue;
			string serName = it->first;
			if (it->second.dataAllocate[t].second >= V)//����Ѿ������𲽼��ˣ�������
				continue;
			server.AllocateToV(sourceAllocate, customer.cusMap, serName, t, V, A);
			//server.AllocateToVNew(sourceAllocate, customer.cusMap, serName, t, V, A);
		}
	}
#ifdef TEST
	cout << server.centerCost << endl;
#endif // TEST

	//�п���820��λ���ǰ��εĺ�һ�죬��cache������һ�����з������������޺�cost
	//server.UpdataHisConstrainAfterFreeCost(timeLen, V);
	server.UpdataHisConstrainAfterFreeCostNew(timeLen, V);


	//�ͻ����������������
	for (int t = 0; t < timeLen; ++t) {
		for (auto it = customer.cusMap.begin(); it != customer.cusMap.end(); ++it) {
			string cusNa = it->first;
			if (it->second.unSatisfiedNum[t] == 0)
				continue;
			for (int i = 0; i < customer.cusMap.find(cusNa)->second.dataNeeds[t].size(); ++i) {
				if (customer.cusMap.find(cusNa)->second.dataNeedsIfSatisfied[t][i] == false) {
					streamInfo temp;
					temp.cusName = it->first;
					temp.streamID = it->second.dataNeeds[t][i].first;
					temp.streamNum = it->second.dataNeeds[t][i].second;
					temp.time = t;
					temp.index = i;//�±�
					streamVec.push_back(temp);
				}
			}
		}
	}
	sort(streamVec.begin(), streamVec.end(), streamInfoDown);
	for (int i = 0; i < streamVec.size(); ++i) {
		bool fullFlag;
		streamInfo curSteam = streamVec[i];
		string cusName = curSteam.cusName;
		string streamID = curSteam.streamID;
		int time = curSteam.time;
		int streamNum = curSteam.streamNum;
		int index = curSteam.index;
		//��һ��cost��С�ķ�����
		string serName = server.findMinCostServer(streamID, streamNum, customer.cusMap, cusName, time, V, A);
		//string serName = server.findMinServer(streamID, streamNum, customer.cusMap, cusName, time, V, A);

		//string serName = server.findMaxConstrainServer(streamNum, customer.cusMap, cusName, time, V);
		if (serName == "") {

			//cout << cusName << "  " << streamNum << "  " << time << endl;
			//for (auto it = customer.cusMap.find(cusName)->second.serConnected.begin(); it != customer.cusMap.find(cusName)->second.serConnected.end(); ++it) {
			//	cout <<it->first << server.serMap.find(it->first)->second.constrain << "   " << server.serMap.find(it->first)->second.cache[time-1].second << "   " << server.serMap.find(it->first)->second.dataAllocate[time].second << endl;
			//}
			cout << "�޽�" << endl;
			return 0;
		}
		//if (cusName == "F" && streamNum == 2816 && time == 313)
		//	cout << endl;

		int restSpace = server.serMap.find(serName)->second.constrain - server.serMap.find(serName)->second.dataAllocate[time].second;
		server.putOneStreamToServer(false, index, streamNum, streamID, sourceAllocate, customer.cusMap, serName, cusName, time, V, A);
		//������һ�����ټ�һ�����ӵ���ȥ������ͻ��˵�maxpointer������

	}


	///******************************************��̬����*************************************************************/

	server.AdjustAllocate(startTime, sourceAllocate, customer.cusMap, V, server.centerDataAllocate);



	readOpe.OutputData(cusNodeIndex, serNodeIndex, sourceAllocate, timeLen, customer.cusMap.size());

#ifdef TEST
	endTime = clock();
	JudgeClass myJudge;




	myJudge.testCache(server, timeLen);//���Ի����Ƿ���ȷ
	myJudge.JudgeServerConstrain(server, timeLen);//���Է�������û�г���������

	double costT = 0;
	for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
		costT += it->second.cost;
	}
	cout << "cal cite cost: " << costT << endl;
	//cout << "cal center cite cost: " << A * server.GetMax(server.CenterCiteHeap).second << endl;
	cout << "cal center cite cost: " << server.centerCost << endl;

	cout << "*******************************" << endl;

	double citeCost = myJudge.calCostTotal(server.serMap, timeLen, V, A);

	sort(server.centerDataAllocate.begin(), server.centerDataAllocate.end(), cmpUp);
	double centerCiteCost = A * server.centerDataAllocate[ceil(timeLen * 0.95) - 1].second;

	cout << "Site Cost: " << citeCost << endl;
	cout << "CenterCist Cost: " << centerCiteCost << endl;

	cout << "Run time:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	cout << "*******************************" << endl;


	//for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
	//	if (it->second.sortDataAllocate[821].second != 0) {
	//		cout << it->first << " " << it->second.sortDataAllocate[821].second << endl;
	//	}
	//}

	//string serName = "Dn";
	//for (uint16_t i = 0; i < server.serMap.find(serName)->second.sortDataAllocate.size(); ++i) {
	//	cout << i << " " << server.serMap.find(serName)->second.sortDataAllocate[i].first << " " << server.serMap.find(serName)->second.sortDataAllocate[i].second << endl;
	//}


#endif // TEST



}