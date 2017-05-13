#include <iostream>
#include <string>
#include <vector>
#include "lex.h"
#include "syn.h"


using namespace std;


int main()
{	
	Lexer lx;
	Parser ps;
	try
	{
		while (lx.checkState()==true)
		{
			//lx.from_file("test2"); 
			//lx.from_string("INSERT INTO Students ('Sergey', 'Ivanov', 18, '145-45-45')"); 

			/* если не вызваны from_file или from_string, читает из stdin */
			lx.process();
			if (lx.isEmpty())
				break;
			//lx.printlex();
			//cout << "<---MAGIC BEGINS HERE--->" << endl;
			ps.SQL();
			if (lex[ps.get_i()].type == LEX_FINISH)
			{
				//cout << "SUCCESS PARSING COMMAND" << endl;
				ps.run();
			}

			else
				throw ps.get_i();
		} 
	} 
	catch (int i)
	{
		cout << "\033[1;31mError: \033[0minvalid lexeme at position " << i << ":" << endl << "TYPE: " << lex[i].GetType() << endl << "VALUE:" << lex[i].GetValue() << endl;
	}
	catch (string s)
	{
		cout << "\033[1;31mError: \033[0mCould not open the file '" << s << "'" << endl;
	}
	catch (Errors err)
	{
		cout << "\033[1;31mError: \033[0m" << getErrorString(err) << endl;
	}
	return 0;
}


