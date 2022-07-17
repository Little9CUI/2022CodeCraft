#include "Server.h"
#include "AllStreamHeap.h"


//��һ������������ʱ���ڱ�ɶ�
void serverClass::MakeCenterHeap(int timeLen) {
	int Parent = 0, Child = 0, heapLength = 0;
	vector<pair<int, int>> temp(timeLen, make_pair(0, 0));
	CenterCiteHeap = new HeapStruct;
	heapLength = ceil(timeLen * 0.05);
	CenterCiteHeap->Elements = new ElementType[heapLength + 1];
	CenterCiteHeap->Size = heapLength;
	CenterCiteHeap->Capacity = heapLength;
	CenterCiteHeap->Elements[0] = make_pair(-1, -1);//�����ڱ�ΪС�ڶ������п���Ԫ�ص�ֵ�������Ժ�������
	//ȫ����ʼ��Ϊ0
	for (int i = 0; i < heapLength; ++i) {
		CenterCiteHeap->Elements[i + 1] = make_pair(0, 0);
	}
	centerDataAllocate = temp;
}


//��ʼ����������ѵ���������100����5��
void serverClass::ServerFreeDaysInit(int timeLen, vector<pair<int, int>>& demands) {

	int freeDays = timeLen - ceil(timeLen * 0.95);
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		it->second.freeDays = freeDays;
	}
	for (int i = 0; i < freeDays; ++i) {
		centerFreeDaySet.insert(demands[i].first);
	}
}


//��ʼ�����з�������������
void serverClass::RecordCusNeeds(unordered_map<string, cusStruct>& cusMap, int timeLen) {
	vector<pair<int, int>> temp(timeLen);
	string cusName;
	vector<unordered_map<string, unordered_map<string,int>>> temp2(timeLen);
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		it->second.dailyNeeds = temp;
		for (int t = 0; t < timeLen; ++t) {
			it->second.dailyNeeds[t].first = t;
			for (auto iter = it->second.cusConnected.begin(); iter != it->second.cusConnected.end(); ++iter) {
				cusName = iter->first;
				for (auto iter = cusMap.find(cusName)->second.dataNeeds[t].begin()
					; iter != cusMap.find(cusName)->second.dataNeeds[t].end(); ++iter) {
					it->second.dailyNeeds[t].second += iter->second;//��x�ͻ��˴����������������Ӻ�
				}
			}
			it->second.dataAllocate[t].first = t;//��ÿ��ʵ�ʷ��������Ҳ��ʼ����
			centerDataAllocate[t].first = t;
		}
		it->second.dailyNeedsMaxHeap = MakeHeap(it->second.dailyNeeds);//������
		for (int i = 1; i <= it->second.dailyNeedsMaxHeap->Size; ++i) {
			it->second.dayIndex[it->second.dailyNeedsMaxHeap->Elements[i].first] = i;
		}
		//����һ��ȫ0�ģ�����ʱ�ָ�
		it->second.dataAllocateOrigin = it->second.dataAllocate;
		it->second.cacheOrigin = it->second.cache;
	}
	streamSource = temp2;
}

//��һ������������ʱ���ڱ�ɶ�
MaxHeap serverClass::MakeHeap(vector<pair<int, int>>& target) {
	int Parent, Child;
	MaxHeap H = new HeapStruct;
	H->Elements = new ElementType[target.size() + 1];
	H->Size = target.size();
	H->Capacity = target.size();
	H->Elements[0] = make_pair(-1, 0x7fffffff);//�����ڱ�Ϊ���ڶ������п���Ԫ�ص�ֵ�������Ժ�������
	for (int i = 0; i < target.size(); ++i) {
		H->Elements[i + 1] = target[i];
	}

	//������
	for (int i = H->Size; i > 0; --i) {
		if (i * 2 > H->Size)
			continue;
		//��һ��������ӵĽڵ�
		ElementType temp = H->Elements[i];//size��СҲҪ��1
		for (Parent = i; Parent * 2 <= H->Size; Parent = Child) {//�о��Ǹý��������
			//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
			Child = Parent * 2;//ָ�������
			if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
				//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
				//�ڶ������ж����Ҷ����ĸ�����
				Child++;
			if (temp.second >= H->Elements[Child].second)//������ֵ�������ĸ����ӻ��󣬾��˳�
			//�����ֵ�ŵ���������
				break;
			else//����ͰѴ���ӷŵ����ڵ���
				H->Elements[Parent] = H->Elements[Child];
		}
		H->Elements[Parent] = temp;
	}
	return H;
}

//��ȡ�Ѷ�Ԫ��
ElementType serverClass::GetMax(MaxHeap H) {
	return H->Elements[1];
}
//�Ƿ�Ϊ��
bool serverClass::IsEmpty(MaxHeap H) {
	if (H->Size == 0)
		return true;
	else
		return false;
}

//ɾ���Ѷ�Ԫ��
ElementType serverClass::DeleteMax(MaxHeap H, string serName) {
	//������H��ȡ����ֵ����Ԫ�أ���ɾ��һ�����
	int Parent, Child;
	ElementType MaxItem, temp;
	if (IsEmpty(H)) {
		printf("�����ѿ�");
		return make_pair(-1, -1);
	}
	MaxItem = H->Elements[1];//ȡ�������Ǹ������
	//�����������ѵ����һ��Ԫ�شӸ��ڵ㿪ʼ���±Ƚϣ�������ֵ�ȸý��
//�����Ҷ��������Ǹ���Ҫ���Ǿͷ���������ڵ��ϣ�����Ͱ������Ǹ�����
//Ų������Ȼ������Ӵ�����Ǹ���㿪ʼ���±Ƚ�
	temp = H->Elements[H->Size--];//size��СҲҪ��1
	for (Parent = 1; Parent * 2 <= H->Size; Parent = Child) {//�о��Ǹý��������
		//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
		Child = Parent * 2;//ָ�������
		if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
			//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
			//�ڶ������ж����Ҷ����ĸ�����
			Child++;
		if (temp.second >= H->Elements[Child].second)//������ֵ�������ĸ����ӻ��󣬾��˳�
		//�����ֵ�ŵ���������
			break;
		else {//����ͰѴ���ӷŵ����ڵ���
			H->Elements[Parent] = H->Elements[Child];
			serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//�ڵ�λ���ƶ�
		}
	}
	H->Elements[Parent] = temp;
	serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//�ڵ�λ���ƶ�
	return MaxItem;
}

//���������е�index���ڵ㣬���ص��������ĸ��ڵ�
void serverClass::adjustHeap(MaxHeap H, int index, string serName) {
	ElementType temp = H->Elements[index];//size��СҲҪ��1
	int Parent, Child;
	for (Parent = index; Parent * 2 <= H->Size; Parent = Child) {//�о��Ǹý��������
		//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
		Child = Parent * 2;//ָ�������
		if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
			//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
			//�ڶ������ж����Ҷ����ĸ�����
			Child++;
		if (temp.second >= H->Elements[Child].second)//������ֵ�������ĸ����ӻ��󣬾��˳�
		//�����ֵ�ŵ���������
			break;
		else {//����ͰѴ���ӷŵ����ڵ���
			H->Elements[Parent] = H->Elements[Child];
			serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//�ڵ�λ���ƶ�
		}
	}
	H->Elements[Parent] = temp;
	serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//�ڵ�λ���ƶ�
	return;
}


//ѡһ���������ķ���������������;ȡ���������޺�������Сֵ
string serverClass::FindMaxServer(vector<vector<string>>& freeServerInDay) {
	string maxServerName;
	int maxConstrain = 0, maxConnection = 0, maxFreeData = 0, maxNeeds = 0, time = 0, newConstrain = 0, curData = 0, curTime = 0, newCache = 0, tempTime = 0, maxCache = 0;
	//vector<string> serVec = {
	//"BQ","Q","A","B","DU","BC","C4","AJ","C","D","Cs","E","DF","P","Cg",
	//"B8","Dn","Dt","Bb","Ak","b","4","Bt","BO","C8","AF","AS","AV","g","CP",
	//"Dq","CY","Ag","d","CS","Dr","Am","Ao","Ar","A3","A7","Cz","BM","BN","C9",
	//"DD","Ce","BR","BV","DA","BW","Ds","Be","Bv","B0","CG","w","B7","CB","q",
	//"CF","CH","CK","Du","c","CT","m","CZ","Cb","Cq","DE","DV","Dw","DZ","Db",
	//"Dh","Dx","Dj","Dm","V","Y","e","i","y","5","7","9","AD","AG","Do","Ab","Dp",
	//"Av","Ax","Ay","Az","A1","A6","A9","BE","BH","Bh","Bo","Br","Bx","Cd","C1",
	//"C5","DJ","DK","Dv","DN","DP","DW","Dg"
	//};
	//for (uint16_t i = 0; i < serVec.size(); ++i) {
	//	auto it = serMap.find(serVec[i]);
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {

		if (it->second.cusConnected.size() == 0)
			continue;
		if (it->second.freeDays == 0)
			continue;
		if (GetMax(it->second.dailyNeedsMaxHeap).second == 0)
			continue;//����������������Ϊ0
		time = GetMax(it->second.dailyNeedsMaxHeap).first;

		//����ǰһ��ʱ�̵Ļ���
		if (time > 0) {
			newConstrain = it->second.constrain - it->second.cache[time - 1].second;
		}
		else {
			newConstrain = it->second.constrain;
		}

		//����Ҫ��������n��ʱ�̣���ǰ�������ۼƻ��治�ܴ��ں�n��ʱ�̵�ʣ��ռ䣬ֱ���ۼӻ���Ϊ0����ʣ��ռ���
		curData = min(GetMax(it->second.dailyNeedsMaxHeap).second, newConstrain);//��ǰʱ�����Ҫװ���˵Ļ�����ʱ��curData����constrain����ȫ�������ֵ
		curTime = time;
		while (curTime < it->second.dataAllocate.size() - 1) {
			newCache = floor(curData * 0.05);
			if (newCache == 0)
				break;
			if (it->second.dataAllocate[curTime + 1].second + newCache > it->second.constrain) {
				//����Ų����ˣ���Ҫ����newConstrain����ԭ���ı�ȡС��
				tempTime = curTime;
				//maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second - it->second.cache[curTime].second;
				//��ǰʱ�������Դ�����һʱ�̵Ļ����С
				maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second;//��Ӧ�ü�ȥcache������ʵ����һ����ʽ��cache+dataallocate=constrain
				//maxConstrain = newConstrain;//��һ�ν���tempTime = curTime = Time
				while (tempTime >= time) {
					//��ǰʱ���������ޣ�ȡ���ڿ��Դ����������󻺴�
					maxConstrain = (maxCache + 1) * 20 - 1;//����ʣ��ռ���1����ôǰһ��ʱ�������Է�39��Ҳ����2/0.05-1
					tempTime--;
					//��ǰŲһ��ʱ�̵���󻺴棬���tempTime���-1����������Ҳ���������ʱ������
					maxCache = maxConstrain - it->second.dataAllocate[tempTime + 1].second;
				}
				//ȡ������������С����Ϊ�µ�����
				newConstrain = min(newConstrain, maxConstrain);
			}
			curData = it->second.dataAllocate[curTime + 1].second + newCache;
			curTime++;
		}

		//maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//��ǰʱ�̵�����Ҫ��ȥ��һʱ�����µĻ���
		if (newConstrain < it->second.constrain) {
			maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//��ǰʱ�̵�����Ҫ��ȥ��һʱ�����µĻ���
		}
		else {
			maxFreeData = GetMax(it->second.dailyNeedsMaxHeap).second;
		}
		//maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//��ǰʱ�̵�����Ҫ��ȥ��һʱ�����µĻ���

		if (maxFreeData > maxNeeds) {
			if (it->second.freeDays > 0) {
				//����Ҫ�а�������
				maxNeeds = maxFreeData;//�߼���ȷ
				//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//����ط����߼�bug������Ч���á�����
				maxServerName = it->first;
				maxConnection = it->second.cusConnected.size();
			}
		}
		//�������һ����ѡ����������
		//else if (maxFreeData == maxNeeds) {
		//	if (it->second.cusConnected.size() > maxConnection) {
		//		if (it->second.freeDays > 0) {
		//			//����Ҫ�а�������
		//			//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//����ط����߼�bug������Ч���á�����
		//			maxNeeds = maxFreeData;//����ط����߼�bug������Ч���á�����
		//			maxServerName = it->first;
		//			maxConnection = it->second.cusConnected.size();
		//		}
		//	}
		//}
		//	if (GetMax(it->second.dailyNeedsMaxHeap).second > maxConnection) {
		//		if (it->second.freeDays > 0) {
		//			//����Ҫ�а�������
		//			//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//����ط����߼�bug������Ч���á�����
		//			maxNeeds = maxFreeData;//����ط����߼�bug������Ч���á�����
		//			maxServerName = it->first;
		//			maxConnection = GetMax(it->second.dailyNeedsMaxHeap).second;
		//		}
		//	}
		//}

	}
	//���û�пͻ��������ˣ����ؾ���""
	if (maxServerName != "") {

		serMap.find(maxServerName)->second.freeDays--;//���δ�����һ
		freeServerInDay[GetMax(serMap.find(maxServerName)->second.dailyNeedsMaxHeap).first].push_back(maxServerName);//��¼��һ���ĸ�����������Ϊ�˰��Σ�����Ҫ�ָ�
		serMap.find(maxServerName)->second.freeDaySet.insert(GetMax(serMap.find(maxServerName)->second.dailyNeedsMaxHeap).first);//��¼����������ڵڼ������ˣ�֮��������
		//serMap.find(maxServerName)->second.maxPointer++;
	}
	return maxServerName;
}

//�õ��������µ�����
int serverClass::getNewConstrain(string serName, int time, int virLimit) {
	int newConstrain = 0;
	if (GetOneServerCurrentDayActualData(serName, time) > virLimit) {
		newConstrain = serMap.find(serName)->second.constrain;
	}
	else {
		newConstrain = virLimit;
	}
	return newConstrain;
}

//�ж�һ�����Ƿ���װ��ȥ����̬������
bool serverClass::IfCanPutAdjust(string serName, int time, int streamNum, int virLimit, const string streamID, const int virCenterLim, unordered_set<int>&centerNodeFree) {
	int curDataAllocatePlusCache = 0, curCache = 1, newConstrain = 0,maxCenterPre=0,centerCostLeft=0,centerGap=0,pretime=time;
	centerCostLeft = virCenterLim - centerDataAllocate[time].second;

	if (centerNodeFree.find(time)== centerNodeFree.end()) {
		if (streamSource[time].find(streamID)->second.find(serName) == streamSource[time].find(streamID)->second.end()) {
			//���û�з��������������maxCenterPre=0
			maxCenterPre = 0;
			if (streamNum > centerCostLeft) {
				return false;
			}
		}
		else {
			maxCenterPre = streamSource[time].find(streamID)->second.find(serName)->second;
			if (streamNum - maxCenterPre > centerCostLeft) {
				return false;
			}
		}
	}

	if (time > 0) {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum + serMap.find(serName)->second.cache[time - 1].second;
	}
	else {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
	}
	newConstrain = getNewConstrain(serName, time, virLimit);

	//��������������ޣ�����fasle
	if (curDataAllocatePlusCache > newConstrain)
		return false;
	curCache = floor(curDataAllocatePlusCache * 0.05);
	while (curCache != 0) {
		if (time >= serMap.find(serName)->second.dataAllocate.size() - 1)
			break;
		newConstrain = getNewConstrain(serName, time + 1, virLimit);
		if (serMap.find(serName)->second.dataAllocate[time + 1].second + curCache > newConstrain)
			return false;
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time + 1].second + curCache;
		time++;
		curCache = floor(curDataAllocatePlusCache * 0.05);

	}
	if (streamNum - maxCenterPre>0) {
		centerDataAllocate[pretime].second += (streamNum - maxCenterPre);
	}
	if (centerNodeFree.find(pretime) == centerNodeFree.end()) {
		if (streamSource[pretime].find(streamID)->second.find(serName) == streamSource[pretime].find(streamID)->second.end()) {
			//���û�з���������������¼������
			streamSource[pretime].find(streamID)->second.insert(make_pair(serName, streamNum));
		}
		else {
			//����з���������������¼���ֵ
			streamSource[pretime].find(streamID)->second.find(serName)->second=max(streamNum, maxCenterPre);
		}
	}

	return true;
}

////�ж�һ�����Ƿ���װ��ȥ����̬������
//bool serverClass::IfCanPutAdjust(string serName, int time, int streamNum, int virLimit) {
//	int curDataAllocatePlusCache = 0, curCache = 1, newConstrain = 0;
//
//	if (time > 0) {
//		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum + serMap.find(serName)->second.cache[time - 1].second;
//	}
//	else {
//		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
//	}
//	newConstrain = getNewConstrain(serName, time, virLimit);
//
//	//��������������ޣ�����fasle
//	if (curDataAllocatePlusCache > newConstrain)
//		return false;
//	curCache = floor(curDataAllocatePlusCache * 0.05);
//	while (curCache != 0) {
//		if (time >= serMap.find(serName)->second.dataAllocate.size() - 1)
//			break;
//		newConstrain = getNewConstrain(serName, time + 1, virLimit);
//		if (serMap.find(serName)->second.dataAllocate[time + 1].second + curCache > newConstrain)
//			return false;
//		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time + 1].second + curCache;
//		time++;
//		curCache = floor(curDataAllocatePlusCache * 0.05);
//
//	}
//	return true;
//}



//�ж�һ�����Ƿ���װ��ȥ
bool serverClass::IfCanPut(string serName, int time, int streamNum, int constrain) {
	int curDataAllocatePlusCache = 0, curCache = 1;
	if (time > 0) {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum + serMap.find(serName)->second.cache[time - 1].second;
	}
	else {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
	}
	//��������������ޣ�����fasle
	if (curDataAllocatePlusCache > constrain)
		return false;
	curCache = floor(curDataAllocatePlusCache * 0.05);
	while (curCache != 0) {
		if (time >= serMap.find(serName)->second.dataAllocate.size() - 1)
			break;
		if (serMap.find(serName)->second.dataAllocate[time + 1].second + curCache > constrain)
			return false;
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time + 1].second + curCache;
		time++;
		curCache = floor(curDataAllocatePlusCache * 0.05);
	}
	return true;
}

void serverClass::UpdataCacheForFirstStep(int time, string serName, int V) {
	int cache = 1, cacheTime = time;
	while (cache != 0 && cacheTime < serMap.find(serName)->second.dataAllocate.size()) {
		if (cacheTime > 0)
			cache = floor((serMap.find(serName)->second.dataAllocate[cacheTime].second + serMap.find(serName)->second.cache[cacheTime - 1].second) * 0.05);
		else
			cache = floor(serMap.find(serName)->second.dataAllocate[cacheTime].second * 0.05);
		//�������һ�¾Ͳ��������������
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		serMap.find(serName)->second.cache[cacheTime].second = cache;

		cacheTime++;
	}
}

void serverClass::UpdataCacheUingInAllocate(int time, string serName, int V) {
	int cache = 1, cacheTime = time;
	while (cacheTime < serMap.find(serName)->second.dataAllocate.size()) {
		if (cacheTime > 0)
			cache = floor((serMap.find(serName)->second.dataAllocate[cacheTime].second + serMap.find(serName)->second.cache[cacheTime - 1].second) * 0.05);
		else
			cache = floor(serMap.find(serName)->second.dataAllocate[cacheTime].second * 0.05);
		//�������һ�¾Ͳ��������������
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		serMap.find(serName)->second.cache[cacheTime].second = cache;
		cacheTime++;
	}
}

void serverClass::UpdataCacheSecondStep(int time, string serName, int V) {
	int cache = 1, cacheTime = time;
	while (cache != 0 && cacheTime < serMap.find(serName)->second.dataAllocate.size()-1) {
		if (cacheTime > 0)
			cache = floor((serMap.find(serName)->second.dataAllocate[cacheTime].second + serMap.find(serName)->second.cache[cacheTime - 1].second) * 0.05);
		else
			cache = floor(serMap.find(serName)->second.dataAllocate[cacheTime].second * 0.05);
		//�������һ�¾Ͳ��������������
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		serMap.find(serName)->second.cache[cacheTime].second = cache;

		//�����һ���������ޣ�����������������
		if (serMap.find(serName)->second.historyConstrain.first == cacheTime + 1 &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {
			//cout << serName << endl;
			serMap.find(serName)->second.historyConstrain.second = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;//����������
			if (serMap.find(serName)->second.historyConstrain.second <= V)//���С��V
				serMap.find(serName)->second.cost = V;
			else {//�������V
				serMap.find(serName)->second.cost =
					double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
					/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
			}
		}
		//������һ������µ�cache���Ƿ���������ޣ�������ھ͸���
		if (serMap.find(serName)->second.freeDaySet.find(cacheTime + 1) == serMap.find(serName)->second.freeDaySet.end() &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {

			serMap.find(serName)->second.historyConstrain.second = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;//����������
			serMap.find(serName)->second.historyConstrain.first = cacheTime + 1;
			if (serMap.find(serName)->second.historyConstrain.second <= V)//���С��V
				serMap.find(serName)->second.cost = V;
			else {//�������V
				serMap.find(serName)->second.cost =
					double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
					/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
			}
		}
		cacheTime++;
	}
}

void serverClass::sortSteamMaxOrder(unordered_map<string, vector<streamInfo>>& streamVecMap, vector<pair<string, int>>& streamKinds) {
	//int num = 0;
	//for (auto it = streamVecMap.begin(); it != streamVecMap.end(); ++it) {
	//	sort(it->second.begin(), it->second.end(), streamInfoDown);
	//	streamKinds[num].first = it->first;
	//	streamKinds[num].second = it->second[0].streamNum;
	//	num++;
	//}
	//sort(streamKinds.begin(), streamKinds.end(), kindCmp);

	int num = 0;
	for (auto it = streamVecMap.begin(); it != streamVecMap.end(); ++it) {
		int sum = 0;
		for (int i = 0; i < it->second.size(); i++) {
			sum += it->second[i].streamNum;
		}
		sort(it->second.begin(), it->second.end(), streamInfoDown);
		//sort(it->second.begin(), it->second.end(), streamInfoUp);
		streamKinds[num].first = it->first;
		//streamKinds[num].second = it->second[0].streamNum;
		streamKinds[num].second = sum;
		num++;
	}
	sort(streamKinds.begin(), streamKinds.end(), kindCmp);

}



void serverClass::FullOneServerNew(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time) {
	int dataAllocate = 0, index = 0, streamNum = 0, restSpace = 0, cache = 0, cacheTime = 0, cusConnectedNum = 0, newConstrain = 0;
	string cusName, streamID, downSerName;
	streamInfo temp, curSteam;
	bool fullServerFlag = false, ifPut = false;
	unordered_map<string, int> cusDataAllocate;
	unordered_map<string, vector<streamInfo>> streamVecMap;
	vector<streamInfo> streamVec;

	cusConnectedNum = serMap.find(serName)->second.cusConnected.size();
	for (auto it = serMap.find(serName)->second.cusConnected.begin(); it != serMap.find(serName)->second.cusConnected.end(); ++it) {
		if (cusMap.find(it->first)->second.unSatisfiedNum[time] == 0)
			continue;
		cusName = it->first;
		cusDataAllocate.insert(make_pair(cusName, 0));
		//��û�з��������
		for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
				continue;
			temp.cusName = cusMap.find(cusName)->first;
			temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
			temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
			temp.time = time;
			temp.index = streamIndex;//�±�
			streamVecMap[temp.streamID].push_back(temp);
			//streamVec.push_back(temp);
		}
	}

	vector<pair<string, int>>streamKinds(streamVecMap.size());

	sortSteamMaxOrder(streamVecMap, streamKinds);

	for (int i = 0; i < streamKinds.size(); i++) {
		// 
		//for (auto it = streamVecMap.begin(); it != streamVecMap.end(); ++it) {
		auto it = streamVecMap.find(streamKinds[i].first);
		if (fullServerFlag)
			break;
		//sort(it->second.begin(), it->second.end(), streamInfoDown);
		for (uint16_t i = 0; i < it->second.size(); ++i) {
			curSteam = it->second[i];
			cusName = curSteam.cusName;
			streamID = curSteam.streamID;
			streamNum = curSteam.streamNum;
			index = curSteam.index;
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index]) {
				continue;
			}
			ifPut = IfCanPut(serName, time, streamNum, serMap.find(serName)->second.constrain);
			if (ifPut) {
				cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;//�Ѿ�������
				cusMap.find(cusName)->second.unSatisfiedNum[time]--;//ɾ���������,������һ
				serMap.find(serName)->second.dataAllocate[time].second += streamNum;//��������һ����ܷ���������
				UpdataCacheForFirstStep(time, serName, V);
				UpdateCenterCiteHisConstrain(serName, time, streamID, streamNum, A);//�������Ľڵ�

				sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, streamNum));//�����ά����
				//restSpace -= streamNum;
				cusDataAllocate.find(cusName)->second += streamNum;
				if (time > 0) {
					restSpace = serMap.find(serName)->second.constrain - serMap.find(serName)->second.dataAllocate[time].second - serMap.find(serName)->second.cache[time - 1].second;
				}
				else {
					restSpace = serMap.find(serName)->second.constrain - serMap.find(serName)->second.dataAllocate[time].second;
				}
				if (restSpace == 0) {
					fullServerFlag = true;
					break;
				}
			}
			else {
				//break;
			}
		}
	}

	//������
	for (auto it = cusDataAllocate.begin(); it != cusDataAllocate.end(); ++it) {
		cusName = it->first;
		dataAllocate = it->second;
		for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
			downSerName = it->first;
			if (downSerName == serName)
				continue;
			index = serMap.find(downSerName)->second.dayIndex.find(time)->second;//��ȡ��Ӧʱ�䣬��Ӧ������±�
			serMap.find(downSerName)->second.dailyNeedsMaxHeap->Elements[index].second -= dataAllocate;
			adjustHeap(serMap.find(downSerName)->second.dailyNeedsMaxHeap, index, downSerName);//�����ѽṹ
		}
	}

}


void serverClass::FullOneServer(double A, int V, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, string serName, int time) {
	int dataAllocate = 0, index = 0, streamNum = 0, restSpace = 0, cache = 0, cacheTime = 0, cusConnectedNum = 0, newConstrain = 0;
	string cusName, streamID, downSerName;
	streamInfo temp, curSteam;
	bool fullServerFlag = false, ifPut = false;
	unordered_map<string, int> cusDataAllocate;
	AllStreamHeapClass myStream;
	vector<streamInfo> streamVec;
	cusConnectedNum = serMap.find(serName)->second.cusConnected.size();
	for (auto it = serMap.find(serName)->second.cusConnected.begin(); it != serMap.find(serName)->second.cusConnected.end(); ++it) {
		if (cusMap.find(it->first)->second.unSatisfiedNum[time] == 0)
			continue;
		cusName = it->first;
		cusDataAllocate.insert(make_pair(cusName, 0));
		//��û�з��������
		for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
				continue;
			temp.cusName = cusMap.find(cusName)->first;
			temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
			temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
			temp.time = time;
			temp.index = streamIndex;//�±�
			streamVec.push_back(temp);
		}
	}

	myStream.MakeHeap(streamVec);
	while (!myStream.IsEmpty()) {
		if (fullServerFlag)
			break;
		curSteam = myStream.DeleteMax();
		cusName = curSteam.cusName;
		streamID = curSteam.streamID;
		streamNum = curSteam.streamNum;
		index = curSteam.index;
		if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index]) {
			continue;
		}
		ifPut = IfCanPut(serName, time, streamNum, serMap.find(serName)->second.constrain);
		if (ifPut) {
			cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;//�Ѿ�������
			cusMap.find(cusName)->second.unSatisfiedNum[time]--;//ɾ���������,������һ
			serMap.find(serName)->second.dataAllocate[time].second += streamNum;//��������һ����ܷ���������
			UpdataCacheForFirstStep(time, serName, V);
			UpdateCenterCiteHisConstrain(serName, time, streamID, streamNum, A);//�������Ľڵ�

			sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, streamNum));//�����ά����
			//restSpace -= streamNum;
			cusDataAllocate.find(cusName)->second += streamNum;
			if (time > 0) {
				restSpace = serMap.find(serName)->second.constrain - serMap.find(serName)->second.dataAllocate[time].second - serMap.find(serName)->second.cache[time - 1].second;
			}
			else {
				restSpace = serMap.find(serName)->second.constrain - serMap.find(serName)->second.dataAllocate[time].second;
			}
			if (restSpace == 0) {
				fullServerFlag = true;
				break;
			}
		}
	}

	//������
	for (auto it = cusDataAllocate.begin(); it != cusDataAllocate.end(); ++it) {
		cusName = it->first;
		dataAllocate = it->second;
		for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
			downSerName = it->first;
			if (downSerName == serName)
				continue;
			index = serMap.find(downSerName)->second.dayIndex.find(time)->second;//��ȡ��Ӧʱ�䣬��Ӧ������±�
			serMap.find(downSerName)->second.dailyNeedsMaxHeap->Elements[index].second -= dataAllocate;
			adjustHeap(serMap.find(downSerName)->second.dailyNeedsMaxHeap, index, downSerName);//�����ѽṹ
		}
	}

}

void serverClass::putOneStreamToServer(bool toV, int index, int needs, string streamID,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
	string serName, string cusName, int time, int V, double A) {
	int curData = 0;

	if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] == true) {
		throw "�Ѿ���������";
		return;
	}
	cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;
	cusMap.find(cusName)->second.unSatisfiedNum[time]--;
	//deleteRequest(streamID, cusName, time, customer, index);
	//cusMap.find(cusName)->second.dataNeeds[time] = 0;//�ͻ�����һ��������½�
	serMap.find(serName)->second.dataAllocate[time].second += needs;//��������һ����ܷ���������
	//UpdateStreamInfo(serName, time, serMap.find(serName)->second.dataAllocate.size(), streamID, needs);//����������ڷ������еĴ�С�������Ľڵ�����
	//UpdateCenterCiteInfo(serName, time, serMap.find(serName)->second.dataAllocate.size(), streamID, needs, A);//����������ڷ������еĴ�С�������Ľڵ�����

	//����cache����
	if (toV) {
		UpdataCacheForFirstStep(time, serName, V);
	}
	else {
		UpdataCacheSecondStep(time, serName, V);
	}

	sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, needs));//�����ά����
	//restSpace -= needs;
	//��ǰʱ�̵�ֵӦ�ü���ǰһʱ�̵�cache
	UpdateCenterCiteHisConstrain(serName, time, streamID, needs, A);//�������Ľڵ�


	if (time > 0) {
		curData = serMap.find(serName)->second.dataAllocate[time].second + serMap.find(serName)->second.cache[time - 1].second;
	}
	else {
		curData = serMap.find(serName)->second.dataAllocate[time].second;
	}

	if (curData > serMap.find(serName)->second.historyConstrain.second) {
		//�����춼�����ˣ�������һ���ֵ�������������ޣ���һ��������µ�cost
		serMap.find(serName)->second.historyConstrain.second = curData;//����������
		serMap.find(serName)->second.historyConstrain.first = time;
		if (serMap.find(serName)->second.historyConstrain.second <= V)//���С��V
			serMap.find(serName)->second.cost = V;
		else {//�������V
			serMap.find(serName)->second.cost =
				double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
				/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
		}
	}
	else {
		//�����춼�����ˣ�������һ���ֵ��С�������ޣ���һ��᲻�����µ�cost

	}
	return;
}

//����Ӧ�ͻ��˷�����Դ��V
void serverClass::AllocateToVNew(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
	string serName, int time, int V, double A) {
	int index = 0, needs = 0, restSpace = 0, cusConnectedNum = serMap.find(serName)->second.cusConnected.size();
	string streamId, cusName;
	bool fullServerFlag = false, ifPut = false;

	for (auto it = serMap.find(serName)->second.cusConnected.begin(); it != serMap.find(serName)->second.cusConnected.end(); ++it) {
		cusName = it->first;
		if (fullServerFlag)
			break;

		streamInfo temp;
		unordered_map<string, int> cusDataAllocate;
		unordered_map<string, vector<streamInfo>> streamVecMap;
		vector<streamInfo> streamVec;

		for (auto it = serMap.find(serName)->second.cusConnected.begin(); it != serMap.find(serName)->second.cusConnected.end(); ++it) {
			if (cusMap.find(it->first)->second.unSatisfiedNum[time] == 0)
				continue;
			cusName = it->first;
			cusDataAllocate.insert(make_pair(cusName, 0));
			//��û�з��������
			for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
				if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
					continue;
				temp.cusName = cusMap.find(cusName)->first;
				temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
				temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
				temp.time = time;
				temp.index = streamIndex;//�±�
				streamVecMap[temp.streamID].push_back(temp);
				//streamVec.push_back(temp);
			}
		}

		vector<pair<string, int>>streamKinds(streamVecMap.size());

		sortSteamMaxOrder(streamVecMap, streamKinds);

		for (int i = 0; i < streamKinds.size(); i++) {
			auto it = streamVecMap.find(streamKinds[i].first);
			if (fullServerFlag)
				break;
			//sort(it->second.begin(), it->second.end(), streamInfoDown);
			for (uint16_t i = 0; i < it->second.size(); ++i) {
				streamId = it->second[i].streamID;//��������
				needs = it->second[i].streamNum;//���Ĵ�С
				index = it->second[i].index;
				cusName = it->second[i].cusName;
				if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] == true)
					continue;
				ifPut = IfCanPut(serName, time, needs, V);
				if (ifPut) {
					putOneStreamToServer(true, index, needs, streamId,
						sourceAllocate, cusMap, serName, cusName, time, V, A);
					if (time > 0) {
						restSpace = V - serMap.find(serName)->second.dataAllocate[time].second - serMap.find(serName)->second.cache[time - 1].second;
					}
					else {
						restSpace = V - serMap.find(serName)->second.dataAllocate[time].second;
					}
					if (restSpace == 0) {
						fullServerFlag = true;
						break;
					}
				}
			}
		}
	}
}


//����Ӧ�ͻ��˷�����Դ��V
void serverClass::AllocateToV(vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
	string serName, int time, int V, double A) {
	int needs = 0, restSpace = 0, cusConnectedNum = serMap.find(serName)->second.cusConnected.size();
	string streamId, cusName;
	bool fullServerFlag = false, ifPut = false;
	for (auto it = serMap.find(serName)->second.cusConnected.begin(); it != serMap.find(serName)->second.cusConnected.end(); ++it) {
		cusName = it->first;
		if (fullServerFlag)
			break;
		for (int i = 0; i < cusMap.find(cusName)->second.dataNeeds[time].size(); ++i) {
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][i] == true)
				continue;
			streamId = cusMap.find(cusName)->second.dataNeeds[time][i].first;//��������
			needs = cusMap.find(cusName)->second.dataNeeds[time][i].second;//���Ĵ�С

			ifPut = IfCanPut(serName, time, needs, V);
			if (ifPut) {
				putOneStreamToServer(true, i, needs, streamId,
					sourceAllocate, cusMap, serName, cusName, time, V, A);
				if (time > 0) {
					restSpace = V - serMap.find(serName)->second.dataAllocate[time].second - serMap.find(serName)->second.cache[time - 1].second;
				}
				else {
					restSpace = V - serMap.find(serName)->second.dataAllocate[time].second;
				}
				if (restSpace == 0) {
					fullServerFlag = true;
					break;
				}
			}
		}
	}
}

void serverClass::UpdataHisConstrainAfterFreeCostNew(int timeLen, int V) {
	int curData;
	int index = ceil(timeLen * 0.95) - 1;//����ȡ����Ȼ�����±꣬��Ҫ��һ

	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		string serName = it->first;
		if (it->second.freeDaySet.size() == 0)
			continue;
		vector<pair<int, int>> vec = it->second.dataAllocate;
		for (uint16_t i = 0; i < vec.size(); ++i) {
			uint16_t t = vec[i].first;
			if (t > 0) {
				vec[i].second += it->second.cache[t - 1].second;
			}
		}
		sort(vec.begin(), vec.end(), cmpUp);
		it->second.historyConstrain.first = vec[index].first;
		it->second.historyConstrain.second = vec[index].second;

		if (it->second.historyConstrain.second <= V)//���С��V
			it->second.cost = V;
		else {//�������V
			it->second.cost =
				double((it->second.historyConstrain.second - V) * (it->second.historyConstrain.second - V))
				/ double(it->second.constrain) + it->second.historyConstrain.second;
		}
	}
}


void serverClass::UpdataHisConstrainAfterFreeCost(int timeLen, int V) {
	int curData;
	string serName;
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		serName = it->first;
		//if (serName == "C")
		//	cout << endl;
		for (int t = 0; t < timeLen; ++t) {
			//���ε���϶�����������
			//if (t == 191 && serName == "b")
			//	cout << endl;
			if (it->second.dataAllocate[t].second > V)
				continue;
			if (t > 0) {
				curData = it->second.dataAllocate[t].second + it->second.cache[t - 1].second;
			}
			else {
				curData = it->second.dataAllocate[t].second;
			}
			//cout << t << "  " << curData << endl;
			if (curData > serMap.find(serName)->second.historyConstrain.second) {
				//�����춼�����ˣ�������һ���ֵ�������������ޣ���һ��������µ�cost
				serMap.find(serName)->second.historyConstrain.second = curData;//����������
				serMap.find(serName)->second.historyConstrain.first = t;
				if (serMap.find(serName)->second.historyConstrain.second <= V)//���С��V
					serMap.find(serName)->second.cost = V;
				else {//�������V
					serMap.find(serName)->second.cost =
						double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
						/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
				}
			}
		}
	}

}

//�ж�����������Ƿ�����
bool serverClass::ifFullServer(const unordered_map<string, serStruct>& serMap, string serName, int time) {
	if (serMap.find(serName)->second.dataAllocate[time].second == serMap.find(serName)->second.constrain) {
		return true;
	}
	else {
		return false;
	}
}

//�����µĿ���
double serverClass::newCitecostCal(string serName, int time, int streamNum, int V) {
	int newData = 0, cache = 0, cacheTime = 0;
	double newCost = 0.0, citeCostBack = 0.0;
	if (time > 0) {
		newData = serMap.find(serName)->second.dataAllocate[time].second + serMap.find(serName)->second.cache[time - 1].second + streamNum;
	}
	else {
		newData = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
	}
	//int newData = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
	if (newData <= serMap.find(serName)->second.historyConstrain.second) {
		//�������֮��С�������ޣ��Ǿͻ���ԭ����cost
		newCost = serMap.find(serName)->second.cost;
	}
	else {
		//��������������ޣ��Ǿ���Ҫ���¼���
		if (newData < V) {
			newCost = V;
		}
		else {
			newCost = ((newData - V) * (newData - V)) / serMap.find(serName)->second.constrain + newData;
		}
	}

	cache = floor(newData * 0.05);
	cacheTime = time;
	//�����Ե�ڵ��cost�����ܻ��֮��������Ӱ��
	while (cache != 0 && cacheTime < serMap.find(serName)->second.dataAllocate.size()-1) {
		if (cacheTime != time) {
			if (cacheTime > 0)
				cache = floor((serMap.find(serName)->second.dataAllocate[cacheTime].second + cache) * 0.05);
			else
				cache = floor(serMap.find(serName)->second.dataAllocate[cacheTime].second * 0.05);
		}
		//�������һ�¾Ͳ��������������
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		//������һ������µ�cache���Ƿ���������ޣ�������ھ͸���
		if (serMap.find(serName)->second.freeDaySet.find(cacheTime + 1) == serMap.find(serName)->second.freeDaySet.end() &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {
			//cout << serName <<" "<< time << endl;
			newData = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;
			if (newData <= V)//���С��V
				citeCostBack = V;
			else {//�������V
				citeCostBack =
					double((newData - V) * (newData - V))
					/ double(serMap.find(serName)->second.constrain) + newData;
			}
		}
		newCost = max(newCost, citeCostBack);
		cacheTime++;
	}
	return newCost;

}


//�ҵ�ǰ������С��
string serverClass::findMinServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A) {
	int maxConstrain = 0, maxConnection = 0, curData = 0;
	string minServer, serName;
	double newCenterCost = 0.0, newCost = 0.0, gapCost = 0.0, minData = 0x7fffffff;
	bool ifPut = false;

	for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
		serName = it->first;
		if (ifFullServer(serMap, serName, time))
			continue;//����������

		ifPut = IfCanPut(serName, time, streamNum, serMap.find(serName)->second.constrain);
		if (ifPut == false)
			continue;
		curData = GetOneServerCurrentDayActualData(serName, time);
		if (curData + streamNum <= V) {
			return serName;
		}
		else {
			if (curData < minData) {
				minData = curData;
				minServer = serName;
			}
		}
	}
#ifdef TEST
	//cout << "Selected: " << minServer << "  minCost: " << minCost << "  maxConnection: " << serMap.find(minServer)->second.cusConnected.size() << endl;
#endif // TEST
	return minServer;
}


//������Ž�ȥ������С��
string serverClass::findMinCostServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A) {
	int maxConstrain = 0, maxConnection = 0;
	string minServer, serName;
	double newCenterCost = 0.0,newCost = 0.0, gapCost = 0.0, minCost = 0x7fffffff;
	bool ifPut = false;

	for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
		serName = it->first;
		if (ifFullServer(serMap, serName, time))
			continue;//����������

		ifPut = IfCanPut(serName, time, streamNum, serMap.find(serName)->second.constrain);
		if (ifPut == false)
			continue;

		newCost = newCitecostCal(serName, time, streamNum, V);
		//newCenterCost = costCalCenter(serName, time, streamNum, V, streamID, A);
		//cout << newCenterCost << " ";
		//cout << centerCost << endl;
		gapCost = newCost - serMap.find(serName)->second.cost;//��ֵ��ѡ�����С�ķ�����
		//gapCost = newCenterCost - centerCost;//��ֵ��ѡ�����С�ķ�����
		gapCost = newCost + newCenterCost - serMap.find(serName)->second.cost - centerCost;//��ֵ��ѡ�����С�ķ�����
		if (gapCost == 0) {
			minCost = gapCost;
			maxConstrain = serMap.find(serName)->second.constrain;
			minServer = serName;
			break;
		}

		if (gapCost < minCost) {
			minCost = gapCost;
			maxConstrain = serMap.find(serName)->second.constrain;
			minServer = serName;
		}
		else if (gapCost == minCost) {
			if (serMap.find(serName)->second.constrain > maxConstrain) {
				minCost = gapCost;
				maxConstrain = serMap.find(serName)->second.constrain;
				minServer = serName;
				maxConnection = serMap.find(serName)->second.cusConnected.size();
			}
			else if (serMap.find(serName)->second.constrain == maxConstrain) {
				if (serMap.find(serName)->second.cusConnected.size() > maxConnection) {
					minCost = gapCost;
					maxConstrain = serMap.find(serName)->second.constrain;
					minServer = serName;
					maxConnection = serMap.find(serName)->second.cusConnected.size();
				}
			}
		}
		else {

		}
	}
#ifdef TEST
	//cout << "Selected: " << minServer << "  minCost: " << minCost << "  maxConnection: " << serMap.find(minServer)->second.cusConnected.size() << endl;
#endif // TEST
	return minServer;
}

//����һ����������ĳһ����������������Ҫ����ǰһ��ʱ�̵�cache
uint16_t serverClass::GetOneServerCurrentDayActualData(string serName, uint16_t t) {
	uint16_t curData;
	if (t > 0) {
		curData = serMap.find(serName)->second.dataAllocate[t].second + serMap.find(serName)->second.cache[t - 1].second;
	}
	else {
		curData = serMap.find(serName)->second.dataAllocate[t].second;
	}
	return curData;
}


//�������Ľڵ�������޶�
void serverClass::adjustCenterCiteHistoryHeap(int index) {
	ElementType temp = CenterCiteHeap->Elements[index];
	int Parent, Child;
	for (Parent = index; Parent * 2 <= CenterCiteHeap->Size; Parent = Child) {//�о��Ǹý��������
		//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
		Child = Parent * 2;//ָ�������
		if ((Child != CenterCiteHeap->Size) && (CenterCiteHeap->Elements[Child].second > CenterCiteHeap->Elements[Child + 1].second))
			//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
			//�ڶ������ж����Ҷ����ĸ�����
			Child++;
		if (temp.second <= CenterCiteHeap->Elements[Child].second)//������ֵ�������ĸ����ӻ��󣬾��˳�
		//�����ֵ�ŵ���������
			break;
		else {//����ͰѴ���ӷŵ����ڵ���
			if (centerHeapIndex.find(CenterCiteHeap->Elements[Child].first) != centerHeapIndex.end()) {//�п���0ʱ�̱��Ƴ�index��map�ˣ���ʼȫ��0�����0��Ӧ���ڶ��У��Ͳ����и�ֵ����
				centerHeapIndex.find(CenterCiteHeap->Elements[Child].first)->second = Parent;
			}
			CenterCiteHeap->Elements[Parent] = CenterCiteHeap->Elements[Child];
		}
	}
	if (centerHeapIndex.find(temp.first) != centerHeapIndex.end()) {
		centerHeapIndex.find(temp.first)->second = Parent;
	}
	else {
		centerHeapIndex.insert(make_pair(temp.first, Parent));//�����϶������µ�ʱ�������
	}
	CenterCiteHeap->Elements[Parent] = temp;
	return;
}


//�������Ľڵ��������
void serverClass::UpdateCenterCiteHisConstrain(string serName, int time, string streamId, int streamNum, double A) {
	bool increaseData = false;
	if (streamSource[time].find(streamId) == streamSource[time].end()) {
		//��һ��������
		streamSource[time][streamId].insert(make_pair(serName,streamNum));
		centerDataAllocate[time].second += streamNum;//��ǰʱ�̷���������
		increaseData = true;
	}
	else {
		//����������
		if (streamSource[time].find(streamId)->second.find(serName) == streamSource[time].find(streamId)->second.end()) {
			//��һ���µķ����������
			streamSource[time].find(streamId)->second.insert(make_pair(serName, streamNum));
			centerDataAllocate[time].second += streamNum;//��ǰʱ�̷���������
			increaseData = true;
		}
		else {
			//�����з��������룬�Ҵ�С�϶����󣬵�ǰʱ�̷���������
			if (streamNum > streamSource[time].find(streamId)->second.find(serName)->second) {
				centerDataAllocate[time].second -= streamSource[time].find(streamId)->second.find(serName)->second;//��ǰʱ�̷���������
				centerDataAllocate[time].second += streamNum;//��ǰʱ�̷���������
				streamSource[time].find(streamId)->second.find(serName)->second = streamNum;
				increaseData = true;
			}
		}
	}
	if (increaseData) {
		//����ڶ��У���Ҫ���е���
		if (centerHeapIndex.find(time) != centerHeapIndex.end()) {
			int centerIndex = centerHeapIndex.find(time)->second;
			CenterCiteHeap->Elements[centerIndex].second = centerDataAllocate[time].second;
			adjustCenterCiteHistoryHeap(centerIndex);
		}
		//������ڶ��У������Ƿ���������ޣ�������������ޣ��ͽ��и��£�����ɶҲ����
		else {
			if (centerDataAllocate[time].second > GetMax(CenterCiteHeap).second) {
				//ɾ��centerHeapIndex��������ޣ�һ��ʼȫ��0���϶���û��
				if (centerHeapIndex.find(GetMax(CenterCiteHeap).first) != centerHeapIndex.end()) {
					centerHeapIndex.erase(GetMax(CenterCiteHeap).first);
				}
				centerHeapIndex.insert(make_pair(time, 1));
				CenterCiteHeap->Elements[1] = make_pair(time, centerDataAllocate[time].second);
				adjustCenterCiteHistoryHeap(1);
			}
		}
	}

	if (CenterCiteHeap->Elements[1].second != 0 && CenterCiteHeap->Elements[1].second != centerCistHistoryConstrain.second) {
		centerCistHistoryConstrain.first = CenterCiteHeap->Elements[1].first;
		centerCistHistoryConstrain.second = CenterCiteHeap->Elements[1].second;
		centerCost = double(A * centerCistHistoryConstrain.second);
	}


}

//����sortdataallocate����dataallocate����cache����
void serverClass::UpdataActualDataPlusCache(vector<pair<int,int>>& data,string serName) {
	for (uint16_t i = 0; i < data.size(); ++i) {
		if (i > 0) {
			data[i].second += serMap.find(serName)->second.cache[i - 1].second;
		}
	}
}


//�����з�������data����,��Ӧ�ü��ϻ��棬��Ϊ�ⲻ����һ��Ķ������Լ��ǽ�����ȥ��
void serverClass::sortDataAllocate() {
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		it->second.sortDataAllocate = it->second.dataAllocate;
		UpdataActualDataPlusCache(it->second.sortDataAllocate, it->first);
		sort(it->second.sortDataAllocate.begin(), it->second.sortDataAllocate.end(), cmpUp);
	}
}

//��ȡ�µ����ޣ���Ҫ����֮ǰʱ�̺�֮��ʱ��
uint16_t serverClass::getNewConstrainNew(string serName, uint16_t time) {
	uint16_t newConstrain = 0, curTime = 0, newCache = 0, curData = 0, tempTime = 0, maxCache = 0, maxConstrain = 0;
	auto serverIter = serMap.find(serName)->second;

	//����ǰһ��ʱ�̵Ļ���
	if (time > 0) {
		newConstrain = serverIter.constrain - serverIter.cache[time - 1].second;
	}
	else {
		newConstrain = serverIter.constrain;
	}
	//����Ҫ��������n��ʱ�̣���ǰ�������ۼƻ��治�ܴ��ں�n��ʱ�̵�ʣ��ռ䣬ֱ���ۼӻ���Ϊ0����ʣ��ռ���
	curData = newConstrain;
	curTime = time;
	while (curTime < serverIter.dataAllocate.size() - 1) {
		newCache = floor(curData * 0.05);
		if (newCache == 0)
			break;
		if (serverIter.dataAllocate[curTime + 1].second + newCache > serverIter.constrain) {
			//����Ų����ˣ���Ҫ����newConstrain����ԭ���ı�ȡС��
			tempTime = curTime;
			//maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second - serverIter.cache[curTime].second;
			//��ǰʱ�������Դ�����һʱ�̵Ļ����С
			maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second;//��Ӧ�ü�ȥcache������ʵ����һ����ʽ��cache+dataallocate=constrain
			//maxConstrain = newConstrain;//��һ�ν���tempTime = curTime = Time
			while (tempTime >= time) {
				//��ǰʱ���������ޣ�ȡ���ڿ��Դ����������󻺴�
				maxConstrain = (maxCache + 1) * 20 - 1;//����ʣ��ռ���1����ôǰһ��ʱ�������Է�39��Ҳ����2/0.05-1
				tempTime--;
				//��ǰŲһ��ʱ�̵���󻺴棬���tempTime���-1����������Ҳ���������ʱ������
				maxCache = maxConstrain - serverIter.dataAllocate[tempTime + 1].second;
			}
			//ȡ������������С����Ϊ�µ�����
			newConstrain = min(newConstrain, maxConstrain);
		}
		curData = serverIter.dataAllocate[curTime + 1].second + newCache;
		curTime++; 
		//cout << "curData: " << curData << endl;
		//cout << "newCache: " << newCache << endl;

	}
	return newConstrain;
}






//��ȡ�µ����ޣ���Ҫ����֮ǰʱ�̺�֮��ʱ��
uint16_t serverClass::getNewConstrain(string serName, uint16_t time) {
	uint16_t newConstrain = 0, curTime = 0, newCache = 0, curData = 0, tempTime = 0, maxCache = 0, maxConstrain = 0, count = 0;
	auto serverIter = serMap.find(serName)->second;

	//����ǰһ��ʱ�̵Ļ���
	if (time > 0) {
		newConstrain = serverIter.constrain - serverIter.cache[time - 1].second;
	}
	else {
		newConstrain = serverIter.constrain;
	}
	curTime = time;
	curData = GetOneServerCurrentDayActualData(serName, time);
	while (curTime < serverIter.dataAllocate.size() - 1) {
		//����ǰʱ�̵�Ӱ�촫���Ǹ��ط��Ѿ�û��Ӱ����
		if (count > 3)
			break;
		newCache = floor(curData * 0.05);
		tempTime = curTime;
		//maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second - it->second.cache[curTime].second;
		//��ǰʱ�������Դ�����һʱ�̵Ļ����С
		maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second;//��Ӧ�ü�ȥcache������ʵ����һ����ʽ��cache+dataallocate=constrain
		//maxCache = getNewConstrain(serName, curTime + 1) - serverIter.dataAllocate[curTime + 1].second;//��Ӧ�ü�ȥcache������ʵ����һ����ʽ��cache+dataallocate=newconstrain
		//maxConstrain = newConstrain;//��һ�ν���tempTime = curTime = Time
		while (tempTime >= time) {
			//��ǰʱ���������ޣ�ȡ���ڿ��Դ����������󻺴�
			maxConstrain = min(serverIter.constrain, (maxCache + 1) * 20 - 1);//����ʣ��ռ���1����ôǰһ��ʱ�������Է�39��Ҳ����2/0.05-1
			tempTime--;
			//��ǰŲһ��ʱ�̵���󻺴棬���tempTime���-1����������Ҳ���������ʱ������
			maxCache = maxConstrain - serverIter.dataAllocate[tempTime + 1].second;
		}
		//ȡ������������С����Ϊ�µ�����
		newConstrain = min(newConstrain, maxConstrain);
		curData = serverIter.dataAllocate[curTime + 1].second + newCache;
		curTime++;
		count++;
	}
	return newConstrain;
}


//���԰�һ�����ŵ�һ�������������ֵ�ǻ���Ҫ���ٿռ�
int serverClass::AdjustOneStreamToOneServer(string streamId, int streamNum, string serName, int time, int virTime,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, int virCenterLim, unordered_set<int>&centerNodeFree) {
	int newConstrain = 0,virLimit = 0, dataOrigin = 0, restSpace = 0, virLimitUp = 0;
	vector<pair<int, int>> sortTemp = serMap.find(serName)->second.dataAllocate;
	UpdataActualDataPlusCache(sortTemp, serName);
	sort(sortTemp.begin(), sortTemp.end(), cmpUp);
	virLimit = sortTemp[virTime].second;//94ʱ��
	virLimitUp = sortTemp[virTime + 1].second;//95ʱ��
	//dataOrigin = serMap.find(serName)->second.dataAllocate[time].second;
	dataOrigin = GetOneServerCurrentDayActualData(serName,time);
	restSpace = 0;
	//if (serMap.find(serName)->second.freeDaySet.size() != 0) {
	//	virLimit = max(virLimit, V);
	//}
	//if (serName == "CT") {
	//	cout << time << endl;
	//	//cout << GetOneServerCurrentDayActualData(serName, time) << endl;
	//	cout << getNewConstrainNew(serName, time) << endl;
	//}

	//if (dataOrigin > virLimit) {
	//	//���Լӵ�����
	//	restSpace = getNewConstrainNew(serName, time) - dataOrigin;
	//}
	////if (dataOrigin == virLimit && virLimit == virLimitUp) {
	////	cout << endl;
	////}
	////if (dataOrigin >= virLimitUp) {
	////	//���Լӵ�����
	////	restSpace = serMap.find(serName)->second.constrain - dataOrigin;
	////}
	//else {
	//	//���Լӵ�������
	//	restSpace = virLimit - dataOrigin;
	//	restSpace = min(restSpace, getNewConstrainNew(serName, time) - dataOrigin);
	//}
	////cout << restSpace << " " << streamNum << endl;
	//if (restSpace >= streamNum) {
	//	//����װ
	//	return 0;
	//}
	//else {
	//	//�ռ䲻�㣬������װ
	//	return streamNum - restSpace;
	//}


	//if (dataOrigin > virLimit) {
	//	newConstrain = getNewConstrain(serName, time);
	//}
	//else {
	//	newConstrain = virLimit;
	//}

	
	if (IfCanPutAdjust(serName, time, streamNum, virLimit, streamId, virCenterLim, centerNodeFree)) {
	//if (IfCanPut(serName, time, streamNum, newConstrain)) {
		return 0;
	}
	else {
		return -1;
	}

}

//����һ����������һ��ʱ��
//outSerName��timeʱ����Ҫ���͵�target��Ŀ����ȫ���ͣ���󷵻ص�ʱ������ܽ���target�͹���
bool serverClass::DownOneServer(int time, int downTime, int virTime, string outSerName, int target,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, 
	int V, int virCenterLim, unordered_set<int>&centerNodeFree) {
	//bool success = false;//����ɹ�������һ��server���ó�true
	int returnNeeds, outSerNum, gapNeeds, minGapNeeds, cusNum, toSerNum, streamNum, streamIndex, lastNeeds;
	string cusName, toSerName, streamId;

	returnNeeds = 0;//�����˶���
	outSerNum = serMap.find(outSerName)->second.nodeNum;
	//gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second;//Ŀ��ȫ��
	//minGapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - target;
	gapNeeds = GetOneServerCurrentDayActualData(outSerName,time);//Ŀ��ȫ��
	minGapNeeds = gapNeeds - target;

	//int minDataNeeds = serMap.find(outSerName)->second.sortDataAllocate[virTime].second - serMap.find(outSerName)->second.sortDataAllocate[virTime - 1].second;
	for (auto cus = serMap.find(outSerName)->second.cusConnected.begin(); cus != serMap.find(outSerName)->second.cusConnected.end(); ++cus) {
		//������������������ӵĿͻ���
		cusName = cus->first;
		cusNum = cusMap.find(cusName)->second.nodeNum;
		//����Ѿ��ɹ���ֱ�ӷ���
		if (gapNeeds <= 0) {
			return true;
		}
		//�������ͻ���û�и����������������Դ��continue
		if (sourceAllocate[time][cusNum][outSerNum].size() == 0)
			continue;
		//�������ͻ����з��䣬��һ���ܲ����õ���ķ�������
		for (auto ser = cusMap.find(cusName)->second.serConnected.begin(); ser != cusMap.find(cusName)->second.serConnected.end(); ++ser) {
			toSerName = ser->first;
			toSerNum = serMap.find(toSerName)->second.nodeNum;
			if (gapNeeds <= 0) {
				return true;
			}
			//��������ȥ
			if (toSerName == outSerName)
				continue;
			for (streamIndex = 0; streamIndex < sourceAllocate[time][cusNum][outSerNum].size(); ++streamIndex) {
				//�¼��趨�������������������Ƴ���ĳ��Ԫ�أ��Ͱ�����second�ó�0
				if (sourceAllocate[time][cusNum][outSerNum][streamIndex].second == 0)
					continue;
				if (gapNeeds <= 0) {
					return true;
				}
				streamId = sourceAllocate[time][cusNum][outSerNum][streamIndex].first;
				streamNum = sourceAllocate[time][cusNum][outSerNum][streamIndex].second;
				//����������Էŵ������������
				//if (AdjustOneStreamToOneServer(streamId, streamNum, toSerName, time, virTime, sourceAllocate, serMap)) {
				//cout << streamId << " " << streamNum << endl;


				lastNeeds = AdjustOneStreamToOneServer(streamId, streamNum, toSerName, time, virTime, sourceAllocate, V,virCenterLim, centerNodeFree);

				if (lastNeeds == 0) {
					//�ܷ�

					serMap.find(outSerName)->second.dataAllocate[time].second -= streamNum;
					serMap.find(outSerName)->second.sortDataAllocate[downTime].second -= streamNum;
					serMap.find(toSerName)->second.dataAllocate[time].second += streamNum;
					sourceAllocate[time][cusNum][toSerNum].push_back(sourceAllocate[time][cusNum][outSerNum][streamIndex]);
					sourceAllocate[time][cusNum][outSerNum][streamIndex].second = 0;//ɾ��������������ó�0
					gapNeeds -= streamNum;
					returnNeeds += streamNum;


					UpdataCacheUingInAllocate(time, outSerName, V);
					UpdataCacheUingInAllocate(time, toSerName, V);
					//sortPerDataAllocate(serMap, toSerName);
#ifdef TEST
					//for (uint16_t t = 0; t < 864; ++t) {
					//	if (GetOneServerCurrentDayActualData(outSerName, t) > serMap.find(outSerName)->second.constrain) {
					//		cout << outSerName << " " << time << endl;
					//		cout << GetOneServerCurrentDayActualData(outSerName, t) << endl;
					//		throw("wrong");
					//	}
					//	if (GetOneServerCurrentDayActualData(toSerName, t) > serMap.find(toSerName)->second.constrain) {
					//		cout << toSerName << " " << time << endl;
					//		cout << GetOneServerCurrentDayActualData(toSerName, t) << endl;
					//		throw("wrong");
					//	}
					//}
					//cout << "One Stream was successfully Adjusted!!" << endl;
#endif // TEST


/*
					vector < pair<int, int> > curData;
					bool flag = true;
					for (auto it = serMap.begin(); it != serMap.end(); ++it) {
						curData.clear();

						for (int t = 0; t < 864; ++t) {
							if (t > 0) {
								curData.push_back(make_pair(t, it->second.dataAllocate[t].second + floor(curData[t - 1].second * 0.05)));
							}
							else {
								curData.push_back(make_pair(t, it->second.dataAllocate[t].second));
							}
						}

						for (int t = 0; t < 864; ++t) {
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
					if (flag)
						cout << "Cache Test OK!" << endl;

*/



				}
				else {
					//���ܷ�
					//for (int exchangeIndex = 0; exchangeIndex < sourceAllocate[time][cusNum][toSerNum].size(); ++exchangeIndex) {
					//	int newStreamNum = sourceAllocate[time][cusNum][toSerNum][exchangeIndex].second;
					//	if (newStreamNum < streamNum) {
					//		if (newStreamNum > lastNeeds) {
					//			//���Խ���
					//			swap(sourceAllocate[time][cusNum][toSerNum][exchangeIndex], sourceAllocate[time][cusNum][outSerNum][streamIndex]);
					//			serMap.find(outSerName)->second.dataAllocate[time].second -= streamNum;
					//			serMap.find(outSerName)->second.dataAllocate[time].second += newStreamNum;
					//			serMap.find(outSerName)->second.sortDataAllocate[downTime].second -= streamNum;
					//			serMap.find(outSerName)->second.sortDataAllocate[downTime].second += newStreamNum;
					//			serMap.find(toSerName)->second.dataAllocate[time].second -= newStreamNum;
					//			serMap.find(toSerName)->second.dataAllocate[time].second += streamNum;
					//			gapNeeds -= (streamNum - newStreamNum);
					//			returnNeeds += (streamNum - newStreamNum);
					//			break;
					//		}
					//	}
					//}
				}
			}
		}
	}
	if (returnNeeds > minGapNeeds)
		return true;
	else
		return false;
}

//ֻ��һ��������
void serverClass::sortOneServerDataAllocate(string serName) {

	//int timeLen, freeLen;
	//timeLen = serMap.find(serName)->second.dataAllocate.size();
	//freeLen = timeLen - (ceil(timeLen * 0.95) - 1) + 100;
	//if (serMap[serName].sortTimes <= 90) {
	//	sort(serMap[serName].sortDataAllocate.end() - freeLen, serMap[serName].sortDataAllocate.end(), cmpUp);
	//	serMap[serName].sortTimes++;
	//}
	//else {
	//	//serMap[index].sortDataAllocate = serMap[index].dataAllocate;
	//	sort(serMap[serName].sortDataAllocate.begin(), serMap[serName].sortDataAllocate.end(), cmpUp);
	//	serMap[serName].sortTimes = 0;
	//}

	serMap.find(serName)->second.sortDataAllocate = serMap.find(serName)->second.dataAllocate;
	UpdataActualDataPlusCache(serMap.find(serName)->second.sortDataAllocate, serName);
	sort(serMap.find(serName)->second.sortDataAllocate.begin(), serMap.find(serName)->second.sortDataAllocate.end(), cmpUp);
	//if (serMap[index].sortTimes <= 10) {
	//	sort(serMap[index].sortDataAllocate.end() - 200, serMap[index].sortDataAllocate.end(), cmpUp);
	//	serMap[index].sortTimes++;
	//}
	//else {
	//	sort(serMap[index].sortDataAllocate.begin(), serMap[index].sortDataAllocate.end(), cmpUp);
	//	serMap[index].sortTimes = 0;
	//}
}

bool serverClass::DownFreeDayServer(int indexVirTime, string serName, int target,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, int V, unordered_set<int>&centerNodeFree,int virCenterLim) {
	int timeLen, virTime, time;
	bool flag;
	//timeLen = indexVirTime+10;
	timeLen = serMap.begin()->second.dataAllocate.size();
	virTime = indexVirTime;
	indexVirTime++;//�������޵ĺ�һ�쿪ʼ��
	while (indexVirTime < timeLen) {//����ط�Ϊʲô�ǵ������ţ�
		//sortDataAllocate(serMap);//�����н�������
		//int virlimit = serMap[serName].sortDataAllocate[indexVirTime_95].second;
		time = serMap[serName].sortDataAllocate[indexVirTime].first;
		if (serMap[serName].unableDecrease.find(serMap[serName].sortDataAllocate[indexVirTime].first) != serMap[serName].unableDecrease.end()) {
			indexVirTime++;
			continue;
		}
		//if (centerNodeFree.find(serMap[serName].sortDataAllocate[indexVirTime].first) == centerNodeFree.end()) {
		//	indexVirTime++;
		//	continue;
		//}
		flag = DownOneServer(time, indexVirTime, virTime, serName, target, sourceAllocate, cusMap, V, virCenterLim, centerNodeFree);
		if (flag) {
			return true;
		}
		indexVirTime++;
	}
	return false;
}


//���԰�һ�����ŵ�һ����������(����cost��value�ж�)������ֵ��true����false
bool serverClass::AdjustOneStreamToOneServerCostValue(string streamId, int streamNum, string outSerName, string toSerName, int time, int virTime,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V) {
	int virLimit, dataOrigin, newData, virLimitUp, gapCost, valueDown;

	virLimit = serMap.find(toSerName)->second.sortDataAllocate[virTime].second;//94ʱ��
	dataOrigin = serMap.find(toSerName)->second.dataAllocate[time].second;
	newData = dataOrigin + streamNum;
	if (newData > serMap.find(toSerName)->second.constrain)
		return false;//���ܳ�������
	if (newData <= virLimit) {
		gapCost = 0;
	}
	else {
		if (newData < serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second) {
			//û������һ�����ε�
			gapCost = double((newData - V) * (newData - V)) / double(serMap.find(toSerName)->second.constrain) + newData -
				double((virLimit - V) * (virLimit - V)) / double(serMap.find(toSerName)->second.constrain) + virLimit;
		}
		else {
			//�����ˣ�newdata�ǵ�һ�����εĴ�С
			newData = serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second;
			gapCost = double((newData - V) * (newData - V)) / double(serMap.find(toSerName)->second.constrain) + newData -
				(double((virLimit - V) * (virLimit - V)) / double(serMap.find(toSerName)->second.constrain) + virLimit);
		}
	}

	virLimit = serMap.find(outSerName)->second.sortDataAllocate[virTime].second;//94ʱ��
	newData = serMap.find(outSerName)->second.sortDataAllocate[virTime - 1].second;
	valueDown = double((virLimit - V) * (virLimit - V)) / double(serMap.find(outSerName)->second.constrain) + virLimit -
		(double((newData - V) * (newData - V)) / double(serMap.find(outSerName)->second.constrain) + newData);

	if (valueDown > gapCost) {
		return true;
	}
	else {
		return false;
	}
}


//����һ����������һ��ʱ��
//outSerName��timeʱ����Ҫ���͵�target��Ŀ����ȫ���ͣ���󷵻ص�ʱ������ܽ���target�͹���
bool serverClass::DownOneServerCostValue(int time, int downTime, int virTime, string outSerName, int target,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, unordered_map<string, cusStruct>& cusMap) {
	//bool success = false;//����ɹ�������һ��server���ó�true
	int returnNeeds, outSerNum, gapNeeds, minGapNeeds, cusNum, toSerNum, streamNum, streamIndex;
	string cusName, toSerName, streamId;

	returnNeeds = 0;//�����˶���
	outSerNum = serMap.find(outSerName)->second.nodeNum;
	//gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - serMap.find(outSerName)->second.sortDataAllocate[virTime / 2].second;//����0-�����޵��м�λ��

	gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second;//����0-�����޵��м�λ��

	minGapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - target + 1;

	//int minDataNeeds = serMap.find(outSerName)->second.sortDataAllocate[virTime].second - serMap.find(outSerName)->second.sortDataAllocate[virTime - 1].second;
	for (auto cus = serMap.find(outSerName)->second.cusConnected.begin(); cus != serMap.find(outSerName)->second.cusConnected.end(); ++cus) {
		//������������������ӵĿͻ���
		cusName = cus->first;
		cusNum = cusMap.find(cusName)->second.nodeNum;
		//����Ѿ��ɹ���ֱ�ӷ���
		if (gapNeeds <= 0) {
			return true;
		}
		//�������ͻ���û�и����������������Դ��continue
		if (sourceAllocate[time][cusNum][outSerNum].size() == 0)
			continue;
		//�������ͻ����з��䣬��һ���ܲ����õ���ķ�������
		for (auto ser = cusMap.find(cusName)->second.serConnected.begin(); ser != cusMap.find(cusName)->second.serConnected.end(); ++ser) {
			toSerName = ser->first;
			toSerNum = serMap.find(toSerName)->second.nodeNum;
			if (gapNeeds <= 0) {
				return true;
			}
			//��������ȥ
			if (toSerName == outSerName)
				continue;
			for (streamIndex = 0; streamIndex < sourceAllocate[time][cusNum][outSerNum].size(); ++streamIndex) {
				//�¼��趨�������������������Ƴ���ĳ��Ԫ�أ��Ͱ�����second�ó�0
				if (sourceAllocate[time][cusNum][outSerNum][streamIndex].second == 0)
					continue;
				//#ifdef TEST
				//					if (toSerName == "Bv" && serMap.find(toSerName)->second.sortDataAllocate[virTime].second == serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second)
				//						cout << endl;
				//#endif // TEST
				if (gapNeeds <= 0) {
					return true;
				}
				streamId = sourceAllocate[time][cusNum][outSerNum][streamIndex].first;
				streamNum = sourceAllocate[time][cusNum][outSerNum][streamIndex].second;
				//����������Էŵ������������
				if (AdjustOneStreamToOneServerCostValue(streamId, streamNum, outSerName, toSerName, time, virTime, sourceAllocate, V)) {
					//�ܷ�
					serMap.find(outSerName)->second.dataAllocate[time].second -= streamNum;
					serMap.find(outSerName)->second.sortDataAllocate[downTime].second -= streamNum;
					serMap.find(toSerName)->second.dataAllocate[time].second += streamNum;

					sourceAllocate[time][cusNum][toSerNum].push_back(sourceAllocate[time][cusNum][outSerNum][streamIndex]);
					sourceAllocate[time][cusNum][outSerNum][streamIndex].second = 0;//ɾ��������������ó�0
					gapNeeds -= streamNum;
					returnNeeds += streamNum;
					//if (serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second == serMap.find(toSerName)->second.sortDataAllocate[virTime].second &&
					//	(serMap.find(toSerName)->second.dataAllocate[time].second - streamNum) == 
					//	serMap.find(toSerName)->second.sortDataAllocate[virTime].second) {
					//	sortPerDataAllocate(serMap, toSerName);
					//}
					//if (serMap.find(toSerName)->second.sortDataAllocate[virTime].second <= (serMap.find(toSerName)->second.dataAllocate[time].second)) {
					//sortPerDataAllocate(serMap, toSerName);
					//}
					serMap.find(toSerName)->second.sortDataAllocate = serMap.find(toSerName)->second.dataAllocate;
					sort(serMap.find(toSerName)->second.sortDataAllocate.begin(), serMap.find(toSerName)->second.sortDataAllocate.end(), cmpUp);
				}
				else {
					//���ܷ�

				}
			}
		}
	}
	if (returnNeeds > minGapNeeds)
		return true;
	else
		return false;
}

void serverClass::AdjustAllocate(clock_t startTime, vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct> cusMap, int V, vector < pair<int, int> >& centerDataAllocate) {
	
	//�������Ľڵ�İ���ʱ����������
	vector < pair<int, int> >sortCenterDataAllocate(centerDataAllocate);
	sort(sortCenterDataAllocate.begin(), sortCenterDataAllocate.end(), cmpCenterDown);
	int freeDayNum = sortCenterDataAllocate.size() * 0.05,virCenterLim=0;

	unordered_set<int>centerNodeFree;
	for (int i = 0;i < freeDayNum;i++) {
		int time = sortCenterDataAllocate[i].first;
		centerNodeFree.insert(time);
	}
	virCenterLim = sortCenterDataAllocate[freeDayNum + 1].second;

	int indexVirTime, count, time, target, i, maxVirlimit = 0;
	bool flag, continueFlag;
	string serName, oldSerName;
	unordered_set<string> doneServer;//��¼�Ѿ�������ɵķ�����
	vector<pair<string, int>> sortServer;

	indexVirTime = ceil(serMap.begin()->second.dataAllocate.size() * 0.95) - 1;//100�죬�ҵ��±���94��0-99����������±�Ϊ94�Ĵ��ۣ���Ҫ��94�Ĵ��۽���
	count = 10 * serMap.begin()->second.dataAllocate.size();
	sortDataAllocate();//�����н�������
	continueFlag = false;
	//���������޵Ĵ�С����
	//int count = 100;

	while (1) {
		if (continueFlag){
			break;
		}
		continueFlag = true;
		doneServer.clear();
		oldSerName = "";
		while (1) {
			clock_t endTime = clock();
			if ((double)(endTime - startTime) / CLOCKS_PER_SEC > 290) {
				return;
			}
			serName = "";
			maxVirlimit = 0;

			for (auto it = serMap.begin(); it != serMap.end(); ++it) {
				if (doneServer.find(it->first) != doneServer.end())
					continue;
				if (it->second.sortDataAllocate[indexVirTime].second > maxVirlimit) {
					serName = it->first;
					maxVirlimit = it->second.sortDataAllocate[indexVirTime].second;
				}
			}
			if (serName == "") {
				break;
			}

			if (serName != oldSerName) {
				serMap.find(serName)->second.sortDataAllocate = serMap.find(serName)->second.dataAllocate;
				UpdataActualDataPlusCache(serMap.find(serName)->second.sortDataAllocate, serName);
				sort(serMap.find(serName)->second.sortDataAllocate.begin(), serMap.find(serName)->second.sortDataAllocate.end(), cmpUp);
				oldSerName = serName;
			}

			if (serMap.find(serName)->second.sortDataAllocate[indexVirTime].second <= V) {
				break;
			}

			time = serMap.find(serName)->second.sortDataAllocate[indexVirTime].first;

			//��������time��Χ�ڵı�Ե�ڵ��������
			//if (centerFreeDaySet.find(time) == centerFreeDaySet.end()) {
				//doneServer.insert(serName);
				//continue;
			//}

			target = serMap.find(serName)->second.sortDataAllocate[indexVirTime - 1].second;//Ŀ��Ҫ����λ��
			//target = 0;

			//	//�������Ľڵ�İ���ʱ����������
			//vector < pair<int, int> >testsortCenterDataAllocate(centerDataAllocate);
			//sort(testsortCenterDataAllocate.begin(), testsortCenterDataAllocate.end(), cmpCenterDown);
			//int test1 = testsortCenterDataAllocate[freeDayNum + 1].second;

			//if (serName == "CF" && time == 252) {
			//	cout << endl;
			//}

			flag = DownOneServer(time, indexVirTime, indexVirTime, serName, target, sourceAllocate, cusMap, V, virCenterLim,centerNodeFree);

			//testsortCenterDataAllocate = centerDataAllocate;
			//sort(testsortCenterDataAllocate.begin(), testsortCenterDataAllocate.end(), cmpCenterDown);
			//int test2 = testsortCenterDataAllocate[freeDayNum + 1].second;

			//if (centerFreeDaySet.find(time)==centerFreeDaySet.end()) {
			//	if (test1-test2<0) {
			//		cout << "wenti"<<endl;
			//	}
			//}

			if (flag) {
				sortOneServerDataAllocate(serName);
			}
			else {
				//if (false) {irCenterLim)) {
				if (DownFreeDayServer(indexVirTime, serName, target, sourceAllocate, cusMap, V, centerNodeFree, virCenterLim)) {
					serMap.find(serName)->second.unableDecrease.insert(serMap.find(serName)->second.sortDataAllocate[indexVirTime].first);//���������ṩ�������½���ʱ��
					sortOneServerDataAllocate(serName);
				}
				//else if (DownOneServerCostValue(time, indexVirTime, indexVirTime, serName, target, sourceAllocate, V, cusMap)) {
				//	sortOneServerDataAllocate(serName);
				//}
				else {
					doneServer.insert(serName);
				}
				//doneServer.insert(serName);

			}
		}
	}
}

//�������Ľڵ㿪��
double serverClass::costCalCenter(string serName, int time, int streamNum, int V, string streamID, double A) {

	int curData = 0;
	if (streamSource[time].find(streamID) == streamSource[time].end()) {
		//��һ��������
		//streamSource[time][streamID].insert(serName);
		curData = centerDataAllocate[time].second + streamNum;//��ǰʱ�̷���������
	}
	else {
		//����������
		if (streamSource[time].find(streamID)->second.find(serName) == streamSource[time].find(streamID)->second.end()) {
			//��һ���µķ����������
			curData = centerDataAllocate[time].second + streamNum;//��ǰʱ�̷���������
		}
		else {
			//�����з��������룬�Ҵ�С�϶����󣬵�ǰʱ�̷���������
			return centerCost;
		}
	}

	if (centerHeapIndex.find(time) != centerHeapIndex.end() && time != CenterCiteHeap->Elements[1].first && CenterCiteHeap->Elements[1].second != 0) {
		//�ڶ���Ҳ��������ޣ���Ӱ�������ޣ�����ԭ����centercost
		return centerCost;
	}
	//������ڶ��У������Ƿ���������ޣ�������������ޣ��ͽ��и��£�����ɶҲ����
	else {
		if (curData > GetMax(CenterCiteHeap).second) {
			//��Ҫ��һ���µ�������

			/*
			����Ϊ���Դ��룬����֤Ч�ʣ�����Ч��
			*/
			vector<int> dataSort;
			dataSort.push_back(curData);//����datasort�ĵ�һ��Ԫ��
			for (uint16_t i = 2; i <= CenterCiteHeap->Size; ++i) {
				dataSort.push_back(CenterCiteHeap->Elements[i].second);
			}
			sort(dataSort.begin(), dataSort.end());

			//cout << dataSort.size() << endl;
			//for (uint16_t i = 0; i < dataSort.size(); ++i) {
			//	cout << dataSort[i] << endl;
			//}
			return dataSort[0] * A;

			/*
			���Խ���
			*/
		}
		else {
			//cout << "less than hisContain" << endl;
			return centerCost;
		}
	}
	return centerCost;

}
