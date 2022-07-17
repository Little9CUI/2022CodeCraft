#include "Customer.h"
#include "Server.h"
#include "ReadFile.h"
#include "AllStreamHeap.h"
#include "Judge.h"


int main() {

	clock_t startTime, endTime;
	startTime = clock();//计时开始
	ReadFileClass readOpe;
	customerClass customer;
	serverClass server;
	int qosConstrain, V, timeLen;
	double A;
	pair<int, int> dataConfig;
	vector<pair<int, int>> sortDemands;


	///**************************************读取数据***************************************************************/
	readOpe.readQosConstrain(qosConstrain, V, A);
	readOpe.readQos(qosConstrain, customer.cusMap, server.serMap);
	timeLen = readOpe.readDemand(customer.cusMap, sortDemands);
	readOpe.readServerData(server.serMap, timeLen);


	///**************************************最大白嫖预处理***************************************************************/

	vector<vector<string>> freeServerInDay(timeLen);//每一天对应的免费服务器名字
	unordered_set<string> emptySet;
	vector<streamInfo> streamVec, streamVec2;

	vector<vector<vector<vector<pair<string, int>>>>> sourceAllocate(timeLen, vector<vector<vector<pair<string, int>>>>(customer.cusMap.size(), vector<vector<pair<string, int>>>(server.serMap.size())));//结果矩阵
	vector<vector<vector<vector<pair<string, int>>>>> sourceAllocateorigin(timeLen, vector<vector<vector<pair<string, int>>>>(customer.cusMap.size(), vector<vector<pair<string, int>>>(server.serMap.size())));//结果矩阵

	vector<string> cusNodeIndex(customer.cusMap.size());
	vector<string> serNodeIndex(server.serMap.size());
	vector <string> nodeIndex(customer.cusMap.size() + server.serMap.size() + 2);
	server.MakeCenterHeap(timeLen);//建立中心节点的最小堆，堆顶元素是虚上限

	readOpe.setNode(nodeIndex, cusNodeIndex, serNodeIndex, server.serMap, customer.cusMap);
	server.ServerFreeDaysInit(timeLen, sortDemands);

	server.RecordCusNeeds(customer.cusMap, timeLen);
	//先把客户每个时刻的零流都处理掉
	//customer.sortStream(timeLen);
	customer.setZeroStreamTrue(timeLen);


	//ofstream ofs;
	//ofs.open(OutputPath, ios::trunc); 
	//for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
	//	ofs << '"'<< it->first << '"' << ",";
	//}
	//ofs.close();


	while (1) {
		//选一个需求最大的服务器，哪一天，哪一个
		string serName;

		serName = server.FindMaxServer(freeServerInDay);
		if (serName == "") {
			//分配完毕
			break;
		}
		int time = server.GetMax(server.serMap.find(serName)->second.dailyNeedsMaxHeap).first;//第几天的

		server.serMap.find(serName)->second.cost = V;
		//server.serMap.find(serName)->second.historyConstrain = V;
		//尽可能满足多的客户端
		server.DeleteMax(server.serMap.find(serName)->second.dailyNeedsMaxHeap, serName);//删掉这一天的，再不选了
		//server.Allocate(sourceAllocate, customer, serName, time);
		//server.FullOneServer(A, V, sourceAllocate, customer.cusMap, serName, time);
		server.FullOneServerNew(A, V, sourceAllocate, customer.cusMap, serName, time);
	}
#ifdef TEST
	cout << server.centerCost << endl;
#endif // TEST

	server.sortDataAllocate();
	//嫖到V
	for (int t = 0; t < timeLen; ++t) {
		//遍历所有服务器
		for (auto it = server.serMap.begin(); it != server.serMap.end(); ++it) {
			//遍历所连接的客户端的所有流
			if (it->second.cusConnected.size() == 0)
				continue;
			//如果没有嫖过，跳过
			if (it->second.freeDaySet.size() == 0)
				continue;
			string serName = it->first;
			if (it->second.dataAllocate[t].second >= V)//如果已经大于起步价了，就跳过
				continue;
			server.AllocateToV(sourceAllocate, customer.cusMap, serName, t, V, A);
			//server.AllocateToVNew(sourceAllocate, customer.cusMap, serName, t, V, A);
		}
	}
#ifdef TEST
	cout << server.centerCost << endl;
#endif // TEST

	//有可能820的位置是白嫖的后一天，有cache，计算一下所有服务器的虚上限和cost
	//server.UpdataHisConstrainAfterFreeCost(timeLen, V);
	server.UpdataHisConstrainAfterFreeCostNew(timeLen, V);


	//客户所有需求排序分配
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
					temp.index = i;//下标
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
		//找一个cost最小的服务器
		string serName = server.findMinCostServer(streamID, streamNum, customer.cusMap, cusName, time, V, A);
		//string serName = server.findMinServer(streamID, streamNum, customer.cusMap, cusName, time, V, A);

		//string serName = server.findMaxConstrainServer(streamNum, customer.cusMap, cusName, time, V);
		if (serName == "") {

			//cout << cusName << "  " << streamNum << "  " << time << endl;
			//for (auto it = customer.cusMap.find(cusName)->second.serConnected.begin(); it != customer.cusMap.find(cusName)->second.serConnected.end(); ++it) {
			//	cout <<it->first << server.serMap.find(it->first)->second.constrain << "   " << server.serMap.find(it->first)->second.cache[time-1].second << "   " << server.serMap.find(it->first)->second.dataAllocate[time].second << endl;
			//}
			cout << "无解" << endl;
			return 0;
		}
		//if (cusName == "F" && streamNum == 2816 && time == 313)
		//	cout << endl;

		int restSpace = server.serMap.find(serName)->second.constrain - server.serMap.find(serName)->second.dataAllocate[time].second;
		server.putOneStreamToServer(false, index, streamNum, streamID, sourceAllocate, customer.cusMap, serName, cusName, time, V, A);
		//分配了一个，再加一个，从弹出去的这个客户端的maxpointer往下找

	}


	///******************************************动态调整*************************************************************/

	server.AdjustAllocate(startTime, sourceAllocate, customer.cusMap, V, server.centerDataAllocate);



	readOpe.OutputData(cusNodeIndex, serNodeIndex, sourceAllocate, timeLen, customer.cusMap.size());

#ifdef TEST
	endTime = clock();
	JudgeClass myJudge;




	myJudge.testCache(server, timeLen);//测试缓存是否正确
	myJudge.JudgeServerConstrain(server, timeLen);//测试服务器有没有超过总上限

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