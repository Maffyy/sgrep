#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <map>
#include <set>
#include <fstream>

using namespace std;

class token 
{
public:

	virtual bool is_Char() {
		return false;
	}
	virtual bool is_BracketOpen() {
		return false;
	}
	virtual bool is_BracketClose() {
		return false;
	}
	virtual bool is_ParenOpen() {
		return false;
	}
	virtual bool is_ParenClose() {
		return false;
	}
	virtual bool is_Hyphen() {
		return false;
	}
	virtual bool is_Star() {
		return false;
	}
	virtual bool is_Caret() {
		return false;
	}
	virtual bool is_Dot() {
		return false;
	}
	virtual bool is_Or() {
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

class t_Char : public token
{
public:
	t_Char(char& name) : name(name) {}

	bool is_Char() {
		return true;
	}
	char get_char() {
		return name;
	}
private:
	char name;
};

using toklist = list<unique_ptr<token>>;
toklist tokenize(const string& regexp) 
{
	toklist t;
	bool backslash = false;
	for (char c : regexp) {
		//cout << c << endl;

		// TO-DO: Osetrit, pokud bude backslash na konci.
		if (c == '\\') {
			backslash = true;
			continue;
		} 
		

#define MATCH_TOKEN(s,l) if (c == s && !backslash) { t.push_back(make_unique<l>()); }
		
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
		if (c != '\\') { backslash = false; }
#undef MATCH_TOKEN

	}
	return t;
}
/*TO-DO: vratit specialni hodnotu, pokud funkce neuspeje u nekterych regexu*/

class Regex  {
	size_t num;

public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;

	virtual bool contain_ParenRegex() {
		return false;
	}
	// TO-DO: pokud nematchuju star, tak mi to vyhodi nejakou jinou hodnotu
	const size_t npos = -1;
};


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
		size_t matched = 0;
		size_t m = 0;
		/* jak se posouvat v seqregexu */

		for (size_t i = begin; i < begin + max_len; i += m)
		{
			m = star->match_part(str, i, max_len);
			//cout << m << endl;
			if (m == 0) { i++;  }
			matched += m;
		}

		//matched = star->match_part(str, begin, max_len);
		//cout << "true" << endl;
		size_t matched2 = seq->match_part(str, begin + matched, max_len);
	//	if (matched2 == 0) { return 0; }
		return matched + matched2;
	}
};

class StarRegex : public Regex {
	unique_ptr<Regex> star;
public:
	StarRegex(unique_ptr<Regex> &&star) : star(move(star)) {};
	// TO-DO: You have defined ParenRegex
	//			Maybe you should implement it here.
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t matched = 0;
		size_t m =0;
		for (size_t i = begin; i < begin+max_len; i+=m)
		{
			m = star->match_part(str, i, max_len);
			//cout << m << endl;
			if (m == 0) { matched = 0; break; }
			matched += m;
		}
		return matched;
	}
};
class ParenRegex : public Regex {
	unique_ptr<Regex> paren;
public:

	ParenRegex(unique_ptr<Regex> &&paren) : paren(move(paren)) {};

	bool contain_ParenRegex() {
		return true;
	}
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		return paren->match_part(str, begin, max_len);
	}
};

class Char : public Regex {
	set<char> t;
	bool caret;
public:
	Char(set<char>&& t, bool caret = false) : t(move(t)), caret(caret) {};

	size_t match_part(const string& str, size_t begin, size_t max_len) {
		if (t.count(str[begin]) && !caret) {
		//	cout << "true" << endl;
			return 1;
		}
		else if (!t.count(str[begin]) && caret) {
			return 0;
		}
		else return 0;
	}
};


unique_ptr<Regex> parse_Regex(toklist& t);

unique_ptr<Regex> parse_SimpleRegex(toklist& t) {

	
	if (!t.empty() && t.front()->is_Char()) {
		set<char> s;
		s.insert(t.front()->get_char());
		t.pop_front();
		return make_unique<Char>(move(s));
	}
	if (!t.empty() && t.front()->is_Dot()) {
		set<char> s;
		for (size_t i = 0; i < 256; i++) { s.insert(i); }
		t.pop_front();
		return make_unique<Char>(move(s));
	}
	char c = 0;
	if (!t.empty() && t.front()->is_BracketOpen()) {
		set<char> s;
		t.pop_front();
		bool caret = false;
		while (!t.front()->is_BracketClose()) {
			if (t.front()->is_Caret()) { caret = true; }
			if (t.front()->is_Char()) { 
				c = t.front()->get_char();
				s.insert(c);
//				cout << "true" << endl;
			}
			if (t.front()->is_Hyphen()) {
				t.pop_front();
				if (!t.empty() && t.front()->is_Char() && c != 0) {
					int i = (int)c + 1;
					while (i != (int)t.front()->get_char()) {
						s.insert(i);
						++i;
					}
				}
				else {
					//cout << "true" << endl;
					throw - 1;
				}
			}
			t.pop_front();
		}
		if (!t.empty() && t.front()->is_BracketClose()) { t.pop_front(); }
		else {
		//	cout << "true" << endl;
			throw - 3;
		}
		if (caret) { return make_unique<Char>(move(s), true); }
		else {
			//cout << "true" << endl;
			return make_unique<Char>(move(s)); }
	}
	// TO-DO: You have to find a way, how to make ParenRegex item.
	if (!t.empty() && t.front()->is_ParenOpen()) {
		t.pop_front();
		auto regex = parse_Regex(t);
		if (!t.empty() && t.front()->is_ParenClose()) {
			t.pop_front();
			return make_unique<ParenRegex>(move(regex));
		}
		else {
			throw - 3;
		}
	}
	throw - 3;
}


unique_ptr<Regex> parse_StarRegex(toklist& t) {
	unique_ptr<Regex> simple = parse_SimpleRegex(t);
	if (!t.empty() && t.front()->is_Star()) {
		t.pop_front();
		return make_unique<StarRegex>(move(simple));
	}
	else {
		return simple;
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

unique_ptr<Regex> parse_OrRegex(toklist& t) {
	unique_ptr<Regex> seq = parse_SeqRegex(t);
	if (!t.empty() && t.front()->is_Or()) {
		t.pop_front();
		return make_unique<OrRegex>(move(seq), parse_OrRegex(t));
	}
	else {
		return seq;
	}
}

unique_ptr<Regex> parse_Regex(toklist& t) {
	return parse_OrRegex(t);
}

//IDE geany

int main(int argc, char ** argv) {

	toklist t = tokenize("c");
	
	int x = 5;

	auto regex = parse_Regex(t);

	cout << regex.get()->match_part("bca", 0, 3) << endl;
	for(;;){}
	return 0;
}





















