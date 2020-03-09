#include "ast_utils.hpp"
#include "code_gen_utils.hpp"

#include <cxxopts.hpp>
#include <clang-c/Index.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    using namespace std;

    cxxopts::Options options("bindgen", "Autogenerate some python bindings.");
    options.add_options()
        ("s,source", "C++ source code", cxxopts::value<string>())
        ("c,compiler_flags", "Clang compiler flag", cxxopts::value<vector<string>>())
        ("p,project_headers", "\"\" includes prepended to generated code", cxxopts::value<vector<string>>())
        ("i,system_headers", "<> includes prepended to generated code", cxxopts::value<vector<string>>())
        ("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      cerr << options.help() << endl;
      exit(0);
    }

    if (result.count("source") == 0)
    {
        cerr << "Please provide C++ source file" << endl;
        exit(-1);
    }

    string source_file_name = result["source"].as<string>();

    vector<string> project_headers = result.count("project_headers") > 0
                                        ? result["project_headers"].as<vector<string>>()
                                        : vector<string>();

    vector<string> system_headers = result.count("system_headers") > 0
                                        ? result["system_headers"].as<vector<string>>()
                                        : vector<string>();

    vector<string> cflags = result.count("compiler_flags") > 0
                                ? result["compiler_flags"].as<vector<string>>()
                                : vector<string>();

    vector<const char*> cstring_cflags;
    for (const string& fc : cflags) cstring_cflags.push_back(fc.c_str());

    // Generate AST
	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		source_file_name.c_str(),
		cstring_cflags.data(), cstring_cflags.size(),
		nullptr, 0,
		CXTranslationUnit_None);

	if (unit == nullptr)
	{
		cerr << "Unable to parse translation unit. Quitting." << endl;
		exit(-1);
	}

	cerr << "Parsing file..." << endl;
  	CXCursor cursor = clang_getTranslationUnitCursor(unit);

    // Look through AST for types marked for binding, as well as
    // all required dependencies
    ast::TraversalData td = ast::find_bind_targets_and_deps(cursor);

    cerr << "Visited types:" << endl;
    for (const auto& kv : td.visited_types)
    {
        cerr << "\t" << kv.first << endl;
    }
    cerr << endl;

    // Generate pybind11 code
    code_gen::init_binding_file(project_headers, system_headers);
    auto namespace_tokens = code_gen::print_namespace_bindings(td);
    code_gen::print_type_bindings(td, namespace_tokens);
    code_gen::end_binding_file();

    // Clean up
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
