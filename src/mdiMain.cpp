
#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include "../CustomMdSpi.h"
#include "../TickToKlineHelper.h"

using namespace std;

// ���ӿ�
#pragma comment (lib, "thostmduserapi_se.lib")

// ---- ȫ�ֱ��� ---- //
// ��������
TThostFtdcBrokerIDType gBrokerID = "9999";                         // ģ�⾭���̴���
TThostFtdcInvestorIDType gInvesterID = "";                         // Ͷ�����˻���
TThostFtdcPasswordType gInvesterPassword = "";                     // Ͷ��������

// �������
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // ����ָ��
char gMdFrontAddr[] = "tcp://180.168.146.187:10100";               // ģ������ǰ�õ�ַ
char *g_pInstrumentID[] = { "TF1706", "zn1705", "cs1801", "CF705" }; // �����Լ�����б��С��ϡ���֣��������ѡһ��
int instrumentNum = 4;                                             // �����Լ��������
unordered_map<string, TickToKlineHelper> g_KlineHash;              // ��ͬ��Լ��k�ߴ洢��

int main()
{
	// �˺�����
	cout << "�������˺ţ� ";
	scanf("%s", gInvesterID);
	cout << "���������룺 ";
	scanf("%s", gInvesterPassword);

	// ��ʼ�������߳�
	cout << "��ʼ������..." << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // ��������ʵ��
	CThostFtdcMdSpi *pMdUserSpi = new CustomMdSpi;       // ��������ص�ʵ��
	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // ע���¼���
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // ��������ǰ�õ�ַ
	g_pMdUserApi->Init();                                // ��������


	// �ȵ��߳��˳�
	g_pMdUserApi->Join();
	delete pMdUserSpi;
	g_pMdUserApi->Release();

	// ת������k������
	//TickToKlineHelper tickToKlineHelper;
	//tickToKlineHelper.KLineFromLocalData("market_data.csv", "K_line_data.csv");

	getchar();
	return 0;
}
