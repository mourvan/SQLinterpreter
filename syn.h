extern "C" 
{
	#include "Table.h"
}

using namespace std;

enum where_type {
	IN,
	LIKE,
	ALL,
	LOGIC
};

struct where_clause
{
	where_type type;
	where_clause();
};

struct wherein_clause : where_clause
{
	wherein_clause();
};

struct wherelike_clause : where_clause
{
	wherelike_clause();
};

struct wherelogic_clause : where_clause
{
	wherelogic_clause();
};

struct statement
{
	Errors err;
	char TableName[80];
	virtual void run()=0;
	virtual ~statement();
};

struct insert_statement : statement  /*DONE*/
{
	vector<string>fieldvalue;
	vector<bool>fieldtype; 
	void run();
};

struct create_statement : statement  /*DONE*/
{
	vector<FieldDef>fielddef;
	void run();
};

struct drop_statement : statement    /*DONE*/
{
	void run();
};

struct select_statement : statement  
{
	where_clause* w;
	bool times=false;
	vector<string>fieldname;
	//vector<FieldDef>newfielddef; открываем исходную таблицу, создаем новую на основе струкруры старой, пока не дошли до конца копируем в буфер из исходной таблицы запись,  записываем в буфер новой таблицы при помощи getText, пихаем в новую таблицу
	
	void run();
};
/*
enum Errors getText(THandle tableHandle, char *fieldName,char **pvalue);
Функция присваивает переменной pvalue указатель на строку - значение поля fieldName

enum Errors getLong(THandle tableHandle, char *fieldName,long *pvalue);
Функция присваивает переменной pvalue целое число - значение поля fieldName.

enum Errors getFieldLen(THandle tableHandle,char*fieldName,unsigned *plen);
Функция выдает длину поля таблицы в байтах.

enum Errors getFieldType(THandle tableHandle, char *fieldName, enum FieldType *ptype);
Функция выдает тип поля таблицы.
*/

struct update_statement : statement
{
	where_clause* w;
	void run();
};

struct delete_statement : statement
{
	where_clause* w;
	void run();
};


class Parser
{
	int i;
	statement *s;
	/* основные */
	void SELECT();      
	void INSERT();  //DONE  
	void UPDATE();		
	void DELETE();		
	void CREATE();	//DONE	
	void DROP();	//DONE
	/* вспомогательные */
	void FIELD_LIST(select_statement *ss); 		
	void WHERE();
	void FIELD(create_statement *cs);		
public:
	//Parser();
	void SQL();         //       
	int get_i() const;
	void run();
};


/*
	CREATE TABLE Students (First_name TEXT (10),
	Surname TEXT (15),
	Age LONG,
	Phone TEXT (9) )
 */
