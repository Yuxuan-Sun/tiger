#include "AST.h"
#include "helper.h"

// #include "helper.h"
/*
 * HERA_data methods
 */
const string indent_math = "    ";


string AST_node_::HERA_data()  // Default used during development; could be removed in final version
{
	// string message = "HERA_data() requested for AST node type not yet having a HERA_code() method";
	// EM_error(message);
	// return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
	// EM_error("HERA data not default one!");
	return "";
}

string A_stringExp_::HERA_data()
{
    string mycode = "DLABEL(" + this->result_label() + ")\n";
    mycode += indent_math + "LP_STRING(" + this->value + ")\n";
    return mycode;
}


string A_opExp_::HERA_data()
{
	return _left->HERA_data()+_right->HERA_data();
}

string A_compExp_::HERA_data()
{
	// if ((this->_oper == A_eqOp) or (this->_oper == A_neqOp))
	// {
	// 	return _left->HERA_data();
	// }
	
	string my_code = "";
	my_code += _left->HERA_data();
	my_code += _right->HERA_data();
	return my_code;
}

string A_callExp_::HERA_data()
{
	// auto func = [](A_exp e, int i) { 
	// 	return e->HERA_data(); 
	// };
	// return goOverList(_args, func, "", 0);
	if (_args != 0)
	{
		return _args->HERA_data();
	} return "";
	
}

string A_seqExp_::HERA_data()
{
	if (_seq == 0) return "";
	return _seq->HERA_data();
}

string A_expList_::HERA_data()
{
	// auto func = [](A_exp e, int i) { 
	// 	return e->HERA_data(); 
	// };
	// return goOverList(this, func, "", 0);
	string my_code = "";
	if (_tail == 0) {
		my_code += _head->HERA_data();
	} else {
		my_code += _head->HERA_data();
		my_code += _tail->HERA_data();
	}	
	return my_code;
}

string A_ifExp_::HERA_data()
{
	if (_else_or_null == 0) return _test->HERA_data() + _then->HERA_data();
	return _test->HERA_data() + _then->HERA_data() + _else_or_null->HERA_data();
}

string A_whileExp_::HERA_data()
{
	return _test->HERA_data() + _body->HERA_data();
}

string A_breakExp_::HERA_data()
{
	return "";
}

string A_forExp_::HERA_data()
{
	return _lo->HERA_data() + _hi->HERA_data() + _body->HERA_data();
}

string A_varExp_::HERA_data()
{
	return _var->HERA_data();
}

string A_simpleVar_::HERA_data()
{
	return "";
}

string A_letExp_::HERA_data()
{
	string my_code = "";

	ST<var_info> outLibrary = varLibrary;
	ST<sym_info> outFuncLibrary = funcLibrary;

	my_code += _decs->HERA_data();
	my_code += _body->HERA_data();

	varLibrary = outLibrary;
	funcLibrary = outFuncLibrary;
	return my_code;
}

string A_dec_::HERA_data()
{
	return "";
}

string A_varDec_::HERA_data()
{
	string my_code = "";
	my_code += _init->HERA_data();

	return my_code;
}

// string goOverDecList(A_decList d, string previous) {
// 	if (d == 0) return previous;
// 	assert(d != 0);
// 	string my_code = previous + d->_head->HERA_data();
// 	if (d->_tail != 0) return goOverDecList(d->_tail, my_code);
// 	else return my_code;
// }

string A_decList_::HERA_data()
{
	string my_code = "";

	if (_tail == 0) my_code += _head->HERA_data();
	else { my_code += _head->HERA_data(); my_code += _tail->HERA_data(); }
	return my_code;
}

string A_assignExp_::HERA_data()
{
	string my_code = "";
	my_code += _exp->HERA_data();
	my_code += _var->HERA_data();
	return my_code;
}

string A_functionDec_::HERA_data()
{
	this->goOverFunctions();
	return this->theFunctions->HERA_data();
}

string A_fundec_::HERA_data()
{
	return _body->HERA_data();
}

string A_fundecList_::HERA_data()
{
	string my_code = "";

	my_code += _head->HERA_data();
	if (_tail != 0) my_code += _tail->HERA_data();

	return my_code;
}