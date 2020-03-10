#pragma once

#include "clang-c/Index.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * Easy CXString printing
 */
std::ostream& operator<<(std::ostream& stream, const CXString& str)
{
    if (const char* c_str = clang_getCString(str))
    {
	    stream << c_str;
    }
	
    clang_disposeString(str);
	return stream;
}

namespace ast
{
static const std::string SCOPE_DELIMITER = "::";

/**
 * Check if cursor is public field
 */
bool is_public_field(const CXCursor& c)
{
    return clang_getCXXAccessSpecifier(c) == CX_CXXPublic;
}

/**
 * Check if cursor type is struct or class
 */
bool is_struct_or_class(const CXCursor& c)
{
    CXCursor decl = clang_getTypeDeclaration(clang_getCursorType(c));
    return decl.kind == CXCursor_StructDecl || decl.kind == CXCursor_ClassDecl;
}

/**
 * Check if cursor type is enum
 */
bool is_enum(const CXCursor& c)
{
    CXCursor decl = clang_getTypeDeclaration(clang_getCursorType(c));
    return decl.kind == CXCursor_EnumDecl;
}

/**
 * Extract comment string associated with cursor
 */
std::string comment_string(const CXCursor& c)
{
    CXString comment_cxstr = clang_Cursor_getRawCommentText(c);
    std::string out = "";

    if (const char* c_str = clang_getCString(comment_cxstr))
    {
        out = c_str;
    }

    clang_disposeString(comment_cxstr);
	return out;
}

/**
 * Extract std::string version of cursor name
 */
std::string cursor_spelling(const CXCursor& c)
{
    CXString name = clang_getCursorSpelling(c);
    std::string out = "";

    if (const char* c_str = clang_getCString(name))
    {
        out = c_str;
    }

    clang_disposeString(name);
	return out;
}

/**
 * Check if bindings should be generated for cursor
 * and its dependencies
 */
bool bind_requested(const CXCursor& c)
{
    static const char* BIND_FLAG = "~~BIND~~";
    return (comment_string(c).find(BIND_FLAG) != std::string::npos);
}

/**
 * Extract unqualified type name from cursor (i.e.
 * outer_namespace::innter_namespace::TypeName)
 */
std::string scoped_type_name(CXCursor cursor, bool start = true)
{
    std::string s = "";
    CXString str = clang_getTypeSpelling(clang_getCursorType(
                    clang_getTypeDeclaration(clang_getCursorType(cursor))));
    s += clang_getCString(str);
    clang_disposeString(str);
    return s;
}

/**
 * Extract namespace scope of unqualified type name
 */
std::string parent_scope(std::string type_str)
{
    auto pos = type_str.rfind("::");

    if (pos == std::string::npos)
    {
        return "";
    }

    type_str.erase(pos);
    return type_str;
}

/**
 * Check if cursor type is from C++ standard library
 */
bool from_std(const CXCursor& c)
{
    std::string scope = parent_scope(scoped_type_name(c));
    while (scope != "")
    {
        if (scope == "std")
        {
            return true;
        }
        scope = parent_scope(scope);
    }

    return false;
}

struct TraversalData
{
    // Recursion depth
    size_t depth;

    // Types that we've already recursed through
    std::unordered_map<std::string, CXCursor> visited_types;
};

/**
 * Recursion util that iterates over all of a cursor's type's
 * members, building dependcy graph of root type
 */
void _recurse_fields(const CXCursor& c, TraversalData* td)
{
    using namespace std;

    clang_Type_visitFields(clang_getCursorType(c),
        [](CXCursor c, CXClientData client_data) {
            
            TraversalData* td = (TraversalData*)client_data;
            for (size_t i = 0; i < td->depth; ++i) cerr << "\t";

            string type_name = scoped_type_name(c);
            bool prev_visited_type = false;

            if (type_name == "")
            {
                cerr << clang_getTypeSpelling(clang_getCursorType(c))
                          << " " << clang_getCursorSpelling(c);
                cerr << " (basic type)";
            }
            else
            {
                cerr << type_name << " "
                          << clang_getCursorSpelling(c);

                if (from_std(c))
                {
                    cerr << " (skipping std)" << std::endl;
                    return CXVisit_Continue;
                }
                else if (!is_public_field(c))
                {
                    cerr << " (skipping priv)" << std::endl;
                    return CXVisit_Continue;
                }
                else if (td->visited_types.find(type_name) !=
                         td->visited_types.end())
                {
                    prev_visited_type = true;
                    cerr << " (visited)";
                }
                else
                {
                    cerr << " (+)";
                }
            }
            cerr << endl;

            if (!prev_visited_type)
            {
                td->depth++;
                _recurse_fields(c, td);
                td->depth--;
            }

            td->visited_types[type_name] = c;

            return CXVisit_Continue;
        },
        td);
}

/**
 * Recursion util that iterates over all of a cursor's type's
 * members, building dependcy graph of root type
 */
void recurse_fields(const CXCursor& c, TraversalData* td)
{
    std::cerr << "Recursing " << clang_getCursorSpelling(c) << std::endl;
    td->depth = 1;

    std::string type_name = scoped_type_name(c);
    std::string type_namespace = parent_scope(type_name);
    td->visited_types[type_name] = c;

    _recurse_fields(c, td);

    std::cerr << std::endl;
}

/**
 * Traverse libclang AST and build dependcy graph for all types
 * marked for binding
 */
TraversalData find_bind_targets_and_deps(CXCursor c)
{
    ast::TraversalData td;

  	clang_visitChildren(
  	 	c,
        [](CXCursor c, CXCursor parent, CXClientData client_data) {

            ast::TraversalData* td = (ast::TraversalData*)client_data;

            if (ast::bind_requested(c))
            {
                ast::recurse_fields(c, td);
            }

  	  	  	return CXChildVisit_Recurse;
  	  	},
  	  	&td);

    td.visited_types.erase("");

    return td;
}

/**
 * Given a struct or class declaration cursor, print out the 
 * names of its public fields.
 *
 * Ex:
 *
 * struct S
 * {
 * public:
 *      int i;
 *      float f;
 *
 * private:
 *      char c;
 * };
 *
 * Would return {"i", "f"}
 */
std::vector<std::string> get_public_field_names(CXCursor c)
{
    using std::vector;
    using std::string;

    assert(is_struct_or_class(c));

    struct Data
    {
        vector<string> names;
        string scoped_type_name;
    };

    Data d;
    d.scoped_type_name = scoped_type_name(c);

    clang_Type_visitFields(clang_getCursorType(c),
        [](CXCursor c, CXClientData client_data) {        

            Data& d = *(Data*)client_data;

            if (is_public_field(c))
            {
                d.names.push_back(cursor_spelling(c));
            }

            return CXVisit_Continue;
        },
        &d);

    return d.names;
}

/**
 * Get enum values for given enum declaration or instantiation.
 * Ex:
 *
 * enum class ExEnum
 * {
 *   A,
 *   B,
 *   C
 * };
 *
 * would return {"A", "B", "C"}.
 */
std::vector<std::string> get_enum_values(CXCursor c)
{
    using std::vector;
    using std::string;

    assert(is_enum(c));

    vector<string> out;

    clang_visitChildren(
        clang_getTypeDeclaration(clang_getCursorType(c)),
        [](CXCursor c, CXCursor parent, CXClientData client_data) {

            auto out = (vector<string>*)client_data;
            CXString val = clang_getCursorSpelling(c);
            out->push_back(clang_getCString(val));
            clang_disposeString(val);

            return CXChildVisit_Recurse;
        },
        &out);

    return out;
}
}
