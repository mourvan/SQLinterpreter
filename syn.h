
extern "C"
{
	#include "Table.h"
}

using namespace std;

struct statement
{
	char TableName[80];
	virtual void run()=0;
};

struct insert_statement : statement
{
	void run();
};

struct create_statement : statement
{
	vector<FieldDef>fields; //содержит int numOfFields;struct FieldDef *fieldsDef;
	void run();
};

struct drop_statement : statement
{
	void run();
};

class Parser
{
	int i;
	insert_statement is;
	create_statement cs;
	drop_statement ds;
	statement *s;
	/* основные */
	void SELECT();      
	void INSERT();      
	void UPDATE();		
	void DELETE();		
	void CREATE();	//DONE	
	void DROP();	//DONE
	/* вспомогательные */
	void FIELD_LIST(); 		
	void WHERE();
	void FIELD();		
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
