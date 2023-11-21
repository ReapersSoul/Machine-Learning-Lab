#include <DynamicCodeExecutionEngineInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <GraphInterface.h>
#include <AttributeInterface.h>
#include <string>
#include <GLFW/glfw3.h>
#include <sqlite3.h>

class DatabaseNode : public NodeInterface {
	std::string DatabasePath = "mnist_training.db";
	std::string sql="SELECT DISTINCT IMAGE, LABEL FROM MNIST_DATA;";
	nlohmann::json data;
	bool refreshDB = true;
public:
	DatabaseNode() {
		TypeID = "DatabaseInputNode";
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			if (data.empty() || refreshDB) {
				refreshDB = false;
				/* open database */
				sqlite3* db;
				int rc = sqlite3_open(DatabasePath.c_str(), &db);
				if (rc) {
					fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
					sqlite3_close(db);
				}
				else {
					fprintf(stderr, "Opened database successfully\n");
				}

				//execute sql
				sqlite3_stmt* stmt;
				rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
				if (rc != SQLITE_OK) {
					fprintf(stderr, "Failed to prepare statement\n");
					fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
					sqlite3_close(db);
				}
				else {
					fprintf(stdout, "Statement prepared successfully\n");
				}

				//get data and put it in json array
				data = nlohmann::json::object();
				bool typeCollected = false;
				data["ColumnCount"] = sqlite3_column_count(stmt);
				data["ColumnNames"] = nlohmann::json::array();
				//get column names
				data["Columns"] = nlohmann::json::object();
				for (int i = 0; i < data["ColumnCount"]; i++) {
					nlohmann::json column = nlohmann::json::object();
					std::string columnName = sqlite3_column_name(stmt, i);
					std::string columnType = sqlite3_column_decltype(stmt, i);
					data["Columns"][columnName]["Type"] = columnType;
					data["ColumnNames"].push_back(columnName);
				}

				while (sqlite3_step(stmt) == SQLITE_ROW) {
					for (int i = 0; i < data["ColumnCount"]; i++)
					{
						std::string columnName = data["ColumnNames"][i];
						std::string columnType = data["Columns"][columnName]["Type"];


						if (columnType == "INTEGER") {
							data["Columns"][columnName]["Data"].push_back(sqlite3_column_int(stmt, i));
						}
						else if (columnType == "REAL") {
							data["Columns"][columnName]["Data"].push_back(sqlite3_column_double(stmt, i));
						}
						else if (columnType == "TEXT") {
							data["Columns"][columnName]["Data"].push_back((char*)sqlite3_column_text(stmt, i));
						}
						else if (columnType == "BLOB") {
							std::vector<unsigned char> blob;
							blob.resize(sqlite3_column_bytes(stmt, i));
							memcpy(&blob[0], sqlite3_column_blob(stmt, i), blob.size());
							data["Columns"][columnName]["Data"].push_back(blob);
						}
						else if (columnType == "NULL") {
							data["Columns"][columnName]["Data"].push_back("NULL");
						}
					}
				}
				data["Type"] = "DataBase";
			}
			GetOutputDataByIndex(0)=data;
		}
	}

	void DrawNodeTitle(ImGuiContext* Context) {
		ImGui::SetCurrentContext(Context);
		ImGui::Text("Database Node");
	}

	void Update() override {

	}

	void Init() override {
		DatabasePath.reserve(255);
		sql.reserve(255);

		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputText("Database Path", (char*)DatabasePath.c_str(), 255);
			}));

		MakeAttribute(1, new AttributeInterface([this]() {
			ImGui::PushItemWidth(300);
			ImGui::InputText("sql statement", (char*)sql.c_str(), 255);
			}));

		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Refresh", &refreshDB);
			}));

		MakeOutput(0, "Output","Any", nlohmann::json::array());

	}

	//void DrawNodeTitle();

	NodeInterface* GetInstance() override {
		DatabaseNode* node = new DatabaseNode();

		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data= NodeInterface::Serialize();
		data["DatabasePath"] = DatabasePath;
		data["Sql"] = sql;
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		DatabasePath = data["DatabasePath"];
		sql = data["Sql"];
		return;
	}


};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) void CleanUp() {

	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new DatabaseNode();
	}
}