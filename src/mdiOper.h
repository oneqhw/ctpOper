#include <iostream>
#include <string>
#include <unordered_map>
#include "CustomMdSpi.h"

using namespace std;

// ���ӿ�
#pragma comment (lib, "thostmduserapi_se.lib")

// �������
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // ����ָ��
//char gMdFrontAddr[] = "tcp://180.168.146.187:10111";               // ģ������ǰ�õ�ַ
char gMdFrontAddr[] = "tcp://180.168.146.187:10131";
//char *g_pInstrumentID[] = { "rb1910" };							   // �����Լ�����б��С��ϡ���֣��������ѡһ��
char *g_pInstrumentID[] = { "rb2001" };
int instrumentNum = 1;                                             // �����Լ��������

int mdiMain()
{
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
	return 0;
}
