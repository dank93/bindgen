#pragma once

#include <array>
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

/**
 * Prepare type name for bindings by replacing '<, >' characters
 * with underscores, and prepend type name with underscore if it is a 
 * templated type, preventing template instantiations from polluting
 * module.
 */
std::string sanitized_if_template_typename(std::string type_name)
{
    auto template_char_pos = [](std::string s) {
        std::array<size_t, 4> positions{
            s.find("<"),
            s.find(","),
            s.find(" "),
            s.find(">"),
        };
        return *std::min_element(positions.begin(), positions.end());
    };

    bool is_template = false;

    size_t pos = template_char_pos(type_name);
    while (pos != std::string::npos)
    {
        is_template = true;
        type_name.replace(pos, 1, "_");      
        pos = template_char_pos(type_name);
    }

    return is_template ? "_" + type_name : type_name;
}

/**
 * Print includes and pybind module macro
 */
void init_binding_file(const std::string& fname)
{
    std::cout << "#include \"" << fname << "\"" << std::endl;
    std::cout << std::endl;
    std::cout << "#include <pybind11/pybind11.h>" << std::endl;
    std::cout << "#include <pybind11/stl.h>" << std::endl;
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
std::unordered_map<std::string, std::string>
print_namespace_bindings(const ast::TraversalData& td)
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

    return expanded;
}

/**
 * Print binding for single C++ struct or class, including public
 * member variables
 */
void print_struct_or_class_bindings(
    const std::string& fully_scoped_type_name,
    const CXCursor& type_decl_cursor,
    const std::unordered_map<std::string, std::string>& namespace_tokens)
{
    using namespace std;

    string type_namespace = ast::parent_scope(fully_scoped_type_name);
    string bare_type_name = end_of_scope(fully_scoped_type_name);
    string sanitized_type_name = sanitized_if_template_typename(bare_type_name);
    const string& namespace_token = namespace_tokens.at(type_namespace);

    cout << "\tpy::class_<" << fully_scoped_type_name << ">("
         << namespace_token << ", \"" << sanitized_type_name
         << "\")" << endl;
    cout << "\t\t.def(py::init<>())";

    vector<string> field_names = ast::get_public_field_names(type_decl_cursor); 

    for (size_t i = 0; i < field_names.size(); i++)
    {
        if (i == 0) cout << endl;

        const string& field = field_names[i];

        cout << "\t\t.def_readwrite(\"" << field
             << "\", &" << fully_scoped_type_name << "::"
             << field << ")";

        if (i != field_names.size() - 1) cout << endl;
    }

    cout << ";" << endl << endl;
}

/**
 * Print pybind code for all types in TraversalData, nested in submodeules
 * according to source namespace
 */
void print_type_bindings(
    const ast::TraversalData& td,
    const std::unordered_map<std::string, std::string>& namespace_tokens)
{
    using namespace std;

    cout << endl;
    for (const auto& kv : td.visited_types)
    {
        print_struct_or_class_bindings(kv.first, kv.second, namespace_tokens);
    }
}

/**
 * Terminate generated file
 */
void end_binding_file()
{
    std::cout << "}" << std::endl;
}
}
