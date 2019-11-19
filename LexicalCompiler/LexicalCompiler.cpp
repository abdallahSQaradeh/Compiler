// LexicalCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
#include<algorithm>
/*#include<bits/stdc++.h>
#include<boost_1_70_0/boost/algorithm/string/split.hpp>
#include<boost_1_70_0/boost/algorithm/string.hpp>*/
#include<stack>

using namespace std;
struct Token
{
	string tok;
	string type;
	int length;
	int column;
	int line;
	int error_column=0;
	string whole_line;
};
class Node {

	string identifier, scope, type;
	int lineNo;
	Node* next;

public:
	Node()
	{
		next = NULL;
	}

	Node(string key, string value, string type, int lineNo)
	{
		this->identifier = key;
		this->scope = value;
		this->type = type;
		this->lineNo = lineNo;
		next = NULL;
	}

	void print()
	{
		cout << "Identifier's Name:" << identifier
			<< "\nType:" << type
			<< "\nScope: " << scope
			<< "\nLine Number: " << lineNo << endl;
	}
	friend class SymbolTable;
};
const int MAX = 100;
class SymbolTable {
	Node* head[MAX];

public:
	SymbolTable()
	{
		for (int i = 0; i < MAX; i++)
			head[i] = NULL;
	}

	int hashf(string id); // hash function 
	bool insert(string id, string scope,
		string Type, int lineno);

	string find(string id);

	bool deleteRecord(string id);

	bool modify(string id, string scope,
		string Type, int lineno);
};

// Function to modify an identifier 
bool SymbolTable::modify(string id, string s,
	string t, int l)
{
	int index = hashf(id);
	Node* start = head[index];

	if (start == NULL)
		return "-1";

	while (start != NULL) {
		if (start->identifier == id) {
			start->scope = s;
			start->type = t;
			start->lineNo = l;
			return true;
		}
		start = start->next;
	}

	return false; // id not found 
}

// Function to delete an identifier 
bool SymbolTable::deleteRecord(string id)
{
	int index = hashf(id);
	Node* tmp = head[index];
	Node* par = head[index];

	// no identifier is present at that index 
	if (tmp == NULL) {
		return false;
	}
	// only one identifier is present 
	if (tmp->identifier == id && tmp->next == NULL) {
		tmp->next = NULL;
		delete tmp;
		return true;
	}

	while (tmp->identifier != id && tmp->next != NULL) {
		par = tmp;
		tmp = tmp->next;
	}
	if (tmp->identifier == id && tmp->next != NULL) {
		par->next = tmp->next;
		tmp->next = NULL;
		delete tmp;
		return true;
	}

	// delete at the end 
	else {
		par->next = NULL;
		tmp->next = NULL;
		delete tmp;
		return true;
	}
	return false;
}

// Function to find an identifier 
string SymbolTable::find(string id)
{
	int index = hashf(id);
	Node* start = head[index];

	if (start == NULL)
		return "-1";

	while (start != NULL) {

		if (start->identifier == id) {
			start->print();
			return start->scope;
		}

		start = start->next;
	}

	return "-1"; // not found 
}

// Function to insert an identifier 
bool SymbolTable::insert(string id, string scope,
	string Type, int lineno)
{
	int index = hashf(id);
	Node* p = new Node(id, scope, Type, lineno);

	if (head[index] == NULL) {
		head[index] = p;
		cout << "\n"
			<< id << " inserted";

		return true;
	}

	else {
		Node* start = head[index];
		while (start->next != NULL)
			start = start->next;

		start->next = p;
		cout << "\n"
			<< id << " inserted";

		return true;
	}

	return false;
}

int SymbolTable::hashf(string id)
{
	int asciiSum = 0;

	for (int i = 0; i < id.length(); i++) {
		asciiSum = asciiSum + id[i];
	}

	return (asciiSum % 100);
}
class lexical {
	static const int keywordsCount = 19;
	static const int punctuationCount = 20;
	static const int operatorsCount = 18;
	char * begin = NULL;
	bool res;
	int error = 0;
	char hold_line[2046];
	int linebuffer = 0;
private:
	char keywords[keywordsCount][10] = {
		"begin","bool","const","var","end"
		,"else","then","false","float","for","from","To",
		"if","integer","and","or","by",
		"true","while"
	};
	char operators[operatorsCount] = {
		'+','-','/','*','>','<','^','=',':','(',')',',','.',';','[',']','{','}'
	};
	int LineCounter = 1;
	int ColumnCounter = 1;
	int tokenIndex = 0;
public:
	vector <Token> Tokens;
	vector<string> Lines_h;
	bool isKeyWord(string token) {
		for (int i = 0; i < keywordsCount; i++) {
			if (token == keywords[i]) {
				return true;
			}
		}
		return false;
	}

	bool isOperator(char symbol) {
		for (int i = 0; i < operatorsCount; i++) {
			if (symbol == operators[i]) {
				return true;
			}
		}
		return false;
	}
	bool isLetter(char symbol) {
		if ((symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z')) {
			return true;
		}
		return 0;
	}
	bool isNumber(char symbol) {
		if (symbol >= '0' && symbol <= '9') {
			return true;
		}
		return false;
	}
	bool isWhiteSpace(char symbol) {
		if (symbol == ' ') {
			return true;
		}
		return false;
	}
	char getNextSymbol(FILE * inputFile) {
		
		char sym = (char)getc(inputFile);
		if (sym != '\n') {
			hold_line[linebuffer++] = sym;
			begin = hold_line;
		}
		else {
			res = strcmp(hold_line,"");
			if(res>0){
			Lines_h.push_back(hold_line);
			}
			memset(&hold_line[0], 0, sizeof(hold_line));
			linebuffer = 0;
			error = 0;
		}
		ColumnCounter++;
		return sym;
	}
	bool Analyzer(FILE * inputFile) {
		char Symbol = getNextSymbol(inputFile);
		while (Symbol != EOF) {
			char Buffer[1000];
			int BufferCounter = 0;
			Buffer[BufferCounter++] = Symbol;
			Buffer[BufferCounter] = NULL;
			Token tok;
			int tokenLine = LineCounter;
			int tokenColumn = ColumnCounter - 1;
			tok.line = tokenLine;
			tok.column = tokenColumn;
			if (isLetter(Symbol) || Symbol == '_') {
				char nextSymbol = getNextSymbol(inputFile);
				int error_id = 0;
				int error_id_under = 0;
				while (nextSymbol != EOF && !isWhiteSpace(nextSymbol) && !isOperator(nextSymbol))
				{
					if (nextSymbol == '\n') {
						break;
					}
					if (isNumber(nextSymbol) && error_id == 0)
					{
						tok.error_column = ColumnCounter-2;
						error_id++;
					}
					if (nextSymbol=='_' &&  error_id_under == 0 &&BufferCounter<=1)
					{
						tok.error_column = ColumnCounter - 2;
						error_id_under++;
					}
					Buffer[BufferCounter++] = nextSymbol;
					Buffer[BufferCounter] = NULL;
					nextSymbol = getNextSymbol(inputFile);
				}
				ungetc(nextSymbol, inputFile);
				hold_line[--linebuffer];
				ColumnCounter--;
				tok.tok = Buffer;
				tok.length = BufferCounter;
				if (isKeyWord(Buffer)) {
					tok.type = "Keyword";
					
				}
				else {
					if (isLetter(Buffer[0]) && isLetter(Buffer[1]))
						tok.type = "id";
					else if (isNumber(Buffer[1]) || isOperator(Buffer[1]) || isNumber(Buffer[0]))
					{
						tok.type = "incorrect Identifier";

					}
					else if (isOperator(Buffer[0]) || Buffer[1] == '_')
					{
						if (Buffer[0] == '_') {
							tok.type = "incorrect Identifier";
							
						}
						else
						{
							tok.type = "incorrect Identifier";
							
						}
					}
					else
					{
						tok.type = "id";
					}
				}
				Tokens.push_back(tok);
			}
			else if (isOperator(Symbol)) {
				char nextSymbol = getNextSymbol(inputFile);
				if (nextSymbol != EOF) {
					if (isOperator(nextSymbol)) {

						if ((Symbol == ':' && nextSymbol == '=')) {
							Buffer[BufferCounter++] = nextSymbol;
							Buffer[BufferCounter] = NULL;
							tok.tok = Buffer;
							tok.length = BufferCounter;
							tok.type = "Operator";
							Tokens.push_back(tok);
						}
						else if ((nextSymbol == '=') || ((Symbol == '+' || Symbol == '>' || Symbol == '<') && Symbol == nextSymbol)) {
							Buffer[BufferCounter++] = nextSymbol;
							Buffer[BufferCounter] = NULL;
							tok.tok = Buffer;
							tok.length = BufferCounter;
							tok.type = "Operator";
							Tokens.push_back(tok);
						}
						else if (nextSymbol == '>' && Symbol == '<')
						{
							Buffer[BufferCounter++] = nextSymbol;
							Buffer[BufferCounter] = NULL;
							tok.tok = Buffer;
							tok.length = BufferCounter;
							tok.type = "Operator";
							Tokens.push_back(tok);
						}


						else if (Symbol == '-' && nextSymbol == '-') {
							while (nextSymbol != EOF)
							{
								nextSymbol = getNextSymbol(inputFile);
								if (nextSymbol == '-') {
									char next = getNextSymbol(inputFile);
									if (next == '-') break;
									else nextSymbol = next;
								}
							}
						}
					}
					else if (Symbol == '-' )
					{
						int isidentifire = 0;
						int isDouble = 0;
						int error_id_ = 0;
						while (nextSymbol != EOF) {
							if (isNumber(nextSymbol)) {
								Buffer[BufferCounter++] = nextSymbol;
								Buffer[BufferCounter] = NULL;
							}
							else if (nextSymbol == '.') {
								char next = getNextSymbol(inputFile);
								if (isNumber(next)) {
									Buffer[BufferCounter++] = nextSymbol;
									Buffer[BufferCounter++] = next;
									Buffer[BufferCounter] = NULL;
									isDouble = 1;
								}
								
								else {
									tok.error_column = ColumnCounter-2;
									ungetc(next, inputFile);
									hold_line[--linebuffer];
									ColumnCounter--;
									ungetc(nextSymbol, inputFile);
									hold_line[--linebuffer];
									ColumnCounter--;
									break;
								}
							}
							else if (isLetter(nextSymbol))
							{
								isidentifire = 1;
								Buffer[BufferCounter++] = nextSymbol;
								Buffer[BufferCounter] = NULL;
								if (error_id_++ == 0) {
									tok.error_column = ColumnCounter - 2;
								}
							}
							else {
								ungetc(nextSymbol, inputFile);
								hold_line[--linebuffer];
								ColumnCounter--;
								break;
							}
							nextSymbol = getNextSymbol(inputFile);
						}
						tok.tok = Buffer;
						tok.length = BufferCounter;
						if (isDouble)
							tok.type = /*"Number(Double)"*/ "number_literal";
						else if (isidentifire)
						{
							tok.type = "incorrect Identifier";
						}
						else tok.type = /*"Number(Integer)"*/"number_literal";
						Tokens.push_back(tok);

						
					

					}
					else {
						tok.tok = Buffer;
						tok.length = BufferCounter;
						tok.type = "Operator";
						Tokens.push_back(tok);
						ungetc(nextSymbol, inputFile);
				
						hold_line[--linebuffer];
						ColumnCounter--;
					}
				}
			}
			else if (Symbol == '"')
			{
				char nextSymbol = getNextSymbol(inputFile);
				int isString = 0;
				Token Pun;
				if (Symbol == '"') {
					Pun.tok = "\"";
					Pun.length = 1;
					Pun.line = tokenLine;
					Pun.column = tokenColumn;
					tok.column++;
					tokenColumn++;
					BufferCounter--;
					while (nextSymbol != EOF && nextSymbol != '"') {
						if (nextSymbol != '\n') {
							if (nextSymbol == '--') {
								nextSymbol = getNextSymbol(inputFile);
								ColumnCounter--;
							}
							Buffer[BufferCounter++] = nextSymbol;
							Buffer[BufferCounter] = NULL;
						}
						nextSymbol = getNextSymbol(inputFile);
					}
					isString = 1;
				}
				tok.tok = Buffer;
				tok.length = BufferCounter;
				if (isString)
					tok.type = "string_literal";
				Tokens.push_back(tok);

			}
			else if (isNumber(Symbol)) {
				char nextSymbol = getNextSymbol(inputFile);
				int isDouble = 0;
				while (nextSymbol != EOF) {
					if (isNumber(nextSymbol)) {
						Buffer[BufferCounter++] = nextSymbol;
						Buffer[BufferCounter] = NULL;
					}
					else if (nextSymbol == '.') {
						char next = getNextSymbol(inputFile);
						if (isNumber(next)) {
							Buffer[BufferCounter++] = nextSymbol;
							Buffer[BufferCounter++] = next;
							Buffer[BufferCounter] = NULL;
							isDouble = 1;
						}
						else {
							
							ungetc(next, inputFile);
							hold_line[--linebuffer];
							ColumnCounter--;
							ungetc(nextSymbol, inputFile);
							hold_line[--linebuffer];
							ColumnCounter--;
							tok.error_column = ColumnCounter;
							break;
						}
					}
					else {
						
						ungetc(nextSymbol, inputFile);
						hold_line[--linebuffer];
						ColumnCounter--;
						break;
					}
					nextSymbol = getNextSymbol(inputFile);
				}
				tok.tok = Buffer;
				tok.length = BufferCounter;
				if (isDouble)
					tok.type = /*"Number(Double)"*/ "number_literal";
				else tok.type = /*"Number(Integer)"*/"number_literal";
				Tokens.push_back(tok);
			}
			else if (Symbol == '\t') ColumnCounter += 3;
			else if (Symbol == '\n') {

				LineCounter++;
				ColumnCounter = 1;
			}
			Symbol = getNextSymbol(inputFile);
		}
		return true;
	}

	Token getNextToken() {
		if (tokenIndex < Tokens.size())
			return Tokens[tokenIndex++];
		else {
			cout << "Invalid" << endl;
			exit(1);
		}
	}
	void print(SymbolTable  &st)
	{
		
		Token Tok;
		int i = 0;
		int line_p = 0;
		int holder = i;
		while (!Tokens.empty() && i <= Tokens.size()-1)		{
			
			
			while (  holder<=Tokens.size()-1 &&(Tok = Tokens.at(holder = i++)).line == line_p+1) {
				if (st.insert(Tok.tok, "local", Tok.type, Tok.line))
				{
					cout << "successful add " << Tok.tok << endl;
				}
				else
				{
					cout<<"failed to add " <<Tok.tok<<endl;
				}
				if (isKeyWord(Tok.tok)) {
					cout << "<" << Tok.tok << ">";
				}
				else if (Tok.type == "Number(Double)" || Tok.type == "Number(Integer)")
				{
					cout << "<" << "number" << "," << Tok.tok << "> ";
				}
				else if (Tok.type == "Identifier")
				{
					cout << "<" << Tok.type << "," << Tok.tok << ">";
				}
				else if (Tok.type == "Operator")
				{
					cout << "<" << Tok.tok << ">";
				}
				else if (Tok.type == "String")
				{
					cout << "<" << "literal" << ",\"" << Tok.tok << "\">" << endl;
				}
				else if (Tok.type == "incorrect Identifier")
				{
					cout << "<" << "incorrect Identifier" << "," << Tok.tok << ">" << " ";
				}
				else if (Tok.type == "Keyword")
				{
					cout << "<" << Tok.type <<">" << " ";
				}
				//cout << Tok.tok << " " << Tok.type << " " << Tok.column << endl;
				if (i == Tokens.size())
					holder = i;
			
			}
			if (holder == Tokens.size())
				i = holder;
			else {
				line_p++;
				--i;
			}
			
			cout << endl;
		}
		
		int line = 0;
		bool check;
		Token toke;
		i = 0;
	while (!Lines_h.empty() && line<Lines_h.size())
	{
		while (!Tokens.empty() && i < Tokens.size() && Tokens.at(i).line==line+1)
		{
			toke =Tokens.at(i);
			if (toke.error_column != 0)
			{
				cout<< Lines_h.at(line) <<  " error on line "<<line+1 << endl;
				for (int u = 0; u < toke.error_column; u++)
					cout << " ";
				cout << "^" << endl;
			}
			i++;

		}

		line++;
	}
}
};
struct Production {
	string name;                          
	char ** productions;

};

class parser {

public :
	typedef map<string, string> inner;
	typedef map<string, inner> outer;
	vector<Token> tokens;
	stack<string> stack2;
	map<string, map<string, string>> parseTable;
	map<string, string> innermap;
	map<string, inner>::iterator it_out;
	map<string, string>::iterator it_inner;
	map<string, vector<string>> first;
	stack<string> stack_x;
	vector<string> terminal = { ";","var",":","id","const",":=","function","(",")","integer","bool","float",
	"string","void",",","{","}","if","while","for","from","to","return","else","true","false","string_literal",
	"+","-","number_literal","<>","=","<=",">=",">","<","and","or","$","*","/" };
	vector<string> all = { "program","declarationSet","declaration","type","funcType"
,"paramSet","paramSetPrime","block","declarationList","variableDeclaration","statementList","statement",
"idPrime","ifPrime","argumentsSet","argumentsSetPrime","arguments","argumentsPrime","expr","exprPrime",
"term","termPrime","factor","factorPrime","factorPrime","logicalOpr","subExpr" };
	bool isTerminal(string str)
	{
		for (int i = 0; i < terminal.size(); i++)
		{
			if (str == terminal[i])
			{
				return 1;
			}
		}
		return false;
	}
	parser()
	{

		//1
		parseTable["program"]["var"] = "declarationSet";
		parseTable["program"]["const"] = "declarationSet";
		parseTable["program"]["function"] = "declarationSet";
		parseTable["program"]["$"] = "";
		//2
		parseTable["declarationSet"]["var"] = "declaration ; declarationSet";
		parseTable["declarationSet"]["const"] = "declaration ; declarationSet";
		parseTable["declarationSet"]["function"] = "declaration ; declarationSet";
		parseTable["declarationSet"]["$"] = "";
		//3
		parseTable["declaration"]["var"] = "var type : id";
		parseTable["declaration"]["const"] = "const type id := expr";
		parseTable["declaration"]["function"] = "function funcType id (paramSet) block";
		//4
		parseTable["type"]["integer"] = "integer";
		parseTable["type"]["bool"] = "bool";
		parseTable["type"]["float"] = "float";
		parseTable["type"]["string"] = "string";
		//5
		parseTable["funcType"]["integer"] = "type";
		parseTable["funcType"]["bool"] = "type";
		parseTable["funcType"]["float"] = "type";
		parseTable["funcType"]["string"] = "type";
		parseTable["funcType"]["void"] = "void";
		//6
		parseTable["paramSet"]["bool"] = "type : id paramSetPrime";
		parseTable["paramSet"]["float"] = "type : id paramSetPrime";
		parseTable["paramSet"]["string"] = "type : id paramSetPrime";
		parseTable["paramSet"]["integer"] = "type : id paramSetPrime";
		//7
		parseTable["paramSetPrime"][")"] = "";
		parseTable["paramSetPrime"][","] = ", type : id paramSetPrime";
		//8
		parseTable["block"]["{"] = "{ declarationList statementList }";
		//9
		parseTable["declarationList"]["var"] = "variableDeclaration ; declaraionList";
		parseTable["declarationList"]["id"] = "";
		parseTable["declarationList"]["{"] = "";
		parseTable["declarationList"]["if"] = "";
		parseTable["declarationList"]["while"] = "";
		parseTable["declarationList"]["for"] = "";
		//10 variableDeclaration
		parseTable["variableDeclaration"]["var"] = "var type : id";
		//11 
		parseTable["statementList"]["id"] = "statement ; statementList";
		parseTable["statementList"]["}"] = "";
		parseTable["statementList"]["if"] = "statement ; statementList";
		parseTable["statementList"]["while"] = "statement ; statementList";
		parseTable["statementList"]["for"] = "statement ; statementList";
		parseTable["statementList"]["return"] = "statement ; statementList";
		//12
		parseTable["statement"]["id"] = "id idPrime";
		parseTable["statement"]["{"] = "block";
		parseTable["statement"]["if"] = "if ( expr ) { statement } ifPrime";
		parseTable["statement"]["while"] = "while (expr) { statement }";
		parseTable["statement"]["for"] = "for ( id from expr to expr ) { statement }";
		parseTable["statement"]["return"] = "return expr";
		//13
		parseTable["idPrime"][":="] = ":= expr";
		parseTable["idPrime"]["("] = "argumentsSet";
		//14
		parseTable["ifPrime"][";"] = "";
		parseTable["ifPrime"]["}"] = "";
		parseTable["ifPrime"]["else"] = "else { statement }";
		//15 arugumentsSet->( argumentsSetPrime )
		parseTable["arugumentsSet"]["("] = "( argumentsSetPrime )";
		//16 argumentsSetPrime
		parseTable["argumentsSetPrime"]["id"] = "arguments";
		parseTable["argumentsSetPrime"]["("] = "arguments";
		parseTable["argumentsSetPrime"][")"] = "";
		parseTable["argumentsSetPrime"]["true"] = "arguments";
		parseTable["argumentsSetPrime"]["string_literal"] = "arguments";
		parseTable["argumentsSetPrime"]["false"] = "arguments";
		parseTable["argumentsSetPrime"]["number_literal"] = "arguments"; 
		//17
		parseTable["arguments"]["id"] = "expr argumentsPrim";
		parseTable["arguments"]["("] = "expr argumentsPrime";
		parseTable["arguments"]["true"] = "expr argumentsPrime";
		parseTable["arguments"]["false"] = "expr argumentsPrime";
		parseTable["arguments"]["string_literal"] = "expr argumentsPrime";
		parseTable["arguments"]["number_literal"] = "expr argumentsPrime";
		//18
		parseTable["argumentsPrime"][","] = ", arguments";
		parseTable["argumentsPrime"][")"] = "";
		//19
		parseTable["expr"]["id"] = "subExpr exprPrime";
		parseTable["expr"]["("] = "subExpr exprPrime";
		parseTable["expr"]["string_literal"] = "subExpr exprPrime";
		parseTable["expr"]["number_literal"] = "subExpr exprPrime";
		parseTable["expr"]["true"] = "subExpr exprPrime";
		parseTable["expr"]["false"] = "subExpr exprPrime";
		//20 exprPrime
		parseTable["exprPrime"][";"] = "";
		parseTable["exprPrime"][")"] = "";
		parseTable["exprPrime"]["}"] = "";
		parseTable["exprPrime"]["to"] = "";
		parseTable["exprPrime"]["+"] = "+ term exprPrime";
		parseTable["exprPrime"]["-"] = "- term exprPrime";
		parseTable["exprPrime"]["<>"] = "relOpr expr exprPrime";
		parseTable["exprPrime"]["<"] = "relOpr expr exprPrime";
		parseTable["exprPrime"][">"] = "relOpr expr exprPrime";
		parseTable["exprPrime"]["<="] = "relOpr expr exprPrime";
		parseTable["exprPrime"][">="] = "relOpr expr exprPrime";
		parseTable["exprPrime"]["="] = "relOpr expr exprPrime";
		parseTable["exprPrime"]["and"] = "logicalOpr expr exprPrime";
		parseTable["exprPrime"]["or"] = "logicalOpr expr exprPrime";
		//21 
		parseTable["term"]["id"] = "factor termPrime";
		parseTable["term"]["("] = "factor termPrime";
		parseTable["term"]["number_literal"] = "factor termPrime";
		//22
		parseTable["termPrime"][";"] = "";
		parseTable["termPrime"][")"] = "";
		parseTable["termPrime"]["}"] = "";
		parseTable["termPrime"]["to"] = "";
		parseTable["termPrime"]["+"] = "";
		parseTable["termPrime"]["-"] = "";
		parseTable["termPrime"]["<>"] = "";
		parseTable["termPrime"]["="] = "";
		parseTable["termPrime"][">"] = "";
		parseTable["termPrime"]["<"] = "";
		parseTable["termPrime"]["<="] = "";
		parseTable["termPrime"][">="] = "";
		parseTable["termPrime"]["and"] = "";
		parseTable["termPrime"]["or"] = ""; 
		parseTable["termPrime"]["*"] = "* factor termPrime";
		parseTable["termPrime"]["/"] = "/ factor termPrime";
		//23
		parseTable["factor"]["id"] = " id factorPrime";
		parseTable["factor"]["("] = " (expr) ";
		parseTable["factor"]["number_literal"] = "number_literal";
		//24 factorPrime
		parseTable["factorPrime"][";"] = "";
		parseTable["factorPrime"]["("] = "argumentsSet";
		parseTable["factorPrime"][")"] = "";
		parseTable["factorPrime"]["}"] = "";
		parseTable["factorPrime"][")"] = "";
		parseTable["factorPrime"]["+"] = "";
		parseTable["factorPrime"]["-"] = "";
		parseTable["factorPrime"]["<>"] = "";
		parseTable["factorPrime"]["="] = "";
		parseTable["factorPrime"]["<="] = "";
		parseTable["factorPrime"][">="] = "";
		parseTable["factorPrime"][">"] = "";
		parseTable["factorPrime"]["<"] = "";
		parseTable["factorPrime"]["and"] = "";
		parseTable["factorPrime"]["or"] = "";
		parseTable["factorPrime"]["*"] = "";
		parseTable["factorPrime"]["/"] = "";
		parseTable["factorPrime"]["to"] = "";
		//25
		parseTable["relOpr"]["<>"] = "<>";
		parseTable["relOpr"]["="] = "=";
		parseTable["relOpr"][">"] = ">";
		parseTable["relOpr"]["<"] = "<";
		parseTable["relOpr"][">="] = ">=";
		parseTable["relOpr"]["<="] = "<=";
		//26 logicalOpr
		parseTable["logicalOpr"]["and"] = "and";
		parseTable["logicalOpr"]["or"] = "or";
		//27
		parseTable["subExpr"]["id"] = "term";
		parseTable["subExpr"]["("] = "term";
		parseTable["subExpr"]["true"] = "true";
		parseTable["subExpr"]["false"] = "false";
		parseTable["subExpr"]["string_literal"] = "string_literal";
		parseTable["subExpr"]["number_literal"] = "number_literal";

	}
	vector<string> Split(string input)
	{
		vector<string> result;
		//boost::split(result, input, "\t");
		size_t pos = input.find(" ");
		size_t initialPos = 0;
		//r.clear();

		// Decompose statement
		while (pos != std::string::npos) {
			result.push_back(input.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = input.find(" ", initialPos);
		}

		// Add the last one
		result.push_back(input.substr(initialPos, std::min(pos, input.size()) - initialPos + 1));

	

		return result; 
	}
	void dofirst()
	{
		
		string handle;
		vector<string> values;
		for (int i = 0; i < 21; i++)
		{
			values = productions.at(all[i]);
			for (int j = 0; j < values.size(); j++)
			{
				it = find(all.begin(), all.end(), values.at(j));
				if (it != all.end())
				{
					std::cout << "Element " << values.at(j) << " found at position : ";
					std::cout << it - all.begin() + 1 << "\n";
				}
				else
					std::cout << "Element not found.\n\n";
			}
		}
	}
	string parseSymbol(string tok, string production)
	{
		bool flag = 0;
		string holder;
		it_out = parseTable.find(production);
		if (it_out != parseTable.end())
		{
			it_inner = (*it_out).second.begin();
			int i = 0;
			while (i != parseTable[production].size()) {
				holder = it_inner->first;
				if (tok.compare(holder) == 0)
				{
					flag = 1;
					break;
				}
				it_inner++;
				i++;
			}
			if (flag == 1)
			{
				return parseTable[production][tok];
			}
			else
			{
				return "error";
			}
		}
		
		return tok;
	}
	string parsing(vector<Token> tokens)
	{
		stack_x.push("$");
		stack_x.push("program");
		Token Tok;
		int i = 0;
		int line_p = 0;
		int holder = i;
		string entry ;
		string entryType;
		while (stack_x.top()!="$" && i<tokens.size()) {
			entry = parseSymbol(tokens[i].tok, stack_x.top());
			entryType = parseSymbol(tokens[i].type, stack_x.top());
			if (stack_x.top() == tokens[i].tok || stack_x.top()== tokens[i].type)
			{
				stack_x.pop();
				i++;
			}
			else if (isTerminal(stack_x.top()))
			{

				cout << "error is terminal "<< stack_x.top()<<" on line "<< tokens[i].line <<" at token "<< tokens[i].tok << endl;
				break;
			}
			else if (entry.compare("error")==0 && entryType.compare("error")==0)
			{
				cout << "error does not exist on table and the top of stack is "<<stack_x.top() <<" and the token is "<<tokens[i].tok<<" on line "<<tokens[i].line << endl;
				return "error";
			}
			else if (entry.compare("error") != 0|| entryType.compare("error") != 0)
			{
				vector<string> splitentry;
				if (entry == "error")
				{
					cout << entryType << endl;
					splitentry = Split(entryType);
				}else
				{
					cout << entry << endl;
					splitentry = Split(entry);
				}
				//cout << entry << endl;
				
				stack_x.pop();
				for (int o = splitentry.size()-1; o >= 0; o--)
				{
					stack_x.push(splitentry[o]);
				}

			}

			}return "";
		}
		
	
private:
	
	vector<string>::iterator it;
	//map<string, vector<string>> first;
	map<string, vector<string>> productions;
	vector<string> program = { "declarationSet" };
	vector<string> declarationSet = { "declaration ; declarationSet","empty" };
	vector<string> declaration = { "var type : id","const id := expr","function funType id (paramSet) block" };
	vector<string> type = { "integer","string","float","bool" };
	vector<string> funcType = { "type","void" };
	vector<string> paramSet = { "type : id p_prime" };
	vector<string> p_prime = { ", type : id p_prime","empty" };
	vector<string> block = { "{ declarationList statementList }" };
	vector<string> declarationList = { "cariableDeclaration ; declarationList","empty" };
	vector<string> variableDeclaration = { "var type : id" };
	vector<string> statementList = { "statement ; statementList","empty" };
	vector<string> argumentsSet = { "()","( arguments )" };
	vector<string> arithmeticOpr = { "*","+","-","/" };
	vector<string> relOpr = { "=","<>","<",">","<=",">=" };
	vector<string> logicalOpr = { "and","or" };
	vector<string> statement = { "if ( expr ) { statement } satatement_prime","while ( expr ) { statement }","for ( id from expr to expr ) { statement }"
	,"return expr","block","id := expr","id argumentsSet"};
	vector<string> statement_prime = { "else { statement }","empty" };
	vector<string> arguments = { "expr arguments_prime" };
	vector<string> arguments_prime = { ", arguments","empty" };
	vector<string> expr = { "id expr_prime","id argumentsSet expr_prime","nuber_literal expr_prime","string_literal expr_prime",
	"true expr_prime","false expr_prime","( expr ) expr_prime" };
	vector<string> expr_prime = { "arithmeticOpr expr expr_prime","relOpr expe expr_prime","logicalOpr expr expr_prime","empty" };
	vector<vector<string>> names = { program,declarationSet,declaration,type,funcType,paramSet,p_prime,block,declarationList
	,variableDeclaration,statementList,argumentsSet,arithmeticOpr,relOpr,logicalOpr,statement,statement_prime,arguments
	,arguments_prime,expr,expr_prime };
};
int main()
{
	SymbolTable st;
	lexical lex;
	vector<Token> tokens;
	char input[200];
	string ab = "abdallah sameer qaradeh";
	
	cout << "enter file name " << endl;
	//cin >> input;
	FILE * inputFile = fopen("inputfile.txt", "r+");
	if (!inputFile) {
		perror("Error");
		exit(0);
	}
	else
	{
		lex.Analyzer(inputFile);
		lex.print(st);
		
	}

	cout << st.find("if") << endl;
	parser pars;
	pars.Split(ab);
	pars.tokens = lex.Tokens;
	pars.parsing(pars.tokens);
	//pars.dofirst();
	system("pause");

}

