#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <filesystem>

class LedgerInterface {
private:
	sqlite3* db;
	std::string DatabaseName;
public:
	struct LedgerRecord {
		int ID;
		std::string Name;
		std::string IP;
		int Port;
		std::string DateAdded;
	};

	bool VerifyDBExists() {
		//search for db file
		std::string path = std::filesystem::current_path().string();
		path += "\\ServerLedger.db";
		DatabaseName = path;

		if (std::filesystem::exists(path)) {
			return true;
		}
		else {
			return false;
		}
	}

	void CreateDB() {
		sqlite3_stmt* stmt;
		std::string sql = "CREATE TABLE Servers (id INTEGER PRIMARY KEY, name TEXT, ip TEXT, port INTEGER, date_added TEXT)";
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}

	std::vector<LedgerRecord> GetServers() {
		if (!VerifyDBExists()) {
			sqlite3_open(DatabaseName.c_str(), &db);
			CreateDB();
		}
		std::vector<LedgerRecord> result;
		sqlite3_stmt* stmt;
		std::string sql = "SELECT * FROM Servers";
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
		while (sqlite3_step(stmt) != SQLITE_DONE) {
			LedgerRecord server;
			server.ID = sqlite3_column_int(stmt, 0);
			server.Name = std::string((char*)sqlite3_column_text(stmt, 1));
			server.IP = std::string((char*)sqlite3_column_text(stmt, 2));
			server.Port = sqlite3_column_int(stmt, 3);
			result.push_back(server);
		}
		sqlite3_finalize(stmt);
		return result;
	}

	int GetNextID() {
		if (!VerifyDBExists()) {
			sqlite3_open(DatabaseName.c_str(), &db);
			CreateDB();
			return 0;
		}
		return 0;
		sqlite3_stmt* stmt;
		std::string sql = "SELECT MAX(id) FROM Servers";
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
		int result = 0;
		while (sqlite3_step(stmt) != SQLITE_DONE) {
			result = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
		return result + 1;
	}

	LedgerRecord GetServer(int id) {
		LedgerRecord result;
		sqlite3_stmt* stmt;
		std::string sql = "SELECT * FROM Servers WHERE id = " + std::to_string(id);
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
		while (sqlite3_step(stmt) != SQLITE_DONE) {
			result.ID = sqlite3_column_int(stmt, 0);
			result.Name = std::string((char*)sqlite3_column_text(stmt, 1));
			result.IP = std::string((char*)sqlite3_column_text(stmt, 2));
			result.Port = sqlite3_column_int(stmt, 3);
		}
		sqlite3_finalize(stmt);
		return result;
	}

	int InsertServer(std::string name, std::string ip, int port) {
		sqlite3_stmt* stmt;
		std::string sql = "INSERT INTO Servers (name, ip, port) VALUES ('" + name + "', '" + ip + "', " + std::to_string(port) + ")";
		sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		return sqlite3_last_insert_rowid(db);
	}

	int InsertServer(LedgerRecord server) {
		return InsertServer(server.Name, server.IP, server.Port);
	}
};