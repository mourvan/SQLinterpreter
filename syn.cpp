#include <iostream>
#include <string>
#include <vector>
#include "lex.h"
#include "syn.h"
#include "Table.h"

/*TODO - в процессе парсинга заполнять statement(where)-структуру, в случае удачного парса выполнить команду функциями из библиотеки
		 CREATE, INSERT, DROP готовы,  */

Parser::Parser() : i(0)
{

}

void Parser::SQL()
{
	if 		(lex[i].type == LEX_SELECT) {i++; SELECT();}
	else if (lex[i].type == LEX_INSERT) {i++; INSERT();}
	else if (lex[i].type == LEX_UPDATE) {i++; UPDATE();}
	else if (lex[i].type == LEX_DELETE) {i++; DELETE();}
	else if (lex[i].type == LEX_CREATE) {i++; CREATE();}
	else if (lex[i].type == LEX_DROP)   {i++; DROP();  }
	else throw i;
}

void Parser::SELECT()
{
	FIELD_LIST();
	if (lex[i].type==LEX_FROM) {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)   {i++;}
	else {throw i;}
	WHERE();
}

/*
   void Parser::FIELD_LIST()
   {

   if (lex[i].type==LEX_TIMES) {i++;}
   else
   {
   int i1;
   int i2;
   for(i1 = i; lex[i1].type==LEX_ID; i1+=2);
   if(i1==i) throw i;	   //не идентификатор
   for(i2=i+1; lex[i2].type==LEX_COMMA && i2<i1-2; i2+=2);
   if(i2!=i1-3) throw i2; //неправильные запятые
   i=(i1-1);
   }
   }
 */

void Parser::FIELD_LIST()
{
	if (lex[i].type==LEX_TIMES) {i++;}
	else{
		while(lex[i-1].type!=LEX_FROM)
		{
			if(lex[i].type==LEX_FINISH) {throw i;}
			if(lex[i].type==LEX_ID)  {i++;}
			else throw i;
			if(lex[i].type==LEX_COMMA || lex[i].type==LEX_FROM){i++;}
			else throw i;

		}
		i--;
	}
}

void Parser::INSERT()
{
	if (lex[i].type==LEX_INTO)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_LPAREN){i++;}
	else throw i;

	while(lex[i-1].type!=LEX_RPAREN)
	{
		if(lex[i].type==LEX_STRING || lex[i].type==LEX_NUM)  {i++;}
		else throw i;

		if(lex[i].type==LEX_COMMA || lex[i].type==LEX_RPAREN){i++;}
		else throw i;
	}
}

void Parser::UPDATE()
{
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_SET)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_EQ)  {i++;}
	else throw i;
	//TODO: ВЫРАЖЕНИЕ
	WHERE();
}

void Parser::DELETE()
{
	if (lex[i].type==LEX_FROM)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	WHERE();
}

void Parser::CREATE()
{
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_LPAREN){i++;}
	else throw i;
	while(lex[i-1].type!=LEX_RPAREN)
	{
		FIELD();
		if(lex[i].type==LEX_COMMA || lex[i].type==LEX_RPAREN){i++;}
		else throw i;
	}

}

void Parser::FIELD()
{
	if (lex[i].type==LEX_ID)  {i++;}
	else throw i;
	if (lex[i].type==LEX_LONG)    {i++;}
	else if(lex[i].type==LEX_TEXT) 
	{
		i++;
		if (lex[i].type==LEX_LPAREN)  {i++;}
		else throw i;
		if (lex[i].type==LEX_NUM || stol(lex[i].value)>0)  {i++;}  
		else throw i;
		if (lex[i].type==LEX_RPAREN)  {i++;}
		else throw i;
	}
	else throw i;
}

void Parser::DROP()
{
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {
		i++;
	}
	else throw i;
}

void Parser::WHERE() 
{
	// stub
}

int Parser::get_i() const 
{
	return i;
}

void create_statement::run() //vector<FieldDef>fields;
{
	TableStruct tstr;
	tstr.numOfFields = fields.size();
	tstr.fieldsDef = &fields[0];
	cout << getErrorString(createTable(TableName,&tstr)) << endl;
}

void insert_statement::run()
{
	//THandle *tableHandle
	//openTable(char *tableName, THandle *tableHandle);
	//createNew(THandle tableHandle)
	//putTextNew(THandle tableHandle, char *fieldName, char *value);
	//putLongNew(THandle tableHandle, char * fieldName, long value);
	//insertzNew(THandle tableHandle);
	//closeTable(THandle tableHandle);
}

void drop_statement::run()
{
	deleteTable(TableName);
}
