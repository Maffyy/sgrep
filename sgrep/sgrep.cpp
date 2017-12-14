#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <map>
#include <fstream>

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

make_token(bracketOpen);
make_token(bracketClose);
make_token(parenOpen);
make_token(parenClose);
make_token(hyphen);
make_token(star);
make_token(caret);
make_token(dot);
make_token(or);


class t_char : public token 
{
public:
	t_char(char name) : name(name) {}

	bool is_char() {
		return true;
	}
	char get_char_name() {
		return name;
	}
private:
	char name;
};


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


using toklist = list<unique_ptr<token>>;
toklist tokenize(string regexp) 
{
	toklist t;
	for (char c : regexp) {

#define MATCH_TOKEN(s,l) if (c == s) { t.push_back(make_unique<l>()); }

		MATCH_TOKEN('[', t_bracketOpen)
		else MATCH_TOKEN(']', t_bracketClose)
		else MATCH_TOKEN('(', t_parenOpen)
		else MATCH_TOKEN(')', t_parenClose)
		else MATCH_TOKEN('-', t_hyphen)
		else MATCH_TOKEN('*', t_star)
		else MATCH_TOKEN('^', t_caret)
		else MATCH_TOKEN('.', t_dot)
		else MATCH_TOKEN('|', t_or)
		else {
		t.push_back(make_unique<t_char>(c));
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
	auto t = tokenize(in);
	//for (auto&i : t) cout << typeid(*i).name() << endl;
	auto p = parse_program(t);
	Scope s;
	p->run(s);







	return 0;
}