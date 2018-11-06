
#pragma once

#include <iostream>
#include <vector>
#include <string>

class test_item
{
public:
	test_item();
	test_item(int i);
	test_item(const test_item&);
	test_item(test_item&&);
	test_item& operator=(const test_item&);
	test_item& operator=(test_item&&) noexcept;
	test_item& operator=(int i);
	~test_item();
	int compare(const test_item&) const;
friend
	std::ostream& operator << (std::ostream&, const test_item&);
private:
	unsigned char magic[8];
	int value;
	enum { uninitialized, proper, movedfrom, deleted } state;
public:
	static const std::vector<std::string>& report();
	static bool error();
};

/**/ inline bool operator < (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) < 0; }

/**/ inline bool operator <= (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) <= 0; }

/**/ inline bool operator == (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) == 0; }

/**/ inline bool operator != (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) != 0; }

/**/ inline bool operator > (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) > 0; }

/**/ inline bool operator >= (const test_item& lhs, const test_item& rhs)
{ return lhs.compare(rhs) >= 0; }

