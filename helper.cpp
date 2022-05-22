#include "helper.h"

#include "AST.h"
#include <tuple>
#include <algorithm>

#include "ST-2024.h"
#include "symbol.h"

#include <unordered_map>


const string indent_math = "    ";  // might want to use something different for, e.g., branches

struct askListArgs
{
	string returnValue;
	int count;
};


std::tuple<string, string, string> SU_label(A_exp _left, A_exp _right)
{
	string my_code = "";
	string left_reg = _left->result_reg_s();
	string right_reg = _right->result_reg_s();
	if (_left->result_reg() == _right->result_reg())
    {
        string rescue_R = "R" + std::to_string(_left->result_reg()+1);
        my_code += _left->HERA_code();
        my_code += indent_math + "MOVE(" + rescue_R + "," + _left->result_reg_s() + ")\n";
        my_code += _right->HERA_code();
		left_reg = rescue_R;
    }
	if ((_left->result_reg() > _right->result_reg()))
	{
		my_code += _left->HERA_code();
		my_code += _right->HERA_code();
	}
	if ((_left->result_reg() < _right->result_reg()))
	{
		my_code += _right->HERA_code();
		my_code += _left->HERA_code();
	}
	return std::make_tuple(my_code, left_reg, right_reg);
}

// template <typename T>

string goOverList(A_expList expList, string (*func)(A_exp,int), string returnValue, int count) {

	if (expList == 0)	return returnValue;
	
	assert(expList != 0);
	
	string my_code = func(expList->_head, count);

	if (expList->_tail == 0)
	{
		// my_code += func(expList->_head, count);
		return returnValue + my_code;
	} else {
		return goOverList(expList->_tail, func, returnValue + my_code, count + 1);
	}

}

string indent(int count){
	string my_code = "";
	for (size_t i = 0; i < count; i++)
	{
		my_code += indent_math;
	}
	return my_code;
}

A_oper ariOpList[] = {A_plusOp, A_minusOp, A_timesOp, A_divideOp, A_modOp};

A_oper compOpList[] = {A_orOp, A_andOp,A_ltOp, A_leOp, A_gtOp, A_geOp};

// https://www.cplusplus.com/reference/algorithm/find/
bool isOpAri(A_oper op) {
	A_oper *p;
	p = std::find(ariOpList, ariOpList+5, op);
	if (p != ariOpList+5) return true;
	return false;
}

bool isOpComp(A_oper op) {
	A_oper *p;
	p = std::find(compOpList, compOpList+6, op);
	if (p != compOpList+6) return true;
	return false;
}


std::unordered_map<string, A_oper> oper_table = {
        {"<", A_ltOp}, {"<=", A_leOp}, 
		{">", A_gtOp}, {">=", A_geOp},
		{"=", A_eqOp}, {"!=", A_neqOp}
};

A_oper find_oper(string s) {
	return oper_table.at(s);
}

Ty_ty symbolToType(Symbol s) {
	if (Symbol_to_string(s) == "string")
	{
		return Ty_String();
	}
	if (Symbol_to_string(s) == "int")
	{
		return Ty_Int();
	}
	if (Symbol_to_string(s) == "bool")
	{
		return Ty_Bool();
	}
	if (Symbol_to_string(s) == "void")
	{
		return Ty_Void();
	}
	
	EM_error("your symbol " + str(s) + " is not a type!");
	return Ty_Error();
}

Symbol typeToSymbol(Ty_ty t) {
	if (t == Ty_Int())
	{
		return to_Symbol("int");
	}
	if (t == Ty_String())
	{
		return to_Symbol("string");
	}
	if (t == Ty_Bool())
	{
		return to_Symbol("bool");
	}
	if (t == Ty_Void())
	{
		return to_Symbol("void");
	}
	EM_error("your type is not valid!");
	return 0;
}

Ty_ty symTypeLookup(Symbol s) {
	if (!is_name_there(s, typeLibrary)) EM_error("your " + Symbol_to_string(s) + " is not a type!");
	return lookup(s, typeLibrary);
}