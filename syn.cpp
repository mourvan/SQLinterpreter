#include <iostream>
#include <string>
#include <vector>
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
	try
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
	catch(int e)
	{
		if (e>0)
		{
			//s->~statement();
			delete s;
		}
		throw;
	}
}

void Parser::SELECT()
{
	select_statement *ss = new select_statement;
	s = ss;
	FIELD_LIST(ss);
	if (lex[i].type==LEX_FROM) {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)   {strcpy(ss->TableName,lex[i].value.c_str()); i++;}
	else {throw i;}
	WHERE();
}

void Parser::FIELD_LIST(select_statement *ss)
{
	if (lex[i].type==LEX_TIMES) {ss->times=true; i++;} //если * то полностью копируем структуру таблицы в vector<FieldDef>
	else
	{
		ss->times=false;
		while(lex[i-1].type!=LEX_FROM)
		{
			if(lex[i].type==LEX_FINISH) {throw i;}
			if(lex[i].type==LEX_ID)  {ss->fieldname.push_back(lex[i].value); i++;}
			else throw i;
			if(lex[i].type==LEX_COMMA || lex[i].type==LEX_FROM){i++;}
			else throw i;
		}
		i--;
	}
}

void Parser::INSERT()
{
	insert_statement *is = new insert_statement; // current statement - insert-statement
	s = is;
	if (lex[i].type==LEX_INTO)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    { strcpy(is->TableName,lex[i].value.c_str()); i++;}
	else throw i;
	if (lex[i].type==LEX_LPAREN){i++;}
	else throw i;

	while(lex[i-1].type!=LEX_RPAREN)
	{
		if(lex[i].type==LEX_STRING || lex[i].type==LEX_NUM)  
		{
			if(lex[i].type==LEX_STRING)
				is->fieldtype.push_back(false);
			else
				is->fieldtype.push_back(true);
			is->fieldvalue.push_back(lex[i].value);
			i++;
		}
		else throw i;

		if(lex[i].type==LEX_COMMA || lex[i].type==LEX_RPAREN){i++;}
		else throw i;
	}
}

void Parser::UPDATE()
{
	update_statement *us = new update_statement;
	s = us;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_SET)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {i++;}
	else throw i;
	if (lex[i].type==LEX_EQ)  {i++;}
	else throw i;
	ALL_EXPR();
	us->expr = pol;
	pol.clear();
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
	create_statement *cs = new create_statement; 
	s = cs;
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {strcpy(cs->TableName,lex[i].value.c_str()); i++;}
	else throw i;
	if (lex[i].type==LEX_LPAREN){i++;}
	else throw i;
	while(lex[i-1].type!=LEX_RPAREN)
	{
		FIELD(cs);
		if(lex[i].type==LEX_COMMA || lex[i].type==LEX_RPAREN){i++;}
		else throw i;
	}
}

void Parser::FIELD(create_statement *cs)
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
	cs->fielddef.push_back(temp);
}

void Parser::DROP()
{
	drop_statement *ds = new drop_statement;
	s = ds;
	if (lex[i].type==LEX_TABLE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ID)    {strcpy(ds->TableName,lex[i].value.c_str());
		i++;
	}
	else throw i;
}

void Parser::WHERE() 
{
	if (lex[i].type==LEX_WHERE)  {i++;}
	else throw i;
	if (lex[i].type==LEX_ALL)   // | WHERE | ... | <--
	{
		where.type = ALL;
		i++;
	}
	else //LIKE, IN, LOGICAL
	{
		int i1 = i;
		lextype l = lex[i].type;
		while ((l != LEX_FINISH) && (l != LEX_LIKE) && (l != LEX_IN) && (l != LEX_ALL))
		{		
			l=lex[i1].type;
			i1++;
		}
		switch (l) 
		{
			case LEX_LIKE: //ready
				{
					where.type = LIKE;
					if (lex[i].type == LEX_ID) {where.field = lex[i].value; i++;}
					else throw i;
					if (lex[i].type == LEX_NOT) {where.neg = true; i++;}				
					if (lex[i].type != LEX_LIKE) throw i;
					else i++;
					if (lex[i].type == LEX_STRING) {where.pattern = lex[i].value; i++;}
					else throw i;
					break;
				}
			case LEX_IN: //ready
				{
					where.type = IN;
					lextype expected_type = ALL_EXPR();
					where.expr = pol;
					pol.clear();
					if (lex[i].type == LEX_NOT) 
					{
						where.neg = true;
						i++;
					}		
					if (lex[i].type != LEX_IN)
						throw i;
					i++;
					if (lex[i].type != LEX_LPAREN)
						throw i;
					i++;
					where.cl = CONST_LIST(expected_type);
					if (lex[i].type != LEX_RPAREN)
						throw i;
					i++;
					break;
				}
				default: //TODO
				{
				where.type = LOGIC;
				//stub
				break;
			}
		}
	}
}

lextype Parser::ALL_EXPR() 
{
	if (lex[i].type == LEX_STRING) 
	{
		pol.push_back(lex[i]);
		i++;
		return LEX_STRING;
	}
	else {
		EXPRESSION();
		if ((pol.size() == 1) && (pol.at(0).type == LEX_ID))
			return LEX_ID;
		else
			return LEX_NUM;
	}
}

void Parser::EXPRESSION() 
{
	Lex curr(LEX_NULL,"");
	ADD_LONG(); 
	if (lex[i].type == LEX_FINISH)
		return;
	lextype l = lex[i].type;

	if ((l == LEX_PLUS) || (l == LEX_MINUS)) {
		curr = lex[i];
		i++;
	}
	else
		return;

	while (true) {
		ADD_LONG();
		pol.push_back(curr);
		if (lex[i].type == LEX_FINISH)
			return;
		lextype l = lex[i].type;
		if ((l == LEX_PLUS) || (l == LEX_MINUS)) {
			curr = lex[i];
			i++;
			continue;
		}
		break;
	}
}

void Parser::ADD_LONG() 
{
	Lex curr(LEX_NULL,"");
	MUL_LONG();
	if (lex[i].type == LEX_FINISH)
		return;
	lextype l = lex[i].type;
	if ((l == LEX_TIMES) || (l == LEX_SLASH) || (l == LEX_PER)) 
	{
		curr = lex[i];
		i++;
	}
	else
		return;
	for(;;) 
	{
		MUL_LONG();
		pol.push_back(curr);
		if (lex[i].type == LEX_FINISH)
			return;
		l = lex[i].type;
		if ((l == LEX_TIMES) || (l == LEX_SLASH) || (l == LEX_PER)) 
		{
			curr = lex[i];
			i++;
			continue;
		}
		break;
	}
}

void Parser::MUL_LONG() 
{
	if (lex[i].type == LEX_LPAREN) 
	{
		i++;
		EXPRESSION();
		if (lex[i].type != LEX_RPAREN)
			throw i;
		i++;
	}
	else 
	{
		lextype l = lex[i].type;
		if ((l != LEX_ID) && (l != LEX_NUM))
			throw i;
		pol.push_back(lex[i]);
		i++;
	}
}

const_list Parser::CONST_LIST(lextype expected_type) 
{
		const_list lst;
		if ((lex[i].type == expected_type) || (expected_type == LEX_ID)) 
		{
			lst.type = lex[i].type;
			if ((lst.type != LEX_STRING) && (lst.type != LEX_NUM))
				throw i;
		}
		else
			throw i;

		for(;;) 
		{
			if (lst.type == lex[i].type) 
			{
				lst.mas.push_back(lex[i].value);
				i++;
			}
			else
				throw i; 

			if (lex[i].type == LEX_COMMA)
				i++;
			else
				break;
		}
		return lst;
	}

int Parser::get_i() const 
{
	return i;
}

void Parser::run()
{
	try
	{
		s->run();
		delete s;
	}
	catch(Errors err)
	{
		//s->~statement();
		delete s;
		throw;
	}
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

		//vector<bool>fieldtype; false=string, true=num
		if(fieldtype[i1]==true)
		{
			l=stol(fieldvalue[i1]);
			if((err = putLongNew(tableHandle, curr->fieldName, l)) != OK)
			{closeTable(tableHandle); throw err;}
		}
		else
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
	fieldtype.clear();
}

void drop_statement::run()
{
	if((err = deleteTable(TableName)) != OK)
		throw err;
}

void select_statement::run() //открываем исходную таблицу, создаем новую на основе струкруры старой, пока не дошли до конца копируем в буфер из исходной таблицы запись,  записываем в буфер новой таблицы при помощи getText, пихаем в новую таблицу
{	
	int i1=0;
	char buf[80],*buf2;
	long bufl;
	THandle tableHandle, newtableHandle;
	char tempfilename[]={"selectedtable\0"};
	TableStruct newtstr;
	vector<FieldDef>newfielddef;
	FieldDef temp;
	if((err = openTable(TableName, &tableHandle)) != OK)
		throw err;
	if (times==false)
	{
		for (auto i = fieldname.begin(); i != fieldname.end(); i++)
		{
			strcpy(temp.name,fieldname[i1].c_str());
			if((err = getFieldType(tableHandle, temp.name, &(temp.type))) != OK)
				{closeTable(tableHandle) ;throw err;}
			if(temp.type==Text)
			{
				getFieldLen(tableHandle, temp.name, &(temp.len));
			}
			newfielddef.push_back(temp);
			i1++;
		}
	}
	else //тупо копируем все поля исходной структуры в newfielddef
	{
		for(i1 = 0; i1 < tableHandle->tableInfo.fieldNumber; i1++)
		{
			FieldStruct *curr = tableHandle->pFieldStruct+i1;
			strcpy(temp.name,curr->fieldName);
			temp.type = curr->type;
			temp.len = curr->len;
			newfielddef.push_back(temp);
			fieldname.push_back(curr->fieldName);
		}
	}
	newtstr.numOfFields = newfielddef.size();
	newtstr.fieldsDef = &newfielddef[0];
	if((err = createTable(tempfilename,&newtstr)) != OK)
		{closeTable(tableHandle) ;throw err;}
	newfielddef.clear();

	if((err = openTable(tempfilename, &newtableHandle)) != OK)
		throw err;
	//идем в цикле по буферу старой таблицы
	//идем в цикле по новой таблице
	moveFirst(tableHandle);

	FieldType type;

	while (afterLast(tableHandle)!=TRUE)
	{
		i1=0;	
		for(int i = 0; i < newtableHandle->tableInfo.fieldNumber; i++)
		{
			strcpy(buf,fieldname[i].c_str());
			if((err = getFieldType(newtableHandle,buf,&type)) != OK)
				throw err;
			if(type==Text)
			{
				if((err = getText(tableHandle,buf,&buf2)) != OK)
					throw err;
				if((err = putText(newtableHandle,buf,buf2)) != OK)
					throw err;
				cout << buf << " " << buf2 << endl;
			}
			else
			{
				if((err = getLong(tableHandle,buf,&bufl)) != OK)
					throw err;
				if((err = putLong(newtableHandle,buf,bufl)) != OK)
					throw err;
			}
		}
		insertzNew(newtableHandle);
		createNew(newtableHandle);
		moveNext(tableHandle);
	}

	//
	if((err = closeTable(newtableHandle)) != OK)
		throw err;
	if((err = closeTable(tableHandle)) != OK)
		throw err;


	//
}

/*
   enum Errors putText(THandle tableHandle, char *fieldName,char *value);
   Функция присваивает полю fieldName - значение-строку 

   enum Errors putLong(THandle tableHandle, char *fieldName,long value);
   Функция присваивает полю fieldName значение - целое число

   enum Errors getText(THandle tableHandle, char *fieldName,char **pvalue);
   Функция присваивает переменной pvalue указатель на строку - значение поля fieldName

   enum Errors getLong(THandle tableHandle, char *fieldName,long *pvalue);
   Функция присваивает переменной pvalue целое число - значение поля fieldName.

   enum Errors getFieldLen(THandle tableHandle,char*fieldName,unsigned *plen);
   Функция выдает длину поля таблицы в байтах.

   enum Errors getFieldType(THandle tableHandle, char *fieldName, enum FieldType *ptype);
   Функция выдает тип поля таблицы.
 */


void update_statement::run()
{
	//stub
}

void delete_statement::run()
{
	//stub
}

statement::~statement()
{

}

