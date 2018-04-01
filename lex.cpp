#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lex.h"

vector<Lex>lex;

Lex::Lex(lextype t, string v = "") :type(t), value(v)
{
}

lextype Lex::GetType() const
{
	return type;
}

string Lex::GetValue() const
{
	return value;
}

int Lexer::look(char c) const
{
  for (int i=0;i<11;i++)
		if (c==dl[i])
			return i;
	return -1;
}

int Lexer::look(string s) const
{
	for (int i=0;i<20;i++)
		if (s == keywords[i])
			return i;
	return -1;
}

void Lexer::printlex() const
{
	for (auto i = lex.begin(); i != lex.end(); ++i)
		cout << "(" << i->GetType() << "," << i->GetValue() << ")" << endl;
}

void Lexer::process()
{
	int c,c1;
	int i,i1;
	lex.clear();
	do							 //по строке
	{
		if ((c=getchar())== EOF)
		{
			state = false;
			break;
		}
		if ((i = look(c)) != -1) //встретился разделитель
		{
			//	cout << i << endl;
			if (buf != "")
			{
				if ((i1 = look(buf)) != -1)
					lex.emplace_back((lextype)i1);
				else
				{
					try{stol(buf);
		
						lex.emplace_back(LEX_NUM, buf);}
					catch(invalid_argument)
						{lex.emplace_back(LEX_ID, buf);}
				}
				buf = "";
			}
			if (c=='>' || c=='<' || c=='!')
			{
				c1=getchar();
				if (c1 == '=')
				{
					if (c=='<')
						lex.emplace_back(LEX_LEQ);
					else if (c=='>')
						lex.emplace_back(LEX_GEQ);
					else if (c=='!')
						lex.emplace_back(LEX_NEQ);
				}
			} else
			{
				//	buf.push_back(c1);
				lex.emplace_back((lextype)(i+20));
			}
		} else if (c==' ' || c=='\n' || c=='\r')			//пробел
		{
			if (buf != "")
			{
				if ((i = look(buf)) != -1)
					lex.emplace_back((lextype)i);
				else
				{
					try{stol(buf);
		
						lex.emplace_back(LEX_NUM, buf);}
					catch(invalid_argument)
						{lex.emplace_back(LEX_ID, buf);}
				}
				buf = "";
			}
		} else if (c=='\'')		//кавычка, записать буфер в лексему, начать читать
		{
			if (buf != "")
			{
				if ((i = look(buf)) != -1)
					lex.emplace_back((lextype)i);
				else
				{
					try{stol(buf);
		
						lex.emplace_back(LEX_NUM, buf);}
					catch(invalid_argument)
						{lex.emplace_back(LEX_ID, buf);}
				}
				buf = "";
			}
			while ((c=getchar())!= EOF && c!='\'')
				buf.push_back(c);
			if (buf != "")
				lex.emplace_back(LEX_STRING, buf);
			buf = "";
		} else
			buf.push_back(c);
	}
	while (c!='\n' && c!='\r');
	lex.emplace_back(LEX_FINISH);
}

void Lexer::from_file(string filename) //перенаправляем ввод с stdin в файл filename
{
	file = open(filename.c_str(),O_RDONLY,0);
	if (file<0) 
	{
		throw filename;
	}
	dup2(file,STDIN_FILENO);		
}

void Lexer::from_string(string s)  //создаем файл .temp со строкой, юзаем from_file(".temp")
{
	file = creat(".temp",0644);
	close(file);
	file = open(".temp", O_APPEND | O_WRONLY);
	write(file,s.c_str(), s.length());
	close(file);
    from_file(".temp");
}

bool Lexer::checkState() const
{
	return state;
}

bool Lexer::isEmpty() const
{
	if (lex.empty()==true || lex[0].type==LEX_FINISH)
		return true;
	else 
		return false;
}

