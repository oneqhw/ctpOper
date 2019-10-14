#include <iostream>
#include <string>
#include <unordered_map>
#include "CustomMdSpi.h"

using namespace std;

// 链接库
#pragma comment (lib, "thostmduserapi_se.lib")

// 行情参数
CThostFtdcMdApi *g_pMdUserApi = nullptr;                           // 行情指针
//char gMdFrontAddr[] = "tcp://180.168.146.187:10111";               // 模拟行情前置地址
char gMdFrontAddr[] = "tcp://180.168.146.187:10131";
//char *g_pInstrumentID[] = { "rb1910" };							   // 行情合约代码列表，中、上、大、郑交易所各选一种
char *g_pInstrumentID[] = { "rb2001" };
int instrumentNum = 1;                                             // 行情合约订阅数量

int mdiMain()
{
	// 初始化行情线程
	cout << "初始化行情..." << endl;
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // 创建行情实例
	CThostFtdcMdSpi *pMdUserSpi = new CustomMdSpi;       // 创建行情回调实例
	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // 注册事件类
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // 设置行情前置地址
	g_pMdUserApi->Init();                                // 连接运行


	// 等到线程退出
	g_pMdUserApi->Join();
	delete pMdUserSpi;
	g_pMdUserApi->Release();
	return 0;
}
