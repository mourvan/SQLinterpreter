#include <iostream>
#include <string>
#include <vector>
#include "lex.h"
#include "syn.h"
#include <cstring>


/*TODO - в процессе парсинга заполнять statement(where)-структуру, в случае удачного парса выполнить команду функциями из библиотеки
		 CREATE, INSERT, DROP готовы,  */

//Parser::Parser() : i(0)
//{
//
//}

void Parser::SQL()
{
	i=0;
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
	s = &is; // current statement - insert-statement
	if (lex[i].type==LEX_INTO)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    { strcpy(is.TableName,lex[i].value.c_str()); i++;}
	else throw i;
	if (lex[i].type==LEX_LPAREN){i++;}
	else throw i;

	while(lex[i-1].type!=LEX_RPAREN)
	{
		if(lex[i].type==LEX_STRING || lex[i].type==LEX_NUM)  {is.fieldvalue.push_back(lex[i].value); i++;}
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
	s = &cs; 
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {strcpy(cs.TableName,lex[i].value.c_str()); i++;}
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
	FieldDef temp;
	if (lex[i].type==LEX_ID)  
	{
		strcpy(temp.name,lex[i].value.c_str()); 
		i++;
	}
	else throw i;
	if (lex[i].type==LEX_LONG)    
	{
		temp.type=Long;
		i++;
	}
	else if(lex[i].type==LEX_TEXT) 
	{
		temp.type=Text;
		i++;
		if (lex[i].type==LEX_LPAREN)  {i++;}
		else throw i;
		if (lex[i].type==LEX_NUM || stol(lex[i].value)>0)  
		{
			temp.len=stol(lex[i].value); 
			i++;
		}  
		else throw i;
		if (lex[i].type==LEX_RPAREN)  {i++;}
		else throw i;
	}
	else throw i;
	cs.fielddef.push_back(temp);
}

void Parser::DROP()
{
	s = &ds;
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {strcpy(ds.TableName,lex[i].value.c_str());
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

void Parser::run()
{
	s->run();
}

void create_statement::run() //vector<FieldDef>fielddef;
{
	TableStruct tstr;
	tstr.numOfFields = fielddef.size();
	tstr.fieldsDef = &fielddef[0];
	if((err = createTable(TableName,&tstr)) != OK)
		throw err;
	fielddef.clear();
}


void insert_statement::run()
{
	int i1=0;
	long l;
	char buf[80];
	THandle tableHandle;
	FieldStruct * curr;
	if((err = openTable(TableName, &tableHandle)) != OK)
		throw err;
		
	if((unsigned)tableHandle->tableInfo.fieldNumber != fieldvalue.size())
	{
		err = FieldNotFound;
		closeTable(tableHandle);
		throw err;
	}	
	
	if((err = createNew(tableHandle)) != OK)
		throw err;
	//cout << tableHandle->pFieldStruct->fieldName << tableHandle->tableInfo.fieldNumber << endl;
	for (auto i = fieldvalue.begin(); i != fieldvalue.end(); i++)  //vector<string>fieldvalue;
	{
		curr = (tableHandle->pFieldStruct)+i1;
		try
		{
			l=stol(fieldvalue[i1]);
			if((err = putLongNew(tableHandle, curr->fieldName, l)) != OK)
				{closeTable(tableHandle); throw err;}
		}
		catch(invalid_argument)
		{
			strcpy(buf, fieldvalue[i1].c_str());
			if((err = putTextNew(tableHandle, curr->fieldName, buf)) != OK)
				{closeTable(tableHandle); throw err;}
		}
		i1++;
	}
	if((err = insertzNew(tableHandle)) != OK)
		{throw err; closeTable(tableHandle);}
	if((err = closeTable(tableHandle)) != OK)
		throw err;
	fieldvalue.clear();
}

void drop_statement::run()
{
	if((err = deleteTable(TableName)) != OK)
		throw err;
}
