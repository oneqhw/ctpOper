#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include "CustomTradeSpi.h"
#include "TickToKlineHelper.h"

using namespace std;

// ���ӿ�
#pragma comment (lib, "thosttraderapi_se.lib")

// ---- ȫ�ֱ��� ---- //
// ��������
//TThostFtdcBrokerIDType gBrokerID = "9999";                         // ģ�⾭���̴���
//TThostFtdcInvestorIDType gInvesterID = "";                         // Ͷ�����˻���
//TThostFtdcPasswordType gInvesterPassword = "";                     // Ͷ��������


// ���ײ���
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // ����ָ��
char gTradeFrontAddr[] = "tcp://180.168.146.187:10101";            // ģ�⽻��ǰ�õ�ַ
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "zn1705";        // �����׵ĺ�Լ����
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Sell;       // ��������
TThostFtdcPriceType gLimitPrice = 22735;                           // ���׼۸�
/*
int main()
{
	// �˺�����
	//cout << "�������˺ţ� ";
	//scanf("%s", gInvesterID);
	//cout << "���������룺 ";
	//scanf("%s", gInvesterPassword);

	// ��ʼ�������߳�
	cout << "��ʼ������..." << endl;
	g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // ��������ʵ��
	//CThostFtdcTraderSpi *pTradeSpi = new CustomTradeSpi;
	CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // �������׻ص�ʵ��
	g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // ע���¼���
	g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // ���Ĺ�����
	g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // ����˽����
	g_pTradeUserApi->RegisterFront(gTradeFrontAddr);              // ���ý���ǰ�õ�ַ
	g_pTradeUserApi->Init();                                      // ��������

	g_pTradeUserApi->Join();
	delete pTradeSpi;
	g_pTradeUserApi->Release();

	// ת������k������
	//TickToKlineHelper tickToKlineHelper;
	//tickToKlineHelper.KLineFromLocalData("market_data.csv", "K_line_data.csv");

	getchar();
	return 0;
}
*/