#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>

std::vector<std::string> LibsPath;
std::vector<std::string> LibraryFiles;

// returns the distance between two directories
int DirectoryDistance(std::string path, std::string subpath)
{
	std::filesystem::path p1 = path;
	std::filesystem::path p2 = subpath;

	int distance = 0;

	// use dijkstra's algorithm to find the distance between the two directories
	while (p1 != p2)
	{
		if (p1.string() == p2.string())
		{
			break;
		}
		if (p1.string().size() > p2.string().size())
		{
			p1 = p1.parent_path();
			distance++;
		}
		else if (p1.string().size() < p2.string().size())
		{
			p2 = p2.parent_path();
			distance++;
		}
		else
		{
			p1 = p1.parent_path();
			p2 = p2.parent_path();
			distance += 2;
		}
	}

	return distance;
}

void FindLibs(std::string path, std::vector<std::string> Libraries, bool first)
{
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			if (entry.path().filename().string() == "Libs")
			{
				printf("Found Libs directory %s\n", std::filesystem::absolute(entry.path()).string().c_str());
				LibsPath.push_back(entry.path().string());
				continue;
			}
			FindLibs(entry.path().string(), Libraries, false);
		}
		else if (entry.is_regular_file() && !first)
		{
			for (int i = 0; i < Libraries.size(); i++)
			{
				if (entry.path().filename().string() == Libraries[i])
				{
					printf("Found library %s\n", std::filesystem::absolute(entry.path()).string().c_str());
					LibraryFiles.push_back(entry.path().string());
				}
			}
		}
	}
}

void FindLibs(std::string path, std::vector<std::string> Libraries)
{
	FindLibs(path, Libraries, true);
}

int main()
{
	try
	{
		// if the config file doesn't exist, create it
		if (!std::filesystem::exists("config.json"))
		{
			// create the config file
			std::ofstream o("config.json");

			// create the json object
			nlohmann::json j;
#if defined(_MSC_VER)
			j["Libraries"] = {
				"boost_filesystem-vc143-mt-gd-x64-1_83.dll",
				"glew32d.dll",
				"glfw3.dll",
				"OpenCL.dll",
				"sqlite3.dll",
				"lua.dll"};
#elif defined(__GNUC__)
			j["Libraries"] = {
				"libboost_filesystem-mgw82-mt-x64-1_73.so",
				"libglew32d.so",
				"libglfw3.so",
				"libOpenCL.so",
				"libsqlite3.so",
				"liblua.so"};
#endif

			j["ExecutableDir"] = "../Machine-Learning-Lab";

			// write the json object to the config file
			o << j << std::endl;

			// close the config file
			o.close();
		}

		// load config.json
		std::ifstream i("config.json");
		nlohmann::json j;
		i >> j;

		std::vector<std::string> Libraries = j["Libraries"];

		FindLibs(j["ExecutableDir"], Libraries);

		for (int i = 0; i < LibraryFiles.size(); i++)
		{
			std::vector<std::pair<int, std::string>> distances;
			for (int j = 0; j < LibsPath.size(); j++)
			{
				distances.push_back(std::make_pair(DirectoryDistance(LibsPath[j], LibraryFiles[i]), LibsPath[j]));
			}
			std::sort(distances.begin(), distances.end(), [](std::pair<int, std::string> a, std::pair<int, std::string> b)
					  {  return a.first < b.first; });
			printf("Found library %s Closest directory is %s\n", LibraryFiles[i].c_str(), distances[0].second.c_str());
			// move the file

			// if the file already exists, delete it
			std::filesystem::copy(LibraryFiles[i], distances[0].second, std::filesystem::copy_options::overwrite_existing);
			// delete the file
			std::filesystem::remove(LibraryFiles[i]);
		}
	}
	catch (std::exception e)
	{
		printf("Error: %s\n", e.what());
	}
}