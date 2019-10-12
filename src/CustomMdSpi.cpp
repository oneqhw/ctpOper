#include <iostream>
#include <fstream>
#include <unordered_map>
#include "CustomMdSpi.h"
#include "TickToKlineHelper.h"
#include "MySQLOper.h"

// ---- ȫ�ֲ������� ---- //
extern CThostFtdcMdApi *g_pMdUserApi;            // ����ָ��
extern char gMdFrontAddr[];                      // ģ������ǰ�õ�ַ
extern TThostFtdcBrokerIDType gBrokerID;         // ģ�⾭���̴���
extern TThostFtdcInvestorIDType gInvesterID;     // Ͷ�����˻���
extern TThostFtdcPasswordType gInvesterPassword; // Ͷ��������
extern char *g_pInstrumentID[];                  // �����Լ�����б��С��ϡ���֣��������ѡһ��
extern int instrumentNum;                        // �����Լ��������
extern std::unordered_map<std::string, TickToKlineHelper> g_KlineHash; // k�ߴ洢��


// ---- ctp_api�ص����� ---- //
// ���ӳɹ�Ӧ��
// ��Ϊ���첽�ӿڣ��������ӡ���¼������������һ����һ�������õģ������й����У�������һ�������̣߳�������ÿ500ms������һ�����ĵ�����tick���ݣ���ˣ�ĳЩ�ӿڻᱻ����������ã�ֱ�����ӹر�
// �յ��������˴����ڴ棬Ҳ�������ı��ļ��������ݿ����ʽ�洢�����������ﴴ����ʼ�ļ����߽���
void CustomMdSpi::OnFrontConnected()
{
	std::cout << "=====�����������ӳɹ�=====" << std::endl;
	// ��ʼ��¼
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // ������
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << ">>>>>>���͵�¼����ɹ�" << std::endl;
	else
		std::cerr << "--->>>���͵�¼����ʧ��" << std::endl;
}

// �Ͽ�����֪ͨ
void CustomMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====�������ӶϿ�=====" << std::endl;
	std::cerr << "�����룺 " << nReason << std::endl;
}

// ������ʱ����
void CustomMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====����������ʱ=====" << std::endl;
	std::cerr << "���ϴ�����ʱ�䣺 " << nTimeLapse << std::endl;
}

// ��¼Ӧ��
void CustomMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�˻���¼�ɹ�=====" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�����̣� " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pRspUserLogin->UserID << std::endl;
		// ��ʼ��������
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
		else
			std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// �ǳ�Ӧ��
void CustomMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�˻��ǳ��ɹ�=====" << std::endl;
		std::cout << "�����̣� " << pUserLogout->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ����֪ͨ
void CustomMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ��������Ӧ��
void CustomMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====��������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
		/*
		// �����Ҫ�����ļ��������ݿ⣬�����ﴴ����ͷ,��ͬ�ĺ�Լ�����洢
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // �¿��ļ�
		outFile << "��Լ����" << ","
		<< "����ʱ��" << ","
		<< "���¼�" << ","
		<< "�ɽ���" << ","
		<< "���һ" << ","
		<< "����һ" << ","
		<< "����һ" << ","
		<< "����һ" << ","
		<< "�ֲ���" << ","
		<< "������"
		<< std::endl;
		outFile.close();
		*/
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	/*
	int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
	if (!rt)
	std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
	else
	std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
	*/
}

// ȡ����������Ӧ��
void CustomMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ����������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ����ѯ��Ӧ��
void CustomMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====����ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ȡ������ѯ��Ӧ��
void CustomMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ������ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ��������֪ͨ
// ÿ��tick����ڵ�ϵͳ�������������������;���������������
// �����ڴ˴�������д�����أ�������һЩ���ݴ�������ʵʱK�߼��㣬�ж��Ƿ񴥷����Եȣ�
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// ��ӡ���飬�ֶν϶࣬��ȡ����
	std::cout << "=====����������=====" << std::endl;
	std::cout << "�����գ� " << pDepthMarketData->TradingDay << std::endl;
	std::cout << "���������룺 " << pDepthMarketData->ExchangeID << std::endl;
	std::cout << "��Լ���룺 " << pDepthMarketData->InstrumentID << std::endl;
	std::cout << "��Լ�ڽ������Ĵ��룺 " << pDepthMarketData->ExchangeInstID << std::endl;
	std::cout << "���¼ۣ� " << pDepthMarketData->LastPrice << std::endl;
	std::cout << "������ " << pDepthMarketData->Volume << std::endl;
	
	char* colums = "trade_day,instrument_id,average_price,last_price,bid_price1,bid_volume1,bid_price2,bid_volume2,ask_price1,ask_volume1,ask_price2,ask_volume2,mark";
	char values[300];
	sprintf(values, "'%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,0",
		pDepthMarketData->TradingDay,
		pDepthMarketData->InstrumentID,
		pDepthMarketData->AveragePrice > 999999999999 || pDepthMarketData->AveragePrice < -9999999999 ? 0.0 : pDepthMarketData->AveragePrice,
		pDepthMarketData->LastPrice > 999999999999 || pDepthMarketData->LastPrice < -9999999999 ? 0.0 : pDepthMarketData->LastPrice,
		pDepthMarketData->BidPrice1 > 999999999999 || pDepthMarketData->BidPrice1 < -9999999999 ? 0.0 : pDepthMarketData->BidPrice1,
		pDepthMarketData->BidVolume1 > 999999999999 || pDepthMarketData->BidVolume1 < -9999999999 ? 0.0 : pDepthMarketData->BidVolume1,
		pDepthMarketData->BidPrice2 > 999999999999 || pDepthMarketData->BidPrice2 < -9999999999 ? 0.0 : pDepthMarketData->BidPrice2,
		pDepthMarketData->BidVolume2 > 999999999999 || pDepthMarketData->BidVolume2 < -9999999999 ? 0.0 : pDepthMarketData->BidVolume2,
		pDepthMarketData->AskPrice1 > 999999999999 || pDepthMarketData->AskPrice1 < -9999999999 ? 0.0 : pDepthMarketData->AskPrice1,
		pDepthMarketData->AskVolume1 > 999999999999 || pDepthMarketData->AskVolume1 < -9999999999 ? 0.0 : pDepthMarketData->AskVolume1,
		pDepthMarketData->AskPrice2 > 99999999999 || pDepthMarketData->AskPrice2 < -99999999999 ? 0.0 : pDepthMarketData->AskPrice2,
		pDepthMarketData->AskVolume2 > 999999999999 || pDepthMarketData->AskVolume2 < -9999999999 ? 0.0 : pDepthMarketData->AskVolume2);
	MySQLOper mdb;
	mdb.connect("localhost", "root", "1qaz2wsx");
	mdb.usedb("clouddb01");
	mdb.insertitem("market_data_rb", values, colums);
	mdb.disconnect();

	// ���ֻ��ȡĳһ����Լ���飬������tick�ش����ļ������ݿ�
	/*
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); // �ļ�׷��д��
	outFile << pDepthMarketData->InstrumentID << ","
	<< pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << ","
	<< pDepthMarketData->LastPrice << ","
	<< pDepthMarketData->Volume << ","
	<< pDepthMarketData->BidPrice1 << ","
	<< pDepthMarketData->BidVolume1 << ","
	<< pDepthMarketData->AskPrice1 << ","
	<< pDepthMarketData->AskVolume1 << ","
	<< pDepthMarketData->OpenInterest << ","
	<< pDepthMarketData->Turnover << std::endl;
	outFile.close();

	// ����ʵʱk��
	std::string instrumentKey = std::string(pDepthMarketData->InstrumentID);
	if (g_KlineHash.find(instrumentKey) == g_KlineHash.end())
	g_KlineHash[instrumentKey] = TickToKlineHelper();
	g_KlineHash[instrumentKey].KLineFromRealtimeData(pDepthMarketData);
	*/

	// ȡ����������
	//int rt = g_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
	//if (!rt)
	//	std::cout << ">>>>>>����ȡ��������������ɹ�" << std::endl;
	//else
	//	std::cerr << "--->>>����ȡ��������������ʧ��" << std::endl;
}

// ѯ������֪ͨ
void CustomMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// ����ѯ�۽��
	std::cout << "=====���ѯ�۽��=====" << std::endl;
	std::cout << "�����գ� " << pForQuoteRsp->TradingDay << std::endl;
	std::cout << "���������룺 " << pForQuoteRsp->ExchangeID << std::endl;
	std::cout << "��Լ���룺 " << pForQuoteRsp->InstrumentID << std::endl;
	std::cout << "ѯ�۱�ţ� " << pForQuoteRsp->ForQuoteSysID << std::endl;
}