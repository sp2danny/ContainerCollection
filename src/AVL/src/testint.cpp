
#include "stdafx.h"

#include <sstream>
#include <vector>
#include <string>
#include <cstring>

#include "boost/multiprecision/cpp_int.hpp"

using namespace boost;

using namespace boost::multiprecision;

using namespace std::string_literals;

using namespace std;

int   aa = 3;
float b  = 0.0f;

template<typename Float>
class Foo
{
public:
	Foo() : f() {}

private:
	Float f;
};

template<typename T>
std::string to_string(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

void foo2()
{
	cpp_int u = 1;
	for (unsigned i = 1; i <= 100; ++i)
		u *= i;
	auto x = gcd(u, 1000);
	if (to_string(x) != ""s)
		std::cout << "odd " << to_string(5);
	std::cout << u << " : " << x << std::endl;
}

unsigned char cconv(char c)
{
	return c;
};

vector<size_t> boyer_moore(string text, string searchstring)
{
	size_t table[256];

	auto slen = searchstring.length();
	if (!slen) return {};
	for (size_t i = 0; i < 255u; ++i)
		table[i] = slen;

	for (size_t i = 0; i < slen; ++i)
	{
		table[ cconv(searchstring[i]) ] = slen-i-1;
	}

	auto pos  = slen-1;
	auto tlen = text.length();
	if (!tlen) return {};

	vector<size_t> ret;

	while (pos < tlen)
	{
		auto lu = table[ cconv(text[pos]) ];
		if (lu)
		{
			pos += lu;
			continue;
		}
		auto spos = pos - slen + 1;
		if (memcmp(searchstring.c_str(), text.c_str() + spos, slen)==0)
		{
			ret.push_back(spos);
		}
		++pos;
	}

	return ret;
}

void FooBar()
{
	string s1 = "det var en gång en liten pilt som hette kalle";
	string s2 = "pilt";

	auto bm = boyer_moore(s1, s2);

	for (auto x : bm)
	{
		cout << x << endl;
	}
}

