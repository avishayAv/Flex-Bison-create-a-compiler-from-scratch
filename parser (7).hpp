#ifndef _HPP_PARSER
#define _HPP_PARSER
#include <string>
#include <vector>
#include <stack>
#include <stdio.h>
#include <string.h>
#include "output.hpp"
#include <stdlib.h>
using namespace std;
#define ERROR_TYPE -300
struct Node {
	virtual ~Node(){}
};
#define YYSTYPE Node*

typedef enum {
	NOT_FOUND,
	MISSMATCH,
	MATCH
} ScopeFunc;

typedef enum {
    PLUS,
    MINUS,
    DIV,
    MUL
} Binops;

typedef enum {
    BGE,
    BLE,
    BGT,
    BLT,
    BEQ,
    BNE
} Relops;

class Expression : public Node {
	public:
	string name;
	string type;
	int size;
	string place;
	vector<int> true_list;
	vector<int> false_list;
	Expression(string name1, string type1, int size1): name(name1),type(type1),size(size1){	}
};


class PreCondition : public Node {
public:
    string label;
    vector<int> true_list;
    vector<int> false_list;
    PreCondition(string label1):label(label1){

    }
};

class Var : public Node{
	public:
	string type;
	string value;
	int numeric_value;	//only in case that the variable is numeric
	Var(string val1,string type1, int numeric_value=-1):value(val1),type(type1), numeric_value(-1){
		if (type == "int")	{
			char* to_convert = new char[val1.length()+1];
			strcpy(to_convert, value.c_str());
			this->numeric_value = strtol(to_convert, NULL, 10);
			//TODO : maybe the bug is here
		}
	}
};

class Identifier : public Node {
	public:
	string name;
	Identifier(string name1): name(name1){}
};


class Type :  public Node{
	public:
	string type;
	Type (string type1): type(type1){}
};

class Parameter{
	public:
	string name;
	string type;
	int size;
	Parameter(string name1, string type1,int size1):name(name1), type(type1),size(size1){}
};

class Record{
	public:
	string name;
	string type;
	int offset;
	vector<Parameter> parameters;	//TODO : should be referenced
	Record(string name1, string type1, int offset1, vector<Parameter> parameters1=vector<Parameter>()):name(name1),type(type1),offset(offset1), parameters(parameters1){}
	
};

class ScopedTable{
	public:
	vector<Record> records;
	ScopedTable* papa;
	string return_type;	//if this scope is a function scope - then return_type equals the return type value
	// and if while scope is while scope
	ScopedTable(vector<Record>& records1, ScopedTable* papa1, string return_type="no_function"): records(records1), papa(papa1), return_type(return_type) {}
	void addRecord(Record& to_add){
		records.push_back(to_add);
	}
	Record ScopedTableSearch(string name_to_find){
		Record record("","",ERROR_TYPE);
		for (int i=0; i< records.size(); i++){
			if (records[i].name == name_to_find){
				record = records[i];
				break;
			}
		}
		return record;
	}
	string capitalize(string type){
		string capital = "";
		for(int i=0; i < type.length(); i++){
			capital += toupper(type[i]);
		}
		return capital;
	}

	void PrintTableBeforeRemove(){
		for (int i = 0; i < records.size(); ++i) {
			string capital = "";
			string type = records[i].type;
			if (type == "function")	{
				string ret_type = capitalize(records[i].parameters[0].type);
				vector<string> params;
				for (int j =1; j<records[i].parameters.size(); j++)	{
					params.push_back(capitalize(records[i].parameters[j].type));
				}
				capital = output::makeFunctionType(ret_type, params);
			}
			else	{
				capital = capitalize(type);
			}
			output::printID(records[i].name,records[i].offset,capital);
		}
	}
};

class SymbolTable{
	public:
	stack<ScopedTable> tables;
	stack<int> offsets;
	SymbolTable(): tables(), offsets(){
		vector<Record> empty_recs;
        ScopedTable first_table(empty_recs, NULL); //TODO: maybe the bug is here

		tables.push(first_table);
		offsets.push(0);


	}
	void RemoveTable() {
		tables.pop();
		offsets.pop();
	}
	
	void insertTable(ScopedTable* parent, int offset,string return_type="no_function"){
		vector<Record> recs;
        ScopedTable to_add(recs, parent,return_type);
		tables.push(to_add);
		offsets.push(offset);
	}
	void Record_insertion(Record record_to_add,int size){
		offsets.top() += size;
		tables.top().records.push_back(record_to_add);
	}
	void Record_removal(int size)	{
		tables.top().records.pop_back();
		offsets.top() -= size;
	}

	void updateRecordFormals(string func_name,vector<Parameter> params){
		ScopedTable* curr_table = &(tables.top());
		while(curr_table->papa){
			curr_table = curr_table->papa;
		}
		for(int i = 0; i<curr_table->records.size();i++){
			if (curr_table->records[i].name == func_name){
				curr_table->records[i].parameters.insert(curr_table->records[i].parameters.end(),params.begin(),params.end());
			}
		}
	}
	Record SymbolTableSearch(string name_to_find){

		Record record("","",ERROR_TYPE);
		ScopedTable* curr_table = &(tables.top());
		record = curr_table->ScopedTableSearch(name_to_find);

		if (record.offset != ERROR_TYPE){
			return record;
		}
		while (curr_table->papa != NULL){
			curr_table = curr_table->papa;
			record = curr_table->ScopedTableSearch(name_to_find);
			if (record.offset != ERROR_TYPE){

				return record;
			}
		}

		return record;
	}

	bool isInScope(string scope_type) {
		ScopedTable *curr_table = &(tables.top());
		bool correct_scope = false;
		while (curr_table) {
			if (curr_table->return_type == scope_type) {
				correct_scope = true;
				break;
			}
			curr_table = curr_table->papa;
		}
		return correct_scope;
	}

	ScopeFunc isInScopeFunc(string scope_type){
		ScopedTable *curr_table = &(tables.top());

		while (curr_table) {
			if (curr_table->return_type == "int" || curr_table->return_type == "byte" || curr_table->return_type == "bool" || curr_table->return_type == "string" || curr_table->return_type == "void") {
				if(scope_type != curr_table->return_type){
					if(!(curr_table->return_type == "int" && scope_type == "byte")){
						return MISSMATCH;
					}
					return MATCH;
				}
				return MATCH;
			}
			curr_table = curr_table->papa;
		}
		return NOT_FOUND;



	}



};

class MyString : public Node{
public:
    string text;
    MyString(string str1): text(str1){}
};

class ExpList : public Node {
	public:
	vector<Parameter> expressions;
	ExpList(vector<Parameter> expressions) : expressions(expressions) {}
};

class Formal : public Node	{
	public:
	string name;
	string type;
	int size;
	Formal(string name1, string type1,int size1):name(name1), type(type1),size(size1){}
};

class FormalList : public Node	{
	public:
	vector<Formal> formals;
	FormalList(vector<Formal> formals) : formals(formals)  {}
};

class Function : public Node	{
    public:
    string name;
	string return_type;
};

class Statement : public Node   {
    public:
    vector<int> next_list;
    vector<int> break_list;     //seperated from next_list for case that we have nested break
    vector<int> continue_list;
    Statement(){}
};

class ElseStatement : public Statement  {
    public:
    bool is_else;
    string label;
    ElseStatement(bool is_else1, string label1) : is_else(is_else1), label(label1)  {}
};

#endif // _HPP_PARSER