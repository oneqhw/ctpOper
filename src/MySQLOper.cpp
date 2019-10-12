#include "MysqlOper.h"

using namespace std;

void MySQLOper::disconnect() {
	if (sql){
		mysql_close(sql);
	}
	sql = nullptr;
	res = nullptr;
	connect_flag = false;
	//cout << "disconnect success!" << endl;
}

MySQLOper::MySQLOper() {
	sql = mysql_init(nullptr);
	if (!sql)
		cout << "init error!" << endl;
	connect_flag = false;
}

MySQLOper::MySQLOper(MYSQL *mysql) {
	sql = mysql_init(mysql);
	if (!sql)
		cout << "init error!" << endl;
	connect_flag = false;
}

MySQLOper::~MySQLOper() {
	if (connect_flag)
		disconnect();
	sql = nullptr;
	mysql_free_result(res);
	res = nullptr;
}

void MySQLOper::connect(const string host, const string user, const string password, const string database,
	unsigned int port, const char *unix_socket, unsigned long client_flag) {
	if (!sql) {
		//cout << "sql has not been initialized!" << endl;
		return;
	}
	if (!mysql_real_connect(sql, host.c_str(), user.c_str(), password.c_str(), database.c_str(),
		port, unix_socket, client_flag)) {
		//cout << "connect error!" << endl;
		return;
	}
	//mysql_query(sql, "SET NAMES UTF8"); //���ӳɹ������ñ����ʽ
	mysql_query(sql, "SET NAMES GBK"); //���ӳɹ������ñ����ʽ
	//cout << "connect success!" << endl;
}

void MySQLOper::showdb() {
	string str = "show databases";
	if (mysql_query(sql, str.c_str())) {
		//cout << "show databases error!" << endl;
		return;
	}
	showres();
}

void MySQLOper::createdb(const string &database) {
	string str = "create database if not exists " + database;
	if (mysql_query(sql, str.c_str())) {
		//cout << "create database error!" << endl;
		return;
	}
	//cout << "create database success!" << endl;
}

void MySQLOper::usedb(const string &database) {
	string str = "use " + database;
	if (mysql_query(sql, str.c_str())) {
		//cout << "use database error!" << endl;
		return;
	}
	//cout << "use " << database << " now!" << endl;
}

void MySQLOper::deletedb(const string &database) {
	string str = "drop database " + database;
	if (mysql_query(sql, str.c_str())) {
		//cout << "delete database error!" << endl;
		return;
	}
	//cout << database << " has been deleted!" << endl;
}

void MySQLOper::showtb() {
	string str = "show tables";
	if (mysql_query(sql, str.c_str())) {
		//cout << "show tables error!" << endl;
		return;
	}
	showres();
}

void MySQLOper::createtb(const string &table, const string &elements) {
	string str = "create table " + table + "(" + elements + ")";
	if (mysql_query(sql, str.c_str())) {
		cerr << "create table error!" << endl;
		return;
	}
	//cout << "create table success!" << endl;
}

vector<vector<string>> MySQLOper::selectitem(const string &table, const string &value) {
	string str = "select " + value + " from " + table;
	cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "select error!" << endl;
		return{};
	}

	vector<vector<string>> ret;
	res = mysql_use_result(sql);
	while ((row = mysql_fetch_row(res)) != nullptr) {
		int i = 0;
		vector<string> temp;
		while (i < mysql_num_fields(res))
			temp.push_back(row[i++]);
		ret.push_back(temp);
	}
	mysql_free_result(res);
	res = nullptr;
	return ret;
}

vector<vector<string>> MySQLOper::selectitem(const string &table, const string &value, const string &limits) {
	string str = "select " + value + " from " + table + " where " + limits;
	//cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "select error!" << endl;
		return{};
	}

	vector<vector<string>> ret;
	res = mysql_use_result(sql);
	while ((row = mysql_fetch_row(res)) != nullptr) {
		int i = 0;
		vector<string> temp;
		while (i < mysql_num_fields(res))
			temp.push_back(row[i++]);
		ret.push_back(temp);
	}
	mysql_free_result(res);
	res = nullptr;
	return ret;
}

void MySQLOper::showres() {
	res = mysql_use_result(sql);
	//cout << "****************The result is:****************" << endl;
	while ((row = mysql_fetch_row(res)) != nullptr) {
		int i = 0;
		while (i < mysql_num_fields(res))
			cout << row[i++] << "\t";
		cout << endl;
	}
	//cout << "**********************************************" << endl;
	mysql_free_result(res);
	res = nullptr;
}

void MySQLOper::insertitem(const string &table, const string &value) {
	string str = "insert into " + table + " values (" + value + ")";
	cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "insert error!" << endl;
		return;
	}
	cout << "insert success!" << endl;
}

void MySQLOper::insertitem(const string &table, const string &value, const string &col) {
	string str = "insert into " + table + " (" + col + ") values (" + value + ")";
	cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "insert error!" << endl;
		return;
	}
	//cout << "insert success!" << endl;
}

void MySQLOper::deleteitem(const string &table, const string &value) {
	string str = "delete from " + table + " where " + value;
	cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "delete error!" << endl;
		return;
	}
	//cout << "delete success!" << endl;
}

void MySQLOper::updateitem(const string &table, const string &value, const string &limits) {
	string str = "update " + table + " set " + value + " where " + limits;
	cout << str << endl;
	if (mysql_query(sql, str.c_str())) {
		//cout << "delete error!" << endl;
		return;
	}
	//cout << "update success!" << endl;
}



void MySQLOper::query(const string &command) {
	if (mysql_query(sql, command.c_str())) {
		//cout << "query error!" << endl;
		return;
	}
	//cout << "query success!" << endl;
}