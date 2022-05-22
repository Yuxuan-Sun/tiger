#include "AST.h"
#include "helper.h"

void A_leafExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
}

void A_root_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// This has been inlined into the root expression constructor,
	//   so it shouldn't actually be needed again...
	EM_error("Strange ... called set_parent_pointers_for_me_and_my_descendants for A_root, rather than relying on constructor");
	// otherwise, we would have done this:
	// assert(my_parent_or_null_if_i_am_the_root == 0);
	// stored_parent = my_parent_or_null_if_i_am_the_root;
	// main_expr->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_opExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// record my parent
	stored_parent = my_parent_or_null_if_i_am_the_root;
	// now, tell my children to record me as theirs... they'll tell the grandkids
	_right->set_parent_pointers_for_me_and_my_descendants(this);
	 _left->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_compExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// record my parent
	stored_parent = my_parent_or_null_if_i_am_the_root;
	// now, tell my children to record me as theirs... they'll tell the grandkids
	_right->set_parent_pointers_for_me_and_my_descendants(this);
	 _left->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_callExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
	if (_args != 0) {
		_args->set_parent_pointers_for_me_and_my_descendants(this);
	}
}



void A_expList_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_head->set_parent_pointers_for_me_and_my_descendants(this);	
	if (_tail != 0)
	{
		_tail->set_parent_pointers_for_me_and_my_descendants(this);	
	}
}

void A_ifExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {

	stored_parent = my_parent_or_null_if_i_am_the_root;
	_test->set_parent_pointers_for_me_and_my_descendants(this);	
	_then->set_parent_pointers_for_me_and_my_descendants(this);	
	if (_else_or_null != 0) {
		_else_or_null->set_parent_pointers_for_me_and_my_descendants(this);	
	}
}

void A_seqExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	if (_seq != 0) {
		_seq->set_parent_pointers_for_me_and_my_descendants(this);
	}
}


void A_whileExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_test->set_parent_pointers_for_me_and_my_descendants(this);	
	_body->set_parent_pointers_for_me_and_my_descendants(this);	
}

void A_breakExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
}

void A_forExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_lo->set_parent_pointers_for_me_and_my_descendants(this);	
	_hi->set_parent_pointers_for_me_and_my_descendants(this);	
	_body->set_parent_pointers_for_me_and_my_descendants(this);	
}
void A_varExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_var->set_parent_pointers_for_me_and_my_descendants(this);
}
void A_simpleVar_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
}

void A_letExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_decs->set_parent_pointers_for_me_and_my_descendants(this);
	_body->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_decList_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_head->set_parent_pointers_for_me_and_my_descendants(this);
	if (_tail != 0) {
	_tail->set_parent_pointers_for_me_and_my_descendants(this);
	}
}

void A_dec_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
}

void A_varDec_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root) {
	stored_parent = my_parent_or_null_if_i_am_the_root;
	_init->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_assignExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
	_var->set_parent_pointers_for_me_and_my_descendants(this);
	_exp->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_functionDec_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
	theFunctions->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_fundecList_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
	_head->set_parent_pointers_for_me_and_my_descendants(this);
	if (_tail != 0) _tail->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_fundec_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
	if (_params != 0) _params->set_parent_pointers_for_me_and_my_descendants(this);
	_body->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_fieldList_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
	_head->set_parent_pointers_for_me_and_my_descendants(this);
	if (_tail != 0) _tail->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_field_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent) {
	stored_parent = my_parent;
}



// The following should never be called, but this "general" version
//   lets us compile and test some things without having to fill in the set_parent_pointers stuff first
// Better design would use "= 0" in the AST_node_ class, and not have this function body,
//   except that we want to leave the bulk of the work for the labs...


void AST_node_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent)
{
	EM_warning("Uh-oh, need to make set_parent_pointers_for_me_and_my_descendants actually do its full job...");
	EM_warning(" rewrite or overrride it, instead of running this code the AST_node_ class.");
}



AST_node_ *AST_node_::get_parent_without_checking()  // NOT FOR GENERAL USE ... this is only for things like checking assertions
{
	return stored_parent;
}
AST_node_ *AST_node_::parent()	// get the parent node, after the 'set parent pointers' pass
{
	assert("parent pointers have been set" && stored_parent);
	return stored_parent;
}

AST_node_ *A_root_::parent()
{
	EM_error("Called parent() for root node. This typically happens when A_root has not defined a method for some inherited attribute.", true);
	throw "Oops, shouldn't get here, if 'true' is on for 'is this error fatal";
}





string AST_node_::inAssign(AST_node_ *child) {
	if (stored_parent == 0) return "none";
	return stored_parent->inAssign(this);
}

string A_assignExp_::inAssign(AST_node_ *child) {
	if (child == _var)
	{
		return _exp->result_reg_s();
	} return "none";
}

bool AST_node_::inBody(AST_node_ *who) {
	if (stored_parent == 0) {EM_error("you asked you in body but you are not in let?"); return true;}
	return stored_parent->inBody(this);
}

bool A_letExp_::inBody(AST_node_ *who) {
	if (who == _body) return true;
	return false;
}

int AST_node_::FPstart(AST_node_ *who) {
	if (stored_parent == 0) return 0;
	return stored_parent->FPstart(this);
}

int A_forExp_::FPstart(AST_node_ *who) {
	if (who == _body) return 2 + stored_parent->FPstart(this);
	return stored_parent->FPstart(this);
}

int A_letExp_::FPstart(AST_node_ *who) {
	if (who == _body) return _decs->FPstart(this) + stored_parent->FPstart(this);
	return stored_parent->FPstart(this);
}

int A_decList_::FPstart(AST_node_ *who) {
	if (who == stored_parent) {
		if (_tail == 0) return _head->FPstart(this);
		return _head->FPstart(this) + _tail->FPstart(this);
	}
	if (who == _head) return stored_parent->FPstart(this);
	return _head->FPstart(this) + stored_parent->FPstart(this);
}

int A_varDec_::FPstart(AST_node_ *who) {
	if (who == stored_parent) return 1;
	string test = stored_parent->print_rep(3, false);
	return stored_parent->FPstart(this);
}

int A_fundec_::FPstart(AST_node_ *who) {
	return 3 + this->_paramsLen + 6;
}




ST<var_info>* AST_node_::myLetVarLibrary() {
	if (stored_parent == 0) {EM_error("you are not in let?"); return 0;}
	return stored_parent->myLetVarLibrary();
}

ST<var_info>* A_letExp_::myLetVarLibrary() {
	return &myVarLibrary;
}

ST<sym_info>* AST_node_::myLetFuncLibrary() {
	if (stored_parent == 0) {EM_error("you are not in let?"); return 0;}
	return stored_parent->myLetFuncLibrary();
}

ST<sym_info>* A_letExp_::myLetFuncLibrary() {
	return &myFuncLibrary;
}



Symbol AST_node_::closestFunction(Symbol name) {
	if (stored_parent == 0) {EM_error("we can't find" + Symbol_to_string(name) + "!"); return 0;}
	return stored_parent->closestFunction(name);
}

// Symbol A_fundec_::closestFunction(Symbol name) {
// 	if (name == _name) return to_Symbol(this->result_label());
// 	return stored_parent->closestFunction(name);
// }





// int fundec_FP_count = 3;
void A_fundec_::init_myVarLibrary(A_fieldList p, int count) {
	if (p == 0) return;
	if (is_name_there(p->_head->_name, this->myVarLibrary)) {
		EM_error("don't name your parameter " 
				+ Symbol_to_string(p->_head->_name) 
				+" the same!");
	}
	this->myVarLibrary = merge(ST<var_info>
								(p->_head->_name, 
								var_info(symTypeLookup(p->_head->_typ), count)), this->myVarLibrary);
	// fundec_FP_count += 1;
	init_myVarLibrary(p->_tail, count+1);
}

std::tuple<int, var_info*> AST_node_::findVarFP(int count, Symbol s) {
	if (stored_parent == 0) return std::make_tuple(-1,nullptr); //null pointer: https://stackoverflow.com/questions/18217221/c-cant-initialize-a-pointer-in-a-pair-to-null
	else return stored_parent->findVarFP(count,s);
}

std::tuple<int, var_info*> A_letExp_::findVarFP(int count, Symbol s) {
	if (is_name_there(s, this->myVarLibrary)) {
		return std::make_tuple(count, &(lookup(s, this->myVarLibrary)));
	} else {
		return stored_parent->findVarFP(count, s);
	}
}

std::tuple<int, var_info*> A_fundec_::findVarFP(int count, Symbol s) {
	if (is_name_there(s, this->myVarLibrary)) {
		return std::make_tuple(count, &(lookup(s, this->myVarLibrary)));
	} else {
		return stored_parent->findVarFP(count+1, s);
	}
}





Ty_fieldList A_fieldList_::toTy_FieldList() {
	if (_tail == 0) return Ty_FieldList(_head->toTy_Field(), 0);
	return Ty_FieldList(_head->toTy_Field(), _tail->toTy_FieldList());
}

Ty_field A_field_::toTy_Field() {
	return Ty_Field(_name, symTypeLookup(_typ));
}



void A_functionDec_::goOverFunctions() {
	theFunctions->goOverFunctions();
}

void A_fundecList_::goOverFunctions() {
	_head->goOverFunctions();
	if (_tail != 0) _tail->goOverFunctions();
}

void A_fundec_::goOverFunctions() {
	string myFunctionLabel = this->result_label();

	if (this->firstGoOver == -1) {
		
		this->firstGoOver = 1;

	if (this->myLetFuncLib == nullptr) this->myLetFuncLib = this->myLetFuncLibrary();
	if (is_name_there(this->_name, *(this->myLetFuncLib)))
	{
		EM_error("don't declare a function already in your parent let!");
		EM_error("you function is:" + Symbol_to_string(this->_name));
	}

	if (this->myLetVarLib == nullptr) this->myLetVarLib = this->myLetVarLibrary();
	if (is_name_there(this->_name, *(this->myLetVarLib)))
	{
		EM_error("don't declare a function with the same name in your parent let!");
		EM_error("you function is:" + Symbol_to_string(this->_name));
	}


	this->myVarLibrary = ST<var_info>();
	this->init_myVarLibrary(this->_params, 3);

	if (_params != 0) this->_params_type = this->_params->toTy_FieldList();
	// if (is_name_there(this->_name, funcLibrary)) EM_error("declaring a function already in the library!");

	*(this->myLetFuncLib) = merge(ST<sym_info>(this->_name, sym_info(Ty_Function(
										symTypeLookup(_result), 
										_params_type), myFunctionLabel)),  
									*(this->myLetFuncLib));
	*(this->myLetVarLib) = merge(ST<var_info>(this->_name, 
									var_info(Ty_Error(), 
									-7)), 
									*(this->myLetVarLib));
	}

	funcLibrary = merge(ST<sym_info>(this->_name, sym_info(Ty_Function(
										symTypeLookup(_result), 
										_params_type), myFunctionLabel)), 
									funcLibrary);

	return;
}





