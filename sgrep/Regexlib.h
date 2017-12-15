#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <sstream>
#include <map>
#include <fstream>

using namespace std;

class Regexp {
public:
	virtual size_t match_part(const string& str, size_t begin, size_t max_len) = 0;
};

