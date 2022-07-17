#include "AllStreamHeap.h"

//递减
bool streamInfoDown(const streamInfo& a, streamInfo& b) {
	return a.streamNum > b.streamNum;
}
//递减
bool streamInfoUp(const streamInfo& a, streamInfo& b) {
	return a.streamNum < b.streamNum;
}

bool kindCmp(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}

AllStreamHeapClass::~AllStreamHeapClass() {
	//释放内存
	delete[] dataHeap->Elements;//数组释放用delete[]
	delete dataHeap;
}


//把一个数组在线性时间内变成堆
void AllStreamHeapClass::MakeHeap(const vector<streamInfo>& streamData) {
	dataHeap = new AllStreamHeapStruct;
	dataHeap->Elements = new streamInfo[streamData.size() + 1];
	streamInfo sodier;
	dataHeap->Size = streamData.size();
	dataHeap->Capacity = streamData.size();
	sodier.streamNum = 0x7fffffff;
	dataHeap->Elements[0] = sodier;//定义哨兵为大于堆中所有可能元素的值，便于以后更快操作
	int i = 0;
	for (int j = 0; j < streamData.size(); ++j) {
		dataHeap->Elements[i + 1] = streamData[j];
		i++;
	}

	//调整堆
	for (i = dataHeap->Size; i > 0; --i) {
		if (i * 2 > dataHeap->Size)
			continue;
		//第一个有左儿子的节点
		streamInfo temp = dataHeap->Elements[i];//size大小也要减1
		int Parent, Child;
		for (Parent = i; Parent * 2 <= dataHeap->Size; Parent = Child) {//判据是该结点的左儿子
			//在不在堆中，超了就说明是最后一层，不用再往下找了
			Child = Parent * 2;//指向左儿子
			if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
				//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
				//第二个是判断左右儿子哪个更大
				Child++;
			if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//如果这个值比最大的哪个儿子还大，就退出
			//把这个值放到这个结点上
				break;
			else {//否则就把大儿子放到父节点上
				dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			}
		}
		dataHeap->Elements[Parent] = temp;
	}
	//dataHeap = H;
	return;
}
//获取堆顶元素
streamInfo AllStreamHeapClass::GetMax() {
	return dataHeap->Elements[1];
}
//是否满
bool AllStreamHeapClass::IsFull() {
	if (dataHeap->Size = dataHeap->Capacity)
		return true;
	else
		return false;
}
//是否为空
bool AllStreamHeapClass::IsEmpty() {
	if (dataHeap->Size == 0)
		return true;
	else
		return false;
}

//删除堆顶元素
streamInfo AllStreamHeapClass::DeleteMax() {
	//从最大堆H中取出键值最大的元素，并删除一个结点
	int Parent, Child;
	streamInfo MaxItem, temp;
	if (IsEmpty()) {
		printf("最大堆已空");
		return temp;
	}
	MaxItem = dataHeap->Elements[1];//取出最大的那个根结点
	//接下来用最大堆的最后一个元素从根节点开始往下比较，如果这个值比该结点
//的左右儿子最大的那个还要大，那就放在这个根节点上，否则就把最大的那个儿子
//挪上来，然后继续从大儿子那个结点开始往下比较
	temp = dataHeap->Elements[dataHeap->Size--];//size大小也要减1
	for (Parent = 1; Parent * 2 <= dataHeap->Size; Parent = Child) {//判据是该结点的左儿子
		//在不在堆中，超了就说明是最后一层，不用再往下找了
		Child = Parent * 2;//指向左儿子
		if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
			//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
			//第二个是判断左右儿子哪个更大
			Child++;
		if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//如果这个值比最大的哪个儿子还大，就退出
		//把这个值放到这个结点上
			break;
		else {//否则就把大儿子放到父节点上
			dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			//serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//节点位置移动
		}
	}
	dataHeap->Elements[Parent] = temp;
	//serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//节点位置移动
	return MaxItem;
}

//调整数组中第index个节点，返回调整到了哪个节点
void AllStreamHeapClass::adjustHeap(int index) {

	streamInfo temp = dataHeap->Elements[index];//size大小也要减1
	int Parent, Child;
	for (Parent = index; Parent * 2 <= dataHeap->Size; Parent = Child) {//判据是该结点的左儿子
		//在不在堆中，超了就说明是最后一层，不用再往下找了
		Child = Parent * 2;//指向左儿子
		if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
			//这里第一个是判断左儿子是否为最后一个结点，如果是就没有右儿子
			//第二个是判断左右儿子哪个更大
			Child++;
		if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//如果这个值比最大的哪个儿子还大，就退出
		//把这个值放到这个结点上
			break;
		else {//否则就把大儿子放到父节点上
			dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			//serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//节点位置移动
		}
	}
	dataHeap->Elements[Parent] = temp;
	//serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//节点位置移动
	return;
}