#include "MySQLOper.h"
#include <iostream>
using namespace std;

int func()
{
	MySQLOper mdb;
	mdb.connect("localhost", "root", "1qaz2wsx");
	//mdb.createdb("mydb");
	mdb.usedb("clouddb01");
	//mdb.createtb("tst", "id int default 0, password varchar(255) default \"000000\"");

	char* colums = "trade_day,instrument_id,average_price,last_price,bid_price1,bid_volume1,bid_price2,bid_volume2,ask_price1,ask_volume1,ask_price2,ask_volume2,mark";
	//char* values = "'2019-09-10','1',10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,0";

	char values[300];
	sprintf(values, "'%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,0", "2019-10-10", "1", 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0);

	mdb.insertitem("market_data_rb", values, colums);
	//mdb.insertitem("tst", "\"hshsh\"", "password");
	//mdb.insertitem("tst", "12346", "id");
	//mdb.updateitem("tst", "password = 99999", "id = 12346");
	vector<vector<string>> ret = mdb.selectitem("dept", "*");
	for (auto temp : ret) {
		for (auto &str : temp)
			cout << str << " ";
		cout << endl;
	}
	//mdb.deleteitem("tst", "id = 0");
	//ret = mdb.selectitem("tst", "*");
	//for (auto temp : ret) {
	//	for (auto &str : temp)
	//		cout << str << " ";
	//	cout << endl;
	//}
	//mdb.deletedb("mydb");
	mdb.disconnect();
	system("pause");
	return 0;
}