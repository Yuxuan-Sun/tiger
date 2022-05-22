#include "AST.h"
#include <tuple>

#include <unordered_map>


struct askListArgs;


std::tuple<string, string, string> SU_label(A_exp _left, A_exp _right);

// string goOverList(AST_node expList, string (*func)(AST_node,int), 
//                 string returnValue = "", int count = 0);

string goOverList(A_expList expList, string (*func)(A_exp,int), 
                string returnValue = "", int count = 0);

string indent(int count = 1);

bool isOpAri(A_oper op);
bool isOpComp(A_oper op);

A_oper find_oper(string s);

Ty_ty symbolToType(Symbol s);
Symbol typeToSymbol(Ty_ty t);

Ty_ty symTypeLookup(Symbol s);