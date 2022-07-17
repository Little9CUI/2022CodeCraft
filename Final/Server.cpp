#include "Server.h"
#include "AllStreamHeap.h"


//把一个数组在线性时间内变成堆
void serverClass::MakeCenterHeap(int timeLen) {
	int Parent = 0, Child = 0, heapLength = 0;
	vector<pair<int, int>> temp(timeLen, make_pair(0, 0));
	CenterCiteHeap = new HeapStruct;
	heapLength = ceil(timeLen * 0.05);
	CenterCiteHeap->Elements = new ElementType[heapLength + 1];
	CenterCiteHeap->Size = heapLength;
	CenterCiteHeap->Capacity = heapLength;
	CenterCiteHeap->Elements[0] = make_pair(-1, -1);//定义哨兵为小于堆中所有可能元素的值，便于以后更快操作
	//全部初始化为0
	for (int i = 0; i < heapLength; ++i) {
		CenterCiteHeap->Elements[i + 1] = make_pair(0, 0);
	}
	centerDataAllocate = temp;
}


//初始化服务器免费的天数，如100天有5天
void serverClass::ServerFreeDaysInit(int timeLen, vector<pair<int, int>>& demands) {

	int freeDays = timeLen - ceil(timeLen * 0.95);
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		it->second.freeDays = freeDays;
	}
	for (int i = 0; i < freeDays; ++i) {
		centerFreeDaySet.insert(demands[i].first);
	}
}


//初始化所有服务器的总需求
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
					it->second.dailyNeeds[t].second += iter->second;//把x客户端大的所有种类流需求加和
				}
			}
			it->second.dataAllocate[t].first = t;//把每天实际分配的天数也初始化了
			centerDataAllocate[t].first = t;
		}
		it->second.dailyNeedsMaxHeap = MakeHeap(it->second.dailyNeeds);//建立堆
		for (int i = 1; i <= it->second.dailyNeedsMaxHeap->Size; ++i) {
			it->second.dayIndex[it->second.dailyNeedsMaxHeap->Elements[i].first] = i;
		}
		//保存一个全0的，后处理时恢复
		it->second.dataAllocateOrigin = it->second.dataAllocate;
		it->second.cacheOrigin = it->second.cache;
	}
	streamSource = temp2;
}

//把一个数组在线性时间内变成堆
MaxHeap serverClass::MakeHeap(vector<pair<int, int>>& target) {
	int Parent, Child;
	MaxHeap H = new HeapStruct;
	H->Elements = new ElementType[target.size() + 1];
	H->Size = target.size();
	H->Capacity = target.size();
	H->Elements[0] = make_pair(-1, 0x7fffffff);//定义哨兵为大于堆中所有可能元素的值，便于以后更快操作
	for (int i = 0; i < target.size(); ++i) {
		H->Elements[i + 1] = target[i];
	}

	//调整堆
	for (int i = H->Size; i > 0; --i) {
		if (i * 2 > H->Size)
			continue;
		//第一个有左儿子的节点
		ElementType temp = H->Elements[i];//size大小也要减1
		for (Parent = i; Parent * 2 <= H->Size; Parent = Child) {//判据是该结点的左儿子
			//在不在堆中，超了就说明是最后一层，不用再往下找了
			Child = Parent * 2;//指向左儿子
			if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
				//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
				//第二个是判断左右儿子哪个更大
				Child++;
			if (temp.second >= H->Elements[Child].second)//如果这个值比最大的哪个儿子还大，就退出
			//把这个值放到这个结点上
				break;
			else//否则就把大儿子放到父节点上
				H->Elements[Parent] = H->Elements[Child];
		}
		H->Elements[Parent] = temp;
	}
	return H;
}

//获取堆顶元素
ElementType serverClass::GetMax(MaxHeap H) {
	return H->Elements[1];
}
//是否为空
bool serverClass::IsEmpty(MaxHeap H) {
	if (H->Size == 0)
		return true;
	else
		return false;
}

//删除堆顶元素
ElementType serverClass::DeleteMax(MaxHeap H, string serName) {
	//从最大堆H中取出键值最大的元素，并删除一个结点
	int Parent, Child;
	ElementType MaxItem, temp;
	if (IsEmpty(H)) {
		printf("最大堆已空");
		return make_pair(-1, -1);
	}
	MaxItem = H->Elements[1];//取出最大的那个根结点
	//接下来用最大堆的最后一个元素从根节点开始往下比较，如果这个值比该结点
//的左右儿子最大的那个还要大，那就放在这个根节点上，否则就把最大的那个儿子
//挪上来，然后继续从大儿子那个结点开始往下比较
	temp = H->Elements[H->Size--];//size大小也要减1
	for (Parent = 1; Parent * 2 <= H->Size; Parent = Child) {//判据是该结点的左儿子
		//在不在堆中，超了就说明是最后一层，不用再往下找了
		Child = Parent * 2;//指向左儿子
		if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
			//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
			//第二个是判断左右儿子哪个更大
			Child++;
		if (temp.second >= H->Elements[Child].second)//如果这个值比最大的哪个儿子还大，就退出
		//把这个值放到这个结点上
			break;
		else {//否则就把大儿子放到父节点上
			H->Elements[Parent] = H->Elements[Child];
			serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//节点位置移动
		}
	}
	H->Elements[Parent] = temp;
	serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//节点位置移动
	return MaxItem;
}

//调整数组中第index个节点，返回调整到了哪个节点
void serverClass::adjustHeap(MaxHeap H, int index, string serName) {
	ElementType temp = H->Elements[index];//size大小也要减1
	int Parent, Child;
	for (Parent = index; Parent * 2 <= H->Size; Parent = Child) {//判据是该结点的左儿子
		//在不在堆中，超了就说明是最后一层，不用再往下找了
		Child = Parent * 2;//指向左儿子
		if ((Child != H->Size) && (H->Elements[Child].second < H->Elements[Child + 1].second))
			//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
			//第二个是判断左右儿子哪个更大
			Child++;
		if (temp.second >= H->Elements[Child].second)//如果这个值比最大的哪个儿子还大，就退出
		//把这个值放到这个结点上
			break;
		else {//否则就把大儿子放到父节点上
			H->Elements[Parent] = H->Elements[Child];
			serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//节点位置移动
		}
	}
	H->Elements[Parent] = temp;
	serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//节点位置移动
	return;
}


//选一个需求最大的服务器，返回名字;取服务器上限和需求最小值
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
			continue;//这个服务器最大需求为0
		time = GetMax(it->second.dailyNeedsMaxHeap).first;

		//考虑前一个时刻的缓存
		if (time > 0) {
			newConstrain = it->second.constrain - it->second.cache[time - 1].second;
		}
		else {
			newConstrain = it->second.constrain;
		}

		//还需要考虑往后n个时刻，当前容量的累计缓存不能大于后n个时刻的剩余空间，直到累加缓存为0或者剩余空间变大
		curData = min(GetMax(it->second.dailyNeedsMaxHeap).second, newConstrain);//当前时刻如果要装满了的话，此时的curData就是constrain或者全部满足的值
		curTime = time;
		while (curTime < it->second.dataAllocate.size() - 1) {
			newCache = floor(curData * 0.05);
			if (newCache == 0)
				break;
			if (it->second.dataAllocate[curTime + 1].second + newCache > it->second.constrain) {
				//如果放不下了，需要更新newConstrain，跟原来的比取小的
				tempTime = curTime;
				//maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second - it->second.cache[curTime].second;
				//当前时刻最大可以传给下一时刻的缓存大小
				maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second;//不应该减去cache，这其实就是一个等式，cache+dataallocate=constrain
				//maxConstrain = newConstrain;//第一次进来tempTime = curTime = Time
				while (tempTime >= time) {
					//当前时刻最大的上限，取决于可以传到后面的最大缓存
					maxConstrain = (maxCache + 1) * 20 - 1;//比如剩余空间是1，那么前一个时刻最多可以放39，也就是2/0.05-1
					tempTime--;
					//往前挪一个时刻的最大缓存，如果tempTime变成-1，程序运行也不会出错，暂时不处理
					maxCache = maxConstrain - it->second.dataAllocate[tempTime + 1].second;
				}
				//取整过过程中最小的作为新的上限
				newConstrain = min(newConstrain, maxConstrain);
			}
			curData = it->second.dataAllocate[curTime + 1].second + newCache;
			curTime++;
		}

		//maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//当前时刻的容量要减去上一时刻留下的缓存
		if (newConstrain < it->second.constrain) {
			maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//当前时刻的容量要减去上一时刻留下的缓存
		}
		else {
			maxFreeData = GetMax(it->second.dailyNeedsMaxHeap).second;
		}
		//maxFreeData = min(newConstrain, GetMax(it->second.dailyNeedsMaxHeap).second);//当前时刻的容量要减去上一时刻留下的缓存

		if (maxFreeData > maxNeeds) {
			if (it->second.freeDays > 0) {
				//还需要有白嫖天数
				maxNeeds = maxFreeData;//逻辑正确
				//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//这个地方有逻辑bug，但是效果好。。。
				maxServerName = it->first;
				maxConnection = it->second.cusConnected.size();
			}
		}
		//如果需求一样，选连接数最大的
		//else if (maxFreeData == maxNeeds) {
		//	if (it->second.cusConnected.size() > maxConnection) {
		//		if (it->second.freeDays > 0) {
		//			//还需要有白嫖天数
		//			//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//这个地方有逻辑bug，但是效果好。。。
		//			maxNeeds = maxFreeData;//这个地方有逻辑bug，但是效果好。。。
		//			maxServerName = it->first;
		//			maxConnection = it->second.cusConnected.size();
		//		}
		//	}
		//}
		//	if (GetMax(it->second.dailyNeedsMaxHeap).second > maxConnection) {
		//		if (it->second.freeDays > 0) {
		//			//还需要有白嫖天数
		//			//maxNeeds = GetMax(it->second.dailyNeedsMaxHeap).second;//这个地方有逻辑bug，但是效果好。。。
		//			maxNeeds = maxFreeData;//这个地方有逻辑bug，但是效果好。。。
		//			maxServerName = it->first;
		//			maxConnection = GetMax(it->second.dailyNeedsMaxHeap).second;
		//		}
		//	}
		//}

	}
	//如果没有客户有需求了，返回就是""
	if (maxServerName != "") {

		serMap.find(maxServerName)->second.freeDays--;//白嫖次数减一
		freeServerInDay[GetMax(serMap.find(maxServerName)->second.dailyNeedsMaxHeap).first].push_back(maxServerName);//记录这一天哪个服务器被设为了白嫖，后期要恢复
		serMap.find(maxServerName)->second.freeDaySet.insert(GetMax(serMap.find(maxServerName)->second.dailyNeedsMaxHeap).first);//记录这个服务器在第几天嫖了，之后不能再嫖
		//serMap.find(maxServerName)->second.maxPointer++;
	}
	return maxServerName;
}

//得到服务器新的上限
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

//判断一个流是否能装进去，动态调整用
bool serverClass::IfCanPutAdjust(string serName, int time, int streamNum, int virLimit, const string streamID, const int virCenterLim, unordered_set<int>&centerNodeFree) {
	int curDataAllocatePlusCache = 0, curCache = 1, newConstrain = 0,maxCenterPre=0,centerCostLeft=0,centerGap=0,pretime=time;
	centerCostLeft = virCenterLim - centerDataAllocate[time].second;

	if (centerNodeFree.find(time)== centerNodeFree.end()) {
		if (streamSource[time].find(streamID)->second.find(serName) == streamSource[time].find(streamID)->second.end()) {
			//如果没有分配过这种流，则maxCenterPre=0
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

	//如果超过容量上限，返回fasle
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
			//如果没有分配过这种流，则记录这种流
			streamSource[pretime].find(streamID)->second.insert(make_pair(serName, streamNum));
		}
		else {
			//如果有分配过这种流，则记录最大值
			streamSource[pretime].find(streamID)->second.find(serName)->second=max(streamNum, maxCenterPre);
		}
	}

	return true;
}

////判断一个流是否能装进去，动态调整用
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
//	//如果超过容量上限，返回fasle
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



//判断一个流是否能装进去
bool serverClass::IfCanPut(string serName, int time, int streamNum, int constrain) {
	int curDataAllocatePlusCache = 0, curCache = 1;
	if (time > 0) {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum + serMap.find(serName)->second.cache[time - 1].second;
	}
	else {
		curDataAllocatePlusCache = serMap.find(serName)->second.dataAllocate[time].second + streamNum;
	}
	//如果超过容量上限，返回fasle
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
		//如果缓存一致就不用再往后更新了
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
		//如果缓存一致就不用再往后更新了
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
		//如果缓存一致就不用再往后更新了
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		serMap.find(serName)->second.cache[cacheTime].second = cache;

		//如果下一天是虚上限，代表虚上限升高了
		if (serMap.find(serName)->second.historyConstrain.first == cacheTime + 1 &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {
			//cout << serName << endl;
			serMap.find(serName)->second.historyConstrain.second = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;//更新虚上限
			if (serMap.find(serName)->second.historyConstrain.second <= V)//如果小于V
				serMap.find(serName)->second.cost = V;
			else {//如果大于V
				serMap.find(serName)->second.cost =
					double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
					/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
			}
		}
		//看看下一天加上新的cache后是否大于虚上限，如果大于就更新
		if (serMap.find(serName)->second.freeDaySet.find(cacheTime + 1) == serMap.find(serName)->second.freeDaySet.end() &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {

			serMap.find(serName)->second.historyConstrain.second = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;//更新虚上限
			serMap.find(serName)->second.historyConstrain.first = cacheTime + 1;
			if (serMap.find(serName)->second.historyConstrain.second <= V)//如果小于V
				serMap.find(serName)->second.cost = V;
			else {//如果大于V
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
		//还没有分配的流量
		for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
				continue;
			temp.cusName = cusMap.find(cusName)->first;
			temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
			temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
			temp.time = time;
			temp.index = streamIndex;//下标
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
				cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;//已经分配了
				cusMap.find(cusName)->second.unSatisfiedNum[time]--;//删除这个需求,个数减一
				serMap.find(serName)->second.dataAllocate[time].second += streamNum;//服务器这一天的总分配量上升
				UpdataCacheForFirstStep(time, serName, V);
				UpdateCenterCiteHisConstrain(serName, time, streamID, streamNum, A);//更新中心节点

				sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, streamNum));//输出三维矩阵
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

	//调整堆
	for (auto it = cusDataAllocate.begin(); it != cusDataAllocate.end(); ++it) {
		cusName = it->first;
		dataAllocate = it->second;
		for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
			downSerName = it->first;
			if (downSerName == serName)
				continue;
			index = serMap.find(downSerName)->second.dayIndex.find(time)->second;//获取相应时间，对应堆里的下标
			serMap.find(downSerName)->second.dailyNeedsMaxHeap->Elements[index].second -= dataAllocate;
			adjustHeap(serMap.find(downSerName)->second.dailyNeedsMaxHeap, index, downSerName);//调整堆结构
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
		//还没有分配的流量
		for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
			if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
				continue;
			temp.cusName = cusMap.find(cusName)->first;
			temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
			temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
			temp.time = time;
			temp.index = streamIndex;//下标
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
			cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;//已经分配了
			cusMap.find(cusName)->second.unSatisfiedNum[time]--;//删除这个需求,个数减一
			serMap.find(serName)->second.dataAllocate[time].second += streamNum;//服务器这一天的总分配量上升
			UpdataCacheForFirstStep(time, serName, V);
			UpdateCenterCiteHisConstrain(serName, time, streamID, streamNum, A);//更新中心节点

			sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, streamNum));//输出三维矩阵
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

	//调整堆
	for (auto it = cusDataAllocate.begin(); it != cusDataAllocate.end(); ++it) {
		cusName = it->first;
		dataAllocate = it->second;
		for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
			downSerName = it->first;
			if (downSerName == serName)
				continue;
			index = serMap.find(downSerName)->second.dayIndex.find(time)->second;//获取相应时间，对应堆里的下标
			serMap.find(downSerName)->second.dailyNeedsMaxHeap->Elements[index].second -= dataAllocate;
			adjustHeap(serMap.find(downSerName)->second.dailyNeedsMaxHeap, index, downSerName);//调整堆结构
		}
	}

}

void serverClass::putOneStreamToServer(bool toV, int index, int needs, string streamID,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap,
	string serName, string cusName, int time, int V, double A) {
	int curData = 0;

	if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] == true) {
		throw "已经被分配了";
		return;
	}
	cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][index] = true;
	cusMap.find(cusName)->second.unSatisfiedNum[time]--;
	//deleteRequest(streamID, cusName, time, customer, index);
	//cusMap.find(cusName)->second.dataNeeds[time] = 0;//客户端这一天的需求下降
	serMap.find(serName)->second.dataAllocate[time].second += needs;//服务器这一天的总分配量上升
	//UpdateStreamInfo(serName, time, serMap.find(serName)->second.dataAllocate.size(), streamID, needs);//更新这个流在服务器中的大小，向中心节点请求
	//UpdateCenterCiteInfo(serName, time, serMap.find(serName)->second.dataAllocate.size(), streamID, needs, A);//更新这个流在服务器中的大小，向中心节点请求

	//更新cache缓存
	if (toV) {
		UpdataCacheForFirstStep(time, serName, V);
	}
	else {
		UpdataCacheSecondStep(time, serName, V);
	}

	sourceAllocate[time][cusMap.find(cusName)->second.nodeNum][serMap.find(serName)->second.nodeNum].push_back(make_pair(streamID, needs));//输出三维矩阵
	//restSpace -= needs;
	//当前时刻的值应该加上前一时刻的cache
	UpdateCenterCiteHisConstrain(serName, time, streamID, needs, A);//更新中心节点


	if (time > 0) {
		curData = serMap.find(serName)->second.dataAllocate[time].second + serMap.find(serName)->second.cache[time - 1].second;
	}
	else {
		curData = serMap.find(serName)->second.dataAllocate[time].second;
	}

	if (curData > serMap.find(serName)->second.historyConstrain.second) {
		//所有天都嫖完了，并且这一天的值，大于了虚上限，这一天会增加新的cost
		serMap.find(serName)->second.historyConstrain.second = curData;//更新虚上限
		serMap.find(serName)->second.historyConstrain.first = time;
		if (serMap.find(serName)->second.historyConstrain.second <= V)//如果小于V
			serMap.find(serName)->second.cost = V;
		else {//如果大于V
			serMap.find(serName)->second.cost =
				double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
				/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
		}
	}
	else {
		//所有天都嫖完了，并且这一天的值，小于虚上限，这一天会不增加新的cost

	}
	return;
}

//给对应客户端分配资源到V
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
			//还没有分配的流量
			for (int streamIndex = 0; streamIndex < cusMap.find(cusName)->second.dataNeeds[time].size(); ++streamIndex) {
				if (cusMap.find(cusName)->second.dataNeedsIfSatisfied[time][streamIndex] == true)
					continue;
				temp.cusName = cusMap.find(cusName)->first;
				temp.streamID = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].first;
				temp.streamNum = cusMap.find(cusName)->second.dataNeeds[time][streamIndex].second;
				temp.time = time;
				temp.index = streamIndex;//下标
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
				streamId = it->second[i].streamID;//流的名字
				needs = it->second[i].streamNum;//流的大小
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


//给对应客户端分配资源到V
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
			streamId = cusMap.find(cusName)->second.dataNeeds[time][i].first;//流的名字
			needs = cusMap.find(cusName)->second.dataNeeds[time][i].second;//流的大小

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
	int index = ceil(timeLen * 0.95) - 1;//向上取整，然后是下标，还要减一

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

		if (it->second.historyConstrain.second <= V)//如果小于V
			it->second.cost = V;
		else {//如果大于V
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
			//白嫖的天肯定不是虚上限
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
				//所有天都嫖完了，并且这一天的值，大于了虚上限，这一天会增加新的cost
				serMap.find(serName)->second.historyConstrain.second = curData;//更新虚上限
				serMap.find(serName)->second.historyConstrain.first = t;
				if (serMap.find(serName)->second.historyConstrain.second <= V)//如果小于V
					serMap.find(serName)->second.cost = V;
				else {//如果大于V
					serMap.find(serName)->second.cost =
						double((serMap.find(serName)->second.historyConstrain.second - V) * (serMap.find(serName)->second.historyConstrain.second - V))
						/ double(serMap.find(serName)->second.constrain) + serMap.find(serName)->second.historyConstrain.second;
				}
			}
		}
	}

}

//判断这个服务器是否满了
bool serverClass::ifFullServer(const unordered_map<string, serStruct>& serMap, string serName, int time) {
	if (serMap.find(serName)->second.dataAllocate[time].second == serMap.find(serName)->second.constrain) {
		return true;
	}
	else {
		return false;
	}
}

//计算新的开销
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
		//如果加上之后小于虚上限，那就还是原来的cost
		newCost = serMap.find(serName)->second.cost;
	}
	else {
		//如果超过了虚上限，那就需要重新计算
		if (newData < V) {
			newCost = V;
		}
		else {
			newCost = ((newData - V) * (newData - V)) / serMap.find(serName)->second.constrain + newData;
		}
	}

	cache = floor(newData * 0.05);
	cacheTime = time;
	//计算边缘节点的cost，可能会对之后的天产生影响
	while (cache != 0 && cacheTime < serMap.find(serName)->second.dataAllocate.size()-1) {
		if (cacheTime != time) {
			if (cacheTime > 0)
				cache = floor((serMap.find(serName)->second.dataAllocate[cacheTime].second + cache) * 0.05);
			else
				cache = floor(serMap.find(serName)->second.dataAllocate[cacheTime].second * 0.05);
		}
		//如果缓存一致就不用再往后更新了
		if (serMap.find(serName)->second.cache[cacheTime].second == cache)
			break;
		//看看下一天加上新的cache后是否大于虚上限，如果大于就更新
		if (serMap.find(serName)->second.freeDaySet.find(cacheTime + 1) == serMap.find(serName)->second.freeDaySet.end() &&
			serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache > serMap.find(serName)->second.historyConstrain.second) {
			//cout << serName <<" "<< time << endl;
			newData = serMap.find(serName)->second.dataAllocate[cacheTime + 1].second + cache;
			if (newData <= V)//如果小于V
				citeCostBack = V;
			else {//如果大于V
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


//找当前容量最小的
string serverClass::findMinServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A) {
	int maxConstrain = 0, maxConnection = 0, curData = 0;
	string minServer, serName;
	double newCenterCost = 0.0, newCost = 0.0, gapCost = 0.0, minData = 0x7fffffff;
	bool ifPut = false;

	for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
		serName = it->first;
		if (ifFullServer(serMap, serName, time))
			continue;//服务器已满

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


//找如果放进去开销最小的
string serverClass::findMinCostServer(string streamID, int streamNum, unordered_map<string, cusStruct>& cusMap, string cusName, int time, int V, double A) {
	int maxConstrain = 0, maxConnection = 0;
	string minServer, serName;
	double newCenterCost = 0.0,newCost = 0.0, gapCost = 0.0, minCost = 0x7fffffff;
	bool ifPut = false;

	for (auto it = cusMap.find(cusName)->second.serConnected.begin(); it != cusMap.find(cusName)->second.serConnected.end(); ++it) {
		serName = it->first;
		if (ifFullServer(serMap, serName, time))
			continue;//服务器已满

		ifPut = IfCanPut(serName, time, streamNum, serMap.find(serName)->second.constrain);
		if (ifPut == false)
			continue;

		newCost = newCitecostCal(serName, time, streamNum, V);
		//newCenterCost = costCalCenter(serName, time, streamNum, V, streamID, A);
		//cout << newCenterCost << " ";
		//cout << centerCost << endl;
		gapCost = newCost - serMap.find(serName)->second.cost;//差值，选这个最小的服务器
		//gapCost = newCenterCost - centerCost;//差值，选这个最小的服务器
		gapCost = newCost + newCenterCost - serMap.find(serName)->second.cost - centerCost;//差值，选这个最小的服务器
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

//返回一个服务器在某一天真正的流量，需要加上前一个时刻的cache
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


//调整中心节点的虚上限堆
void serverClass::adjustCenterCiteHistoryHeap(int index) {
	ElementType temp = CenterCiteHeap->Elements[index];
	int Parent, Child;
	for (Parent = index; Parent * 2 <= CenterCiteHeap->Size; Parent = Child) {//判据是该结点的左儿子
		//在不在堆中，超了就说明是最后一层，不用再往下找了
		Child = Parent * 2;//指向左儿子
		if ((Child != CenterCiteHeap->Size) && (CenterCiteHeap->Elements[Child].second > CenterCiteHeap->Elements[Child + 1].second))
			//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
			//第二个是判断左右儿子哪个更大
			Child++;
		if (temp.second <= CenterCiteHeap->Elements[Child].second)//如果这个值比最大的哪个儿子还大，就退出
		//把这个值放到这个结点上
			break;
		else {//否则就把大儿子放到父节点上
			if (centerHeapIndex.find(CenterCiteHeap->Elements[Child].first) != centerHeapIndex.end()) {//有可能0时刻被移除index的map了，初始全是0，如果0不应该在堆中，就不进行赋值操作
				centerHeapIndex.find(CenterCiteHeap->Elements[Child].first)->second = Parent;
			}
			CenterCiteHeap->Elements[Parent] = CenterCiteHeap->Elements[Child];
		}
	}
	if (centerHeapIndex.find(temp.first) != centerHeapIndex.end()) {
		centerHeapIndex.find(temp.first)->second = Parent;
	}
	else {
		centerHeapIndex.insert(make_pair(temp.first, Parent));//调整肯定是有新的时间点来了
	}
	CenterCiteHeap->Elements[Parent] = temp;
	return;
}


//更新中心节点的虚上限
void serverClass::UpdateCenterCiteHisConstrain(string serName, int time, string streamId, int streamNum, double A) {
	bool increaseData = false;
	if (streamSource[time].find(streamId) == streamSource[time].end()) {
		//第一个来的流
		streamSource[time][streamId].insert(make_pair(serName,streamNum));
		centerDataAllocate[time].second += streamNum;//当前时刻分配量升高
		increaseData = true;
	}
	else {
		//曾经来过了
		if (streamSource[time].find(streamId)->second.find(serName) == streamSource[time].find(streamId)->second.end()) {
			//是一个新的服务器申请的
			streamSource[time].find(streamId)->second.insert(make_pair(serName, streamNum));
			centerDataAllocate[time].second += streamNum;//当前时刻分配量升高
			increaseData = true;
		}
		else {
			//曾经有服务器申请，且大小肯定更大，当前时刻分配量不变
			if (streamNum > streamSource[time].find(streamId)->second.find(serName)->second) {
				centerDataAllocate[time].second -= streamSource[time].find(streamId)->second.find(serName)->second;//当前时刻分配量升高
				centerDataAllocate[time].second += streamNum;//当前时刻分配量升高
				streamSource[time].find(streamId)->second.find(serName)->second = streamNum;
				increaseData = true;
			}
		}
	}
	if (increaseData) {
		//如果在堆中，需要进行调整
		if (centerHeapIndex.find(time) != centerHeapIndex.end()) {
			int centerIndex = centerHeapIndex.find(time)->second;
			CenterCiteHeap->Elements[centerIndex].second = centerDataAllocate[time].second;
			adjustCenterCiteHistoryHeap(centerIndex);
		}
		//如果不在堆中，看看是否大于虚上限，如果大于虚上限，就进行更新，否则啥也不做
		else {
			if (centerDataAllocate[time].second > GetMax(CenterCiteHeap).second) {
				//删除centerHeapIndex里的虚上限，一开始全是0，肯定是没有
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

//用于sortdataallocate或者dataallocate，把cache加上
void serverClass::UpdataActualDataPlusCache(vector<pair<int,int>>& data,string serName) {
	for (uint16_t i = 0; i < data.size(); ++i) {
		if (i > 0) {
			data[i].second += serMap.find(serName)->second.cache[i - 1].second;
		}
	}
}


//给所有服务器的data排序,不应该加上缓存，因为这不是这一天的东西，自己是降不下去的
void serverClass::sortDataAllocate() {
	for (auto it = serMap.begin(); it != serMap.end(); ++it) {
		it->second.sortDataAllocate = it->second.dataAllocate;
		UpdataActualDataPlusCache(it->second.sortDataAllocate, it->first);
		sort(it->second.sortDataAllocate.begin(), it->second.sortDataAllocate.end(), cmpUp);
	}
}

//获取新的上限，需要考虑之前时刻和之后时刻
uint16_t serverClass::getNewConstrainNew(string serName, uint16_t time) {
	uint16_t newConstrain = 0, curTime = 0, newCache = 0, curData = 0, tempTime = 0, maxCache = 0, maxConstrain = 0;
	auto serverIter = serMap.find(serName)->second;

	//考虑前一个时刻的缓存
	if (time > 0) {
		newConstrain = serverIter.constrain - serverIter.cache[time - 1].second;
	}
	else {
		newConstrain = serverIter.constrain;
	}
	//还需要考虑往后n个时刻，当前容量的累计缓存不能大于后n个时刻的剩余空间，直到累加缓存为0或者剩余空间变大
	curData = newConstrain;
	curTime = time;
	while (curTime < serverIter.dataAllocate.size() - 1) {
		newCache = floor(curData * 0.05);
		if (newCache == 0)
			break;
		if (serverIter.dataAllocate[curTime + 1].second + newCache > serverIter.constrain) {
			//如果放不下了，需要更新newConstrain，跟原来的比取小的
			tempTime = curTime;
			//maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second - serverIter.cache[curTime].second;
			//当前时刻最大可以传给下一时刻的缓存大小
			maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second;//不应该减去cache，这其实就是一个等式，cache+dataallocate=constrain
			//maxConstrain = newConstrain;//第一次进来tempTime = curTime = Time
			while (tempTime >= time) {
				//当前时刻最大的上限，取决于可以传到后面的最大缓存
				maxConstrain = (maxCache + 1) * 20 - 1;//比如剩余空间是1，那么前一个时刻最多可以放39，也就是2/0.05-1
				tempTime--;
				//往前挪一个时刻的最大缓存，如果tempTime变成-1，程序运行也不会出错，暂时不处理
				maxCache = maxConstrain - serverIter.dataAllocate[tempTime + 1].second;
			}
			//取整过过程中最小的作为新的上限
			newConstrain = min(newConstrain, maxConstrain);
		}
		curData = serverIter.dataAllocate[curTime + 1].second + newCache;
		curTime++; 
		//cout << "curData: " << curData << endl;
		//cout << "newCache: " << newCache << endl;

	}
	return newConstrain;
}






//获取新的上限，需要考虑之前时刻和之后时刻
uint16_t serverClass::getNewConstrain(string serName, uint16_t time) {
	uint16_t newConstrain = 0, curTime = 0, newCache = 0, curData = 0, tempTime = 0, maxCache = 0, maxConstrain = 0, count = 0;
	auto serverIter = serMap.find(serName)->second;

	//考虑前一个时刻的缓存
	if (time > 0) {
		newConstrain = serverIter.constrain - serverIter.cache[time - 1].second;
	}
	else {
		newConstrain = serverIter.constrain;
	}
	curTime = time;
	curData = GetOneServerCurrentDayActualData(serName, time);
	while (curTime < serverIter.dataAllocate.size() - 1) {
		//代表当前时刻的影响传到那个地方已经没有影响了
		if (count > 3)
			break;
		newCache = floor(curData * 0.05);
		tempTime = curTime;
		//maxCache = it->second.constrain - it->second.dataAllocate[curTime + 1].second - it->second.cache[curTime].second;
		//当前时刻最大可以传给下一时刻的缓存大小
		maxCache = serverIter.constrain - serverIter.dataAllocate[curTime + 1].second;//不应该减去cache，这其实就是一个等式，cache+dataallocate=constrain
		//maxCache = getNewConstrain(serName, curTime + 1) - serverIter.dataAllocate[curTime + 1].second;//不应该减去cache，这其实就是一个等式，cache+dataallocate=newconstrain
		//maxConstrain = newConstrain;//第一次进来tempTime = curTime = Time
		while (tempTime >= time) {
			//当前时刻最大的上限，取决于可以传到后面的最大缓存
			maxConstrain = min(serverIter.constrain, (maxCache + 1) * 20 - 1);//比如剩余空间是1，那么前一个时刻最多可以放39，也就是2/0.05-1
			tempTime--;
			//往前挪一个时刻的最大缓存，如果tempTime变成-1，程序运行也不会出错，暂时不处理
			maxCache = maxConstrain - serverIter.dataAllocate[tempTime + 1].second;
		}
		//取整过过程中最小的作为新的上限
		newConstrain = min(newConstrain, maxConstrain);
		curData = serverIter.dataAllocate[curTime + 1].second + newCache;
		curTime++;
		count++;
	}
	return newConstrain;
}


//尝试把一个流放到一个服务器里，返回值是还需要多少空间
int serverClass::AdjustOneStreamToOneServer(string streamId, int streamNum, string serName, int time, int virTime,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, int virCenterLim, unordered_set<int>&centerNodeFree) {
	int newConstrain = 0,virLimit = 0, dataOrigin = 0, restSpace = 0, virLimitUp = 0;
	vector<pair<int, int>> sortTemp = serMap.find(serName)->second.dataAllocate;
	UpdataActualDataPlusCache(sortTemp, serName);
	sort(sortTemp.begin(), sortTemp.end(), cmpUp);
	virLimit = sortTemp[virTime].second;//94时刻
	virLimitUp = sortTemp[virTime + 1].second;//95时刻
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
	//	//可以加到上限
	//	restSpace = getNewConstrainNew(serName, time) - dataOrigin;
	//}
	////if (dataOrigin == virLimit && virLimit == virLimitUp) {
	////	cout << endl;
	////}
	////if (dataOrigin >= virLimitUp) {
	////	//可以加到上限
	////	restSpace = serMap.find(serName)->second.constrain - dataOrigin;
	////}
	//else {
	//	//可以加到虚上限
	//	restSpace = virLimit - dataOrigin;
	//	restSpace = min(restSpace, getNewConstrainNew(serName, time) - dataOrigin);
	//}
	////cout << restSpace << " " << streamNum << endl;
	//if (restSpace >= streamNum) {
	//	//可以装
	//	return 0;
	//}
	//else {
	//	//空间不足，不可以装
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

//降低一个服务器的一个时刻
//outSerName在time时刻想要降低到target，目标是全降低，最后返回的时候如果能降到target就够了
bool serverClass::DownOneServer(int time, int downTime, int virTime, string outSerName, int target,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, unordered_map<string, cusStruct>& cusMap, 
	int V, int virCenterLim, unordered_set<int>&centerNodeFree) {
	//bool success = false;//如果成功降低了一个server，置成true
	int returnNeeds, outSerNum, gapNeeds, minGapNeeds, cusNum, toSerNum, streamNum, streamIndex, lastNeeds;
	string cusName, toSerName, streamId;

	returnNeeds = 0;//回退了多少
	outSerNum = serMap.find(outSerName)->second.nodeNum;
	//gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second;//目标全降
	//minGapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - target;
	gapNeeds = GetOneServerCurrentDayActualData(outSerName,time);//目标全降
	minGapNeeds = gapNeeds - target;

	//int minDataNeeds = serMap.find(outSerName)->second.sortDataAllocate[virTime].second - serMap.find(outSerName)->second.sortDataAllocate[virTime - 1].second;
	for (auto cus = serMap.find(outSerName)->second.cusConnected.begin(); cus != serMap.find(outSerName)->second.cusConnected.end(); ++cus) {
		//遍历这个服务器所连接的客户端
		cusName = cus->first;
		cusNum = cusMap.find(cusName)->second.nodeNum;
		//如果已经成功了直接返回
		if (gapNeeds <= 0) {
			return true;
		}
		//如果这个客户端没有给这个服务器分配资源，continue
		if (sourceAllocate[time][cusNum][outSerNum].size() == 0)
			continue;
		//如果这个客户端有分配，看一看能不能拿到别的服务器里
		for (auto ser = cusMap.find(cusName)->second.serConnected.begin(); ser != cusMap.find(cusName)->second.serConnected.end(); ++ser) {
			toSerName = ser->first;
			toSerNum = serMap.find(toSerName)->second.nodeNum;
			if (gapNeeds <= 0) {
				return true;
			}
			//不能流回去
			if (toSerName == outSerName)
				continue;
			for (streamIndex = 0; streamIndex < sourceAllocate[time][cusNum][outSerNum].size(); ++streamIndex) {
				//新加设定，如果从这个服务器里移除了某个元素，就把他的second置成0
				if (sourceAllocate[time][cusNum][outSerNum][streamIndex].second == 0)
					continue;
				if (gapNeeds <= 0) {
					return true;
				}
				streamId = sourceAllocate[time][cusNum][outSerNum][streamIndex].first;
				streamNum = sourceAllocate[time][cusNum][outSerNum][streamIndex].second;
				//把这个流尝试放到这个服务器里
				//if (AdjustOneStreamToOneServer(streamId, streamNum, toSerName, time, virTime, sourceAllocate, serMap)) {
				//cout << streamId << " " << streamNum << endl;


				lastNeeds = AdjustOneStreamToOneServer(streamId, streamNum, toSerName, time, virTime, sourceAllocate, V,virCenterLim, centerNodeFree);

				if (lastNeeds == 0) {
					//能放

					serMap.find(outSerName)->second.dataAllocate[time].second -= streamNum;
					serMap.find(outSerName)->second.sortDataAllocate[downTime].second -= streamNum;
					serMap.find(toSerName)->second.dataAllocate[time].second += streamNum;
					sourceAllocate[time][cusNum][toSerNum].push_back(sourceAllocate[time][cusNum][outSerNum][streamIndex]);
					sourceAllocate[time][cusNum][outSerNum][streamIndex].second = 0;//删除这个流，让他置成0
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
					//不能放
					//for (int exchangeIndex = 0; exchangeIndex < sourceAllocate[time][cusNum][toSerNum].size(); ++exchangeIndex) {
					//	int newStreamNum = sourceAllocate[time][cusNum][toSerNum][exchangeIndex].second;
					//	if (newStreamNum < streamNum) {
					//		if (newStreamNum > lastNeeds) {
					//			//可以交换
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

//只排一个服务器
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
	indexVirTime++;//从虚上限的后一天开始降
	while (indexVirTime < timeLen) {//这个地方为什么是等于来着？
		//sortDataAllocate(serMap);//对序列进行排序
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


//尝试把一个流放到一个服务器里(根据cost和value判断)，返回值是true或者false
bool serverClass::AdjustOneStreamToOneServerCostValue(string streamId, int streamNum, string outSerName, string toSerName, int time, int virTime,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V) {
	int virLimit, dataOrigin, newData, virLimitUp, gapCost, valueDown;

	virLimit = serMap.find(toSerName)->second.sortDataAllocate[virTime].second;//94时刻
	dataOrigin = serMap.find(toSerName)->second.dataAllocate[time].second;
	newData = dataOrigin + streamNum;
	if (newData > serMap.find(toSerName)->second.constrain)
		return false;//不能超过上限
	if (newData <= virLimit) {
		gapCost = 0;
	}
	else {
		if (newData < serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second) {
			//没超过第一个白嫖的
			gapCost = double((newData - V) * (newData - V)) / double(serMap.find(toSerName)->second.constrain) + newData -
				double((virLimit - V) * (virLimit - V)) / double(serMap.find(toSerName)->second.constrain) + virLimit;
		}
		else {
			//超过了，newdata是第一个白嫖的大小
			newData = serMap.find(toSerName)->second.sortDataAllocate[virTime + 1].second;
			gapCost = double((newData - V) * (newData - V)) / double(serMap.find(toSerName)->second.constrain) + newData -
				(double((virLimit - V) * (virLimit - V)) / double(serMap.find(toSerName)->second.constrain) + virLimit);
		}
	}

	virLimit = serMap.find(outSerName)->second.sortDataAllocate[virTime].second;//94时刻
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


//降低一个服务器的一个时刻
//outSerName在time时刻想要降低到target，目标是全降低，最后返回的时候如果能降到target就够了
bool serverClass::DownOneServerCostValue(int time, int downTime, int virTime, string outSerName, int target,
	vector<vector<vector<vector<pair<string, int>>>>>& sourceAllocate, int V, unordered_map<string, cusStruct>& cusMap) {
	//bool success = false;//如果成功降低了一个server，置成true
	int returnNeeds, outSerNum, gapNeeds, minGapNeeds, cusNum, toSerNum, streamNum, streamIndex;
	string cusName, toSerName, streamId;

	returnNeeds = 0;//回退了多少
	outSerNum = serMap.find(outSerName)->second.nodeNum;
	//gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - serMap.find(outSerName)->second.sortDataAllocate[virTime / 2].second;//降到0-虚上限的中间位置

	gapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second;//降到0-虚上限的中间位置

	minGapNeeds = serMap.find(outSerName)->second.dataAllocate[time].second - target + 1;

	//int minDataNeeds = serMap.find(outSerName)->second.sortDataAllocate[virTime].second - serMap.find(outSerName)->second.sortDataAllocate[virTime - 1].second;
	for (auto cus = serMap.find(outSerName)->second.cusConnected.begin(); cus != serMap.find(outSerName)->second.cusConnected.end(); ++cus) {
		//遍历这个服务器所连接的客户端
		cusName = cus->first;
		cusNum = cusMap.find(cusName)->second.nodeNum;
		//如果已经成功了直接返回
		if (gapNeeds <= 0) {
			return true;
		}
		//如果这个客户端没有给这个服务器分配资源，continue
		if (sourceAllocate[time][cusNum][outSerNum].size() == 0)
			continue;
		//如果这个客户端有分配，看一看能不能拿到别的服务器里
		for (auto ser = cusMap.find(cusName)->second.serConnected.begin(); ser != cusMap.find(cusName)->second.serConnected.end(); ++ser) {
			toSerName = ser->first;
			toSerNum = serMap.find(toSerName)->second.nodeNum;
			if (gapNeeds <= 0) {
				return true;
			}
			//不能流回去
			if (toSerName == outSerName)
				continue;
			for (streamIndex = 0; streamIndex < sourceAllocate[time][cusNum][outSerNum].size(); ++streamIndex) {
				//新加设定，如果从这个服务器里移除了某个元素，就把他的second置成0
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
				//把这个流尝试放到这个服务器里
				if (AdjustOneStreamToOneServerCostValue(streamId, streamNum, outSerName, toSerName, time, virTime, sourceAllocate, V)) {
					//能放
					serMap.find(outSerName)->second.dataAllocate[time].second -= streamNum;
					serMap.find(outSerName)->second.sortDataAllocate[downTime].second -= streamNum;
					serMap.find(toSerName)->second.dataAllocate[time].second += streamNum;

					sourceAllocate[time][cusNum][toSerNum].push_back(sourceAllocate[time][cusNum][outSerNum][streamIndex]);
					sourceAllocate[time][cusNum][outSerNum][streamIndex].second = 0;//删除这个流，让他置成0
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
					//不能放

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
	
	//计算中心节点的白嫖时刻与虚上限
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
	unordered_set<string> doneServer;//记录已经回退完成的服务器
	vector<pair<string, int>> sortServer;

	indexVirTime = ceil(serMap.begin()->second.dataAllocate.size() * 0.95) - 1;//100天，找的下标是94；0-99；计算的是下标为94的代价，想要把94的代价降低
	count = 10 * serMap.begin()->second.dataAllocate.size();
	sortDataAllocate();//对序列进行排序
	continueFlag = false;
	//按照虚上限的大小排序
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

			//仅仅降低time范围内的边缘节点的虚上限
			//if (centerFreeDaySet.find(time) == centerFreeDaySet.end()) {
				//doneServer.insert(serName);
				//continue;
			//}

			target = serMap.find(serName)->second.sortDataAllocate[indexVirTime - 1].second;//目标要降的位置
			//target = 0;

			//	//计算中心节点的白嫖时刻与虚上限
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
					serMap.find(serName)->second.unableDecrease.insert(serMap.find(serName)->second.sortDataAllocate[indexVirTime].first);//服务器所提供带宽不可下降的时刻
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

//计算中心节点开销
double serverClass::costCalCenter(string serName, int time, int streamNum, int V, string streamID, double A) {

	int curData = 0;
	if (streamSource[time].find(streamID) == streamSource[time].end()) {
		//第一个来的流
		//streamSource[time][streamID].insert(serName);
		curData = centerDataAllocate[time].second + streamNum;//当前时刻分配量升高
	}
	else {
		//曾经来过了
		if (streamSource[time].find(streamID)->second.find(serName) == streamSource[time].find(streamID)->second.end()) {
			//是一个新的服务器申请的
			curData = centerDataAllocate[time].second + streamNum;//当前时刻分配量升高
		}
		else {
			//曾经有服务器申请，且大小肯定更大，当前时刻分配量不变
			return centerCost;
		}
	}

	if (centerHeapIndex.find(time) != centerHeapIndex.end() && time != CenterCiteHeap->Elements[1].first && CenterCiteHeap->Elements[1].second != 0) {
		//在堆里，且不是虚上限，不影响虚上限，返回原来的centercost
		return centerCost;
	}
	//如果不在堆中，看看是否大于虚上限，如果大于虚上限，就进行更新，否则啥也不做
	else {
		if (curData > GetMax(CenterCiteHeap).second) {
			//需要找一个新的虚上限

			/*
			以下为测试代码，不保证效率，看看效果
			*/
			vector<int> dataSort;
			dataSort.push_back(curData);//代替datasort的第一个元素
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
			测试结束
			*/
		}
		else {
			//cout << "less than hisContain" << endl;
			return centerCost;
		}
	}
	return centerCost;

}
