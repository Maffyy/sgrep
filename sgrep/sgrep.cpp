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
	virtual char get_char() {
		throw - 1;
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
	char get_char() {
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


class Regex  {
	size_t num;
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;
};
/*
Mam dva konstruktory
jeden prijima jenom 

*/
class OrRegex : public Regex {
	unique_ptr<Regex> seq;
	unique_ptr<Regex> or ;
public:
	OrRegex(unique_ptr<Regex> &&seq, unique_ptr<Regex> && or ) : seq(move(seq)), or (move(or )) {};

	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t left = seq->match_part(str, begin, max_len);
		size_t right = or->match_part(str, begin, max_len);
		if (left > right) { return left; }
		else { return right; }
	}
};
class SeqRegex : public Regex {
	unique_ptr<Regex> star, seq;
public:
	SeqRegex(unique_ptr<Regex> &&star, unique_ptr<Regex> seq) : star(move(star)), seq(move(seq)) {};

	size_t match_part(const string& str, size_t begin, size_t max_len) {
		return seq->match_part(str, begin, max_len) + star->match_part(str, begin, max_len);
	}
};

class StarRegex : public Regex {
	unique_ptr<Regex> simple, star;
public:
	StarRegex(unique_ptr<Regex> &&simple) : simple(move(simple)) {};
	StarRegex(unique_ptr<Regex> &&simple, unique_ptr<Regex> &&star) : simple(move(simple)), star(move(star)) {};

	size_t match_part(const string& str, size_t begin, size_t max_len) {
		return simple->match_part(str, begin, max_len) + star->match_part(str, begin, max_len);
	}
};




class Star : public Regex {
	toklist t;
	unique_ptr<Regex> r;
public:
	Star(toklist t) : t(t) {};
	Star(unique_ptr<Regex> &&r) : r(move(r)) {};

	size_t match_part(const string& str, size_t begin, size_t max_len) {
		
	}
};


class Char : public Regex {
	toklist t;
public:
	Char(toklist t) : t(t) {};
	size_t match_part(const string& str, size_t begin, size_t max_len) {
	/*	bool caret = false;
		if (!t.empty) {
			if (t.front()->is_bracketOpen && t.back()->is_bracketClose) {
				t.pop_front();
				t.pop_back();
				if (t.front()->is_caret) {
					t.pop_front();
					caret = true;
				}
				char c;
				while (!t.empty()) {
					if (t.front()->is_char()) {
						c = t.front()->get_char();
						t.pop_front();
						if (!caret && c == str[begin] ) {
							return 1;
						}
						if (caret && c != str[begin]) {
							return 1;
						}

						if (!t.empty() && t.front()->is_hyphen()) {
							t.pop_front();
							char c2 = t.front()->get_char();
							
							if (int(str[begin]) > int(c) && int(str[begin]) <= int(c2) && !caret) {
								return 1;
							}
						}
						
					}
					if (t.front()->is_char()) {

					}
				}
			}
			if (t.front()->is_dot()) {
				t.pop_front();
			}
			if (t.front()->is_char()) {
				t.pop_front();
			}
		
		}
	*/
	}
};



/*regular expression parser */
unique_ptr<Regex> parse_Regex(toklist& t) {
	return parse_OrRegex(t);
}
unique_ptr<Regex> parse_OrRegex(toklist& t) {
	unique_ptr<Regex> seq = parse_SeqRegex(t);
	if (!t.empty() && t.front()->is_or()) {
		t.pop_front();
		return make_unique<OrRegex>(move(seq), parse_OrRegex(t));
	}
	else {
		return seq;
	}
}
unique_ptr<Regex> parse_SeqRegex(toklist& t) {
	unique_ptr<Regex> star = parse_StarRegex(t);
	if (!t.empty()) {
		return make_unique<SeqRegex>(move(star), parse_SeqRegex(t));
	}
	else {
		return star;
	}
}
unique_ptr<Regex> parse_StarRegex(toklist& t) {
	unique_ptr<Regex> simple = parse_SimpleRegex(t);
	if (!t.empty() && t.front()->is_star()) {
		return make_unique<StarRegex>(move(simple), parse_StarRegex(t));
	}
	else {
		return simple;
	}
}

unique_ptr<Regex> parse_SimpleRegex(toklist& t) {
	
	if ((!t.empty) && (t.front()->is_char() || (t.front()->is_bracketOpen()))) {
		return make_unique<Char>(t);
	}
	if (!t.empty && t.front()->is_parenOpen()) {
		t.pop_front();
		auto regex = parse_Regex(t);
		if (!t.empty() && t.front()->is_parenClose()) {
			t.pop_front();
			return regex;
		}
	}
}
























int main(int argc, char ** argv) {
	toklist t = tokenize(argv[0]);
	auto regex = 

	return 0;
}





















