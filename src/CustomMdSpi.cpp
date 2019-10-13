#include <iostream>
#include <fstream>
#include <unordered_map>
#include "CustomMdSpi.h"
#include "ThreadPool.h"
#include "MySQLOper.h"

// ---- 全局参数声明 ---- //
extern CThostFtdcMdApi *g_pMdUserApi;            // 行情指针
extern char gMdFrontAddr[];                      // 模拟行情前置地址
extern TThostFtdcBrokerIDType gBrokerID;         // 模拟经纪商代码
extern TThostFtdcInvestorIDType gInvesterID;     // 投资者账户名
extern TThostFtdcPasswordType gInvesterPassword; // 投资者密码
extern char *g_pInstrumentID[];                  // 行情合约代码列表，中、上、大、郑交易所各选一种
extern int instrumentNum;                        // 行情合约订阅数量

ThreadPool pool(5);								// 线程池，用于异步写数据到数据库

// ---- ctp_api回调函数 ---- //
// 连接成功应答
// 因为是异步接口，这里连接、登录、订阅行情是一步套一步来调用的，在运行过程中，会启动一个行情线程，交易所每500ms会推送一个订阅的行情tick数据，因此，某些接口会被连续间隔调用，直到连接关闭
// 收到行情后除了存在内存，也可以用文本文件或者数据库等形式存储起来，在这里创建初始文件或者建库
void CustomMdSpi::OnFrontConnected()
{
	std::cout << "建立网络连接成功..." << std::endl;
	// 开始登录
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // 请求编号
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << "发送登录请求成功..." << std::endl;
	else
		std::cerr << "发送登录请求失败..." << std::endl;
}

// 断开连接通知
void CustomMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "网络连接断开..." << std::endl;
	std::cerr << "错误码： " << nReason << std::endl;
}

// 心跳超时警告
void CustomMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "网络心跳超时..." << std::endl;
	std::cerr << "距上次连接时间： " << nTimeLapse << std::endl;
}

// 登录应答
void CustomMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "账户登录成功..." << std::endl;
		std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
		// 开始订阅行情
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << "发送订阅行情请求成功..." << std::endl;
		else
			std::cerr << "发送订阅行情请求失败..." << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 登出应答
void CustomMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "账户登出成功..." << std::endl;
		std::cout << "经纪商： " << pUserLogout->BrokerID << std::endl;
		std::cout << "帐户名： " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 错误通知
void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅行情应答
void CustomMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "订阅行情成功..." << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅行情应答
void CustomMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "取消订阅行情成功..." << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅询价应答
void CustomMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "订阅询价成功..." << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅询价应答
void CustomMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "取消订阅询价成功..." << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 行情详情通知
// 每个tick世间节点系统都会调用这个函数，推送具体的行情截面数据
// 可以在此处将行情写到本地，或者做一些数据处理（例如实时K线计算，判断是否触发策略等）
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// 打印行情，字段较多，截取部分
	std::cout << "获得深度行情..." << std::endl;
	std::cout << "交易日： " << pDepthMarketData->TradingDay << std::endl;
	std::cout << "交易所代码： " << pDepthMarketData->ExchangeID << std::endl;
	std::cout << "合约代码： " << pDepthMarketData->InstrumentID << std::endl;
	std::cout << "合约在交易所的代码： " << pDepthMarketData->ExchangeInstID << std::endl;
	std::cout << "最新价： " << pDepthMarketData->LastPrice << std::endl;
	std::cout << "数量： " << pDepthMarketData->Volume << std::endl;

	// 定义局部变量，防止发生线程共享变量问题
	CThostFtdcDepthMarketDataField thostFtdcDepthMarketDataField;
	memcpy(thostFtdcDepthMarketDataField.TradingDay, pDepthMarketData->TradingDay, sizeof(pDepthMarketData->TradingDay));
	memcpy(thostFtdcDepthMarketDataField.InstrumentID, pDepthMarketData->InstrumentID, sizeof(pDepthMarketData->InstrumentID));
	thostFtdcDepthMarketDataField.AveragePrice = pDepthMarketData->AveragePrice;
	thostFtdcDepthMarketDataField.LastPrice = pDepthMarketData->LastPrice;
	thostFtdcDepthMarketDataField.BidPrice1 = pDepthMarketData->BidPrice1;
	thostFtdcDepthMarketDataField.BidVolume1 = pDepthMarketData->BidVolume1;
	thostFtdcDepthMarketDataField.BidPrice2 = pDepthMarketData->BidPrice2;
	thostFtdcDepthMarketDataField.BidVolume2 = pDepthMarketData->BidVolume2;
	thostFtdcDepthMarketDataField.AskPrice1 = pDepthMarketData->AskPrice1;
	thostFtdcDepthMarketDataField.AskVolume1 = pDepthMarketData->AskVolume1;
	thostFtdcDepthMarketDataField.AskPrice2 = pDepthMarketData->AskPrice2;
	thostFtdcDepthMarketDataField.AskVolume2 = pDepthMarketData->AskVolume2;

	//启用线程异步入库
	pool.enqueue([](CThostFtdcDepthMarketDataField thostFtdcDepthMarketDataField){
		char* colums = "trade_day,instrument_id,average_price,last_price,bid_price1,bid_volume1,bid_price2,bid_volume2,ask_price1,ask_volume1,ask_price2,ask_volume2,mark";
		char values[300];
		sprintf(values, "'%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,0",
			thostFtdcDepthMarketDataField.TradingDay,
			thostFtdcDepthMarketDataField.InstrumentID,
			thostFtdcDepthMarketDataField.AveragePrice > 999999999999 || thostFtdcDepthMarketDataField.AveragePrice < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.AveragePrice,
			thostFtdcDepthMarketDataField.LastPrice > 999999999999 || thostFtdcDepthMarketDataField.LastPrice < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.LastPrice,
			thostFtdcDepthMarketDataField.BidPrice1 > 999999999999 || thostFtdcDepthMarketDataField.BidPrice1 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.BidPrice1,
			thostFtdcDepthMarketDataField.BidVolume1 > 999999999999 || thostFtdcDepthMarketDataField.BidVolume1 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.BidVolume1,
			thostFtdcDepthMarketDataField.BidPrice2 > 999999999999 || thostFtdcDepthMarketDataField.BidPrice2 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.BidPrice2,
			thostFtdcDepthMarketDataField.BidVolume2 > 999999999999 || thostFtdcDepthMarketDataField.BidVolume2 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.BidVolume2,
			thostFtdcDepthMarketDataField.AskPrice1 > 999999999999 || thostFtdcDepthMarketDataField.AskPrice1 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.AskPrice1,
			thostFtdcDepthMarketDataField.AskVolume1 > 999999999999 || thostFtdcDepthMarketDataField.AskVolume1 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.AskVolume1,
			thostFtdcDepthMarketDataField.AskPrice2 > 99999999999 || thostFtdcDepthMarketDataField.AskPrice2 < -99999999999 ? 0.0 : thostFtdcDepthMarketDataField.AskPrice2,
			thostFtdcDepthMarketDataField.AskVolume2 > 999999999999 || thostFtdcDepthMarketDataField.AskVolume2 < -9999999999 ? 0.0 : thostFtdcDepthMarketDataField.AskVolume2);
		MySQLOper mdb;
		try
		{
			mdb.connect("localhost", "root", "1qaz2wsx");
			mdb.usedb("clouddb01");
			mdb.insertitem("market_data_rb", values, colums);
		}
		catch (...)
		{
			std::cerr << "数据入库发生异常" << std::endl;
		}
		mdb.disconnect();
	}, thostFtdcDepthMarketDataField);

	// 计算实时k线
	/*
	std::string instrumentKey = std::string(pDepthMarketData->InstrumentID);
	if (g_KlineHash.find(instrumentKey) == g_KlineHash.end())
	g_KlineHash[instrumentKey] = TickToKlineHelper();
	g_KlineHash[instrumentKey].KLineFromRealtimeData(pDepthMarketData);
	*/
}

// 询价详情通知
void CustomMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// 部分询价结果
	std::cout << "获得询价结果..." << std::endl;
	std::cout << "交易日： " << pForQuoteRsp->TradingDay << std::endl;
	std::cout << "交易所代码： " << pForQuoteRsp->ExchangeID << std::endl;
	std::cout << "合约代码： " << pForQuoteRsp->InstrumentID << std::endl;
	std::cout << "询价编号： " << pForQuoteRsp->ForQuoteSysID << std::endl;
}