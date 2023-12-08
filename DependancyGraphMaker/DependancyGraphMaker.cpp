#include <sqlite3.h>

#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>


std::map<std::filesystem::path,std::vector<std::filesystem::path>> dependancyGraph;
void PrintDependancyGraph()
{
	for (auto& [key, value] : dependancyGraph)
	{
		printf("%s\n", key.string().c_str());
		for (auto& dependancy : value)
		{
			printf("\t%s\n", dependancy.string().c_str());
		}
	}
}

std::vector<std::filesystem::path> cpp_files;
std::vector<std::filesystem::path> h_files;

int main(int argc, char* argv[])
{
	std::filesystem::path startPath = "../../../../";

	// Get all cpp files
	for (auto& p : std::filesystem::recursive_directory_iterator(startPath))
	{
		if (p.path().extension() == ".cpp")
		{
			cpp_files.push_back(p.path());
		}
	}
	// Get all h files
	for (auto& p : std::filesystem::recursive_directory_iterator(startPath))
	{
		if (p.path().extension() == ".h")
		{
			h_files.push_back(p.path());
		}
	}

	//open all cpp files and find all includes
	for (auto& cpp_file : cpp_files)
	{
		std::ifstream file(cpp_file);
		std::string line;
		while (std::getline(file, line))
		{
			//match regex #include ".*" or #include <.*>
			std::regex include_regex("#include \"(.*)\"");
			std::smatch include_match;
			if (std::regex_search(line, include_match, include_regex))
			{
				std::string include = std::filesystem::path(include_match[1].str()).filename().string();
				if (include == ".*"|| include == "(.*)")
				{
					continue;
				}
				dependancyGraph[cpp_file.filename().string()].push_back(include);
				if (dependancyGraph.find(include) == dependancyGraph.end())
				{
					dependancyGraph[include] = std::vector<std::filesystem::path>();
				}
			}

			std::regex include_regex2("#include <(.*)>");
			std::smatch include_match2;
			if (std::regex_search(line, include_match2, include_regex2))
			{
				std::string include = std::filesystem::path(include_match2[1].str()).filename().string();
				if (include == ".*" || include == "(.*)")
				{
					continue;
				}
				dependancyGraph[cpp_file.filename().string()].push_back(include);
				if (dependancyGraph.find(include) == dependancyGraph.end())
				{
					dependancyGraph[include] = std::vector<std::filesystem::path>();
				}
			}
		}
	}

	//open all h files and find all includes
	for (auto& h_file : h_files)
	{
		std::ifstream file(h_file);
		std::string line;
		while (std::getline(file, line))
		{
			//match regex #include ".*" or #include <.*>
			std::regex include_regex("#include \"(.*)\"");
			std::smatch include_match;
			if (std::regex_search(line, include_match, include_regex))
			{
				std::string include = std::filesystem::path(include_match[1].str()).filename().string();
				if (include == ".*" || include == "(.*)")
				{
					continue;
				}
				dependancyGraph[h_file.filename().string()].push_back(include);
				if (dependancyGraph.find(include) == dependancyGraph.end())
				{
					dependancyGraph[include] = std::vector<std::filesystem::path>();
				}
			}

			std::regex include_regex2("#include <(.*)>");
			std::smatch include_match2;
			if (std::regex_search(line, include_match2, include_regex2))
			{
				std::string include = std::filesystem::path(include_match2[1].str()).filename().string();
				if (include == ".*" || include == "(.*)")
				{
					continue;
				}
				dependancyGraph[h_file.filename().string()].push_back(include);
				if (dependancyGraph.find(include) == dependancyGraph.end())
				{
					dependancyGraph[include] = std::vector<std::filesystem::path>();
				}
			}
		}
	}
	PrintDependancyGraph();

	//create graphic
	std::ofstream graph("graph.dot");
	graph << "digraph \"Source Tree\" {\n";
	graph << "\toverlap = scale;\n";
	graph << "\tsize = \"200,200\";\n";
	graph << "\tratio = fill;\n";
	graph << "\tfontSize = 16;\n";
	graph << "\tfontname = \"Verdana\";\n";
	graph << "\tclusterrank = global;\n";
	//graph << "\tsplines=ortho;\n";
	for (auto& [key, value] : dependancyGraph)
	{
		for (auto& dependancy : value)
		{
			//if .h file the color is blue
			if (key.extension() == ".h")
			{
				graph << "\t\"" << key.string() << "\" [style=filled, fillcolor=blue];\n";
			}
			//if .cpp file the color is red
			else if (key.extension() == ".cpp")
			{
				graph << "\t\"" << key.string() << "\" [style=filled, fillcolor=red];\n";
			}
			graph << "\t\"" << key.string() << "\" -> \"" << dependancy.string() << "\";\n";
		}
	}
	graph << "}\n";
	graph.close();

	std::string	command = "dot -x -Goverlap=scale -Tjpg graph.dot > graph.jpg";
	system(command.c_str());
	command = "dot -x -Goverlap=scale -Tpng graph.dot > graph.png";
	system(command.c_str());

	return 0;
}