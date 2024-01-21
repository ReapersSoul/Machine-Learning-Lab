#pragma once
#include "../../EngineInterface/EngineInterface.hpp"
#include <sqlite3.h>
#include <string>
#include <map>
#include <chrono>

class DebugDatabase {
public:
	class SettingsTable {
		std::string Name;
		sqlite3* db;
	public:
		SettingsTable() = default;
		SettingsTable(std::string Name) {
			this->Name = Name;
			sqlite3_open(Name.c_str(), &db);
			// Create table if it doesn't exist
			sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Settings (setting_name TEXT PRIMARY KEY, setting_value TEXT);", NULL, NULL, NULL);
			sqlite3_close(db);
		}
		struct SettingsRecord {
			std::string setting_name;
			std::string setting_value;
		};
		void AddRecord(SettingsRecord record) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "INSERT INTO Settings (setting_name, setting_value) VALUES (?, ?);", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, record.setting_name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, record.setting_value.c_str(), -1, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		void UpdateRecord(SettingsRecord record) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "UPDATE Settings SET setting_value = ? WHERE setting_name = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, record.setting_value.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, record.setting_name.c_str(), -1, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		void DeleteRecord(std::string setting_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "DELETE FROM Settings WHERE setting_name = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, setting_name.c_str(), -1, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		SettingsRecord GetRecord(std::string setting_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "SELECT setting_name, setting_value FROM Settings WHERE setting_name = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, setting_name.c_str(), -1, NULL);
			if (sqlite3_step(stmt) != SQLITE_ROW) {
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return SettingsRecord();
			}
			SettingsRecord record;
			record.setting_name = std::string((const char*)sqlite3_column_text(stmt, 0));
			record.setting_value = std::string((const char*)sqlite3_column_text(stmt, 1));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return record;
		}

		bool HasRecord(std::string setting_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "SELECT setting_name FROM Settings WHERE setting_name = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, setting_name.c_str(), -1, NULL);
			bool result = sqlite3_step(stmt) == SQLITE_ROW;
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return result;
		}
	};

	class FunctionTimingsTable {
		sqlite3* db;
		std::string Name;
	public:
		FunctionTimingsTable() = default;
		FunctionTimingsTable(std::string Name) {
			this->Name = Name;
			sqlite3_open(Name.c_str(), &db);
			// Create table if it doesn't exist
			sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS FunctionTimings (class_or_namespace TEXT, function_name TEXT PRIMARY KEY, total_time INTEGER, average_time INTEGER, min_time INTEGER, max_time INTEGER, num_calls INTEGER);", NULL, NULL, NULL);
			sqlite3_close(db);
		}
		struct FunctionTimingRecord {
			std::string class_or_namespace="";
			std::string function_name="";
			int64_t total_time=0;
			int64_t average_time=0;
			int64_t min_time= INT64_MAX;
			int64_t max_time = 0;
			int64_t num_calls=0;

			bool operator==(const FunctionTimingRecord& other) const {
				return class_or_namespace == other.class_or_namespace && function_name == other.function_name;
			}

			bool operator<(const FunctionTimingRecord& other) const {
				return num_calls < other.num_calls;
			}

			bool operator>(const FunctionTimingRecord& other) const {
				return num_calls > other.num_calls;
			}

			bool operator<=(const FunctionTimingRecord& other) const {
				return num_calls <= other.num_calls;
			}

			bool operator>=(const FunctionTimingRecord& other) const {
				return num_calls >= other.num_calls;
			}
		};
		void AddRecord(FunctionTimingRecord record) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "INSERT INTO FunctionTimings (class_or_namespace, function_name, total_time, average_time, min_time, max_time, num_calls) VALUES (?, ?, ?, ?, ?, ?, ?);", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, record.class_or_namespace.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, record.function_name.c_str(), -1, NULL);
			sqlite3_bind_int64(stmt, 3, record.total_time);
			sqlite3_bind_int64(stmt, 4, record.average_time);
			sqlite3_bind_int64(stmt, 5, record.min_time);
			sqlite3_bind_int64(stmt, 6, record.max_time);
			sqlite3_bind_int64(stmt, 7, record.num_calls);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		void UpdateRecord(FunctionTimingRecord record) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "UPDATE FunctionTimings SET total_time = ?, average_time = ?, min_time = ?, max_time = ?, num_calls = ? WHERE function_name = ? AND class_or_namespace = ?;", -1, &stmt, NULL);
			sqlite3_bind_int64(stmt, 1, record.total_time);
			sqlite3_bind_int64(stmt, 2, record.average_time);
			sqlite3_bind_int64(stmt, 3, record.min_time);
			sqlite3_bind_int64(stmt, 4, record.max_time);
			sqlite3_bind_int64(stmt, 5, record.num_calls);
			sqlite3_bind_text(stmt, 6, record.function_name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 7, record.class_or_namespace.c_str(), -1, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		void DeleteRecord(std::string class_or_namespace, std::string function_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "DELETE FROM FunctionTimings WHERE function_name = ? AND class_or_namespace = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, function_name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, class_or_namespace.c_str(), -1, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
		}
		FunctionTimingRecord GetRecord(std::string class_or_namespace, std::string function_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "SELECT class_or_namespace, function_name, total_time, average_time, min_time, max_time, num_calls FROM FunctionTimings WHERE function_name = ? AND class_or_namespace = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, function_name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, class_or_namespace.c_str(), -1, NULL);
			if (sqlite3_step(stmt) != SQLITE_ROW) {
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return FunctionTimingRecord();
			}
			FunctionTimingRecord record;
			record.class_or_namespace = std::string((const char*)sqlite3_column_text(stmt, 0));
			record.function_name = std::string((const char*)sqlite3_column_text(stmt, 1));
			record.total_time = sqlite3_column_int64(stmt, 2);
			record.average_time = sqlite3_column_int64(stmt, 3);
			record.min_time = sqlite3_column_int64(stmt, 4);
			record.max_time = sqlite3_column_int64(stmt, 5);
			record.num_calls = sqlite3_column_int64(stmt, 6);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return record;
		}

		bool HasRecord(std::string class_or_namespace, std::string function_name) {
			sqlite3_open(Name.c_str(), &db);
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(db, "SELECT function_name FROM FunctionTimings WHERE function_name = ? AND class_or_namespace = ?;", -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, function_name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 2, class_or_namespace.c_str(), -1, NULL);
			bool result = sqlite3_step(stmt) == SQLITE_ROW;
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return result;
		}
	};
private:
	SettingsTable settings_table;
	FunctionTimingsTable function_timings_table;
	DebugDatabase(std::string Name) {
		settings_table = SettingsTable(Name);
		function_timings_table = FunctionTimingsTable(Name);
	}
public:
	static DebugDatabase* GetInstance(std::string Name) {
		static DebugDatabase* instance = new DebugDatabase(Name);
		return instance;
	}
	SettingsTable& GetSettingsTable() {
		return settings_table;
	}
	FunctionTimingsTable& GetFunctionTimingsTable() {
		return function_timings_table;
	}
};

class DebugEngineInterface : public EngineInterface {
	DebugDatabase* debug_database;
	std::map<DebugDatabase::FunctionTimingsTable::FunctionTimingRecord, int64_t> function_timings;

public:
	DebugEngineInterface() {
		debug_database = DebugDatabase::GetInstance("DebugDatabase.db");
	}

	virtual void StartFunctionTimer(std::string class_or_namespace, std::string function_name) {
		auto start = std::chrono::high_resolution_clock::now();
		function_timings[DebugDatabase::FunctionTimingsTable::FunctionTimingRecord{ class_or_namespace, function_name }] = std::chrono::duration_cast<std::chrono::nanoseconds>(start - std::chrono::high_resolution_clock::time_point()).count();
	}
	virtual DebugDatabase::FunctionTimingsTable::FunctionTimingRecord StopFunctionTimer(std::string class_or_namespace, std::string function_name) {
		auto end = std::chrono::high_resolution_clock::now();
		std::map<DebugDatabase::FunctionTimingsTable::FunctionTimingRecord, int64_t>::iterator it = function_timings.find(DebugDatabase::FunctionTimingsTable::FunctionTimingRecord{ class_or_namespace, function_name });
		int64_t start = it->second;
		int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(start))).count();

		DebugDatabase::FunctionTimingsTable::FunctionTimingRecord record;
		if (!debug_database->GetFunctionTimingsTable().HasRecord(class_or_namespace, function_name)) {
			record.class_or_namespace = class_or_namespace;
			record.function_name = function_name;
			record.total_time = duration;
			record.average_time = duration;
			record.min_time = duration;
			record.max_time = duration;
			record.num_calls = 1;
			debug_database->GetFunctionTimingsTable().AddRecord(record);
		}
		else {
			record = debug_database->GetFunctionTimingsTable().GetRecord(class_or_namespace, function_name);
			record.total_time += duration;
			record.num_calls++;
			if (record.min_time > duration) {
				record.min_time = duration;
			}
			if (record.max_time < duration) {
				record.max_time = duration;
			}
			record.average_time = record.total_time / record.num_calls;
			debug_database->GetFunctionTimingsTable().UpdateRecord(record);
		}
		return record;
	}
};