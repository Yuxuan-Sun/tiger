%{

/* In this first section of the lex file (between %{ and %}),
   we define C++ functions that will be used later in the actions of part 3 */
   
 // Dave's list of useful references for flex and bison for C++:
 //  http://www.jonathanbeard.io/tutorials/FlexBisonC++
 //  http://dinosaur.compilertools.net/flex/flex_19.html
 //  https://www.gnu.org/software/bison/manual/html_node/Split-Symbols.html

#include <stdlib.h>
#include "tigerParseDriver.h"
#include "tiger-grammar.tab.hpp"
#include "util.h"
#include "ST-2024.h"




// next line from https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Scanner.html#Calc_002b_002b-Scanner
static yy::location loc;

// The function below is somewhat overly verbose;
// printint	{ return yy::tigerParser::make_ID(yytext,loc); }
 // print {return yy::tigerParser::make_ID(yytext,loc);} 
//  it is designed to serve as an example of
//  (a) calling a C function to process something from lex (see the "INT" pattern below), and
//  (b) processing a collection of characters one at a time, relying on their ASCII values

static int textToInt(std::string the_text)  // a C-style array of char will be converted to a String
{
	// here's a C-language way of doing this
	char zero = '0';  // the character 0
	char nine = '9';
	int result = 0;

	// the length() method of class string returns an "unsigned int", so make i the same type
	for (unsigned int i=0; i<the_text.length(); i++) {
		// the_text[i] is the i'th numeral, e.g. a '4' or a '2'
		// We need to convert this to a number, such as 4 or 2,
		//  and rely on the fact that the ASCII character set
		//  has '0', then '1', then '2', etc. in numeric order.
		// So '0'-'0' gives us 0, and '1'-'0' gives us 1, etc.
		// Start by saving the current character in a variable:
		char numeral = the_text[i];
		EM_debug("numer "+numeral);
		
		// We should get here *only* if we have only numerals,
		//  since we're called only for the pattern {digit}+ below.
		//
		assert (numeral >= zero and numeral <= nine);
		
		// now convert the numeral
		int numeral_value = numeral-zero;
		
		// and update the total value
		result = 10*result+numeral_value;
	}
	return result;
	
	// alternate implementation: use hc_string's to_int function:
	// return to_int(the_text);
}

static string textToStr(std::string the_text)
{
	string result = "";

	// the length() method of class string returns an "unsigned int", so make i the same type
	for (unsigned int i=0; i<the_text.length(); i++) {
		if (i != 0 and i != the_text.length()-1) {
			if (the_text[i] == '\\') {
				if (i == the_text.length()-2) EM_error("special character at the end? not good");
				i += 1;
				switch (the_text[i])
				{
					case 'n':
						result += '\n';
						break;
					case 't':
						result += '\t';
						break;
					case '\"':
						result += '\"';
						break;
					case '\\':
						result += '\\';
						break;
					case '^':
						i += 1;
						if (the_text[i] == 'i' or the_text[i] == 'I') {
							result += '\t';
						}
						if (the_text[i] == 'j' or the_text[i] == 'J') {
							result += '\n';
						}
						break;
					default:
						char zero = '0';  // the character 0
						char nine = '9';
						if (the_text[i] >= zero and the_text[i] <= nine){
							string decStr = "";
							decStr += the_text[i];
							decStr += the_text[i+1];
							decStr += the_text[i+2];
							EM_debug("working on cahracter + string" + decStr);
							i += 2;
							char decCode = textToInt(decStr);
							EM_debug("imhere!");
							result += decCode;
							break;
						}
						break;
				}
			} else {
			result += the_text[i];
			}
		}
	}
	return repr(result);
}


// This uses some stuff created by flex, so it's easiest to just put it here.
int tigerParseDriver::parse (const std::string &f)
{
	fileName = f;

	if (fileName == "" || fileName == "-") {
		yyin = stdin;
	} else if (!(yyin = fopen (fileName.c_str (), "r"))) {
		error ("cannot open " + fileName + ".");
		exit (EXIT_FAILURE);
	}

	yy::tigerParser parser (*this);
	int res = parser.parse ();  // sets this->AST_root

	fclose (yyin);
	return res;
}

%}

/* In this second section of the lex file (after the %}),
   we can define variables in terms of regular expressions.
   A few other things, like %option, can also be used here.
   C-style comments (like this one) are also legal. */

/* options from the example */
%option noyywrap nounput
/* Not using these: batch debug noinput */


integer	[0-9]+

/* real numbers don't occur in tiger, but if they did,
   and we always wanted a "." with at least one numeral preceding it,
   we could do this: */
real	[0-9]+\.[0-9]*(e-?[0-9]+)?

identifier [a-zA-Z0-9\_]+


/* In the third section of the lex file (after the %%),
   we can define the patterns for each token
   in terms of regular expressions and the variables above,
   and give the action (as C++ code) for each token.
   Comments are legal only inside the actions. */

%%

%{
/* Surrounding four lines, and other things involving "loc", are from
   https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Scanner.html#Calc_002b_002b-Scanner */
  // Code run each time yylex is called.
  loc.step();
%}


[ \t]	{ loc.step(); }
[\n\r]	{ loc.lines(yyleng); loc.step(); }
\/\*([^\*]|\*+[^\*\/])*\*+\/	 { loc.step(); }

\+		{ return yy::tigerParser::make_PLUS(loc); }
\-		{ return yy::tigerParser::make_MINUS(loc); }
\*		{ return yy::tigerParser::make_TIMES(loc); }
\/		{ return yy::tigerParser::make_DIVIDE(loc); }
mod		{ return yy::tigerParser::make_MOD(loc); }
\(		{ return yy::tigerParser::make_LPAREN(loc); }
\)		{ return yy::tigerParser::make_RPAREN(loc); }
\;		{ return yy::tigerParser::make_SEMICOLON(loc);	}
\,		{ return yy::tigerParser::make_COMMA(loc);	}
\:		{ return yy::tigerParser::make_COLON(loc);	}
\=		{ return yy::tigerParser::make_EQ(loc);	}
\!=		{ return yy::tigerParser::make_NEQ(loc);}
\<		{ return yy::tigerParser::make_LT(loc);	}
\<=		{ return yy::tigerParser::make_LE(loc);	}
\>		{ return yy::tigerParser::make_GT(loc);	}
\>=		{ return yy::tigerParser::make_GE(loc);	}


\|		{ return yy::tigerParser::make_OR(loc);	}
\&		{ return yy::tigerParser::make_AND(loc);	}
\!		{ return yy::tigerParser::make_NOT(loc);	}



{integer}	{
   return yy::tigerParser::make_INT(textToInt(yytext), loc);
   /* textToInt is defined above */
   /* make_INT, make_END from example at https://www.gnu.org/software/bison/manual/html_node/Complete-Symbols.html#Complete-Symbols */	  
   }

\"((\\+\")|[^\"])*\" {
	return yy::tigerParser::make_STRING(textToStr(yytext), loc);
	/* think about bad quote!!!!!!! */
}

"true" {	return yy::tigerParser::make_TRUE(yytext,loc);	}
"false" {	return yy::tigerParser::make_FALSE(yytext,loc);	}

"if" {	return yy::tigerParser::make_IF(loc);	}
"then" {	return yy::tigerParser::make_THEN(loc);	}
"else" {	return yy::tigerParser::make_ELSE(loc);	}

"while" {	return yy::tigerParser::make_WHILE(loc);	}
"do"	{	return yy::tigerParser::make_DO(loc);	}
"break"	{	return yy::tigerParser::make_BREAK(loc);	}

"for" {	return yy::tigerParser::make_FOR(loc);	}
"to"	{	return yy::tigerParser::make_TO(loc);	}
"do"	{	return yy::tigerParser::make_DO(loc);	}
\:\=	{	return yy::tigerParser::make_ASSIGN(loc);	}

"let" {	return yy::tigerParser::make_LET(loc);	}
"in"	{	return yy::tigerParser::make_IN(loc);	}
"end"	{	return yy::tigerParser::make_END_LET(loc);	}
"var"	{	return yy::tigerParser::make_VAR(loc);	}

"string" {	return yy::tigerParser::make_TYPE(yytext, loc);	}
"int" {	return yy::tigerParser::make_TYPE(yytext, loc);	}
"bool" {	return yy::tigerParser::make_TYPE(yytext, loc);	}

"function" {  return yy::tigerParser::make_FUNCTION(loc);	}

"type" {  return yy::tigerParser::make_TYPE_DEC(loc);	}
"array" {  return yy::tigerParser::make_ARRAY(loc);	}
"of" {  return yy::tigerParser::make_OF(loc);	}


{identifier} {	return yy::tigerParser::make_ID(yytext, loc); }



\<[Ee][Oo][Ff]\>		{ return yy::tigerParser::make_END(loc); /* this RE matches the literal five characters <EOF>, regardless of upper/lower case   */ }
<<EOF>>					{ return yy::tigerParser::make_END(loc); /* <<EOF>> is a flex built-in for an actual end of a file, when there is no more input */ }

.	{ string it = "?"; it[0] = yytext[0]; EM_error("illegal token: " + it); }
%%
