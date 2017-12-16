#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <map>
#include <fstream>
#include "Regexlib.h"


using namespace std;

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


using regexlist = list<unique_ptr<Regex>>;

class Regex  {
	size_t num;
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;
};

class OrRegex : public Regex {

};
class StarRegex : public Regex {

};
class SimpleRegex : public Regex {

};
class InBrackets : public Regex {

};
class BracketsItem : public Regex {

};
/*
char a star budou obsahovat tokeny
*/


class Char : public Regex { 
	string t;
public:
	Char(string t) : t(t) {};
	
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		if (t[0] == '.') {		// tecka matchuje cokoliv
			return 1;
		}
		else if (t[0] != '[') {			
			if (t[0] == str[begin]) {
				return 1;
			}
			else {		
				return 0;
			}
		}
		else {							// pokud je prvni znak zavorka, musim vybirat znak  z mnoziny
			if (t[1] == '^') {

			}
			else {
				for (size_t i = 1; i < t.length - 2; i++)
				{
					if (token[i+1] == '-') {

					}
				}
			}
		}

	}
};
class BracketsItem {
public:

};

class Star : public Regex {
string token;
public: 
	Star(string token) : token(token) {};
	
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t matched = 0;
		for (size_t i = begin; i < max_len; i+=token.length)
		{
			/*vždycky se posunu o delku tokenu a kouknu se jestli dana cast je rovna*/
			if (token == str.substr(begin, token.length)) {
				matched += token.length;
			}
		}
		return matched;
	}
};

























int main(int argc, char ** argv) {
	toklist t = tokenize(argv[0]);
	auto regex = 

	return 0;
}





















