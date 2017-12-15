#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <map>
#include <fstream>
#include "Regexlib.h"


using namespace std;


/*
	Trida, ktera rozezna o jaky token se jedna
	pouzivam u vsech funkci atribut virtual
*/
class token
{
public:

	virtual bool is_char() {
		return false;
	}
	virtual bool is_bracketOpen() {
		return false;
	}
	virtual bool is_bracketClose() {
		return false;
	}
	virtual bool is_parenOpen() {
		return false;
	}
	virtual bool is_parenClose() {
		return false;
	}
	virtual bool is_hyphen() {
		return false;
	}
	virtual bool is_star() {
		return false;
	}
	virtual bool is_caret() {
		return false;
	}
	virtual bool is_dot() {
		return false;
	}
	virtual bool is_or() {
		return false;
	}
	virtual string get_var_name() {
		throw - 1;
	}
};



/*
Makro pro zachytavani tokenu
za dva hashtagy a "a" se vypise nazev promenny

vytvorim 8 tokenu 

	pro zachytavani charu, zavorek,
	hranatych zavorek ... atd.
*/


#define make_token(a) class t_##a : public token { \
public:bool is_##a() { return true; }};

make_token(BracketOpen);
make_token(BracketClose);
make_token(ParenOpen);
make_token(ParenClose);
make_token(Hyphen);
make_token(Star);
make_token(Caret);
make_token(Dot);
make_token(Or);
make_token(Char);


/*
	vytvari token t_char ze znakù

		Mozna jeste zrusim.
			-> protože jsem prave vytvoril token pro Char a z "politickych" duvodu to bude 
			   lepsi
*/
class t_Char : public token 
{
public:
	t_Char(char name) : name(name) {}

	bool is_char() {
		return true;
	}
	char get_char_name() {
		return name;
	}
private:
	char name;
};

/*
	vytvari vsechny ostatni tokeny
*/
class t_var : public token
{
public:
	t_var(const string& name) : name(name) {}

	bool is_var() {
		return true;
	}

	string get_var_name() {
		return name;
	}
private:
	string name;
};


/*
	Vytvor seznam tokenu pomoci funkce tokenize()
*/

using toklist = list<unique_ptr<token>>;
toklist tokenize(string regexp) 
{
	toklist t;
	for (char c : regexp) {

#define MATCH_TOKEN(s,l) if (c == s) { t.push_back(make_unique<l>()); }

		MATCH_TOKEN('[', t_BracketOpen)
		else MATCH_TOKEN(']', t_BracketClose)
		else MATCH_TOKEN('(', t_ParenOpen)
		else MATCH_TOKEN(')', t_ParenClose)
		else MATCH_TOKEN('-', t_Hyphen)
		else MATCH_TOKEN('*', t_Star)
		else MATCH_TOKEN('^', t_Caret)
		else MATCH_TOKEN('.', t_Dot)
		else MATCH_TOKEN('|', t_Or)
		else {
		t.push_back(make_unique<t_Char>(c));
		}

#undef MATCH_TOKEN(s,t)
	}
	return t;
}

int main(int argc, char ** argv) 
{
	
	string filename;
	if (argc == 2)
		filename = argv[1];
	else {
		cerr << "You cannot enter parameters" << endl;
		return(1);
	}
	ifstream in(filename);
	if (!in.good()) {
		cerr << "File cannot be open" << endl;
		return 2;
	}
	toklist tokenList = tokenize(argv[0]);

	auto AST = parse_Regex(tokenList);


	return 0;
}


/*Regex bude muj interface
v nem budu mit funkci, ktera zapocitava pocet schod
*/
class Regex {
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;
	list<unique_ptr<Regex>> l;
};


class OrRegex : public Regex
{
public:
	list<unique_ptr<Regex>> l;
};
class SeqRegex : public Regex
{
public:
	list<unique_ptr<Regex>> l;
private:
	string regex;
};
class StarRegex : public Regex
{
public:
	list<unique_ptr<Regex>> l;
private:
	string regex;
};
class SimpleRegex : public Regex
{
public:
	list<unique_ptr<Regex>> l;
private:
	string regex;
};
class StarRegex : public Regex
{
public:
	list<unique_ptr<Regex>> l;
private:
	string regex;
};
class InBrackets {
public:
private:
	string token;
};
class BracketsItem {
public:
private:
	string token;
};



Regex parse_Regex(toklist &t) {
	//Regex r;

	//if (t.empty) { return nullptr; }
	toklist regex;
	for (auto &i : t )
	{
		if (i.get()->get_var_name != "t_Or") {
			
		}
		else {
			regex.push_back(i);
		}
	}

}
unique_ptr<Regex> parse_OrRegex(toklist &t) {

}
unique_ptr<Regex> parse_SeqRegex(toklist &t) {

}
unique_ptr<Regex> parse_StarRegex(toklist &t) {

}
unique_ptr<Regex> parse_SimpleRegex(toklist &t) {

}
