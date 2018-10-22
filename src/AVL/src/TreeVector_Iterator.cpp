
#include "TreeVector.hpp"

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator*() -> T&
{
	return node->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator-> () -> T*
{
	return std::addressof(node->item());
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator[](int idx) -> T&
{
	idx += owner->nodeIdx(node);
	NodeP n = owner->getIdx(owner->root->left(), idx);
	return n->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator++() -> iterator&
{
	node = owner->nextNode(node);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator--() -> iterator&
{
	node = owner->prevNode(node);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator++(int) -> iterator
{
	iterator temp = *this;
	++*this;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator--(int) -> iterator
{
	iterator temp = *this;
	--*this;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator+=(std::ptrdiff_t dst) -> iterator&
{
	int idx = owner->nodeIdx(node);
	idx += dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator-=(std::ptrdiff_t dst) -> iterator&
{
	int idx = owner->nodeIdx(node);
	idx -= dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator-(iterator other) -> std::ptrdiff_t
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 - idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator+(std::ptrdiff_t dst) -> iterator
{
	iterator temp = *this;
	temp += dst;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator-(std::ptrdiff_t dst) -> iterator
{
	iterator temp = *this;
	temp -= dst;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator==(iterator other) -> bool
{
	assert(owner == other.owner);
	return node == other.node;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator!=(iterator other) -> bool
{
	assert(owner == other.owner);
	return node != other.node;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator<(iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 < idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator<=(iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 <= idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator>(iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 > idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator>=(iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 >= idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator*() -> const T&
{
	return node->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-> () -> const T*
{
	return std::addressof(node->item());
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator[](int idx) -> const T&
{
	idx += owner->nodeIdx(node);
	NodeP n = owner->getIdx(owner->root->left(), idx);
	return n->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator++() -> const_iterator&
{
	node = owner->nextNode(node);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator--() -> const_iterator&
{
	node = owner->prevNode(node);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator++(int) -> const_iterator
{
	const_iterator temp = *this;
	++*this;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator--(int) -> const_iterator
{
	const_iterator temp = *this;
	--*this;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator+=(std::ptrdiff_t dst)
	-> const_iterator&
{
	int idx = owner->nodeIdx(node);
	idx += dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-=(std::ptrdiff_t dst)
	-> const_iterator&
{
	int idx = owner->nodeIdx(node);
	idx -= dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-(const_iterator other)
	-> std::ptrdiff_t
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 - idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator+(std::ptrdiff_t dst)
	-> const_iterator
{
	const_iterator temp = *this;
	temp += dst;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-(std::ptrdiff_t dst)
	-> const_iterator
{
	const_iterator temp = *this;
	temp -= dst;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator==(const_iterator other) -> bool
{
	assert(owner == other.owner);
	return node == other.node;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator!=(const_iterator other) -> bool
{
	assert(owner == other.owner);
	return node != other.node;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator<(const_iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 < idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator<=(const_iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 <= idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator>(const_iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 > idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator>=(const_iterator other) -> bool
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 >= idx2;
}

