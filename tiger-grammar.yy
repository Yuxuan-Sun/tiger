// Note: to learn how to invoke bison/flex in Eclipse, see https://www.eclipse.org/forums/index.php/t/1075215/
// And refer to the example at https://www.gnu.org/software/bison/manual/html_node/A-Complete-C_002b_002b-Example.html#A-Complete-C_002b_002b-Example

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.2"
%defines
%define api.parser.class {tigerParser}

/* next two from https://www.gnu.org/software/bison/manual/html_node/Complete-Symbols.html#Complete-Symbols */
%define api.value.type variant
%define api.token.constructor

%code requires
{
class tigerParseDriver;
}
%param { tigerParseDriver& driver }

// According to the Example, this turns on "location tracking"
%locations
// Thanks to Ian Fisher HC '19 for explaining the details of the parts I'd failed to get working


%{
#include "tigerParseDriver.h"
#include "helper.h"
%}

%token END  0
%token <bool> TRUE FALSE
%token <int>  INT
%token <std::string> ID STRING TYPE
%token LPAREN RPAREN
%token LCOMMENT RCOMMENT
%token SEMICOLON COMMA

// NOTE that bison complains if you have the same symbol listed as %token (above) and %type (below)
//      so if you want to add attributes to a token, remove it from the list below

%token 
  COLON L_SQUARE_BRACKET R_SQUARE_BRACKET 
  L_CURLY_BRACE R_CURLY_BRACE
  ARRAY TYPE_DEC IF WHILE FOR TO DO LET IN END_LET OF 
  BREAK NIL
  FUNCTION VAR DOT
  PLUS MINUS TIMES DIVIDE UMINUS ASSIGN EQ NEQ LT LE GT GE OR AND NOT
;

/* precedence (stickiness) ... put the stickiest stuff at the bottom of the list */



%precedence THEN
%precedence ELSE
%precedence DO
%left OR
%left AND
%left EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%left UMINUS NOT

/* Attributes types for nonterminals are next, e.g. struct's from tigerParseDriver.h */
%type <expAttrs>  exp
%type <expListAttrs> expList
%type <expListAttrs> seqList
%type <varAttrs> var

%type <decAttrs> dec
%type <decAttrs> vardec
%type <decListAttrs> vardecList
%type <decListAttrs> decList

%type <fieldAttrs> tyfield
%type <fieldListAttrs> tyfieldList
%type <fundecAttrs> fundec
%type <fundecListAttrs> fundecList
%type <tyAttrs> ty


// The line below means our grammar must not have conflicts
//  (no conflicts means it is "an LALR(1) grammar",
//   meaning it must be unambiguous and have some other properties).
%expect 15


%%

%start program;
program: exp[main]	{ EM_debug("Got the main expression of our tiger program.", $main.AST->pos());
		 			  driver.AST = new A_root_($main.AST);
		 			}
	;

exp:  INT[i]					{ $$.AST = A_IntExp(Position::fromLex(@i), $i);
								//   $$.type = Ty_Int();
								  $$.height = 1;
								  EM_debug("Got int " + str($i), $$.AST->pos());
								}
	| STRING[i]					{ $$.AST = A_StringExp(Position::fromLex(@i), $i);
								  // $$.type = Ty_String();
								  EM_debug("Got str " + $i, $$.AST->pos());
								}
| TRUE[t]
{
	$$.AST = A_BoolExp(Position::fromLex(@t),true);
	// $$.type = Ty_Bool();
	EM_debug("Got bool ", $$.AST->pos());
}
| FALSE[f]
{
	$$.AST = A_BoolExp(Position::fromLex(@f),false);
	// $$.type = Ty_Bool();
	EM_debug("Got bool ", $$.AST->pos());
}

| exp[exp1] PLUS exp[exp2]	
{ 
	$$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
					A_plusOp,  $exp1.AST,$exp2.AST);
	// $$.type = Ty_Int();
	//   if (! ($exp1.type == Ty_Int() & $exp2.type == Ty_Int())) EM_error("Wait, + needs integer operands", false, $$.AST->pos());
	EM_debug("Got plus expression.", $$.AST->pos());
}

| exp[exp1] MINUS exp[exp2]	{ $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
												   A_minusOp, $exp1.AST,$exp2.AST);
								  // $$.type = Ty_Int();
								//   if (! ($exp1.type == Ty_Int() & $exp2.type == Ty_Int())) EM_error("Wait, - needs integer operands", false, $$.AST->pos());
								  EM_debug("Got subtraction expression.", $$.AST->pos());
								}
| MINUS exp[exp1] %prec UMINUS
{
	$$.AST = A_OpExp($exp1.AST->pos(), A_minusOp, 
				A_IntExp(Position::undefined(), 0),$exp1.AST);
	// $$.type = Ty_Int();
	// if (! ($exp1.type == Ty_Int())) EM_error("Wait, - needs integer operands", false, $$.AST->pos());
	EM_debug("Got uminus expression.", $$.AST->pos());
}

| exp[exp1] TIMES exp[exp2]	
{ 
	$$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
						A_timesOp, $exp1.AST,$exp2.AST);
	// $$.type = Ty_Int();
	// if (! ($exp1.type == Ty_Int() & $exp2.type == Ty_Int())) EM_error("Wait, * needs integer operands", false, $$.AST->pos());
	$$.height = ($exp1.height>$exp2.height) ?($exp1.height+1) : ($exp2.height+1);
	EM_debug("Got times expression.", $$.AST->pos());
}

| exp[exp1] DIVIDE exp[exp2]
{
	// $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
	// 				A_divideOp, $exp1.AST,$exp2.AST);
	$$.AST = A_CallExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
						to_Symbol("div"), A_ExpList($exp1.AST,A_ExpList($exp2.AST,0)));

}
| exp[exp1] MOD exp[exp2]
{
	// $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
	// 				A_modOp, $exp1.AST,$exp2.AST);
	// EM_debug("Got mod expression.", $$.AST->pos());
		$$.AST = A_CallExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
						to_Symbol("mod"), A_ExpList($exp1.AST,A_ExpList($exp2.AST,0)));
}



| exp[exp1] LT exp[exp2]
{
	$$.AST = A_CompExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
					A_ltOp, $exp1.AST,$exp2.AST);
}
| exp[exp1] LE exp[exp2]
{
	$$.AST = A_CompExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
					A_leOp, $exp1.AST,$exp2.AST);
}
| exp[exp1] GT exp[exp2]
{
	$$.AST = A_CompExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
					A_gtOp, $exp1.AST,$exp2.AST);
}
| exp[exp1] GE exp[exp2]
{
	$$.AST = A_CompExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
					A_geOp, $exp1.AST,$exp2.AST);
}

| exp[exp1] EQ exp[exp2]
{
	Position p = Position::range($exp1.AST->pos(), $exp2.AST->pos());
	$$.AST = A_CompExp(p,
					// A_andOp, 
					A_eqOp,
					$exp1.AST,
					$exp2.AST);
}

| exp[exp1] NEQ exp[exp2]
{
	Position p = Position::range($exp1.AST->pos(), $exp2.AST->pos());
	$$.AST = A_CompExp(p,
					// A_orOp, 
					A_neqOp,
					$exp1.AST,
					$exp2.AST);
}

| NOT exp[exp1] %prec NOT
{
	$$.AST = A_IfExp($exp1.AST->pos(), 
	$exp1.AST, A_BoolExp($exp1.AST->pos(), false), 
	A_BoolExp($exp1.AST->pos(), true));
}
| exp[exp1] LT GT exp[exp2]
{
	Position p = Position::range($exp1.AST->pos(), $exp2.AST->pos());
	$$.AST = A_CompExp(p,
					// A_andOp, 
					A_neqOp,
					$exp1.AST,
					$exp2.AST);
}

| exp[exp1] AND exp[exp2]	
{ 
	$$.AST = A_IfExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
				$exp1.AST,  $exp2.AST, A_BoolExp($exp2.AST->pos(), false));
}
| exp[exp1] OR exp[exp2]	
{ 
	$$.AST = A_IfExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
	$exp1.AST, A_BoolExp($exp2.AST->pos(), true), $exp2.AST);
}



| LPAREN[lp] seqList[list1] RPAREN[rp]
{ 
	Position p = Position::range(Position::fromLex(@lp), Position::fromLex(@rp));
	$$.AST = A_SeqExp(p, $list1.AST);
}

/* | ID[id] LPAREN expList[list1] RPAREN[rp]
{ 
	$$.AST = A_CallExp(Position::range(Position::fromLex(@id), Position::fromLex(@rp)), 
						to_Symbol($id), $list1.AST);
} */
| ID[id] LPAREN RPAREN[rp]
{ 
	if (is_name_there(to_Symbol($id), stdFuncLibrary)) {
		if (symLookup(to_Symbol($id), stdFuncLibrary)._numParam != 0) {
			EM_error("parameter number wrong");
		}
	}
	$$.AST = A_CallExp(Position::range(Position::fromLex(@id), Position::fromLex(@rp)), 
						to_Symbol($id), 0);
}
| ID[id] LPAREN exp[exp1] RPAREN[rp]
{ 
	if (is_name_there(to_Symbol($id), stdFuncLibrary)) {
		if (symLookup(to_Symbol($id), stdFuncLibrary)._numParam != 1) {
			EM_error("parameter number wrong");
		}
	}
	Position P = Position::range(Position::fromLex(@id), Position::fromLex(@rp));
	$$.AST = A_LetExp(P,
			A_DecList(A_VarDec(P, to_Symbol("firstParam"), to_Symbol("none"), $exp1.AST), 0), 
			A_ExpList(A_CallExp(P, 
						to_Symbol($id),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("firstParam"))),0)),0));
}
| ID[id] LPAREN exp[exp1] COMMA exp[exp2] RPAREN[rp]
{ 
	if (is_name_there(to_Symbol($id), stdFuncLibrary)) {
		if (symLookup(to_Symbol($id), stdFuncLibrary)._numParam != 2) {
			EM_error("parameter number wrong");
		}
	}
	Position P = Position::range(Position::fromLex(@id), Position::fromLex(@rp));
	$$.AST = A_LetExp(P,
			A_DecList(A_VarDec(P, to_Symbol("firstParam"), to_Symbol("none"), $exp1.AST),
			A_DecList(A_VarDec(P, to_Symbol("secondParam"), to_Symbol("none"), $exp2.AST), 0)), 
			A_ExpList(A_CallExp(P, 
						to_Symbol($id),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("firstParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("secondParam"))),0))),0));

}
| ID[id] LPAREN exp[exp1] COMMA exp[exp2] COMMA exp[exp3] RPAREN[rp]
{ 
	if (is_name_there(to_Symbol($id), stdFuncLibrary)) {
		if (symLookup(to_Symbol($id), stdFuncLibrary)._numParam != 3) {
			EM_error("parameter number wrong");
		}
	}
	Position P = Position::range(Position::fromLex(@id), Position::fromLex(@rp));
	$$.AST = A_LetExp(P,
			A_DecList(A_VarDec(P, to_Symbol("firstParam"), to_Symbol("none"), $exp1.AST),
			A_DecList(A_VarDec(P, to_Symbol("secondParam"), to_Symbol("none"), $exp2.AST),
			A_DecList(A_VarDec(P, to_Symbol("thirdParam"), to_Symbol("none"), $exp3.AST), 0))), 
			A_ExpList(A_CallExp(P, 
						to_Symbol($id),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("firstParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("secondParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("thirdParam"))),0)))),0));

}
| ID[id] LPAREN exp[exp1] COMMA exp[exp2] COMMA exp[exp3] COMMA exp[exp4] RPAREN[rp]
{ 
	Position P = Position::range(Position::fromLex(@id), Position::fromLex(@rp));
	$$.AST = A_LetExp(P,
			A_DecList(A_VarDec(P, to_Symbol("firstParam"), to_Symbol("none"), $exp1.AST),
			A_DecList(A_VarDec(P, to_Symbol("secondParam"), to_Symbol("none"), $exp2.AST),
			A_DecList(A_VarDec(P, to_Symbol("thirdParam"), to_Symbol("none"), $exp3.AST),
			A_DecList(A_VarDec(P, to_Symbol("fourthParam"), to_Symbol("none"), $exp4.AST), 0)))), 
			A_ExpList(A_CallExp(P, 
						to_Symbol($id),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("firstParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("secondParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("thirdParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("fourthParam"))),0))))),0));

}
| ID[id] LPAREN exp[exp1] COMMA exp[exp2] COMMA exp[exp3] COMMA exp[exp4] COMMA exp[exp5] RPAREN[rp]
{ 
	Position P = Position::range(Position::fromLex(@id), Position::fromLex(@rp));
	$$.AST = A_LetExp(P,
			A_DecList(A_VarDec(P, to_Symbol("firstParam"), to_Symbol("none"), $exp1.AST),
			A_DecList(A_VarDec(P, to_Symbol("secondParam"), to_Symbol("none"), $exp2.AST),
			A_DecList(A_VarDec(P, to_Symbol("thirdParam"), to_Symbol("none"), $exp3.AST),
			A_DecList(A_VarDec(P, to_Symbol("fourthParam"), to_Symbol("none"), $exp4.AST),
			A_DecList(A_VarDec(P, to_Symbol("fifthParam"), to_Symbol("none"), $exp5.AST), 0))))), 
			A_ExpList(A_CallExp(P, 
						to_Symbol($id),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("firstParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("secondParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("thirdParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("fourthParam"))),
						A_ExpList(A_VarExp(P, A_SimpleVar(P, to_Symbol("fifthParam"))),0)))))),0));

}
/* | ID[id] LPAREN expList[list1] RPAREN[rp]
{ 
	$$.AST = A_CallExp(Position::range(Position::fromLex(@id), Position::fromLex(@rp)), 
						to_Symbol($id), $list1.AST);
} */
| MOD LPAREN[lp] expList[list1] RPAREN[rp]
{ 
	$$.AST = A_CallExp(Position::range(Position::fromLex(@lp), Position::fromLex(@rp)), 
						to_Symbol("mod"), $list1.AST);
}

| IF exp[exp1] THEN exp[exp2] ELSE exp[exp3]
{
	$$.AST = A_IfExp(Position::range($exp1.AST->pos(), $exp3.AST->pos()), 
					$exp1.AST, $exp2.AST, $exp3.AST);
	EM_debug("got ifelsethen");
}
| IF exp[exp1] THEN exp[exp2]
{
	$$.AST = A_IfExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
					$exp1.AST, $exp2.AST, 0);
	EM_debug("got ifthen");
}
| WHILE exp[exp1] DO exp[exp2]
{
	$$.AST = A_WhileExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), 
					$exp1.AST, $exp2.AST);
	EM_debug("got while");
}
| BREAK[b]
{
	$$.AST = A_BreakExp(Position::fromLex(@b));
	EM_debug("got a break");
}
| FOR ID[id] ASSIGN exp[exp1] TO exp[exp2] DO exp[exp3]
{
	$$.AST = A_ForExp(Position::range($exp1.AST->pos(), $exp3.AST->pos()), 
						to_Symbol($id), $exp1.AST, $exp2.AST, $exp3.AST);
	EM_debug("got for expression");
}

| LPAREN exp[exp1] SEMICOLON seqList[list1] RPAREN
{
	$$.AST = A_SeqExp(Position::range($exp1.AST->pos(), $list1.AST->pos()),
					A_ExpList($exp1.AST,$list1.AST));
}
| var[id]
{
	$$.AST = A_VarExp(Position::fromLex(@id), $id.AST);
}
| LET decList[declist1] IN seqList[seqlist1] END_LET
{
	$$.AST = A_LetExp(Position::range($declist1.AST->pos(), $seqlist1.AST->pos()),
						$declist1.AST, $seqlist1.AST);
}
|
var[var1] ASSIGN exp[exp1]
{
	$$.AST = A_AssignExp(Position::range($var1.AST->pos(), $exp1.AST->pos()),
						$var1.AST, $exp1.AST);
}
;
		

var:
ID[id]					
{ 
	$$.AST = A_SimpleVar(Position::fromLex(@id), to_Symbol($id));
}



seqList: 
exp[exp1] SEMICOLON seqList[exp2] 
{
	$$.AST = A_ExpList($exp1.AST,$exp2.AST);
}
| exp[exp1]
{
	$$.AST = A_ExpList($exp1.AST,0);
}
|  /* empty */
{
	$$.AST = 0;
}
;

expList:
exp[exp1]
{
	$$.AST = A_ExpList($exp1.AST,0);
}
| exp[exp1] COMMA expList[list1] 
{
	$$.AST = A_ExpList($exp1.AST,$list1.AST);
}
|  /* empty */
{
	$$.AST = 0;
}
;

vardec:
VAR[v] ID[id] COLON TYPE[t] ASSIGN exp[exp1]
{
	$$.AST = A_VarDec(Position::range(Position::fromLex(@v), $exp1.AST->pos()),
						to_Symbol($id), to_Symbol($t), $exp1.AST);
}
|
VAR[v] ID[id] ASSIGN exp[exp1]
{
	$$.AST = A_VarDec(Position::range(Position::fromLex(@v), $exp1.AST->pos()),
						to_Symbol($id), to_Symbol("none"), $exp1.AST);
}
;

/* vardecList:
vardec[vardec1]
{
	$$.AST = A_DecList($vardec1.AST, 0);
}
|
vardec[vardec1] vardecList[list1]
{
	$$.AST = A_DecList($vardec1.AST, $list1.AST);
} */
dec:
vardec[vardec1]
{
	$$.AST = $vardec1.AST;
}
|
fundecList[fundec1]
{
	$$.AST = A_FunctionDec($fundec1.AST->pos(), $fundec1.AST);
}
;

decList:
dec[dec1]
{
	$$.AST = A_DecList($dec1.AST, 0);
}
|
dec[dec1] decList[list1]
{
	$$.AST = A_DecList($dec1.AST, $list1.AST);
}
;


tyfield:
ID[id] COLON TYPE[t]
{
	$$.AST = A_Field(Position::range(Position::fromLex(@id), Position::fromLex(@t)),
						to_Symbol($id), to_Symbol($t));
}
;

tyfieldList:
tyfield[field1] COMMA tyfieldList[list1]
{
	$$.AST = A_FieldList($field1.AST, $list1.AST);
}
|
tyfield[field1]
{
	$$.AST = A_FieldList($field1.AST, 0);
}
|  /* empty */
{
	$$.AST = 0;
}
;


fundec:
FUNCTION ID[id] LPAREN tyfieldList[list1] RPAREN COLON TYPE[t] EQ exp[exp1]
{
	$$.AST = A_Fundec(Position::range(Position::fromLex(@id), $exp1.AST->pos()),
					to_Symbol($id), $list1.AST, to_Symbol($t), $exp1.AST);
}
|
FUNCTION ID[id] LPAREN tyfieldList[list1] RPAREN EQ exp[exp1]
{
	$$.AST = A_Fundec(Position::range(Position::fromLex(@id), $exp1.AST->pos()),
				to_Symbol($id), $list1.AST, to_Symbol("void"), $exp1.AST);
}
;

fundecList:
fundec[fun1]
{
	$$.AST = A_FundecList($fun1.AST, 0);
}
|
fundec[fun1] fundecList[list1]
{
	$$.AST = A_FundecList($fun1.AST, $list1.AST);
}

;

%%

void
yy::tigerParser::error(const location_type& l,
          	       const std::string& m)
  {
	  EM_debug("In yy::tigerParser::error");
	  EM_error(m, true, Position::fromLex(l));
  }
