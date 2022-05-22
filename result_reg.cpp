#include "AST.h"
#include <initializer_list>
#include <algorithm>

/*
 * methods for working with "result_reg" attribute
 */


/* static int next_unique_number = 1; */
static int minReg = 4; // leave R1 empty forever for printint

int find_max_explist(A_expList list, int max = minReg) {
    // if (list == 0) return 0;
    if (list == 0) return max;
    int current = list->_head->result_reg();
    current = current > max ? current : max;

    return list->_tail == 0 ? current : find_max_explist(list->_tail, current);
    
}

int A_exp_::init_result_reg()  // generate unique numbers, starting from 1, each time this is called
{
	// for those who've taken CS355/356, this should be an atomic transaction, in a concurrent environment
	// int my_number = next_unique_number;
	// next_unique_number = next_unique_number + 1;
	// // end of atomic transaction
	// return my_number;
    EM_error("shouldn't call ur default init_result_reg!");
    EM_error(repr(this));
    return -1;
}

int A_leafExp_::init_result_reg()
{
    // if (value == 0) return 0;
    // int my_number = next_unique_number;
    // return my_number;
    return minReg;
}

int A_intExp_::init_result_reg()
{
    if (value == 0)
    {
        return 0;
    } else {
        return minReg;
    }
    
}

// int A_stringExp_::init_result_reg()
// {
//     int my_number = next_unique_number;
//     next_unique_number += 1;
//     return my_number;
// }

int A_callExp_::init_result_reg() {
    return std::max(find_max_explist(_args,4),1);
}

int A_opExp_::init_result_reg() {
    int su1 = _left->result_reg();
    int su2 = _right->result_reg();
    if (su1 == su2) {
        return su1+1;
    } else {
        return su1 > su2 ? su1 : su2;
    }
}

int A_compExp_::init_result_reg() {
    int su1 = _left->result_reg();
    int su2 = _right->result_reg();
    if (su1 == su2) {
        return su1+1;
    } else {
        return su1 > su2 ? su1 : su2;
    }
}


int A_seqExp_::init_result_reg() {
    // if (_seq == 0) return 0;
    // return find_max_explist(_seq,4);
    return find_max_explist(_seq);
}

int A_expList_::init_result_reg() {
    if (_tail == 0){
		return _head->result_reg();
	} else {
        // next_unique_number = next_unique_number - (_head->result_reg()-3);
		return _tail->result_reg();
	}
}

int A_ifExp_::init_result_reg() {
    if (_else_or_null == 0) return std::max(_test->result_reg(),_then->result_reg());
    return std::max({_test->result_reg(),
                    _then->result_reg(),
                    _else_or_null->result_reg()}); 
                    // https://codereview.stackexchange.com/questions/26100/maximum-of-three-values-in-c
}

int A_whileExp_::init_result_reg() {
    return std::max(_test->result_reg(), _body->result_reg());
}

int A_breakExp_::init_result_reg() {
    return minReg;
}

int A_forExp_::init_result_reg() {
    return std::max({_lo->result_reg(), _hi->result_reg(), _body->result_reg()});
}

int A_varExp_::init_result_reg() {
    return _var->result_reg();
}

int A_var_::init_result_reg() {
    EM_error("don't call a_var default init_result_reg, write urs");
    return -1;
}

int A_simpleVar_::init_result_reg() {
    return minReg;
}

int A_letExp_::init_result_reg() {
    return std::max(_decs->result_reg(), _body->result_reg());
}

int A_dec_::init_result_reg() {
    EM_error("never call default dec init result reg!");
    return -1;
}

int A_varDec_::init_result_reg() {
    return _init->result_reg();
}

int goOverDecList_reg(A_decList d, int reg) {
	if (d == 0) return reg;
	assert(d != 0);

	if (d->_head->result_reg() > reg) return goOverDecList_reg(d->_tail, d->_head->result_reg());
    else return goOverDecList_reg(d->_tail, reg);
    
}

int A_decList_::init_result_reg() {
    return goOverDecList_reg(this, minReg);
}

int A_assignExp_::init_result_reg() {
    return std::max(_var->result_reg(), _exp->result_reg());
}

int A_functionDec_::init_result_reg() {
    return this->theFunctions->result_reg();
}

int A_fundecList_::init_result_reg() {
    _head->result_reg();
    if (_tail!=0) _tail->result_reg();
    return minReg;
}

int A_fundec_::init_result_reg() {
    return std::max(_paramsAndResultReg, _body->result_reg());
}