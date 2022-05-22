#include <iostream>
#include <string>
#include "AST.h"

static int next_unique_number = 1;

string AST_node_::init_result_label()
{
    return "neverusethis";
}

string A_stringExp_::init_result_label()
{
    string my_label = "string"+str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

string A_opExp_::init_result_label()
{
    string my_label = "op"+str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

string A_compExp_::init_result_label()
{
    string my_label = "comp"+str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

// string A_controlExp_::init_result_label()
// {
//     string my_label = "useYourOwnLabel!";
//     return my_label;
// }

string A_ifExp_::init_result_label()
{
    string my_label = "if"+str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

string A_whileExp_::init_result_label()
{
    string my_label = "while"+str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

string A_breakExp_::init_result_label()
{
    // EM_error(stored_parent->print_rep(3, false));
    string whichLoop = stored_parent->result_label(this);

    while (! ((whichLoop.substr(0,5) == "while" ) 
            or (whichLoop.substr(0,3) == "for")))
    {
        if (stored_parent->stored_parent == 0)
        {
            EM_error("your break is not inside any loop!!!!");
        }
         AST_node_ *oldparent = stored_parent;
        this->set_parent_pointers_for_me_and_my_descendants(stored_parent->stored_parent);
        whichLoop = stored_parent->result_label(oldparent);
    }

    return whichLoop;
    
    // return stored_parent->result_label();
}

string A_forExp_::result_label(AST_node_ *who) {
   
    if ((who == _lo) or (who == _hi))
    {
        EM_error("don't ask me break from lo or hi");
    }
    

    if (this->stored_result_label.compare("-1") == 0)   
		{
			this->stored_result_label = this->init_result_label();
		}
		return this->stored_result_label;

}

string A_forExp_::init_result_label()
{
    string my_label = "for" + str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}

string A_fundec_::init_result_label()
{
    string my_label = Symbol_to_string(this->_name) + str(next_unique_number);
    next_unique_number += 1;
    return my_label;
}