#include "ast_utils.hpp"
#include "code_gen_utils.hpp"

#include "clang-c/Index.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    // Get args...
    if (argc < 2)
    {
        std::cerr << "Please provide C++ source file" << std::endl;
    }

    int num_compiler_args = argc - 2;

    const char* const* compiler_args = nullptr;
    if (num_compiler_args > 0)
    {
        compiler_args = num_compiler_args > 0 ? &argv[1] : nullptr;
    }

    std::string source_file_name = argv[argc - 1];

    // Generate AST
	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		source_file_name.c_str(),
		compiler_args, num_compiler_args,
		nullptr, 0,
		CXTranslationUnit_None);

	if (unit == nullptr)
	{
		std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
		exit(-1);
	}

	std::cerr << "Parsing file..." << std::endl;
  	CXCursor cursor = clang_getTranslationUnitCursor(unit);

    // Look through AST for types marked for binding, as well as
    // all required dependencies
    ast::TraversalData td = ast::find_bind_targets_and_deps(cursor);

    std::cerr << "Visited types:" << std::endl;
    for (const auto& kv : td.visited_types)
    {
        std::cerr << "\t" << kv.first << std::endl;
    }
    std::cerr << std::endl;

    // Generate pybind11 code
    code_gen::init_binding_file(source_file_name);
    code_gen::print_namespace_bindings(td);
    code_gen::print_type_bindings(td);
    code_gen::end_binding_file();

    // Clean up
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
