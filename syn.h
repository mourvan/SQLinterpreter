#include "Table.h"

using namespace std;

class Parser
{
	int i;
	void SELECT();      //
	void FIELD_LIST();  //DONE
	void INSERT();      //DONE
	void UPDATE();		//
	void DELETE();		//
	void CREATE();		//DONE
	void FIELD();		//DONE
	void DROP();		//DONE
	void WHERE();		//
public:
	Parser();
	void SQL();         //       
	int get_i() const;
};

struct statement
{
	char* TableName;
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
/*
	CREATE TABLE Students (First_name TEXT (10),
	Surname TEXT (15),
	Age LONG,
	Phone TEXT (9) )
 */
