/*
 * REVISED 2024 prototype of ST.cpp (goes with ST-2024.h, ST-2024.t)
 *
 *  Allows simpler creation notation for a single-scope symbol table
 *
 * To use this file:
 *  Remove ST.cpp (or put "#if 0" at the top, and "#endif" at the end)
 *  Insert, as the __VERY_FIRST_LINE__ of ST.h:   #include "ST-2024.h"
 *  (Nothing needs to be done about ST.t, I think)
 *  Add "ST-2024" to your CMakeList.txt file, so it gets built and used.
 */

#include <logic.h>
#include "errormsg.h"
#include "ST.h"
#include "types.h"



sym_info::sym_info(Ty_ty func, string label) {
	assert(func->kind == Ty_function);
		this->_func = func;
		int numParam = 0;
		Ty_fieldList current = func->u.function.parameter_types;		
		while (current != 0)
		{
			numParam += 1;
			current = current->tail;
		}
		this->_numParam = numParam;

		if (func->u.function.return_type != Ty_Void()) {
			this->_numReturn = 1;
		}
		this->_label = label;
}

typedef ST<sym_info> func_table;
func_table funcLibrary  = func_table(
	 {
		std::pair(to_Symbol("ord"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(Ty_Field(to_Symbol("s"), Ty_String()), 0))
			 )),
		std::pair(to_Symbol("chr"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(Ty_Field(to_Symbol("i"), Ty_Int()), 0))
			 )),
		std::pair(to_Symbol("size"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(Ty_Field(to_Symbol("s"), Ty_String()), 0))
			 )),
		std::pair(to_Symbol("substring"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("first"), Ty_Int()), Ty_FieldList(
						 Ty_Field(to_Symbol("n"), Ty_Int()),
						 0
					 )))))),
		std::pair(to_Symbol("concat"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(
				 Ty_Field(to_Symbol("s1"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("s2"), Ty_String()), 
					 0
					 ))))),
		std::pair(to_Symbol("tstrcmp"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("s1"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("s2"), Ty_String()), 
					 0
					 ))))),
		// arithmetic
		std::pair(to_Symbol("div"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("num"), Ty_Int()), Ty_FieldList(
					 Ty_Field(to_Symbol("den"), Ty_Int()), 
					 0
					 ))))),
		std::pair(to_Symbol("mod"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("num"), Ty_Int()), Ty_FieldList(
					 Ty_Field(to_Symbol("den"), Ty_Int()), 
					 0
					 ))))),
		std::pair(to_Symbol("not"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		// output and input
		std::pair(to_Symbol("getchar_ord"), sym_info(
			 Ty_Function(Ty_Int(), 0))),
		std::pair(to_Symbol("putchar_ord"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("flush"), sym_info(
			Ty_Function(0,0))),
		std::pair(to_Symbol("printint"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("printbool"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("b"), Ty_Bool()),
				 0)))),
		std::pair(to_Symbol("print"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()),
				 0)))),
		std::pair(to_Symbol("println"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()),
				 0)))),
		std::pair(to_Symbol("getchar"), sym_info(
			 Ty_Function(Ty_String(), 0))),
		std::pair(to_Symbol("ungetchar"), sym_info(
			 Ty_Function(Ty_Void(), 0))),
		std::pair(to_Symbol("getline"), sym_info(
			 Ty_Function(Ty_String(), 0))),
		std::pair(to_Symbol("getint"), sym_info(
			 Ty_Function(Ty_Int(), 0))),
			//  other 
		std::pair(to_Symbol("exit"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("malloc"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("n_cells"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("free"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("address"), Ty_Int()),
				 0)))),
	}
	);
func_table stdFuncLibrary  = func_table(
	 {
		std::pair(to_Symbol("ord"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(Ty_Field(to_Symbol("s"), Ty_String()), 0))
			 )),
		std::pair(to_Symbol("chr"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(Ty_Field(to_Symbol("i"), Ty_Int()), 0))
			 )),
		std::pair(to_Symbol("size"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(Ty_Field(to_Symbol("s"), Ty_String()), 0))
			 )),
		std::pair(to_Symbol("substring"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("first"), Ty_Int()), Ty_FieldList(
						 Ty_Field(to_Symbol("n"), Ty_Int()),
						 0
					 )))))),
		std::pair(to_Symbol("concat"), sym_info(
			 Ty_Function(Ty_String(), Ty_FieldList(
				 Ty_Field(to_Symbol("s1"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("s2"), Ty_String()), 
					 0
					 ))))),
		std::pair(to_Symbol("tstrcmp"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("s1"), Ty_String()), Ty_FieldList(
					 Ty_Field(to_Symbol("s2"), Ty_String()), 
					 0
					 ))))),
		// arithmetic
		std::pair(to_Symbol("div"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("num"), Ty_Int()), Ty_FieldList(
					 Ty_Field(to_Symbol("den"), Ty_Int()), 
					 0
					 ))))),
		std::pair(to_Symbol("mod"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("num"), Ty_Int()), Ty_FieldList(
					 Ty_Field(to_Symbol("den"), Ty_Int()), 
					 0
					 ))))),
		std::pair(to_Symbol("not"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		// output and input
		std::pair(to_Symbol("getchar_ord"), sym_info(
			 Ty_Function(Ty_Int(), 0))),
		std::pair(to_Symbol("putchar_ord"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("flush"), sym_info(
			Ty_Function(0,0))),
		std::pair(to_Symbol("printint"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("printbool"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("b"), Ty_Bool()),
				 0)))),
		std::pair(to_Symbol("print"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()),
				 0)))),
		std::pair(to_Symbol("println"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("s"), Ty_String()),
				 0)))),
		std::pair(to_Symbol("getchar"), sym_info(
			 Ty_Function(Ty_String(), 0))),
		std::pair(to_Symbol("ungetchar"), sym_info(
			 Ty_Function(Ty_Void(), 0))),
		std::pair(to_Symbol("getline"), sym_info(
			 Ty_Function(Ty_String(), 0))),
		std::pair(to_Symbol("getint"), sym_info(
			 Ty_Function(Ty_Int(), 0))),
			//  other 
		std::pair(to_Symbol("exit"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("i"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("malloc"), sym_info(
			 Ty_Function(Ty_Int(), Ty_FieldList(
				 Ty_Field(to_Symbol("n_cells"), Ty_Int()),
				 0)))),
		std::pair(to_Symbol("free"), sym_info(
			 Ty_Function(Ty_Void(), Ty_FieldList(
				 Ty_Field(to_Symbol("address"), Ty_Int()),
				 0)))),
	}
	);

var_info::var_info() {
	this->_var_type = Ty_Error();
	this->save_FP = -1;
	this->write = false;
}

var_info::var_info(Ty_ty var_type, int save_FP, bool write) {
	this->_var_type = var_type;
	this->save_FP = save_FP;
	this->write = write;
}

ST<var_info> varLibrary = ST<var_info>();
ST<var_info> emptyVarLibrary = ST<var_info>();

ST<Ty_ty> typeLibrary = ST<Ty_ty>(
	{
		std::pair(to_Symbol("string"), Ty_String()),
		std::pair(to_Symbol("bool"), Ty_Bool()),
		std::pair(to_Symbol("int"), Ty_Int()),
		std::pair(to_Symbol("void"), Ty_Void())
	}
);

sym_info symLookup(Symbol s, ST<sym_info> lib) {
	if (!is_name_there(s, lib)) EM_error(Symbol_to_string(s) + " is not in library!");
	return lookup(s, lib);
}

var_info varLookup(Symbol s, ST<var_info> lib) {
	if (!is_name_there(s, lib)) EM_error(Symbol_to_string(s) + " is not in library!");
	return lookup(s, lib);
}

Ty_ty tyLookup(Symbol s, ST<Ty_ty> lib) {
	if (!is_name_there(s, lib)) EM_error(Symbol_to_string(s) + " is not in library!");
	return lookup(s, lib);
}

// ST<var_info> varLibrary = ST<var_info>({
// 		std::pair(to_Symbol("i"), var_info(
// 			 Ty_Int(), 3)),
// 		std::pair(to_Symbol("j"), var_info(
// 			 Ty_Int(), 4)),
// });

// typedef ST<var_info> var_table;
// var_table varLibrary = var_table(
// 	{
// 		std::pair(to_Symbol("x"), var_info(
// 			 Ty_Int(), 2
// 			 )),
// 		std::pair(to_Symbol("y"), var_info(
// 			 Ty_String(), 3
// 			 ))
// 	}
// );

/* Note that this is just some examples, since most C++ compilers require that the implementation of template functions be #included and thus I put it in ST.t */

// NOTE: Normally this 'struct' would be in a .h file, but we only need it within ST.cc, for examples,
//       so I'm putting it here.

struct example_sym_info { // what do we know about a symbol?
public:
	example_sym_info(int the_pos,  // record the place it was declared, for error messages
					  // other attributes could go here, e.g.
			  int the_whatever_else
		);
  // leave data public, which is the default for 'struct'
	int pos;
	int whatever_else;		

	string __repr__();
	string __str__();

};
// make an abbreviation "ST_example" for a symbol table with the example sym info
//   (also would be in a .h, usually)
typedef ST<example_sym_info> ST_example;


// Now, the methods of struct example_sym_info ... these would be in a .cc file

example_sym_info::example_sym_info(int the_pos, int the_whatever_else) : pos(the_pos)
{
	// The bit after the single ":" above initializes "pos" with the value "the_pos".
	// It's a lot like putting "pos = the_pos;" in the body,
	//  but the latter would first build a null "pos" and then re-define it.

	// Just to do something in the example, we'll ensure that whatever_else isn't negative:
	if (the_whatever_else < 0) {
		whatever_else = 0;
	} else {
		whatever_else = the_whatever_else;
	}
}
string example_sym_info::__repr__()
{
	return "position: " + repr(pos) + ", whatever else: " + repr(whatever_else);
}
string example_sym_info::__str__()
{
	return this->__repr__();
}





static int length(const char *c_style_string)
{
	return string(c_style_string).length();
}


// call ST_examples from tiger.cpp, and run tiger with "-d", if you want to see these run.
void ST_examples()
{
/*
  Show the symbol table calls that could be used for the tiger program

    let
	var wombat : int := 20
	var arthropod : int := 2
    in
	(let
	    var wombat : int := 35
	 in
	    wombat+arthropod  // should be 35+2, i.e., 37
	 end)
       +
	(let
	    var arthropod : int := 4
	 in
	    wombat/arthropod  // should be 20/4, i.e., 5
	 end)
    end

    In this example, we store as "sym info" the position and the integer values above.

    Storing more useful things (like, e.g., type information) would be done by
     creating a different type of sym_info

    Below, I need to_Symbol to turn names into type "Symbol";
     this isn't always necessary in the rest of the compiler,
     since that's how variable names are stored already in your AST.
*/

   // outside the outer let we'd have the standard library, e.g.
   ST_example outside_outer_let =
     ST_example( { std::pair(to_Symbol("print"),    example_sym_info(0,  0)),
                   std::pair(to_Symbol("printint"), example_sym_info(0,  0)),
                   std::pair(to_Symbol("div"),      example_sym_info(0,  0)),
                   std::pair(to_Symbol("mod"),      example_sym_info(0,  0))
                         // etc., etc,. etc.
     } );


   // these might be computed along the outer let's "declist"
   ST_example outer_let_decs  =
     ST_example( { std::pair(to_Symbol("wombat"),    example_sym_info(22, 20)),
                   std::pair(to_Symbol("arthropod"), example_sym_info(32, 2)) } );

   ST_example avail_in_outer_let   = MergeAndShadow(outer_let_decs, outside_outer_let);

   // and now, the inner let's...

   ST_example first_inner_let_decs       = ST_example(to_Symbol("wombat"),    example_sym_info(64, 35));
   ST_example avail_in_first_inner_let   = MergeAndShadow(first_inner_let_decs, avail_in_outer_let);

   EM_debug("Symbols available in 'wombat + arthropod' node:\n" + str(avail_in_first_inner_let));
   EM_debug("Lookup of 'wombat'    here produces: " + str(lookup(to_Symbol("wombat"), avail_in_first_inner_let)));
   EM_debug("Lookup of 'arthropod' here produces: " + str(lookup(to_Symbol("arthropod"), avail_in_first_inner_let)));


   ST_example second_inner_let_decs       = ST_example(to_Symbol("arthropod"), example_sym_info(124, 4));


   ST_example avail_in_second_inner_let  = MergeAndShadow(second_inner_let_decs, avail_in_outer_let);

   EM_debug("Symbols available in 'wombat / arthropod' node:\n" + str(avail_in_second_inner_let));
   EM_debug("Lookup of 'wombat'    here produces: " + str(lookup(to_Symbol("wombat"), avail_in_second_inner_let)));
   EM_debug("Lookup of 'arthropod' here produces: " + str(lookup(to_Symbol("arthropod"), avail_in_second_inner_let)));

   try {
	   EM_debug("Looking up 'C. Elagans' in first table: ");
	   lookup(to_Symbol("C. Elegans"), avail_in_first_inner_let);
	   EM_debug("... that's strange, we should have thrown an exception!");
   }
   catch(const ST_example::undefined_symbol &missing) {
	   EM_debug("... and sure enough, there was a problem with missing symbol named " + str(missing.name));
   }
}

void ST_test() // NOTE: This is supposed to throw&catch some exceptions, since it's testing error cases.
{
	try {
		// empty ST
		ST_example t0 = ST_example();

		// create a 1-item ST
		ST_example t1 = ST_example(to_Symbol("Dave"),example_sym_info(10, 0));
		assert(lookup(to_Symbol("Dave"), t1).pos == 10);
		try {
			lookup(to_Symbol("Pat"), t1);
			assert(false && "lookup should have failed above");
		}
		catch(ST_example::undefined_symbol missing) {
			assert(Symbols_are_equal(missing.name, to_Symbol("Pat")));
		}

		try {
			lookup(to_Symbol("Dave"), t0);
			assert(false && "lookup should have failed above");
		}
		catch(ST_example::undefined_symbol missing) {
			assert(Symbols_are_equal(missing.name, to_Symbol("Dave")));
		}

		// now try a bad fuse:
		try {
			fuse(t1, ST_example(to_Symbol("Dave"), example_sym_info(15, 0)));
			assert(false && "fuse should have failed above");
		}
		catch(ST_example::duplicate_symbol dup) {
			assert(Symbols_are_equal(to_Symbol("Dave"), dup.name));
		}


		// combine t1 and a new symbol, producing a new ST
		ST_example t12 = fuse(t1, ST_example(to_Symbol("Pat"), example_sym_info(20, 0)));
		assert(lookup(to_Symbol("Dave"), t12).pos == 10);
		assert(lookup(to_Symbol("Pat"), t12).pos == 20);

		// the above should not affect t1
		try {
			lookup(to_Symbol("Pat"), t1);
			assert(false && "lookup should have failed above");
		}
		catch(ST_example::undefined_symbol missing) {
			assert(Symbols_are_equal(missing.name, to_Symbol("Pat")));
		}

		// another ST
		ST_example t34 = fuse(ST_example(to_Symbol("Jamie"), example_sym_info(30, 0)),
							ST_example(to_Symbol("Kris"), example_sym_info(40, 0)));
		assert(lookup(to_Symbol("Jamie"), t34).pos == 30);
		assert(lookup(to_Symbol("Kris"), t34).pos == 40);

		// another ST containing to_Symbol("Pat")
		ST_example t342a = fuse(t34,ST_example(to_Symbol("Pat"),example_sym_info(25, 0)));
		assert(lookup(to_Symbol("Jamie"), t342a).pos == 30);
		assert(lookup(to_Symbol("Kris"), t342a).pos == 40);
		assert(lookup(to_Symbol("Pat"), t342a).pos == 25);

		// fuse disjoint tables 
		ST_example t1234 = fuse(t12, t34);
		assert(lookup(to_Symbol("Dave"), t1234).pos == 10);
		assert(lookup(to_Symbol("Pat"), t1234).pos == 20);
		assert(lookup(to_Symbol("Jamie"), t1234).pos == 30);
		assert(lookup(to_Symbol("Kris"), t1234).pos == 40);

		// fail to fuse non-disjoint tables
		try {
			fuse(t12, t342a);
			assert(false && "fuse should have failed above");
		}
		catch(ST_example::duplicate_symbol dup) {
			assert(Symbols_are_equal(to_Symbol("Pat"), dup.name));
		}

		// combine t12 and t342a with t342a shadowing t12
		// ST_example t12 = fuse(t1, ST_example(to_Symbol("Pat"), example_sym_info(20, 0)));
		// assert(lookup(to_Symbol("Dave"), t12).pos == 10);
		// assert(lookup(to_Symbol("Pat"), t12).pos == 20);
		ST_example t12342a = merge(t342a, t12);
		assert(lookup(to_Symbol("Dave"), t12342a).pos == 10);
		assert(lookup(to_Symbol("Pat"), t12342a).pos == 25);
		assert(lookup(to_Symbol("Jamie"), t12342a).pos == 30);
		assert(lookup(to_Symbol("Kris"), t12342a).pos == 40);

		assert(lookup(to_Symbol("Pat"), t12).pos == 20);
	}
	catch (...) {
		assert(false && "test_st got an unexpected exception");
	}
}


// IMPLEMENTATION


// For now, a symbol table can just be a list
// In fact, we can ensure that the symbols at the outer scope
//   are always further along in the list, so if we just find
//   the first symbol with the given name, it will be the one
//   at the inner scope.


