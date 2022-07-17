#include <Judge.h>

void JudgeClass::JudgeServerConstrain(serverClass& myServer,uint16_t timeLen) {

	for (auto it = myServer.serMap.begin(); it != myServer.serMap.end(); ++it) {
		for (uint16_t t = 0; t < timeLen; ++t) {
			if (myServer.GetOneServerCurrentDayActualData(it->first, t) > it->second.constrain) {
				cout << it->first << " " << t  << " " << myServer.GetOneServerCurrentDayActualData(it->first, t) << endl;;
				throw("超出容量上限");
			}
		}
	}
	cout << "server constrain OK!" << endl;
}



//测试虚上线，没写完！！！！！！！！！！！
void JudgeClass::testHisConstrain(serverClass& myServer,uint16_t timeLen) {
	bool flag = true;
	int index = ceil(timeLen * 0.95) - 1;//向上取整，然后是下标，还要减一

	for (auto it = myServer.serMap.begin(); it != myServer.serMap.end(); ++it) {
		it->second.sortDataAllocate = it->second.dataAllocate;
		sort(it->second.sortDataAllocate.begin(), it->second.sortDataAllocate.end(), cmpUp);

		if (it->second.historyConstrain.second != it->second.sortDataAllocate[index].second) {
			cout << it->first << " " << "记录虚上限：" << it->second.historyConstrain.second << "实际虚上限" << it->second.historyConstrainHeap->Elements[1].second << endl;
			flag = false;
		}
	}
	if (flag) {
		cout << "History constrain test OK!" << endl;
	}
}

//测试中心节点cost
void JudgeClass::testCenterCost(serverClass& myServer, uint16_t timeLen, double A) {
	int index = ceil(timeLen * 0.95) - 1;//向上取整，然后是下标，还要减一

	sort(myServer.centerDataAllocate.begin(), myServer.centerDataAllocate.end(), cmpUp);
	if (A * double(myServer.centerDataAllocate[index].second) != myServer.centerCost) {
		cout << "center cost wrong!" << endl;
		cout << "actual: " << A * myServer.centerDataAllocate[index].second << " " << "record: " << myServer.centerCost << endl;
		throw("center wrong");

	}
	else {
		cout << "center cost OK!" << endl;
	}
}


void JudgeClass::testCache(serverClass& myServer,uint16_t timeLen) {
	vector < pair<int, int> > curData;
	bool flag = true;
	for (auto it = myServer.serMap.begin(); it != myServer.serMap.end(); ++it) {
		curData.clear();

		for (int t = 0; t < timeLen; ++t) {
			if (t > 0) {
				curData.push_back(make_pair(t, it->second.dataAllocate[t].second + floor(curData[t - 1].second * 0.05)));
			}
			else {
				curData.push_back(make_pair(t, it->second.dataAllocate[t].second));
			}
		}

		for (int t = 0; t < timeLen; ++t) {
			if (t > 0) {
				if (floor((curData[t].second) * 0.05) != it->second.cache[t].second) {
					flag = false;
					cout << "cache saved wrong!" << endl;
					cout << "time: " << t << " server: " << it->first << endl;
					cout << "Actual: " << floor((curData[t].second) * 0.05) << "   " << "Saved: " << it->second.cache[t].second << endl;
				}
			}
		}
	}
	if(flag)
		cout << "Cache Test OK!" << endl;
}

void JudgeClass::testOneCache(serverClass& myServer, string serName, uint16_t timeLen) {
	for (int t = 0; t < timeLen; ++t) {
		if (t > 0) {
			if (floor((myServer.serMap.find(serName)->second.dataAllocate[t].second +
				myServer.serMap.find(serName)->second.cache[t - 1].second) * 0.05) != myServer.serMap.find(serName)->second.cache[t].second) {
				cout << "time: " << t << " server: " << myServer.serMap.find(serName)->first << endl;
				cout << floor((myServer.serMap.find(serName)->second.dataAllocate[t].second + myServer.serMap.find(serName)->second.cache[t - 1].second) * 0.05)
					<< "   " << myServer.serMap.find(serName)->second.cache[t].second << endl;
				cout << "wrong" << endl;
				return;
			}
		}
		else {
			if (floor(myServer.serMap.find(serName)->second.dataAllocate[t].second * 0.05) != myServer.serMap.find(serName)->second.cache[t].second) {
				cout << "time: " << t << " server: " << myServer.serMap.find(serName)->first << endl;
				cout << floor(myServer.serMap.find(serName)->second.dataAllocate[t].second * 0.05)
					<< "   " << myServer.serMap.find(serName)->second.cache[t].second << endl;
				cout << "wrong" << endl;
				return;
			}
		}
	}
}

void JudgeClass::JudgeCustomerNeeds(uint16_t timeLen, const serverClass& myServer, const customerClass& myCustomer, const vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate) {
	uint16_t timeLength = timeLen;
	for (uint16_t t = 0; t < timeLength; ++t) {
		for (auto cus = myCustomer.cusMap.begin(); cus != myCustomer.cusMap.end(); ++cus) {
			uint16_t cusNum = cus->second.nodeNum;
			//记录这个客户端当天的所有流
			unordered_map<string, uint16_t> streamAllocateRecord;
			for (uint16_t i = 0; i < cus->second.dataNeeds[t].size(); ++i) {
				if (cus->second.dataNeeds[t][i].second == 0) {
					continue;
				}
				streamAllocateRecord[cus->second.dataNeeds[t][i].first] = 0;
			}
			uint16_t streamNum = streamAllocateRecord.size(), streamNumCount = 0;//记录这个客户的流的数量
			//查输出矩阵
			for (uint16_t serNumIter = 0; serNumIter < sourceAllocate[t][cusNum].size(); ++serNumIter) {
				for (uint16_t streamIndex = 0; streamIndex < sourceAllocate[t][cusNum][serNumIter].size(); ++streamIndex) {
					string streamId = sourceAllocate[t][cusNum][serNumIter][streamIndex].first;
					if (sourceAllocate[t][cusNum][serNumIter][streamIndex].second == 0)
						continue;
					if (streamAllocateRecord.find(streamId) != streamAllocateRecord.end()) {
						if (streamAllocateRecord.find(streamId)->second == 0) {
							streamAllocateRecord.find(streamId)->second = 1;
							streamNumCount++;
						}
						else {
							cout << "多次分配问题" << endl;
							cout << "time: " << t;
							cout << " customer: " << cus->first;
							cout << " streamid: " << streamId << endl;
							throw("分配次数超过一次，重复分配了");
						}
					}
					else {
						cout << "客户没有这个流的问题" << endl;
						cout << "time: " << t;
						cout << " customer: " << cus->first;
						throw("这个客户没有这个流的需求");
					}
				}
			}
			if (streamNum != streamNumCount) {
				cout << "分配流数量不匹配问题" << endl;
				cout << "time: " << t;
				cout << " customer: " << cus->first;
				cout << streamNum << " ";
				cout << streamNumCount << endl;
				throw("分配次数不一致");
			}
		}
	}
	cout << "用户所有流匹配完成！" << endl;
	return;
}

//计算总需求
double JudgeClass::calCostTotal(unordered_map < string, serStruct >& server, int timeLen, int V, double A) {
	double ans = 0, newCost = 0, cost95, centerCost95;
	int index;
	bool flag = true;
	vector < pair<int, int> > tempData;
	index = ceil(timeLen * 0.95) - 1;//向上取整，然后是下标，还要减一
	for (auto it = server.begin(); it != server.end(); ++it) {
		tempData.clear();
		//if (it->first == "b")
		//	cout << endl;
		for (int t = 0; t < it->second.dataAllocate.size(); ++t) {
			if (t > 0) {
				tempData.push_back(make_pair(t, it->second.dataAllocate[t].second + floor(tempData[t - 1].second * 0.05)));
			}
			else {
				tempData.push_back(make_pair(t, it->second.dataAllocate[t].second));
			}
			//for (auto iter = it->second.centerAllocate.begin(); iter != it->second.centerAllocate.end(); ++iter) {
			//	if(iter->second.find(t) != iter->second.end())
			//		centerCost[t] += iter->second[t];
			//}
		}

		it->second.sortDataAllocate = tempData;
		sort(it->second.sortDataAllocate.begin(), it->second.sortDataAllocate.end(), cmpUp);
		if (it->second.sortDataAllocate[it->second.sortDataAllocate.size() - 1].second == 0)
			continue;
		cost95 = it->second.sortDataAllocate[index].second;
		if (cost95 <= V) {
			newCost = V;
			ans += newCost;
		}
		else {
			newCost = double((cost95 - V) * (cost95 - V)) / double(it->second.constrain) + cost95;
			ans += newCost;
		}

		//if (cost95 != it->second.historyConstrain.second) {
		//	//if (cost95 != it->second.historyConstrainHeap->Elements[1].second) {
		//	cout << it->first << ": " << endl;
		//	cout << "Actual time: " << it->second.sortDataAllocate[index].first << " Actual Num: " << cost95 << endl;
		//	cout <<	"Cal time: " << it->second.historyConstrain.first << "Cal Num: " << it->second.historyConstrain.second << endl;
		//	flag = false;
		//}
		if (it->second.historyConstrain.second > cost95) {
			cout << "Down successfully!" << endl;
			cout << it->first << ": " << "Before: " << it->second.historyConstrain.second << "  After: " << cost95 << endl;
		}
		else if(it->second.historyConstrain.second < cost95){
			cout << "Down failed!" << endl;
			cout << it->first << ": " << "Before: " << it->second.historyConstrain.second << "  After: " << cost95 << endl;
		}

	}
	if (flag) {
		cout << "History Constrain Test OK!" << endl;
	}

	return ans;
}

