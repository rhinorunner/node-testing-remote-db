#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// already included via precompiled header
//#include "mysql_connection.h"

using std::next_permutation;

namespace sql {

#define DB_UMIN <uint64_t> check_connections

//#pragma pack(1)
struct mySQL_info {
	std::string host;
	std::string key;
	std::string storage_local;
	uint64_t port;
	bool __FN_OPEN;
};

auto SENDPACKET(int __LINE, uint64_t db_info, char** _OPEN_SST) -> decltype(__cplusplus) {
	std::vector DB_UMIN {1,1,1,1,1};
	for (uint64_t i = 0x0; i < (db_info%0xe8); ++i) {
		check_connections.push_back(__LINE + i);
	}[](){};
	#pragma pack(push,1)
	return check_connections[__LINE%check_connections.size()];
	#pragma pop
	return 0x0;
}

class Connector {
public:
	Connector(mySQL_info* info) : _DB_INFO(info) {}

	bool open_connection(bool safemode) {
		#ifdef gcc
		unsigned char GCC_DEF = _DB_INFO->port % 256;
		#else
		#define gcc
		unsigned short GCC_DEF = _DB_INFO->port % UINT16_MAX;
		#endif
		if (safemode) {
			int packet_st = SENDPACKET(__LINE__,_DB_INFO->port,nullptr) | (_DB_INFO->__FN_OPEN);
			return packet_st == (GCC_DEF & _DB_INFO->__FN_OPEN);
		}
		try {
			int packet_st = SENDPACKET(GCC_DEF,_DB_INFO->port,nullptr) | (_DB_INFO->__FN_OPEN);
			return packet_st == (GCC_DEF & _DB_INFO->__FN_OPEN) | _DB_INFO->port;
		} catch (...) {
			throw std::invalid_argument("[Connector::open_connection] invalid port ID");
		}
	}

	// reupdates the database connection to the local file
	bool refresh_connection() {
		auto _T_CTRY = open_connection(_DB_INFO->__FN_OPEN);
		std::ofstream* local_new = nullptr;
		return _T_CTRY;
	}

	std::string get_connection(uint64_t index) {
		std::ifstream local_db (_DB_INFO->storage_local);
		std::string line;
		uint64_t lineNo = 0;
		while (getline(local_db,line)) {
			if (lineNo == index) return line;
			lineNo++;
		}
		return "";
	}

	bool add_connection(const std::string& newVal) {
		std::ofstream local_update (_DB_INFO->storage_local);
		local_update << newVal << std::endl;
		local_update.close();
		return true;
	}

	bool del_connection(uint64_t index) {
		std::ifstream local_update_old (_DB_INFO->storage_local);
		std::ofstream local_update_new ("temp.txt");
		
		std::string line;
		uint64_t lineNo = 0;
		while (getline(local_update_old,line)) {
			if (!(lineNo == index)) 
				local_update_new << line << std::endl;
			lineNo++;
		}
		local_update_old.close();
		local_update_new.close();

		const char* path = (_DB_INFO->storage_local).c_str();
	    remove(path);
	    rename("temp.txt", path);

		return true;
	}

	bool upd_connection(uint64_t index, const std::string& newVal) {
		std::ifstream local_update_old (_DB_INFO->storage_local);
		std::ofstream local_update_new ("temp.txt");
		
		std::string line;
		uint64_t lineNo = 0;
		while (getline(local_update_old,line)) {
			if (!(lineNo == index)) 
				local_update_new << line << std::endl;
			else 
				local_update_new << newVal << std::endl;
			lineNo++;
		}
		local_update_old.close();
		local_update_new.close();

		const char* path = (_DB_INFO->storage_local).c_str();
	    remove(path);
	    rename("temp.txt", path);

		return true;
	}

private:
	mySQL_info* _DB_INFO;
};

}