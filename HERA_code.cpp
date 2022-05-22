#include "AST.h"
#include <tuple>

#include "ST-2024.h"
#include "types.h"

#include "helper.h"

const string indent1 = "    ";

string functionsData = "// ------ user defined functions ------\n ";

int FP_counter = 0;

/*
 * HERA_code methods
 */

string easy(string op, string first, string second, string optionResult="") {
	if (optionResult == "") return indent1 + op + "(" + first + "," + second + ")\n";
	return indent1 + op + "(" + optionResult + "," + first + "," + second + ")\n";
}


string AST_node_::HERA_code()  // Default used during development; could be removed in final version
{
	string message = "HERA_code() requested for AST node type not yet having a HERA_code() method";
	EM_error(message);
	return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
}


string A_root_::HERA_code()
{
	// return "SET(R4, 3)SUB(R4, R0, R4)MOVE(R5,R4)SET(R4, 7)MUL(R5, R5, R4)SUB(R5, R0, R5)";
	/* return main_expr->HERA_data() + "CBON()\n\n" + main_expr->HERA_code(); */
	varLibrary = emptyVarLibrary;
	string my_code="";
	string lib = "#include <Tiger-stdlib-stack-data.hera>";
	my_code += main_expr->HERA_data();
	my_code += lib + "\nCBON()\n\n";
	my_code +=  main_expr->HERA_code() + "HALT() \n #include <Tiger-stdlib-stack.hera> \n";  // was SETCB for HERA 2.3;
	my_code += functionsData;
	return my_code;
}


string A_intExp_::HERA_code()
{
	if (value == 0) return "";
	return indent1 + "SET(" + result_reg_s() + ", " + str(value) +")\n";
}

string A_stringExp_::HERA_code()
{
	return indent1 + "SET(" + this->result_reg_s() + "," + this->result_label() + ")\n";
}

string A_nilExp_::HERA_code()
{
	return "";
}

string A_boolExp_::HERA_code()
{
	string my_code = "";
	// my_code += indent1 + "SET(" + this->result_reg_s() + ",";
	// my_code += (this->value) ? "-1" : "0";
	// my_code += ")\n";

	if (this->value)
	{
		my_code += easy("SET", this->result_reg_s(), "1");
		// my_code += easy("SUB", "R0", this->result_reg_s(), this->result_reg_s());
	} else {
		my_code += easy("SET", this->result_reg_s(), "0");
	}
	

	return my_code;
}


static string HERA_ari_op(Position p, A_oper op) // needed for opExp
{
	switch (op) {
	case A_plusOp:
		return "ADD";
	case A_timesOp:
		return "MUL";	// was MULT for HERA 2.3
	case A_minusOp:
		return "SUB";
	case A_andOp:
		return "AND";
	case A_eqOp:
		return "AND";
	case A_orOp:
		return "OR";
	case A_neqOp:
		return "OR";
	default:
		EM_error("Unhandled case in HERA_math_op", false, p);
		return "Oops_unhandled_hera_math_op";
	}
}

static string HERA_comp_op(Position p, A_oper op)
{
	switch (op)
	{
	case A_eqOp:
		return "BZ";
	case A_neqOp:
		return "BNZ";
	case A_ltOp:
		return "BL";
	case A_leOp:
		return "BLE";
	case A_gtOp:
		return "BG";
	case A_geOp:
		return "BGE";
	case A_andOp:
		return "AND";
	case A_orOp:
		return "OR";
	default:
		EM_error("Unhandled case in HERA_comp_op", false, p);
		return "Oops_unhandled_hera_comp_op";
	}
}

static string efficientComp(Position p, A_oper op, string result, string left, string right)
{
	string my_code = "";
	my_code += "// we do some comparison now ------- \n";
	switch (op)
	{
	case A_ltOp:
		my_code += easy("SUB", left, right, result);
		break;
	case A_leOp:
		my_code += easy("SUB", left, right, result);
		my_code += easy("INC", result, "1");
		break;
	case A_gtOp:
		my_code += easy("SUB", right, left, result);
		break;
	case A_geOp:
		my_code += easy("SUB", right, left, result);
		my_code += easy("DEC", result, "1");
		break;
	case A_andOp:
		my_code += easy("AND", left, right, result);
		break;
	case A_eqOp:
		my_code += easy("AND", left, right, result);
		break;
	case A_orOp:
		my_code += easy("OR", left, right, result);
		break;
	case A_neqOp:
		my_code += easy("OR", left, right, result);
		break;
	default:
		EM_error("efficient Unhandled case in HERA_comp_op", false, p);
		return "Oops_unhandled_hera_comp_op";
	}
	my_code += "// comparison done ------ \n";
	return my_code;
}

string A_opExp_::HERA_code()
{
		    
	string my_code, left_reg, right_reg;
	tie(my_code, left_reg, right_reg) = SU_label(_left,_right);

	// if (_oper == A_notOp)
	// {
	// 	my_code += "SET(" + left_reg + ",0xffff)\n";
	// 	my_code += "XOR(" + right_reg + "," + left_reg + "," + right_reg + ")\n";
	// 	my_code += "MOVE(" + this->result_reg_s() +","+ right_reg + ")\n";
	// 	return my_code;
	// }

	// assert(isOpAri(_oper));
	
	my_code += indent1 + (HERA_ari_op(pos(), _oper) + "(" +
				this->result_reg_s() + ", " +
				left_reg + ", " + right_reg + ")\n");
	return my_code;
}

string A_compExp_::HERA_code()
{
	// assert(isOpComp(_oper));
	string my_code, left_reg, right_reg;
	tie(my_code, left_reg, right_reg) = SU_label(_left,_right);
	string result_reg = this->result_reg_s();
	if (_left->typecheck() == Ty_String() && _left->typecheck() == Ty_String())
	{
		my_code += "//--------- some string compare stuff -----\n";
		my_code += easy("MOVE", "FP_alt", "SP");
		my_code += easy("INC", "SP", "5");
		my_code += easy("STORE", "3", "FP_alt", left_reg);
		my_code += easy("STORE", "4", "FP_alt", right_reg);
		my_code += easy("CALL", "FP_alt", "tstrcmp");
		my_code += easy("LOAD", "3", "FP_alt", result_reg);
		my_code += easy("DEC", "SP", "5");
		my_code += easy("CMP", result_reg, "R0");
		// switch (_oper)
		// {
		// case A_leOp:
		// 	my_code += easy("DEC", result_reg, "1");
		// 	break;
		// case A_ltOp:
		// 	break;
		// case A_geOp:
		// 	my_code += easy("SUB", "R0", result_reg, result_reg);
		// 	my_code += easy("DEC", result_reg, "1");
		// 	break;
		// case A_gtOp:
		// 	my_code += easy("SUB", "R0", result_reg, result_reg);
		// 	break;
		// default:
		// 	EM_error("compare string op weird!");
		// 	break;
		// }
		my_code += "//------string compare done------\n";
		// return my_code;
	} else {
		my_code += indent1 + easy("CMP", left_reg, right_reg);
	}

	// if (_oper == A_notOp)
	// {
	// 	my_code += "SET(" + left_reg + ",0xffff)\n";
	// 	my_code += "XOR(" + right_reg + "," + left_reg + "," + right_reg + ")\n";
	// 	my_code += "MOVE(" + this->result_reg_s() +","+ right_reg + ")\n";
	// 	return my_code;
	// }
	string myOp = (HERA_comp_op(pos(), _oper));
	// string result = this->result_reg_s();
	string label = this->result_label();

	my_code += indent1 + myOp + "(" + label + ")\n";
	my_code += indent1 + easy("SET", result_reg, "0");
	my_code += indent1 + "BR" + "(" + label + "_end" + ")\n";
	my_code += indent1 + "LABEL(" + label + ")\n";
	my_code += indent1 + easy("SET", result_reg, "1");
	my_code += indent1 + "LABEL(" + label + "_end" + ")\n";

	// my_code += indent1 + (HERA_comp_op(pos(), _oper) + "(" +
	// 			this->result_reg_s() + ", " +
	// 			left_reg + ", " + right_reg + ")\n");
	
	// my_code += efficientComp(pos(), _oper, this->result_reg_s(), left_reg, right_reg);

	return my_code;
}

string argsHelper(A_expList _args, int count){
	string output = "";
	A_expList args = _args;
	while(args != 0) {
		output = output + args->_head->HERA_code() 
						+ indent1 + "STORE(" + args->_head->result_reg_s() + ", " 
						+ std::to_string(count) + ", FP_alt)\n";
		args = args->_tail;
		count++;
	}
	return output;
}

string A_callExp_::HERA_code()
{
	// my_code += indent1 + "SET(R1," + str(value) + ")\n";
	
	// string my_code = _args->HERA_code();
	// my_code += indent1 + "MOVE(R1," + _args->result_reg_s() + ")\n";
	// my_code += indent1 + "CALL(FP_alt," + str(_func) + ")\n";
	// my_code += indent1 + "MOVE("+ this->result_reg_s() +",R1)\n";
	
	int SL = 0;
	var_info* currentVar = NULL;
	if (!is_name_there(_func, stdFuncLibrary)) {
		std::tie(SL, currentVar) = this->findVarFP(0, this->_func);
		if (SL == -1) EM_error("your call " + Symbol_to_string(this->_func) + " not in library(simpleVar HERAcode)");		
	}
	// if (this->inBody(this)) SL = SL-1;

	string my_code = "";
	string functionName = "";
	string functionLabel = symLookup(_func, funcLibrary)._label;
	if (functionLabel == "")
	{
		functionName = Symbol_to_string(_func);
	} else {
		functionName = functionLabel;
	}
	
	string SPcount = str(3+1+std::max(symLookup(_func, funcLibrary)._numParam,symLookup(_func, funcLibrary)._numReturn));
	// if (_args != 0) my_code = _args->HERA_code();
	my_code += "// --------- now im calling " + functionName + " -----\n";

	// my_code += indent1 + "MOVE(R3, FP_alt)\n";
	
	my_code += indent1 + "MOVE(FP_alt, SP)\n";
	my_code += indent1 + "INC(SP," + SPcount + ")\n";
	// my_code += indent1 + "STORE(R3, 2, FP_alt)\n";

	// auto func = [](A_exp e, int i) { 
	// 	string code = "";
	// 	code += e->HERA_code();
	// 	code += indent1 + "STORE(" + e->result_reg_s() +"," + str(3+i) + ",FP_alt)\n";
	// 	return code;
	// };
	
	// my_code += goOverList(_args, func);
	my_code += argsHelper(_args, 3);

	my_code += indent1 + "STORE(FP, 2, FP_alt)\n";
	for (int i = 0; i < SL; i++)
	{
		if (i == 0) my_code += "LOAD(Rt, 2, FP)\n";
		else my_code += "LOAD(Rt, 2, Rt)\n";
		my_code += "STORE(Rt, 2, FP_alt)\n ";
	}
	my_code += indent1 + "CALL(FP_alt," + functionName + ")\n";
	my_code += indent1 + "LOAD(" + this->result_reg_s() + ",3,FP_alt)\n";
	
	// my_code += indent1 + "LOAD(FP_alt, 2, FP_alt) // restore FP_alt to where it was \n";
	
	my_code += indent1 + "DEC(SP," + SPcount + ")\n";
	my_code += "// ------- called " + functionName + "  -------\n";

	return my_code;

	// if (str(_func).compare("printint") == 0) {
	// 	my_code += indent1 + "MOVE(R1," + _args->result_reg_s() + ")\n";
	// 	// return _args->_head->HERA_code() + indent1 + "MOVE(R1," + _args->_head->result_reg_s() + ")\n" + my_code;
	// } else {
	// 	my_code += indent1 + "MOVE(R1," + _args->result_label() + ")\n";
	// }
}



string A_ifExp_::HERA_code()
{
	string my_code = "";
	my_code += _test->HERA_code();
	my_code += indent1 + "CMP(" + _test->result_reg_s() + ",R0)\n";
	my_code += "BZ(" + this->result_label() + ")\n";
	my_code += _then->HERA_code();
	my_code += easy("MOVE",this->result_reg_s(), _then->result_reg_s());
	my_code += "BR(end" + this->result_label() + ")\n";
	my_code += "LABEL(" + this->result_label() + ")\n";
	if (_else_or_null != 0)
	{
		my_code += _else_or_null->HERA_code();
		my_code += easy("MOVE",this->result_reg_s(), _else_or_null->result_reg_s());
	}
	my_code += "LABEL(end" + this->result_label() + ")\n";
	return my_code;
}

string A_whileExp_::HERA_code()
{
	string my_code = "";
	my_code += "LABEL(begin_" + this->result_label() + ")\n";
	my_code += _test->HERA_code();
	my_code += indent1 + "CMP(" + _test->result_reg_s() + ",R0)\n";
	my_code += "BZ(" + this->result_label() + ")\n";
	my_code += _body->HERA_code();
	my_code += easy("MOVE",this->result_reg_s(), _body->result_reg_s());
	my_code += "BR(begin_" + this->result_label() + ")\n";
	my_code += "LABEL(" + this->result_label() + ")\n";
	return my_code;
}


string A_forExp_::HERA_code()
{
	string my_code = "";

	FP_counter = FPstart(this);

	string r1 = str(FP_counter+0);
	string r2 = str(FP_counter+1);
	

	my_code += "// ------- i'm a for " + this->result_label() +" loop ------ \n";
	my_code += "INC(SP,2)\n";

	my_code += _lo->HERA_code();
	my_code += indent1 + "STORE(" + _lo->result_reg_s() + "," + r1 +",FP)\n";
	my_code += _hi->HERA_code();
	my_code += indent1 + "STORE(" + _hi->result_reg_s() + "," + r2 +",FP)\n";

	ST<var_info> outVarLibrary = varLibrary;
	varLibrary = merge(ST<var_info>(_var, var_info(Ty_Int(), FP_counter+0, false)), varLibrary);

	my_code += indent1 + "LABEL(begin_" + this->result_label() + ")\n";
	my_code += indent1 + "LOAD(R1," + r1 +",FP)\n";
	my_code += indent1 + "LOAD(R2," + r2 +",FP)\n";
	my_code += indent1 + "CMP(R2,R1)\n";
	my_code += indent1 + "BL(" + this->result_label() + ")\n";
	
	// FP_counter += 2;

	my_code += indent1 + _body->HERA_code();
	my_code += indent1 + "LOAD(R1, " + r1 + ", FP)\n";
	my_code += indent1 + "INC(R1, 1)\n";
	my_code += indent1 + "STORE(R1, " + r1 + ", FP)\n";
	my_code += indent1 + "BR(begin_" + this->result_label() + ")\n";
	my_code += indent1 + "LABEL(" + this->result_label() + ")\n";
	my_code += "DEC(SP,2)\n";
	my_code += "// ------- i'm end of for " + this->result_label() +" loop ------ \n";


	// FP_counter -= 2;

	varLibrary = outVarLibrary;
	return my_code;
}

string A_breakExp_::HERA_code()
{
	string my_code = indent1 + "BR(" + this->result_label() + ")\n";
	return my_code;
}

string A_seqExp_::HERA_code()
{
	if (_seq == 0) return "";
	
	return _seq->HERA_code() 
		+ indent1 + "MOVE("+this->result_reg_s()+","+_seq->result_reg_s()+")\n";
}


string A_expList_::HERA_code()
{
	// auto func = [](A_exp e, int i) { 
	// 	return e->HERA_code(); 
	// };
	// return goOverList(this, func, "", 0);

	if (_tail == 0)
	{
		return _head->HERA_code();
	} else {
		return _head->HERA_code()+_tail->HERA_code();
	}
}
string A_varExp_::HERA_code()
{
	return _var->HERA_code();
}

string A_simpleVar_::HERA_code()
{

	string my_code = "";
	int SL = 0;
	int FPlocation = 0;
	string loadFrom = "FP";
	var_info* currentVar = NULL;
	if (is_name_there(_sym, varLibrary)) {
		currentVar = &(lookup(_sym, varLibrary)); 
		FPlocation = currentVar->save_FP;
	} else {
		std::tie(SL, currentVar) = this->findVarFP(0, this->_sym);
		FPlocation = currentVar->save_FP;
		if (SL != -1)
		{
			for (int i = 0; i < SL; i++)
			{
				if (i == 0) my_code += "LOAD(R3, 2, FP)\n";
				else my_code += "LOAD(R3, 2, R3)\n";
			}
			loadFrom = "R3";
		} else {
		EM_error("your variable " + Symbol_to_string(_sym) + " not in library(simpleVar HERAcode)");	
		}
	}
	
	string inAssignReg = this->inAssign(this);
	if (inAssignReg != "none")
	{
		//assign/update value for the variable
		if (!currentVar->write) EM_error("don't try to write " + Symbol_to_string(_sym) + " !");
		my_code += indent1 + "STORE(" + inAssignReg + "," 
				+ str(FPlocation) + "," + loadFrom + ")\n";
	} else {
		//use the variable
		my_code += indent1 + "LOAD(" + this->result_reg_s() + "," 
				+ str(FPlocation) + "," + loadFrom + ")\n";
	}

	return my_code;
}

string A_letExp_::HERA_code()
{	
	// EM_error(this->print_rep(2,false));
	string my_code = "";
	my_code += "// --------- let -----------\n";

	int oldFP = FP_counter;
	int myFPcounter = _decs->FPstart(this);
	if (myFPcounter > 0) my_code += "INC(SP," + str(myFPcounter) + ")\n";

	ST<var_info> outLibrary = varLibrary;
	ST<sym_info> outFuncLibrary = funcLibrary;


	my_code += _decs->HERA_code();
	my_code += "// --------- in ------------\n";
	my_code += _body->HERA_code();
	my_code += indent1+"MOVE("+this->result_reg_s()+","+_body->result_reg_s()+")\n";
	my_code += "// --------- end -----------\n";
	
	varLibrary = outLibrary;
	funcLibrary = outFuncLibrary;

	if (myFPcounter > 0) my_code += "DEC(SP," + str(myFPcounter) + ")\n";
	FP_counter = oldFP;
	return my_code;
}

string A_dec_::HERA_code()
{
	string my_code = "";

	return my_code;
}

string A_varDec_::HERA_code()
{
	//update the variable libary in my closest parent let
	ST<var_info>* letLib = this->myLetVarLibrary();
	

	string my_code = "";
	int myFPcounter = this->FPstart(this);

	if (this->FPstart(this) != 0) {
		FP_counter = this->FPstart(this);
	}
	
	my_code += _init->HERA_code();
	my_code += indent1+"STORE("+_init->result_reg_s()+","+str(myFPcounter)+",FP)\n";

	if (Symbol_to_string(this->_typ) == "none") this->_typ = typeToSymbol(this->_init->typecheck());
	varLibrary = merge(ST<var_info>(_var, 
									var_info(symbolToType(_typ), 
									myFPcounter)), 
									varLibrary);
	*letLib = merge(ST<var_info>(_var, 
									var_info(symbolToType(this->_typ), 
									myFPcounter)), 
									*letLib);
	return my_code;
}

// std::tuple<string, int> goOverDecList_code(A_decList d, string previous, int countFP) {
// 	if (d == 0) return std::make_tuple(previous, countFP);
// 	assert(d != 0);

// 	string my_code = previous + d->_head->HERA_code();
// 	if (d->_tail != 0) return goOverDecList_code(d->_tail, my_code, countFP+1);
// 	return std::make_tuple(my_code, countFP+1);
// }

string A_decList_::HERA_code()
{
	string my_code = "";
	// int countFP = 0;
	// std::tie(my_code, countFP) = goOverDecList_code(this, "", countFP);

	// this->my_FP = countFP;
	if (_tail == 0) my_code = _head->HERA_code();
	else { my_code = _head->HERA_code(); my_code += _tail->HERA_code(); }
	return my_code;
}

string A_assignExp_::HERA_code()
{
	string my_code = "";
	my_code += _exp->HERA_code();
	my_code += _var->HERA_code();

	return my_code;
}

string A_functionDec_::HERA_code()
{
	this->goOverFunctions();
	return this->theFunctions->HERA_code();
}

string A_fundec_::HERA_code()
{
	string my_code = "";
	string myFunctionName = this->result_label();
	my_code += "// user defining " + myFunctionName + " ------\n";
	my_code += "LABEL(" + myFunctionName + ")\n";
	my_code += easy("STORE", "0", "FP", "PC_ret");
	my_code += easy("STORE", "1", "FP", "FP_alt");

	my_code += "INC(SP,6)\n";
	for (int i = 0; i < 5; i++)
	{
		my_code += indent1+"STORE(R"+str(4+i)+","+str(3+this->_paramsLen+i+1)+",FP)\n";
	}
	

	ST<var_info> oldVarLibrary = varLibrary;
	varLibrary = this->myVarLibrary; //only allow to use parameters as variable

	my_code += _body->HERA_code();
	varLibrary = oldVarLibrary;
	my_code += easy("STORE", "3", "FP", _body->result_reg_s());

	for (int i = 0; i < 5; i++)
	{
		my_code += indent1+"LOAD(R"+str(4+i)+","+str(3+this->_paramsLen+i+1)+",FP)\n";
	}
	my_code += "DEC(SP,6)\n";
	my_code += easy("LOAD", "0", "FP", "PC_ret");
	my_code += easy("LOAD", "1", "FP", "FP_alt");
	my_code += easy("RETURN", "FP_alt", "PC_ret");
	functionsData += my_code;
	return "";
}

string A_fundecList_::HERA_code()
{
	string my_code = "";

	my_code += _head->HERA_code();
	if (_tail != 0) my_code += _tail->HERA_code();

	return my_code;
}