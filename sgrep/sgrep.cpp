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
class token : public Regex
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



class t_Char 
{
public:
	t_Char(string name) : name(name) {}

	bool is_char() {
		return true;
	}
	string get_char_name() {
		return name;
	}
private:
	string name;
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
	toklist tokenList = tokenize(argv[0]);
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
	return 0;
}

class Regex : public token
{
	string str;
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) { }

	virtual bool is_OrRegex() {
		return false;
	}
	virtual bool is_SeqRegex() {
		return false;
	}
	virtual bool is_StarRegex() {
		return false;
	}
	virtual bool is_SimpleRegex() {
		return false;
	}
	virtual bool is_InBrackets() {
		return false;
	}
	virtual bool is_BracketsItem() {
		return false;
	}
	/*virtual toklist getTokens() {
		throw - 1;
	}*/
	virtual string getString() {
		return str;
	}
};

class OrRegex : public Regex
{
	unique_ptr<SeqRegex> seq;
	unique_ptr<OrRegex> or;
public:
	OrRegex(unique_ptr<SeqRegex> &&seq) : seq(move(seq)) {};
	OrRegex(unique_ptr<SeqRegex> &&seq, unique_ptr<OrRegex> &&or) : seq(move(seq)), or(move(or)) {};

	bool is_OrRegex() {
		return true;
	}
};
class SeqRegex : public Regex {
	unique_ptr<Regex> star, seq;
public:
	SeqRegex(unique_ptr<Regex> &&star) : star(move(star)) {};
	SeqRegex(unique_ptr<Regex> &&star, unique_ptr<Regex> seq) : star(move(star)), seq(move(seq)) {};


	bool is_SeqRegex() {
		return true;
	}
};
class StarRegex : public Regex {
	unique_ptr<Regex> simple, star;
	string str;
public:
	StarRegex(unique_ptr<Regex> &&simple) : simple(move(simple)) {};
	StarRegex(unique_ptr<Regex> &&simple, unique_ptr<Regex> &&star) : simple(move(simple)), star(move(star)) {};

	bool is_StarRegex() {
		return true;
	}
};


class SimpleRegex : public Regex {
	unique_ptr<Regex> r;
	unique_ptr<token> t1;
	unique_ptr<token> t2;
	unique_ptr<token> t3;
	unique_ptr<token> t4;
public:
	/*
	first constructor can represent two rules:
		ParenOpen Regex ParenClose
		BracketOpen InBrackets BracketClose
			-> InBrackets is defined as a regex
				-> maybe I will change the definition
	*/
	SimpleRegex(unique_ptr<token> &&t1, unique_ptr<Regex> &&r, unique_ptr<token> &&t2) : t1(move(t1)), r(move(r)), t2(move(t2)) {};
	SimpleRegex(unique_ptr<token> &&t1, unique_ptr<token> &&t2, unique_ptr<Regex> &&r, unique_ptr<token> &&t3) : t1(move(t1)), t2(move(t2)), r(move(r)), t3(move(t3)) {};
	SimpleRegex(unique_ptr<token> &&t1) : t1(move(t1)) {};
	/*last constructor represents Dot or Char */

	bool is_SimpleRegex() {
		return true;
	}
};
class InBrackets : Regex {
	unique_ptr<Regex> brItem, inBr;
public:
	InBrackets(unique_ptr<Regex> &&brItem) : brItem(move(brItem)) {};
	InBrackets(unique_ptr<Regex> &&brItem, unique_ptr<Regex> &&inBr) : brItem(move(brItem)), inBr(move(inBr)) {};

	bool is_InBrackets() {
		return true;
	}
};
class BracketsItem : Regex {
	unique_ptr<token> t,t2,t3;
public: 
	BracketsItem(unique_ptr<token> t) : t(move(t)) {};
	BracketsItem(unique_ptr<token> t, unique_ptr<token> t2, unique_ptr<token> t3) : t(move(t)), t2(move(t2)), t3(move(t3)) {};

	bool is_BracketsItem() {
		return true;
	}
};






/*regular expression parser*/
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
	if (!t.empty() && t.front()->is_SeqRegex) {
		return make_unique<SeqRegex>(move(star), parse_SeqRegex(t));
	}
	else {
		return star;
	}
}
unique_ptr<Regex> parse_StarRegex(toklist& t) {
	unique_ptr<Regex> simple = parse_SimpleRegex(t);
	if (!t.empty() && t.front()->is_star) {
		t.pop_front();
		return make_unique<StarRegex>(move(simple), parse_StarRegex(t));
	}
	else {
		return simple;
	}
}
unique_ptr<Regex> parse_SimpleRegex(toklist& t) {
	
	if (!t.empty() && t.front()->is_parenOpen) {
		t.pop_front();
		unique_ptr<Regex> reg = parse_Regex(t);
		if (!t.empty() && t.front()->is_parenClose) {
			return make_unique<SimpleRegex>(reg);
		}
		else {
			/*spatne uzavorkovani*/
		}
	}
	else if (!t.empty() && t.front()->is_bracketOpen) {
		t.pop_front();
		unique_ptr<Regex> br = parse_InBrackets(t);

		if (!t.empty() && t.front()->is_bracketClose) {
			return make_unique<SimpleRegex>(br);
		}
		else {
			/* spatne uzavorkovani */
		}
	}
	else if (!t.empty() && t.front()->is_dot) {
		t.pop_front();
		t_Dot d;
		return make_unique<SimpleRegex>(d);
	}
	else if (!t.empty() && t.front()->is_char) {
		t_Char c(t.pop_front);
		return make_unique<SimpleRegex>(c);
	}
}

/* Bracket parser */
unique_ptr<Regex> parse_InBrackets(toklist& t) {

	if (!t.empty() && t.front()->is_BracketsItem) {

	}

}
unique_ptr<Regex> parse_BracketsItem(toklist& t) {

}
























