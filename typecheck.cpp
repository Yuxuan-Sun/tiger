#include "util.h"
#include "AST.h"
#include "errormsg.h"

#include "types.h"

#include "ST-2024.h"
#include "helper.h"

//
// Change this comment to describe the attributes you'll be using
//  to check types in tiger programs.
//

#define SYM_INFO_FOR_STDLIB

// void typecheck(AST_node_ *root)
// {
//  // This function should take care of doing type checking,
//  //  assuming "root" is the root of a tiger AST.
//  // It should call on some functions to define the attributes
//  //  related to type checking (and, in so doing, produce any necessary error messages).
    
// }

Ty_ty AST_node_::typecheck()  // Default used during development; could be removed in final version
{
    string message = "never call the default typecheck! implement some";
    EM_error(message);
    return Ty_Error();
}

Ty_ty A_root_::typecheck()  // Default used during development; could be removed in final version
{
    return main_expr->typecheck();
}

Ty_ty A_intExp_::typecheck()
{
    return Ty_Int();
}

Ty_ty A_stringExp_::typecheck()
{
    return Ty_String();
}

Ty_ty A_boolExp_::typecheck()
{
    return Ty_Bool();
}

Ty_ty A_nilExp_::typecheck()
{
    return Ty_Void();
}

Ty_ty A_opExp_::typecheck()
{
    if ((_left->typecheck() == Ty_Int()) && (_right->typecheck() == Ty_Int()))
    {
        return Ty_Int();
    } else {
        EM_error("operation need int at both side!" + repr(this));
        EM_error("left" + to_String(_left->typecheck()));
        EM_error("left" + to_String(_right->typecheck()));
        return Ty_Error();
    }
}

Ty_ty A_compExp_::typecheck()
{
    if (_oper == A_notOp) {
        if (_right->typecheck() != Ty_Bool()) EM_error("! only takes bool");
        return Ty_Bool();
    }
    if (((_left->typecheck() == Ty_Int()) 
            && (_right->typecheck() == Ty_Int()))
        || ((_left->typecheck() == Ty_String()) 
            && (_right->typecheck() == Ty_String()))
        || ((_left->typecheck() == Ty_Bool()) 
            && (_right->typecheck() == Ty_Bool())))
    {
        return Ty_Bool();
    } else {
        EM_error("comp need int at both side!" + repr(this->stored_parent));
        EM_error("left is \n" + to_String(_left->typecheck()));
        EM_error("right \n" + to_String(_right->typecheck()));
        return Ty_Error();
    }
}

Ty_ty A_callExp_::typecheck()
{
    if (!is_name_there(_func, funcLibrary)) EM_error("you are calling a function " + Symbol_to_string(_func) + " that does not exist!");
    
    Ty_ty func = lookup(_func, funcLibrary)._func;
    Ty_fieldList listParam = func->u.function.parameter_types;

    if (_args != 0) _args->typecheck(listParam);


    return func->u.function.return_type;
    // if (str(_func).compare("printint") == 0) 
    // {
    //  if (_args->typecheck() == Ty_Int()) return Ty_Void();
    // }
    // if (str(_func).compare("print") == 0) 
    // {
    //  if (_args->typecheck() == Ty_String()) return Ty_Void();
    // }
    // EM_error("args not match expected type");
    // return Ty_Error();
    
}

Ty_ty A_ifExp_::typecheck()
{
    if (_test->typecheck() != Ty_Bool()) 
    {
        EM_error("test need to be bool"); 
        return Ty_Error();
    }
    if (_else_or_null != 0)
    {
        if (_then->typecheck() != _else_or_null->typecheck()) 
    {
        EM_error("same type true and false!"); 
        return Ty_Error();
    }
        return _then->typecheck();
    }

    return Ty_Void();
}

Ty_ty A_whileExp_::typecheck(){
    if (_test->typecheck() != Ty_Bool()) 
    {
        EM_error("test need to be bool"); 
        return Ty_Error();
    }
    return _body->typecheck();
}

Ty_ty A_breakExp_::typecheck(){
    return Ty_Void();
}

Ty_ty A_forExp_::typecheck(){
    if ((_lo->typecheck() != Ty_Int()) or (_hi->typecheck() != Ty_Int())) {
        EM_error("lo and high must be int!!");
        return Ty_Error();
    }
    ST<var_info> outVarLibrary = varLibrary;
    varLibrary = merge(ST<var_info>(_var, var_info(Ty_Int(), -1)), varLibrary);
    _body->typecheck();
    varLibrary = outVarLibrary;
    return Ty_Void();
}

Ty_ty A_seqExp_::typecheck()
{
    if (_seq == 0) return Ty_Void();
    return _seq->typecheck();
}

Ty_ty A_expList_::typecheck(Ty_fieldList expected)
{
    if (expected == 0)
    {
        if (_tail == 0)
        {
            return _head->typecheck();
        } else {
            _head->typecheck();
            return _tail->typecheck();
        }
    } else {
        Ty_ty result = _head->typecheck();
        if (result == expected->head->ty) {
        if (_tail == 0)
        {
            return result;
        } else {
            return _tail->typecheck(expected->tail);
        }
        }
    }
    EM_error("expList typecheck failed you should not be here");
    return Ty_Error();

}

Ty_ty A_varExp_::typecheck()
{
    return _var->typecheck();
}

Ty_ty A_simpleVar_::typecheck()
{
    if (is_name_there(this->_sym, varLibrary))
    {
        return lookup(this->_sym, varLibrary)._var_type;
    } else {
        int SL = 0;
        var_info* currentVar = NULL;
        std::tie(SL, currentVar) = this->findVarFP(0, this->_sym);
        if (SL != -1) {
            return currentVar->_var_type;
        }
        else {
        EM_error("typecheck: you variable " + Symbol_to_string(this->_sym) + " doesn't exist in library yet");
        return Ty_Error();
        }
    }
    // if (is_name_there(this->_sym, stored_parent->myVarLibrary))
    // {
    //  return lookup(this->_sym, stored_parent->myVarLibrary)._var_type;
    // } else {
    //  EM_error(stored_parent->print_rep(2, true));
    //  EM_error("you variable doesn't exist in library yet(typecheck)");
    //  return Ty_Error();
    // }
    
}

Ty_ty A_letExp_::typecheck()
{
    ST<var_info> outLibrary = varLibrary;
    ST<sym_info> outFuncLibrary = funcLibrary;
    _decs->typecheck();
    // EM_error(_body->print_rep(3, false));
    Ty_ty my_type = _body->typecheck();
    varLibrary = outLibrary;
    funcLibrary = outFuncLibrary;
    return my_type;
}

Ty_ty A_dec_::typecheck()
{
    EM_error("don't call default one of A_dec!!");
    return Ty_Void();
}


Ty_ty A_varDec_::typecheck()
{
    ST<var_info>* letLib = this->myLetVarLibrary();
    if (this->alreadyChecked == -1) {
	if (is_name_there(_var, *letLib))
    {
		EM_error(stored_parent->stored_parent->print_rep(3, false));
        EM_error("type check don't declare a varialbe already in your let!");
        EM_error("you varaible is:" + Symbol_to_string(_var));
    }
	}
    
	this->alreadyChecked = 1;

    Ty_ty recordedType;
    if (Symbol_to_string(this->_typ) == "none")
    {
        this->_typ = typeToSymbol(_init->typecheck());
        recordedType = _init->typecheck();
        varLibrary = merge(ST<var_info>(this->_var, 
                                        var_info(recordedType, 
                                        -1)), 
                                        varLibrary);
        *letLib = merge(ST<var_info>(_var, 
                                    var_info(recordedType, 
                                    -1)), 
                                    *letLib);
        return recordedType;
    }
    
    recordedType = lookup(_typ, typeLibrary);
    if (recordedType == _init->typecheck()) {
        varLibrary = merge(ST<var_info>(this->_var, 
                                        var_info(recordedType, 
                                        -1)), 
                                        varLibrary);
        *letLib = merge(ST<var_info>(_var, 
                                    var_info(recordedType, 
                                    -1)), 
                                    *letLib);
        return recordedType;
    }
    
    EM_error("typecheck failed: var "+ Symbol_to_string(this->_var) + " declared type "
    + Symbol_to_string(this->_typ) + " not match the actual expression.");
    return Ty_Error();
    
}

std::tuple<Ty_ty, int> goOverDecList_type(A_decList d, int countFP) {
    if (d == 0) return std::make_tuple(Ty_Void(),countFP);
    
    d->_head->typecheck();
    return goOverDecList_type(d->_tail, countFP+1);
}


Ty_ty A_decList_::typecheck()
{
    Ty_ty my_type;
    // int countFP;
    // std::tie(my_type, countFP) = goOverDecList_type(this,0);
    //need to do this properly
    // this->my_FP = countFP;
    // EM_error(this->print_rep(3, false));
    // EM_error("\n\nFP is "+ str(this->my_FP));
    if (_tail == 0) return _head->typecheck();
    else { _head->typecheck(); _tail->typecheck(); }
    return Ty_Void();
}


Ty_ty A_assignExp_::typecheck()
{
    if (_var->typecheck() != _exp->typecheck())
    {
        EM_error("assign typecheck failed!");
        return Ty_Error();
    } else {
        return Ty_Void();
    }
    
}

Ty_ty A_functionDec_::typecheck()
{
    this->goOverFunctions();
    
    return this->theFunctions->typecheck();
}

Ty_ty A_fundec_::typecheck()
{
    
    // string myFunctionName = this->result_label();
    this->_paramsLen = symLookup(_name, funcLibrary)._numParam;
    this->_paramsAndResultReg = std::max(_paramsLen,symLookup(_name, funcLibrary)._numReturn);
    
    ST<var_info> oldVarLibrary = varLibrary;
    varLibrary = this->myVarLibrary;

    Ty_ty bodyType = _body->typecheck();

    varLibrary = oldVarLibrary;

    return lookup(_result, typeLibrary);
}

Ty_ty A_fundecList_::typecheck()
{
    Ty_ty headType = _head->typecheck();
    if (_tail != 0) return _tail->typecheck();
    return headType;
}


