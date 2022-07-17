#include "ReadFile.h"

void ReadFileClass::readQosConstrain(int& qosConstrain, int& V, double& A) {
	fstream ifs;
	string s, temp;
	ifs.open(configPath, ios::in);
	if (!ifs.is_open())
		cout << "No File!" << endl;
	while (getline(ifs, s)) {
		if (s.back() == '\r') {

			s.pop_back();
		}
		if (s[0] == 'q') {
			for (int i = 0; i < s.size(); ++i) {
				if (s[i] == '=') {
					temp = s.substr(i + 1, s.size() - i - 1);
					qosConstrain = stoi(temp);
					break;
				}
			}
		}
		if (s[0] == 'b') {
			for (int i = 0; i < s.size(); ++i) {
				if (s[i] == '=') {
					temp = s.substr(i + 1, s.size() - i - 1);
					V = stoi(temp);
					break;
				}
			}
		}
		if (s[0] == 'c') {
			for (int i = 0; i < s.size(); ++i) {
				if (s[i] == '=') {
					temp = s.substr(i + 1, s.size() - i - 1);
					A = stod(temp);
					break;
				}
			}
		}
	}
	ifs.close();
	return;
}

void ReadFileClass::readQos(int qosConstrain, unordered_map < string, cusStruct >& customer, unordered_map < string, serStruct >& serverMap) {
	fstream ifs;
	string s;
	vector<string> cus;
	string cusSingle;
	string server;
	bool flag = true;//��¼�ǲ��ǵ�һ��
	ifs.open(qosPath, ios::in);

	if (!ifs.is_open())
		cout << "No File!" << endl;
	while (getline(ifs, s)) {
		if (s.back() == '\r') {
			s.pop_back();
		}
		if (flag) {
			//��һ��
			int i = 0;
			while (s[i++] != ',');
			while (i < s.size()) {
				if (s[i] == ',') {
					cus.push_back(cusSingle);
					cusSingle.clear();
					i++;
					continue;
				}
				cusSingle.push_back(s[i]);
				i++;
			}
			//�������һ��
			cus.push_back(cusSingle);
			flag = false;
		}
		else {
			int i = 0;
			int cusNum = 0;//��¼�ͻ��ı��
			string qosNeed;
			server.clear();
			//��¼����������
			while (s[i] != ',') {
				server.push_back(s[i]);
				i++;
			}
			i++;//��������
			//��ȡ�÷�������Ӧ�Ŀͻ���ʱ�ӣ�������������ͼӵ�map��
			while (i < s.size()) {
				while (i < s.size() && s[i] != ',') {
					qosNeed.push_back(s[i]);
					i++;
				}
				i++;
				int temp = stoi(qosNeed);
				qosNeed.clear();
				if (temp < qosConstrain) {
					//�������ʱ������
					customer[cus[cusNum]].serConnected[server] = 0;
					serverMap[server].cusConnected[cus[cusNum]] = 0;
					//customer[cus[cusNum]].serverConnected.push_back(make_pair(server, 0));
				}
				cusNum++;
			}
		}
	}
}

//�����ܹ�����ʱ��
int ReadFileClass::readDemand(unordered_map < string, cusStruct >& customer, vector<pair<int, int>>& demands) {
	fstream ifs;
	string name, s, temp, lastTimeData, timeData, streamId;
	int data, count, dataNum, t, i, qosConstrain = 0;
	unordered_map<string, vector<pair<string, int>>> cusDataPerDay;
	vector<string> cusName;
	vector < pair<string, int>> curData;
	ifs.open(demandPath, ios::in);
	if (!ifs.is_open())
		cout << "No File!" << endl;
	getline(ifs, s);
	if (s.back() == '\r') {
		s.pop_back();
	}
	i = 0;
	while (s[i++] != ',');//������һ���ַ��� ʱ��
	while (s[i++] != ',');//�����ڶ����ַ��� ����id
	//��¼���пͻ�������
	while (i < s.size()) {
		temp = "";
		while (i < s.size() && s[i] != ',') {
			temp.push_back(s[i]);
			i++;
		}
		cusName.push_back(temp);
		i++;
	}

	//��ȡÿһʱ�̵Ĳ�ͬ��������
	lastTimeData = "";
	t = 0;//�ڼ���ʱ��
	while (getline(ifs, s)) {
		if (s.back() == '\r') {
			s.pop_back();
		}
		i = 0;
		count = 0;//��¼�ͻ��ı��
		curData.clear();
		timeData = "";
		streamId = "";
		while (s[i] != ',') {
			//��һ��ʱ���Ƿ�һ��
			timeData.push_back(s[i]);
			i++;
		}
		if (timeData != lastTimeData) {
			lastTimeData = timeData;
			data = 0;
			for (auto it = cusDataPerDay.begin(); it != cusDataPerDay.end(); ++it) {
				name = it->first;
				customer.find(name)->second.dataNeeds.push_back(it->second);
				customer.find(name)->second.dataNeedsIfSatisfied.push_back(vector<bool>(customer.find(name)->second.dataNeeds.back().size()));
				//customer.find(name)->second.originDataNeeds.push_back(it->second);
				customer.find(name)->second.unSatisfiedNum.push_back(customer.find(name)->second.dataNeeds.back().size());
				for (auto iter = it->second.begin(); iter != it->second.end(); ++iter) {
					data += iter->second;
				}
			}
			if (t > 0)
				demands.push_back(make_pair(t - 1, data));
			cusDataPerDay.clear();
			t++;//����һ��ʱ����
		}
		i++;
		while (s[i] != ',') {
			//��¼����ID
			streamId.push_back(s[i]);
			i++;
		}
		i++;
		while (i < s.size()) {
			string dataStr;
			while (i < s.size() && s[i] != ',') {
				dataStr.push_back(s[i]);
				i++;
			}
			i++;
			dataNum = stoi(dataStr);
			//if (dataNum != 0)
			cusDataPerDay[cusName[count++]].push_back(make_pair(streamId, dataNum));
			//else
			//	count++;
			//customer.find(cusName[count++])->second.dataNeeds[t].push_back(make_pair(streamId,dataNum));
		}
	}
	//���һ��ʱ�̵�����
	data = 0;
	for (auto it = cusDataPerDay.begin(); it != cusDataPerDay.end(); ++it) {
		name = it->first;
		customer.find(name)->second.dataNeeds.push_back(it->second);
		customer.find(name)->second.dataNeedsIfSatisfied.push_back(vector<bool>(customer.find(name)->second.dataNeeds.back().size()));
		customer.find(name)->second.unSatisfiedNum.push_back(customer.find(name)->second.dataNeeds.back().size());
		//customer.find(name)->second.originDataNeeds.push_back(it->second);
		for (auto iter = it->second.begin(); iter != it->second.end(); ++iter) {
			data += iter->second;
		}
	}
	demands.push_back(make_pair(t - 1, data));
	sort(demands.begin(), demands.end(), cmp);
	return t;
}


void ReadFileClass::readServerData(unordered_map < string, serStruct >& server, int timeLen) {
	fstream ifs;
	string s;
	vector<pair<int, int>> initDataLimit(timeLen, make_pair(0, 0));
	vector<pair<int, int>> initDataAllocate(timeLen, make_pair(0, 0));
	ifs.open(site_bandwidthPath, ios::in);
	if (!ifs.is_open())
		cout << "No File!" << endl;
	getline(ifs, s);//������һ��
	if (s.back() == '\r') {
		s.pop_back();
	}
	while (getline(ifs, s)) {
		if (s.back() == '\r') {
			s.pop_back();
		}
		int i = 0;
		string serverName;
		string dataServer;
		while (s[i] != ',') {
			serverName.push_back(s[i]);
			i++;
		}
		i++;
		while (i < s.size()) {
			dataServer.push_back(s[i]);
			i++;
		}
		int dataInt = stoi(dataServer);
		server[serverName].constrain = dataInt;
		server[serverName].dataAllocate = initDataAllocate;
		server[serverName].cache = initDataAllocate;
		//server[serverName].limitTimeSeq = initDataLimit;//�����������г�ʼ��
	}
}


//���������Ϳͻ��˱��
void ReadFileClass::setNode(vector <string>& nodeIndex, vector<string>& cusNodeIndex, vector<string>& serNodeIndex, unordered_map < string, serStruct >& server, unordered_map < string, cusStruct >& customer) {
	int count = 0;
	int netCount = 1;
	for (auto it = customer.begin(); it != customer.end(); ++it) {
		nodeIndex[netCount] = it->first;
		cusNodeIndex[count] = it->first;
		it->second.nodeNum = count;
		//it->second.nodeNetNum = netCount;
		++count;
		++netCount;
	}
	count = 0;
	for (auto it = server.begin(); it != server.end(); ++it) {
		nodeIndex[netCount] = it->first;
		serNodeIndex[count] = it->first;
		it->second.nodeNum = count;
		//it->second.nodeNetNum = netCount;
		++count;
		++netCount;
	}
}

void ReadFileClass::OutputFile(vector<string>& ans) {
	ofstream ofs;
	ofs.open(OutputPath, ios::trunc); // ׷��д�ļ�
	for (int i = 0; i < ans.size(); i++) {
		if (i == ans.size() - 1) {
			ofs << ans[i];
		}
		else {
			ofs << ans[i] << endl;
		}
	}
	ofs.close();
}


void ReadFileClass::OutputData(vector<string>& cusNodeIndex, vector<string>& serNodeIndex, const vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int timeLen, int cusNum) {
	vector<string> ans;
	string cusName, s, serName;
	bool flag;
	for (int t = 0; t < sourceAllocate.size(); ++t) {
		for (int cus = 0; cus < sourceAllocate[0].size(); ++cus) {
			cusName = cusNodeIndex[cus];
			s = "";
			s += cusName;
			s += ':';
			int count = 0;
			for (int ser = 0; ser < sourceAllocate[0][0].size(); ++ser) {
				serName = serNodeIndex[ser];
				if (sourceAllocate[t][cus][ser].size() == 0)
					continue;
				flag = false;
				for (int i = 0; i < sourceAllocate[t][cus][ser].size(); ++i) {
					if (sourceAllocate[t][cus][ser][i].second != 0) {
						flag = true;//ֻ����һ����������0�Ϳ��Լ���
						break;
					}
				}
				if (flag == false)//��������������������
					continue;
				s += '<';
				s += serName;
				s += ',';
				for (int i = 0; i < sourceAllocate[t][cus][ser].size(); ++i) {
					if (sourceAllocate[t][cus][ser][i].second == 0)
						continue;
					s += sourceAllocate[t][cus][ser][i].first;
					s += ',';
				}
				s.pop_back();//������Ǹ����ŵ���ȥ
				s += ">,";
				count++;
			}
			if (count > 0)
				s.pop_back();//������Ǹ����ŵ���ȥ
			ans.push_back(s);
		}
	}
	if (ans.size() != (timeLen * cusNum) || ans.size() == 0)
		return;
	OutputFile(ans);
}
