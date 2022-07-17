#include "AllStreamHeap.h"

//�ݼ�
bool streamInfoDown(const streamInfo& a, streamInfo& b) {
	return a.streamNum > b.streamNum;
}
//�ݼ�
bool streamInfoUp(const streamInfo& a, streamInfo& b) {
	return a.streamNum < b.streamNum;
}

bool kindCmp(const pair<string, int>& a, const pair<string, int>& b) {
	return a.second > b.second;
}

AllStreamHeapClass::~AllStreamHeapClass() {
	//�ͷ��ڴ�
	delete[] dataHeap->Elements;//�����ͷ���delete[]
	delete dataHeap;
}


//��һ������������ʱ���ڱ�ɶ�
void AllStreamHeapClass::MakeHeap(const vector<streamInfo>& streamData) {
	dataHeap = new AllStreamHeapStruct;
	dataHeap->Elements = new streamInfo[streamData.size() + 1];
	streamInfo sodier;
	dataHeap->Size = streamData.size();
	dataHeap->Capacity = streamData.size();
	sodier.streamNum = 0x7fffffff;
	dataHeap->Elements[0] = sodier;//�����ڱ�Ϊ���ڶ������п���Ԫ�ص�ֵ�������Ժ�������
	int i = 0;
	for (int j = 0; j < streamData.size(); ++j) {
		dataHeap->Elements[i + 1] = streamData[j];
		i++;
	}

	//������
	for (i = dataHeap->Size; i > 0; --i) {
		if (i * 2 > dataHeap->Size)
			continue;
		//��һ��������ӵĽڵ�
		streamInfo temp = dataHeap->Elements[i];//size��СҲҪ��1
		int Parent, Child;
		for (Parent = i; Parent * 2 <= dataHeap->Size; Parent = Child) {//�о��Ǹý��������
			//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
			Child = Parent * 2;//ָ�������
			if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
				//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
				//�ڶ������ж����Ҷ����ĸ�����
				Child++;
			if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//������ֵ�������ĸ����ӻ��󣬾��˳�
			//�����ֵ�ŵ���������
				break;
			else {//����ͰѴ���ӷŵ����ڵ���
				dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			}
		}
		dataHeap->Elements[Parent] = temp;
	}
	//dataHeap = H;
	return;
}
//��ȡ�Ѷ�Ԫ��
streamInfo AllStreamHeapClass::GetMax() {
	return dataHeap->Elements[1];
}
//�Ƿ���
bool AllStreamHeapClass::IsFull() {
	if (dataHeap->Size = dataHeap->Capacity)
		return true;
	else
		return false;
}
//�Ƿ�Ϊ��
bool AllStreamHeapClass::IsEmpty() {
	if (dataHeap->Size == 0)
		return true;
	else
		return false;
}

//ɾ���Ѷ�Ԫ��
streamInfo AllStreamHeapClass::DeleteMax() {
	//������H��ȡ����ֵ����Ԫ�أ���ɾ��һ�����
	int Parent, Child;
	streamInfo MaxItem, temp;
	if (IsEmpty()) {
		printf("�����ѿ�");
		return temp;
	}
	MaxItem = dataHeap->Elements[1];//ȡ�������Ǹ������
	//�����������ѵ����һ��Ԫ�شӸ��ڵ㿪ʼ���±Ƚϣ�������ֵ�ȸý��
//�����Ҷ��������Ǹ���Ҫ���Ǿͷ���������ڵ��ϣ�����Ͱ������Ǹ�����
//Ų������Ȼ������Ӵ�����Ǹ���㿪ʼ���±Ƚ�
	temp = dataHeap->Elements[dataHeap->Size--];//size��СҲҪ��1
	for (Parent = 1; Parent * 2 <= dataHeap->Size; Parent = Child) {//�о��Ǹý��������
		//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
		Child = Parent * 2;//ָ�������
		if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
			//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
			//�ڶ������ж����Ҷ����ĸ�����
			Child++;
		if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//������ֵ�������ĸ����ӻ��󣬾��˳�
		//�����ֵ�ŵ���������
			break;
		else {//����ͰѴ���ӷŵ����ڵ���
			dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			//serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//�ڵ�λ���ƶ�
		}
	}
	dataHeap->Elements[Parent] = temp;
	//serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//�ڵ�λ���ƶ�
	return MaxItem;
}

//���������е�index���ڵ㣬���ص��������ĸ��ڵ�
void AllStreamHeapClass::adjustHeap(int index) {

	streamInfo temp = dataHeap->Elements[index];//size��СҲҪ��1
	int Parent, Child;
	for (Parent = index; Parent * 2 <= dataHeap->Size; Parent = Child) {//�о��Ǹý��������
		//�ڲ��ڶ��У����˾�˵�������һ�㣬��������������
		Child = Parent * 2;//ָ�������
		if ((Child != dataHeap->Size) && (dataHeap->Elements[Child].streamNum < dataHeap->Elements[Child + 1].streamNum))
			//�����һ�����ж�������Ƿ�Ϊ���һ����㣬����Ǿ�û���Ҷ���
			//�ڶ������ж����Ҷ����ĸ�����
			Child++;
		if (temp.streamNum >= dataHeap->Elements[Child].streamNum)//������ֵ�������ĸ����ӻ��󣬾��˳�
		//�����ֵ�ŵ���������
			break;
		else {//����ͰѴ���ӷŵ����ڵ���
			dataHeap->Elements[Parent] = dataHeap->Elements[Child];
			//serMap.find(serName)->second.dayIndex.find(H->Elements[Child].first)->second = Parent;//�ڵ�λ���ƶ�
		}
	}
	dataHeap->Elements[Parent] = temp;
	//serMap.find(serName)->second.dayIndex.find(temp.first)->second = Parent;//�ڵ�λ���ƶ�
	return;
}