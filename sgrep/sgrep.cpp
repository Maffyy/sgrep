#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <set>
#include <fstream>

using namespace std;

/* Token Interface */
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
};
/*Creates token classes*/
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
	t_Char(const char& name) : name(name) {}

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
	for (size_t i = 0; i < regexp.length(); ++i) {
		/*If there is a backslash, move to next cycle and process next char*/
		if (regexp[i] == '\\' && !backslash) {			
			backslash = true;
			continue;
		}
		if (backslash) {
			t.push_back(make_unique<t_Char>(regexp[i]));
			backslash = false;
			continue;
		}
/*Parse tokens to Token list*/
#define MATCH_TOKEN(s,l) if (regexp[i] == s ) { t.push_back(make_unique<l>()); }
		
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
		t.push_back(make_unique<t_Char>(regexp[i]));
		}
		
#undef MATCH_TOKEN
	}
	return t;
}

class Regex  {
	size_t num;
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;
	virtual bool contain_ParenRegex() {
		return false;
	}
	const size_t npos = -1;
};


class OrRegex : public Regex {
	unique_ptr<Regex> seq;
	unique_ptr<Regex> Or ;
public:
	OrRegex(unique_ptr<Regex> &&seq, unique_ptr<Regex> &&Or ) : seq(move(seq)), Or(move(Or)) {};
	bool contain_ParenRegex() {
		if (seq->contain_ParenRegex() || seq->contain_ParenRegex()) {
			return true;
		}
	}
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t left = seq->match_part(str, begin, max_len);
		size_t right = Or->match_part(str, begin, max_len);
		if (left == npos && right == npos) { return npos; }
		else if (left != npos && right == npos) { return left; }
		else { return right; }
	}
};
class SeqRegex : public Regex {
	unique_ptr<Regex> star, seq;
public:
	SeqRegex(unique_ptr<Regex> &&star, unique_ptr<Regex> seq) : star(move(star)), seq(move(seq)) {};
	bool contain_ParenRegex() {
		if (star->contain_ParenRegex() || seq->contain_ParenRegex()) {
			return true;
		}
	}
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		if (star->contain_ParenRegex()) {
			size_t matchedParen = star->match_part(str, begin, max_len);
			return matchedParen;
		}
		size_t matched = star->match_part(str, begin, max_len);
		if (matched == npos && (!star->contain_ParenRegex() || !seq->contain_ParenRegex())) { return npos; }
		if (seq->contain_ParenRegex()) {
			size_t matchedParen2 = seq->match_part(str, begin+matched, max_len);
			return matchedParen2;
		}
		size_t matched2 = seq->match_part(str, begin + matched, max_len);
		if (matched2 == npos) { return npos; }
		return matched + matched2;
	}
};

class StarRegex : public Regex {
	unique_ptr<Regex> star;
public:
	StarRegex(unique_ptr<Regex> &&star) : star(move(star)) {};
	bool contain_ParenRegex() {
		if (star->contain_ParenRegex()) {
			return true;
		}
	}
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t matched = 0;
		size_t m =0;
		for (size_t i = begin; i < str.length(); i+=m)
		{
			m = star->match_part(str, i, max_len);
			if (m == npos) { return matched; }
			matched += m;
		}
		return matched;
	}
};
class ParenRegex : public Regex {
	unique_ptr<Regex> paren;
public:
	ParenRegex(unique_ptr<Regex> &&paren) : paren(move(paren)) {};
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		size_t m = paren->match_part(str, begin, max_len);
		if (m == npos) { return npos; }
			cout << str.substr(begin) << endl;
		return m;
	}
	bool contain_ParenRegex() {
		return true;
	}
};
class Char : public Regex {
	set<char> t;
	bool caret;
public:
	Char(set<char>&& t, bool caret = false) : t(move(t)), caret(caret) {};
	bool contain_ParenRegex() {
		return false;
	}
	size_t match_part(const string& str, size_t begin, size_t max_len) {
		if (begin > str.length() - 1) { return npos; }
		if (t.count(str[begin]) && !caret) {
			return 1;
		}
		else if (!t.count(str[begin]) && caret) {
			return 1;
		}
		else return npos;
	}
};
unique_ptr<Regex> parse_Regex(toklist& t);
unique_ptr<Regex> parse_SimpleRegex(toklist& t) {

	/*Catch Char*/
	if (!t.empty() && t.front()->is_Char()) {
		set<char> s;
		s.insert(t.front()->get_char());
		t.pop_front();
		return make_unique<Char>(move(s));
	}
	/*Catch Dot*/
	if (!t.empty() && t.front()->is_Dot()) {
		set<char> s;
		for (size_t i = 0; i < 256; i++) { s.insert(i); }
		t.pop_front();
		return make_unique<Char>(move(s));
	}
	/*Catch Brackets*/
	char c = 0;
	if (!t.empty() && t.front()->is_BracketOpen()) {
		set<char> s;
		t.pop_front();
		bool caret = false;
		/*Catch all Chars in brackets*/
		while (!t.front()->is_BracketClose()) {
			if (t.front()->is_Caret()) { caret = true; }
			if (t.front()->is_Char()) { 
				c = t.front()->get_char();
				s.insert(c);
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
					throw "Missing EndChar for Hyphen";
				}
			}
			t.pop_front();
		}
		if (!t.empty() && t.front()->is_BracketClose()) { t.pop_front(); }
		else {
			throw "BracketClose Missing";
		}
		if (caret) { return make_unique<Char>(move(s), true); }
		else {
			return make_unique<Char>(move(s)); }
	}
	/*Catch ParenRegex*/
	if (!t.empty() && t.front()->is_ParenOpen()) {
		t.pop_front();
		auto regex = parse_Regex(t);
		
		if (!t.empty() && t.front()->is_ParenClose()) {
			t.pop_front();
			return  make_unique<ParenRegex>(move(regex));
		}
		else {
			throw "ParenClose Missing";
		}
	}
	if (!t.empty()) {
		throw "Loading regular expression failed!";
	}
	throw "Undefined Error";
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
	if (!t.empty() && !t.front()->is_Or() && !t.front()->is_ParenClose()) {
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

void processFile(toklist& t, ifstream& in) {
	auto regex = parse_Regex(t);
	const size_t npos = -1;
	while (in.good() && !in.eof()) {
		string line;
		getline(in,line);
		size_t res = -1;
		if (res == line.length()) {
			cout << line << endl;
		}
	}
}
void processInput(toklist& t) {
	auto regex = parse_Regex(t);
	const size_t npos = -1;
	while (cin.good() && !cin.eof())
	{
		string line;
		cin >> line;
		size_t res = -1;
	    res = regex->match_part(line, 0, line.length());
			if (res == line.length() && !regex->contain_ParenRegex()) { 
				cout << line << endl;
			}
	}
}
int main(int argc, char ** argv) {
	string filename;
	if (argc == 3) {
		filename = argv[2];
		toklist t = tokenize(argv[1]);
		ifstream in(filename);
		if (!in.good()) {
			cerr << "file cannot be open" << endl;
			return 1;
		}
		processFile(t, in);
	}
	else if (argc == 2) {
		toklist t = tokenize(argv[1]);
		processInput(t);
	}
	else {
		cout << "Wrong parameters" << endl;
		return 2;
	}
	return 0;
}

