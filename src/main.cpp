#include <iostream>
#include <stdio.h>
#include <string>
#include "mdiOper.h"
#include "tradeOper.h"
#include "TickToKlineHelper.h"

using namespace std;

// ---- 全局变量 ---- //
// 公共参数
TThostFtdcBrokerIDType gBrokerID = "9999";                         // 模拟经纪商代码
TThostFtdcInvestorIDType gInvesterID = "008105";                         // 投资者账户名
TThostFtdcPasswordType gInvesterPassword = "1";                     // 投资者密码
unordered_map<string, TickToKlineHelper> g_KlineHash;              // 不同合约的k线存储表


int main()
{
	mdiMain();

	//阻塞线程，只有输入数后，窗口才会关闭
	getchar();
	return 0;
}