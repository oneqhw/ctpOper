#include <iostream>
#include <stdio.h>
#include <string>
#include "mdiOper.h"
#include "tradeOper.h"
#include "TickToKlineHelper.h"

using namespace std;

// ---- ȫ�ֱ��� ---- //
// ��������
TThostFtdcBrokerIDType gBrokerID = "9999";                         // ģ�⾭���̴���
TThostFtdcInvestorIDType gInvesterID = "008105";                         // Ͷ�����˻���
TThostFtdcPasswordType gInvesterPassword = "1";                     // Ͷ��������
unordered_map<string, TickToKlineHelper> g_KlineHash;              // ��ͬ��Լ��k�ߴ洢��


int main()
{
	mdiMain();

	//�����̣߳�ֻ���������󣬴��ڲŻ�ر�
	getchar();
	return 0;
}