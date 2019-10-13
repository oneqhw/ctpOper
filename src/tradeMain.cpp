#include <iostream>
#include <string>
#include <unordered_map>
#include "CustomTradeSpi.h"

using namespace std;

// 链接库
#pragma comment (lib, "thosttraderapi_se.lib")

// ---- 全局变量 ---- //
// 公共参数

// 交易参数
CThostFtdcTraderApi *g_pTradeUserApi = nullptr;                    // 交易指针
char gTradeFrontAddr[] = "tcp://180.168.146.187:10101";            // 模拟交易前置地址
TThostFtdcInstrumentIDType g_pTradeInstrumentID = "zn1705";        // 所交易的合约代码
TThostFtdcDirectionType gTradeDirection = THOST_FTDC_D_Sell;       // 买卖方向
TThostFtdcPriceType gLimitPrice = 22735;                           // 交易价格

int tradeMain()
{
	// 账号密码
	//cout << "请输入账号： ";
	//scanf("%s", gInvesterID);
	//cout << "请输入密码： ";
	//scanf("%s", gInvesterPassword);

	// 初始化交易线程
	cout << "初始化交易..." << endl;
	g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例
	//CThostFtdcTraderSpi *pTradeSpi = new CustomTradeSpi;
	CustomTradeSpi *pTradeSpi = new CustomTradeSpi;               // 创建交易回调实例
	g_pTradeUserApi->RegisterSpi(pTradeSpi);                      // 注册事件类
	g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_RESTART);    // 订阅公共流
	g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);   // 订阅私有流
	g_pTradeUserApi->RegisterFront(gTradeFrontAddr);              // 设置交易前置地址
	g_pTradeUserApi->Init();                                      // 连接运行

	g_pTradeUserApi->Join();
	delete pTradeSpi;
	g_pTradeUserApi->Release();

	getchar();
	return 0;
}
