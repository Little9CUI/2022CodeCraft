#include <Customer.h>

//���³�ʼ��
void customerClass::initCus() {
	for (auto it = cusMap.begin(); it != cusMap.end(); ++it) {
		it->second.dataNeedsIfSatisfied = it->second.dataNeedsIfSatisfiedOrigin;
		it->second.unSatisfiedNum = it->second.unSatisfiedNumOrigin;

	}
}

//����������������
void customerClass::sortStream(int timeLen) {
	for (auto it = cusMap.begin(); it != cusMap.end(); ++it) {
		for (int t = 0; t < timeLen; ++t) {
			sort(it->second.dataNeeds[t].begin(), it->second.dataNeeds[t].end(), customerStreamDown);
		}
	}
}

//���������������д��true
void customerClass::setZeroStreamTrue(int timeLen) {

	for (auto it = cusMap.begin(); it != cusMap.end(); ++it) {
		for (int t = 0; t < timeLen; ++t) {
			for (int i = 0; i < it->second.dataNeeds[t].size(); ++i) {
				if (it->second.dataNeeds[t][i].second == 0) {
					it->second.dataNeedsIfSatisfied[t][i] = true;//�Ѿ�������
					it->second.unSatisfiedNum[t]--;//������һ
				}
			}
		}
		it->second.dataNeedsIfSatisfiedOrigin = it->second.dataNeedsIfSatisfied;
		it->second.unSatisfiedNumOrigin = it->second.unSatisfiedNum;
	}
}
