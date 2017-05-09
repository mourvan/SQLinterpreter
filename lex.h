#include <string>

using namespace std;

enum lextype
{
	/* Служебные */
	LEX_NULL,		// 0
	LEX_SELECT,		// 1
	LEX_INSERT,		// 2
	LEX_UPDATE,		// 3	
	LEX_DELETE,		// 4
	LEX_CREATE,		// 5
	LEX_DROP,		// 6
	LEX_FROM,		// 7
	LEX_WHERE,		// 8
	LEX_INTO,		// 9
	LEX_SET,		// 10
	LEX_TABLE,		// 11
	LEX_LONG,		// 12
	LEX_TEXT,		// 13
	LEX_NOT,		// 14
	LEX_AND,		// 15
	LEX_OR,			// 16
	LEX_LIKE,		// 17
	LEX_IN,			// 18
	LEX_ALL,		// 19

	/* Разделители */
	LEX_COMMA,		// 20
	LEX_LPAREN,		// 21
	LEX_RPAREN,		// 22
	LEX_EQ,			// 23
	LEX_LSS,		// 24
	LEX_GTR,		// 25
	LEX_PLUS,		// 26
	LEX_MINUS,		// 27
	LEX_TIMES,		// 28
	LEX_SLASH,		// 29
	LEX_LEQ,		// 30
	LEX_NEQ,		// 31
	LEX_GEQ,		// 32
	LEX_PER,		// 33

	/* Числа и идентификаторы.. */
	LEX_NUM,		// 34
	LEX_ID,			// 35
	LEX_STRING,		// 36
	
	/* Признак конца */
	LEX_FINISH      // 37
};

const string keywords[] =
{		
	"",// 0 позиция 0 не используется
	"SELECT",	// 1
	"INSERT",	// 2
	"UPDATE",	// 3	
	"DELETE",	// 4
	"CREATE",	// 5
	"DROP",		// 6
	"FROM",		// 7
	"WHERE",	// 8
	"INTO",		// 9
	"SET",		// 10
	"TABLE",	// 11
	"LONG",		// 12
	"TEXT",		// 13
	"NOT",		// 14
	"AND",		// 15
	"OR",		// 16
	"LIKE",		// 17
	"IN",		// 18
	"ALL",		// 19
	// Разделители
	",",		// 20  
	"(",		// 21
	")",		// 22
	"=", 		// 23
	"<",		// 24
	">",		// 25
	"+", 		// 26
	"-", 		// 27
	"*",		// 28
	"/",		// 29
	"%%",       // 30		
	"<=",		// 31
	"!=", 		// 32
	">=" 		// 33
};

const char dl[] = 
{
	',',		// 20	
	'(',		// 21
	')',		// 22
	'=', 		// 23
	'<',		// 24
	'>',		// 25
	'+', 		// 26
	'-', 		// 27
	'*',		// 28
	'/',		// 29
	'%'		    // 30
		//"<=',		// 31
		//"!=", 		// 32
		//">=", 		// 33
}; 


class Lex
{	
public:				
	lextype type;
	string value;		
	Lex(lextype t, string v);
	lextype GetType() const;
	string GetValue() const;
}; 

extern vector<Lex>lex;

class Lexer
{
	bool state = true;
	int file;
	string buf = "";
	int look(char c) const;
	int look(string s) const;
public:
	void from_file(string filename); 
	void from_string(string s); 
	void printlex() const;
	void process();
	bool checkState() const;
	bool isEmpty() const ;

	//читаем символы в буфер до разделителя,кавычки или пробела
	//если буфер не пустой
	//проверяем считанные символы на наличие в keywords, пихаем лексему, look
	//иначе проверяем первый символ на число, пихаем лексему
	//иначе идентификатор, пихаем лексему

	//если разделитель-пихаем лексему
	//кавычка-читаем до кавычки, пихаем лексему
	//пробел-заного буфер

	//кавычки - читаем до ближайшей кавычки, считаем как строку
};

