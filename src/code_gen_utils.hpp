#pragma once

#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace code_gen
{
static const std::string MODULE_TOKEN = "_module_";

/**
 * Extract name of pybind11 object that will represent
 * a submodule / c++ namespace (e.g. a::b -> a__b__)
 */
std::string pb_namespace_token(std::string ns)
{
    size_t pos = 0;
    std::string token = "";
    do {
        pos = ns.find(ast::SCOPE_DELIMITER);
        token += ns.substr(0, pos);
        ns.erase(0, pos + ast::SCOPE_DELIMITER.length());
        token += "__";
    } while (pos != std::string::npos);

    return token;
}

/**
 * Get innermost namespace of string representing nested
 * namespaces (e.g. a::b::c -> c)
 */
std::string end_of_scope(const std::string& ns)
{
    if (ns.rfind(ast::SCOPE_DELIMITER) == std::string::npos)
    {
        return ns;
    }

    return ns.substr(
            ns.rfind(ast::SCOPE_DELIMITER) 
                    + ast::SCOPE_DELIMITER.length());
}

void init_binding_file(const std::string& fname)
{
    std::cout << "#include \"" << fname << "\"" << std::endl;
    std::cout << std::endl;
    std::cout << "#include <pybind11/pybind11.h>" << std::endl;
    std::cout << std::endl;
    std::cout << "namespace py = pybind11;" << std::endl;
    std::cout << std::endl;
    std::cout << "PYBIND11_MODULE(generated_pybind, "
              << MODULE_TOKEN << ")" << std::endl;
    std::cout << "{" << std::endl;
}

/**
 * Iterate over visited namespaces and print pybind11 code to
 * generate submodules that reflect the original nested namespace
 * structure
 */
void print_namespace_bindings(const ast::TraversalData& td)
{
    using namespace std;

    // Get vector of namespaces
    unordered_set<string> namespaces;
    for (const auto& kv : td.visited_types)
    {
        namespaces.insert(ast::parent_scope(kv.first));
    }
    namespaces.erase("");

    unordered_map<string, string> expanded;

    // Empty string namespae is module-level token
    expanded[""] = MODULE_TOKEN;

    for (string ns : namespaces)
    {
        string ns_from_global = "";
        size_t pos = 0;
        do {
            pos = ns.find(ast::SCOPE_DELIMITER);
            ns_from_global += ns.substr(0, pos);
    
            // Have not encountered this namespace before.
            // Get its parent, create a new pybind submodule
            // within its parent submodule, and store the namespace
            // and new submodule token.
            if (expanded.find(ns_from_global) == expanded.end())
            {
                string parent_scope = ast::parent_scope(ns_from_global);
                string parent_token = expanded[parent_scope];

                string ns_token = pb_namespace_token(ns_from_global);
                expanded[ns_from_global] = ns_token;

                cout << "\tpy::module " << ns_token << " = "
                          << parent_token << ".def_submodule(\""
                          << end_of_scope(ns_from_global) 
                          << "\");" << endl;
            }

            ns.erase(0, pos + ast::SCOPE_DELIMITER.length());
            ns_from_global += ast::SCOPE_DELIMITER;
        } while (pos != string::npos);
    }
}

void print_type_bindings(const ast::TraversalData& td)
{
    using namespace std;

    cout << endl;
    for (const auto& kv : td.visited_types)
    {
        const string& type_name = kv.first;
        const CXCursor& type_cursor = kv.second;
        vector<string> field_names = 
            ast::get_public_field_names(type_cursor); 

        cout << type_name << " fields:" << endl;
        for (const string& field : field_names)
        {
            cout << "\t" << field << endl;
        }
    }
}

void end_binding_file()
{
    std::cout << "}" << std::endl;
}
}
