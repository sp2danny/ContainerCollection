

#include <algorithm>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T, template<typename...> class Alloc = std::allocator>
class TreeVector
{
	union Node;
	typedef Node* NodeP;

public:
	// types
	struct iterator;
	struct const_iterator;
	typedef T              value_type;
	typedef T&             reference;
	typedef const T&       const_reference;
	typedef T*             pointer;
	typedef const T&       const_pointer;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;
	typedef Alloc<Node>    allocator_type;

	// standard lot
	TreeVector();
	TreeVector(const allocator_type&);
	TreeVector(const TreeVector&);
	TreeVector(TreeVector&&);
	TreeVector& operator=(const TreeVector&);
	TreeVector& operator=(TreeVector&&) noexcept;
	~TreeVector();
	void swap(TreeVector&) noexcept;
	void clear();

	struct iterator : std::iterator<std::random_access_iterator_tag, T>
	{
		iterator() = default;
		T& operator*() const;
		T* operator->();
		T& operator[](int);
		iterator& operator++();
		iterator& operator--();
		iterator operator++(int);
		iterator operator--(int);
		iterator& operator+=(std::ptrdiff_t);
		iterator& operator-=(std::ptrdiff_t);
		std::ptrdiff_t operator-(iterator);
		iterator operator+(std::ptrdiff_t);
		iterator operator-(std::ptrdiff_t);
		bool operator==(iterator);
		bool operator!=(iterator);
		bool operator<(iterator);
		bool operator<=(iterator);
		bool operator>(iterator);
		bool operator>=(iterator);

	private:
		iterator(NodeP n, TreeVector* o) : node(n), owner(o) {}
		NodeP       node;
		TreeVector* owner;
		friend class TreeVector;
		friend struct const_iterator;
	};

	struct const_iterator : std::iterator<std::random_access_iterator_tag, const T>
	{
		const_iterator() = default;
		const T& operator*();
		const T* operator->();
		const T& operator[](int);
		const_iterator& operator++();
		const_iterator& operator--();
		const_iterator operator++(int);
		const_iterator operator--(int);
		const_iterator& operator+=(std::ptrdiff_t);
		const_iterator& operator-=(std::ptrdiff_t);
		std::ptrdiff_t operator-(const_iterator);
		const_iterator operator+(std::ptrdiff_t);
		const_iterator operator-(std::ptrdiff_t);
		bool operator==(const_iterator);
		bool operator!=(const_iterator);
		bool operator<(const_iterator);
		bool operator<=(const_iterator);
		bool operator>(const_iterator);
		bool operator>=(const_iterator);

		const_iterator(typename TreeVector::iterator i) : node(i.node), owner(i.owner) {}

	private:
		const_iterator(NodeP n, const TreeVector* o) : node(n), owner(o) {}
		NodeP             node;
		const TreeVector* owner;
		friend class TreeVector;
	};

	// items
	void push_back(const T&);
	void push_back(T&&);
	void push_front(const T&);
	void push_front(T&&);

	void pop_back();
	void pop_front();

	iterator insert(iterator, const T&);
	iterator insert(iterator, T&&);

	template<typename... Args>
	iterator emplace(iterator, Args&&...);

	template<typename... Args>
	T& emplace_back(Args&&...);

	template<typename... Args>
	T& emplace_front(Args&&...);

	iterator erase(iterator);
	void     erase(iterator, iterator);

	// iterate
	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const { return begin(); }
	const_iterator cend() const { return end(); }

	typedef std::reverse_iterator<iterator> reverse_iterator;

	reverse_iterator rbegin() { return reverse_iterator{end()}; }
	reverse_iterator rend() { return reverse_iterator{begin()}; }

	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	const_reverse_iterator rbegin() const { return const_reverse_iterator{cend()}; }
	const_reverse_iterator rend() const { return const_reverse_iterator{cbegin()}; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
	const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }

	// capacity
	std::size_t size() const;
	std::size_t max_size() const;
	bool        empty() const;

	void reserve(std::size_t) {}
	void shrink_to_fit() {}

	// access
	T& back();
	T& front();
	T& operator[](std::size_t idx);
	T& at(std::size_t idx);

	const T& back() const;
	const T& front() const;
	const T& operator[](std::size_t idx) const;
	const T& at(std::size_t idx) const;

	std::size_t    index_of(iterator) const;
	std::size_t    index_of(const_iterator) const;
	iterator       nth(std::size_t);
	const_iterator nth(std::size_t) const;

	// operations
	iterator       upper_bound(const T&);
	iterator       lower_bound(const T&);
	const_iterator upper_bound(const T&) const;
	const_iterator lower_bound(const T&) const;
	iterator       unstable_insert_position(const T&); // slightly faster than upper_bound

	void sort() { sort(std::less<T>()); }
	template<typename Op>
	void sort(Op&&);

	void stable_sort() { stable_sort(std::less<T>()); }
	template<typename Op>
	void stable_sort(Op&&);

	void merge(TreeVector& other) { merge(other, std::less<T>()); }
	template<typename Op>
	void merge(TreeVector&, Op&&);
	void merge(TreeVector&& other) { merge(other, std::less<T>()); }
	template<typename Op>
	void merge(TreeVector&& other, Op&& op) { merge(other, op);	}

	bool is_sorted() const { return is_sorted(std::less<T>()); }
	template<typename Op>
	bool is_sorted(Op&&) const;

	void unique();
	void remove(const T&);
	template<typename Op>
	void remove_if(Op&&);

	void reverse();

	void resize(std::size_t, const T& = T{});

	void splice(iterator pos, TreeVector& other, iterator item);
	void splice(iterator pos, TreeVector& other);
	void splice(iterator pos, TreeVector&& other) { splice(pos, other); }
	void splice(iterator pos, TreeVector& other, iterator b, iterator e);

	int compare(const TreeVector&) const;

	TreeVector(std::initializer_list<T>);
	TreeVector(std::size_t, const T&);
	template<typename It, typename Cat = typename std::iterator_traits<It>::iterator_category>
	TreeVector(It, It);

	template<typename It, typename Cat = typename std::iterator_traits<It>::iterator_category>
	void assign(It, It);
	void assign(std::size_t, const T&);

	template<typename Cont>
	void assign(const Cont&);
	void assign(std::initializer_list<T>);

	template<typename OS>
	void print(OS& out) const
	{
		printTree(out, root->left(), nullptr, true);
	}

	std::string to_string() const;

private:
	auto prevNode(NodeP p) const -> NodeP;
	auto nextNode(NodeP p) const -> NodeP;

	auto head() const -> NodeP&;

	NodeP firstNode() const;
	NodeP lastNode() const;

	NodeP getIdx(NodeP, std::size_t) const;

	typedef std::uint32_t uint;

	struct NodeS
	{
		uint   weight, height;
		NodeP  parent, left, right;
		T      item;
		template<typename... Args>
		NodeS(Args&&... args) : item(std::forward<Args>(args)...) {}
	};

	struct SentryS
	{
		uint   weight, height;
		NodeP  parent, left, right;
	};

	struct sentry_tag { };
	struct node_tag { };

	union Node
	{
		NodeS   n;
		SentryS s;
		Node(sentry_tag);
		template<typename... Args>
		Node(node_tag, Args&&...);
		~Node();

		int    balance();
		bool   sentry();
		uint&  weight();
		uint&  height();
		NodeP& parent();
		NodeP& left();
		NodeP& right();
		T&     item();
	};

	std::size_t nodeIdx(NodeP) const;

	template<typename... Args>
	NodeP makeNode(Args&&...);

	NodeP makeSentry();

	void unmakeNode(NodeP);

	void fixHW(NodeP);
	void linkL(NodeP, NodeP);
	void linkR(NodeP, NodeP);
	bool isLeft(NodeP) const;
	void setEmpty();

	void helperUnlink(NodeP);

	typedef std::vector<NodeP> NVec;

	void recursiveFlatten(NodeP, NVec&);
	template<typename Op>
	void recursiveFlatten(NodeP, NVec&, NVec&, Op&&);

	template<typename RanIt>
	void construct(RanIt, RanIt, std::random_access_iterator_tag);

	template<typename FwdIt>
	void construct(FwdIt, FwdIt, std::forward_iterator_tag);

	template<typename It, typename Cat = typename std::iterator_traits<It>::iterator_category>
	void construct(It b, It e)
	{
		construct(b, e, Cat{});
	}

	void construct(std::size_t, const T&);

	template<typename It>
	void construct_p(It, It);

	struct Trunk;

	template<typename OS>
	void printTrunks(OS& out, Trunk* p) const;
	template<typename OS>
	void printTree(OS& out, NodeP n, Trunk* prev, bool is_left) const;

	struct RV;
	void toStr(NodeP, RV&) const;

	void treeInsert(NodeP, NodeP);
	void treeBalance(NodeP);
	auto treeRotateRightLeft(NodeP node) -> NodeP;
	auto treeRotateLeftRight(NodeP node) -> NodeP;
	auto treeRotateRight(NodeP node) -> NodeP;
	auto treeRotateLeft(NodeP node) -> NodeP;
	void treeDeleteNode(NodeP node);

	Alloc<Node> alloc;

	NodeP root;
	NodeP nil;

	friend struct iterator;
	friend struct const_iterator;

#ifndef NDEBUG
	bool integrity(NodeP) const;

public:
	bool integrity() const;
#else
public:
	bool integrity() const { return true; }
#endif
};

template<typename T, template<typename...> class A>
bool operator<(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) < 0;
}

template<typename T, template<typename...> class A>
bool operator<=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) <= 0;
}

template<typename T, template<typename...> class A>
bool operator>(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) > 0;
}

template<typename T, template<typename...> class A>
bool operator>=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) >= 0;
}

template<typename T, template<typename...> class A>
bool operator==(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) == 0;
}

template<typename T, template<typename...> class A>
bool operator!=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) != 0;
}

template<typename T, template<typename...> class A>
void swap(TreeVector<T, A>& a, TreeVector<T, A>& b)
{
	a.swap(b);
}

template<typename T, template<typename...> class A>
std::ostream& operator<<(std::ostream& out, const TreeVector<T, A>& tv)
{
	tv.print(out);
	return out;
}

template<typename T, template<typename...> class A>
std::string to_string(const TreeVector<T, A>& tv)
{
	return tv.to_string();
}

// ----------------------------------------------------------------------------

template<typename T, template<typename...> class A>
void TreeVector<T, A>::setEmpty()
{
	root->height() = root->weight() = 0;
	nil->height()  = nil->weight()  = 0;
	root->parent() = root->left() = root->right() = nil;
	nil->parent()  = nil->left()  = nil->right()  = nil;
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector()
{
	root = makeSentry();
	nil  = makeSentry();
	setEmpty();
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector(const allocator_type& a) : alloc(a)
{
	root = makeSentry();
	nil  = makeSentry();
	setEmpty();
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector(const TreeVector& other) : TreeVector()
{
	construct(other.begin(), other.end());
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector(TreeVector&& other)
{
	swap(other);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::operator=(const TreeVector& other) -> TreeVector&
{
	clear();
	construct(other.begin(), other.end());
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::operator=(TreeVector&& other) noexcept -> TreeVector&
{
	swap(other);
	return *this;
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::~TreeVector()
{
	clear();
	alloc.deallocate(root, 1);
	alloc.deallocate(nil, 1);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::swap(TreeVector& other) noexcept
{
	using std::swap;
	swap(alloc, other.alloc);
	swap(root, other.root);
	swap(nil, other.nil);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::clear()
{
	static void (*rec_clr)(NodeP, TreeVector*, NodeP) = [](NodeP node, TreeVector* me, NodeP nil) {
		if (node == nil)
			return;
		rec_clr(node->left(), me, nil);
		rec_clr(node->right(), me, nil);
		me->unmakeNode(node);
	};

	rec_clr(head(), this, nil);
	setEmpty();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::back() -> T&
{
	NodeP p = lastNode();
	assert(!p->sentry());
	return p->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::back() const -> const T&
{
	NodeP p = lastNode();
	assert(!p->sentry());
	return p->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::front() -> T&
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return p->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::front() const -> const T&
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return p->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::prevNode(NodeP n) const -> NodeP
{
	if (n == root)
	{
		while (n->right() != nil)
			n = n->right();
		return n;
	}

	NodeP nTemp;

	if (n->left() != nil)
	{
		n = n->left();
		while (n->right() != nil)
			n = n->right();
	} else
	{
		nTemp = n;
		n     = n->parent();
		while ((n != nil) && (n->left() == nTemp))
		{
			nTemp = n;
			n     = n->parent();
		}
	}
	return n;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::nextNode(NodeP n) const -> NodeP
{
	if (n == root)
	{
		while (n->left() != nil)
			n = n->left();
		return n;
	}

	Node* nTemp;

	if (n->right() != nil)
	{
		n = n->right();
		while (n->left() != nil)
			n = n->left();
	} else
	{
		nTemp = n;
		n     = n->parent();
		while ((n != nil) && (n->right() == nTemp))
		{
			nTemp = n;
			n     = n->parent();
		}
	}

	return n;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::head() const -> NodeP&
{
	return root->left();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::firstNode() const -> NodeP
{
	NodeP p = head();
	if (p == nil)
		return root;
	while (p->left() != nil)
		p = p->left();
	return p;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::lastNode() const -> NodeP
{
	NodeP p = head();
	if (p == nil)
		return root;
	while (p->right() != nil)
		p = p->right();
	return p;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::getIdx(NodeP p, std::size_t idx) const -> NodeP
{
	assert(!p->sentry());

	if (idx == p->weight()) return root;

	auto lw = p->left()->weight();

	if (lw == idx)
		return p;

	if (idx < lw)
		return getIdx(p->left(), idx);

	idx -= lw + 1;
	return getIdx(p->right(), idx);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::operator[](std::size_t idx) -> T&
{
	return getIdx(head(), idx)->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::operator[](std::size_t idx) const -> const T&
{
	return getIdx(head(), idx)->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::at(std::size_t idx) -> T&
{
	if (idx >= size())
		throw std::out_of_range("index");
	return getIdx(head(), idx)->item();
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::at(std::size_t idx) const -> const T&
{
	if (idx >= size())
		throw std::out_of_range("index");
	return getIdx(head(), idx)->item();
}

template<typename T, template<typename...> class A>
std::size_t TreeVector<T, A>::index_of(iterator i) const
{
	assert(i.owner == this);
	return nodeIdx(i.node);
}

template<typename T, template<typename...> class A>
std::size_t TreeVector<T, A>::index_of(const_iterator i) const
{
	assert(i.owner == this);
	return nodeIdx(i.node);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::nth(std::size_t idx) -> iterator
{
	auto sz = size();
	if (idx == sz)
		return end();
	if (idx > sz)
		throw std::out_of_range("index");
	Node* n = getIdx(head(), idx);
	return {n, this};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::nth(std::size_t idx) const -> const_iterator
{
	auto sz = size();
	if (idx == sz)
		return end();
	if (idx > sz)
		throw std::out_of_range("index");
	Node* n = getIdx(head(), idx);
	return {n, this};
}

// ----------------------------------------------------------------------------

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::iterator::operator*() const -> T&
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
auto TreeVector<T, A>::const_iterator::operator+=(std::ptrdiff_t dst) -> const_iterator&
{
	int idx = owner->nodeIdx(node);
	idx += dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-=(std::ptrdiff_t dst) -> const_iterator&
{
	int idx = owner->nodeIdx(node);
	idx -= dst;
	node = owner->getIdx(owner->root->left(), idx);
	return *this;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-(const_iterator other) -> std::ptrdiff_t
{
	assert(owner == other.owner);
	int idx1 = owner->nodeIdx(node);
	int idx2 = owner->nodeIdx(other.node);
	return idx1 - idx2;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator+(std::ptrdiff_t dst) -> const_iterator
{
	const_iterator temp = *this;
	temp += dst;
	return temp;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::const_iterator::operator-(std::ptrdiff_t dst) -> const_iterator
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

// ----------------------------------------------------------------------------

template<typename T, template<typename...> class A>
TreeVector<T, A>::Node::Node(sentry_tag)
	: s()
{
}

template<typename T, template<typename...> class A>
template<typename... Args>
TreeVector<T, A>::Node::Node(node_tag, Args&&... args) : n(std::forward<Args>(args)...)
{
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::Node::~Node()
{
	if (s.weight)
		n.item.~T();
}

template<typename T, template<typename...> class A>
uint& TreeVector<T, A>::Node::weight()
{
	return s.weight;
}

template<typename T, template<typename...> class A>
uint& TreeVector<T, A>::Node::height()
{
	return s.height;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::parent() -> NodeP&
{
	return s.parent;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::left() -> NodeP&
{
	return s.left;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::right() -> NodeP&
{
	return s.right;
}

template<typename T, template<typename...> class A>
T& TreeVector<T, A>::Node::item()
{
	assert(!sentry());
	return n.item;
}

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::Node::sentry()
{
	return !s.weight;
}

template<typename T, template<typename...> class A>
int TreeVector<T, A>::Node::balance()
{
	return right()->height() - left()->height();
}

// ----------------------------------------------------------------------------

template<typename T, template<typename...> class A>
struct TreeVector<T, A>::Trunk
{
	Trunk*      prev;
	const char* str;
};

template<typename T, template<typename...> class A>
template<typename OS>
void TreeVector<T, A>::printTrunks(OS& out, Trunk* p) const
{
	if (!p)
	{
		return;
	}
	printTrunks(out, p->prev);
	out << p->str;
}

template<typename T, template<typename...> class A>
template<typename OS>
void TreeVector<T, A>::printTree(OS& out, NodeP n, Trunk* prev, bool is_left) const
{
	if (n == nil)
		return;

	Trunk this_disp = {prev, "     "};
	auto  prev_str  = this_disp.str;
	printTree(out, n->right(), &this_disp, true);

	if (!prev)
	{
		this_disp.str = "---";
	} else if (is_left)
	{
		this_disp.str = ".--";
		prev_str      = "    |";
	} else
	{
		this_disp.str = "`--";
		prev->str     = prev_str;
	}

	printTrunks(out, &this_disp);
	out << " " << n->item();
	out << " (" << std::showpos << n->balance() << std::noshowpos << "," << n->weight() << "," << n->height() << ")\n";

	if (prev)
	{
		prev->str = prev_str;
	}
	this_disp.str = "    |";

	printTree(out, n->left(), &this_disp, false);
	if (!prev)
	{
		out << ("");
	}
}

template<typename T, template<typename...> class A>
struct TreeVector<T, A>::RV
{
	typedef std::vector<std::string> StrVec;
	StrVec                           sv;
	int                              trunk;
};

template<typename T, template<typename...> class A>
std::string TreeVector<T, A>::to_string() const
{
	RV rv;
	toStr(root->left(), rv);
	std::string str;
	for (auto&& x : rv.sv)
		str += x + "\n";
	return str;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::toStr(NodeP p, RV& rv) const
{
	if (p == nil)
	{
		rv.sv.clear();
		rv.sv.push_back("-()");
		rv.trunk = 0;
		return;
	}

	bool ln = p->left() == nil;
	bool rn = p->right() == nil;

	RV res, lft, rgt;
	if (!ln)
		toStr(p->left(), lft);
	if (!rn)
		toStr(p->right(), rgt);

	int lsz = lft.sv.size();
	int rsz = rgt.sv.size();

	res.sv.reserve(lsz + 3 + rsz);

	std::string tr = " |  ";
	std::string em = "    ";
	std::string lt = "  / ";
	std::string rt = "  \\ ";

	if (!ln)
	{
		for (int i = 0; i < lft.trunk; ++i)
		{
			res.sv.push_back(em + lft.sv[i]);
		}
		res.sv.push_back("   ." + lft.sv[lft.trunk]);
		if (lft.trunk + 1 < lsz)
			res.sv.push_back(lt + lft.sv[lft.trunk + 1]);
		else
			res.sv.push_back(lt);
		for (int i = lft.trunk + 2; i < lsz; ++i)
		{
			res.sv.push_back(tr + lft.sv[i]);
		}
		res.sv.push_back(tr);
	}

	std::stringstream me;
	me << ((ln && rn) ? "-- <" : "-+-- <");
	me << p->item() << ">";
	if (!(ln && rn))
		me << " w:" << p->weight() << " h:" << p->height() << " b:" << std::showpos << p->balance() << std::noshowpos;
	res.trunk = res.sv.size();
	res.sv.push_back(me.str());

	if (!rn)
	{
		res.sv.push_back(tr);
		for (int i = 0; i < rgt.trunk - 1; ++i)
		{
			res.sv.push_back(tr + rgt.sv[i]);
		}
		if (rgt.trunk - 1 >= 0)
			res.sv.push_back(rt + rgt.sv[rgt.trunk - 1]);
		else
			res.sv.push_back(rt);
		res.sv.push_back("   `" + rgt.sv[rgt.trunk]);
		for (int i = rgt.trunk + 1; i < rsz; ++i)
		{
			res.sv.push_back(em + rgt.sv[i]);
		}
	}

	rv = std::move(res);
}

// ----------------------------------------------------------------------------

template<typename T, template<typename...> class A>
std::size_t TreeVector<T, A>::nodeIdx(NodeP n) const
{
	if (n == root)
		return head()->weight();
	bool lft;
	int  idx = n->left()->weight();
	while (true)
	{
		lft = isLeft(n);
		n   = n->parent();
		if (n == root)
			break;
		if (!lft)
			idx += 1 + n->left()->weight();
	}
	return idx;
}

template<typename T, template<typename...> class A>
template<typename... Args>
auto TreeVector<T, A>::makeNode(Args&&... args) -> NodeP
{
	NodeP n = alloc.allocate(1);
	new (n) Node(node_tag{}, std::forward<Args>(args)...);
	return n;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::makeSentry() -> NodeP
{
	NodeP n = alloc.allocate(1);
	new (n) Node(sentry_tag{});
	return n;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::unmakeNode(NodeP p)
{
	p->~Node();
	alloc.deallocate(p, 1);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::linkL(NodeP p, NodeP l)
{
	p->left()   = l;
	l->parent() = p;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::linkR(NodeP p, NodeP r)
{
	p->right()  = r;
	r->parent() = p;
}

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::isLeft(NodeP n) const
{
	return n->parent()->left() == n;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::fixHW(NodeP p)
{
	p->height() = 1 + std::max(p->left()->height(), p->right()->height());
	p->weight() = 1 + p->left()->weight() + p->right()->weight();
}

template<typename T, template<typename...> class A>
template<typename It, typename>
void TreeVector<T, A>::assign(It b, It e)
{
	clear();
	construct(b, e);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::assign(std::size_t sz, const T& t)
{
	clear();
	construct(sz, t);
}

template<typename T, template<typename...> class A>
template<typename Cont>
void TreeVector<T, A>::assign(const Cont& cnt)
{
	clear();
	construct(cnt.begin(), cnt.end());
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::assign(std::initializer_list<T> il)
{
	clear();
	construct(il.begin(), il.end());
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector(std::initializer_list<T> il) : TreeVector()
{
	construct(il.begin(), il.end());
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::TreeVector(std::size_t n, const T& t) : TreeVector()
{
	construct(n, t);
}

template<typename T, template<typename...> class A>
template<typename It, typename>
TreeVector<T, A>::TreeVector(It b, It e) : TreeVector()
{
	construct(b, e);
}

template<typename T, template<typename...> class A>
template<typename It>
void TreeVector<T, A>::construct(It b, It e, std::random_access_iterator_tag)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, It, It, NodeP) = [](TreeVector* self, NodeP par, NodeP& me, It a,
	                                                                  It b, NodeP nil) {
		auto sz = b - a;
		if (sz == 0)
		{
			me = nil;
			return;
		}
		if (sz == 1)
		{
			me           = self->makeNode(*a);
			me->parent() = par;
			me->left() = me->right() = nil;
			me->weight() = me->height() = 1;
			return;
		}
		auto p       = sz / 2;
		me           = self->makeNode(a[p]);
		me->parent() = par;
		me->weight() = me->height() = -1;
		ra(self, me, me->left(), a, a + p, nil);
		ra(self, me, me->right(), a + p + 1, b, nil);
		self->fixHW(me);
	};

	ra(this, root, head(), b, e, nil);
}

template<typename T, template<typename...> class A>
template<typename It>
void TreeVector<T, A>::construct(It b, It e, std::forward_iterator_tag)
{
	while (b != e)
	{
		push_back(*b);
		++b;
	}
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::construct(std::size_t n, const T& t)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, std::size_t, const T&,
	                  NodeP) = [](TreeVector* self, NodeP par, NodeP& me, std::size_t sz, const T& t, NodeP nil) {
		if (sz == 0)
		{
			me = nil;
			return;
		}
		if (sz == 1)
		{
			me           = self->makeNode(t);
			me->parent() = par;
			me->left() = me->right() = nil;
			return;
		}
		auto p = sz / 2;
		me     = self->makeNode(t);
		ra(self, me, me->left(), p, t, nil);
		ra(self, me, me->right(), p - 1, t, nil);
		self->fixHW(me);
	};

	ra(this, root, head(), n, t, nil);
}

template<typename T, template<typename...> class A>
template<typename It>
void TreeVector<T, A>::construct_p(It b, It e)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, It, It, NodeP) =
		[](TreeVector* self, NodeP par, NodeP& me, It a, It b, NodeP nil)
	{
		auto sz = b - a;
		assert(sz >= 0);
		if (sz == 0)
		{
			me = nil;
			return;
		}
		if (sz == 1)
		{
			me           = *a;
			me->parent() = par;
			me->left() = me->right() = nil;
			me->weight() = me->height() = 1;
			return;
		}
		auto p       = sz / 2;
		me           = a[p];
		me->parent() = par;
		ra(self, me, me->left(), a, a + p, nil);
		ra(self, me, me->right(), a + p + 1, b, nil);
		self->fixHW(me);
	};

	ra(this, root, head(), b, e, nil);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::pop_back()
{
	NodeP p = lastNode();
	assert(!p->sentry());
	treeDeleteNode(p);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::pop_front()
{
	NodeP p = firstNode();
	assert(!p->sentry());
	treeDeleteNode(p);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::push_back(const T& t)
{
	insert(end(), t);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::push_back(T&& t)
{
	insert(end(), (T &&) t);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::push_front(const T& t)
{
	insert(begin(), t);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::push_front(T&& t)
{
	insert(begin(), (T &&) t);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::insert(iterator i, const T& t) -> iterator
{
	return emplace(i, t);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::insert(iterator i, T&& t) -> iterator
{
	return emplace(i, (T &&) t);
}

template<typename T, template<typename...> class A>
template<typename... Args>
auto TreeVector<T, A>::emplace(iterator i, Args&&... args) -> iterator
{
	NodeP p = makeNode(std::forward<Args>(args)...);

	treeInsert(i.node, p);

	return {p, this};
}

template<typename T, template<typename...> class A>
template<typename... Args>
T& TreeVector<T, A>::emplace_back(Args&&... args)
{
	return *emplace(end(), std::forward<Args>(args)...);
}

template<typename T, template<typename...> class A>
template<typename... Args>
T& TreeVector<T, A>::emplace_front(Args&&... args)
{
	return *emplace(begin(), std::forward<Args>(args)...);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::upper_bound(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		if (data < node->item())
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		}
	}
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::lower_bound(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		if (data < node->item())
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else if (node->item() < data)
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		} else
		{
			while (true)
			{
				NodeP prv = prevNode(node);
				if (prv == nil)
					break;
				if (prv == root)
					break;
				if (prv->item() < data)
					break;
				node = prv;
			}
			return {node, this};
		}
	}
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::upper_bound(const T& t) const -> const_iterator
{
	return {((TreeVector*)this)->upper_bound(t)};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::lower_bound(const T& t) const -> const_iterator
{
	return {((TreeVector*)this)->lower_bound(t)};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::unstable_insert_position(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		auto&& ni = node->item();
		if (data < ni)
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else if (ni < data)
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		} else
		{
			return {node, this};
		}
	}
}

template<typename T, template<typename...> class A>
template<typename Op>
bool TreeVector<T, A>::is_sorted(Op&& op) const
{
	if (size() <= 1)
		return true;
	auto curr = begin();
	auto next = curr;
	++next;
	while (next != end())
	{
		if (!op(*curr, *next))
			return false;
		curr = next;
		++next;
	}
	return true;
}

template<typename T, template<typename...> class A>
template<typename Op>
void TreeVector<T, A>::sort(Op&& op)
{
	NVec vec;
	vec.reserve(size());

	recursiveFlatten(head(), vec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool { return op(lhs->item(), rhs->item()); };

	std::sort(vec.begin(), vec.end(), NodeLess);

	construct_p(vec.begin(), vec.end());
}

template<typename T, template<typename...> class A>
template<typename Op>
void TreeVector<T, A>::stable_sort(Op&& op)
{
	NVec vec;
	vec.reserve(size());

	recursiveFlatten(head(), vec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool { return op(lhs->item(), rhs->item()); };

	std::stable_sort(vec.begin(), vec.end(), NodeLess);

	construct_p(vec.begin(), vec.end());
}

template<typename T, template<typename...> class A>
template<typename Op>
void TreeVector<T, A>::merge(TreeVector& other, Op&& op)
{
	if (this == &other)
		return;
	if (other.empty())
		return;
	if (empty())
	{
		swap(other);
		return;
	}

	NVec   tvec, ovec, mvec;
	size_t tsz = size();
	size_t osz = other.size();
	tvec.reserve(tsz);
	ovec.reserve(osz);
	mvec.reserve(tsz + osz);

	recursiveFlatten(head(), tvec);
	other.recursiveFlatten(other.head(), ovec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool { return op(lhs->item(), rhs->item()); };

	std::merge(tvec.begin(), tvec.end(), ovec.begin(), ovec.end(), std::back_inserter(mvec), NodeLess);

	construct_p(mvec.begin(), mvec.end());

	other.setEmpty();
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::reverse()
{

	static void (*rec_rev)(NodeP) = [](NodeP p) -> void {
		if (p->sentry())
			return;
		using std::swap;
		swap(p->left(), p->right());
		rec_rev(p->left());
		rec_rev(p->right());
	};

	rec_rev(head());
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::resize(std::size_t sz, const T& t)
{
	std::size_t curr = size();
	if (sz == curr)
		return;
	if (!sz)
	{
		clear();
		return;
	}

	NVec vec;
	vec.reserve(std::max(sz, curr));

	recursiveFlatten(head(), vec);

	if (sz < curr)
	{
		for (auto i = sz; i < curr; ++i)
		{
			NodeP p = vec[i];
			unmakeNode(p);
		}
		construct_p(vec.begin(), vec.begin() + sz);
	} else
	{
		for (auto i = curr; i < sz; ++i)
		{
			NodeP p = makeNode(t);
			vec.push_back(p);
		}
		construct_p(vec.begin(), vec.end());
	}
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other, iterator item)
{
	assert(pos.owner == this);
	assert(item.owner == &other);
	NodeP where = pos.node;
	NodeP node  = item.node;
	if (node != where)
	{
		other.helperUnlink(node);
		treeInsert(where, node);
	}
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other)
{
	assert(this != &other);

	NVec vec;
	vec.reserve(size() + other.size());

	iterator ii = begin();
	while (ii != pos)
	{
		vec.push_back(ii.node);
		++ii;
	}

	other.recursiveFlatten(other.head(), vec);

	while (ii != end())
	{
		vec.push_back(ii.node);
		++ii;
	}

	construct_p(vec.begin(), vec.end());
	other.setEmpty();
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other, iterator b, iterator e)
{
	assert(pos.owner == this);
	assert(b.owner == &other);
	assert(e.owner == &other);

	assert(this != &other);

	NVec tvec, ovec;

	auto ti = begin();
	auto te = end();
	auto oi = other.begin();
	auto oe = other.end();

	while (ti != pos)
	{
		tvec.push_back(ti.node);
		++ti;
	}
	while (oi != b)
	{
		ovec.push_back(oi.node);
		++oi;
	}
	while (oi != e)
	{
		tvec.push_back(oi.node);
		++oi;
	}
	while (ti != te)
	{
		tvec.push_back(ti.node);
		++ti;
	}
	while (oi != oe)
	{
		ovec.push_back(oi.node);
		++oi;
	}

	construct_p(tvec.begin(), tvec.end());
	other.construct_p(ovec.begin(), ovec.end());
}

template<typename T, template<typename...> class A>
int TreeVector<T, A>::compare(const TreeVector& other) const
{
	if (this == &other)
		return 0;

	auto lb = begin();
	auto rb = other.begin();
	auto le = end();
	auto re = other.end();

	while (true)
	{
		if ((lb == le) && (rb == re))
			return 0;
		if (lb == le)
			return -1;
		if (rb == re)
			return +1;
		if (*lb < *rb)
			return -1;
		if (*rb < *lb)
			return +1;
		++lb;
		++rb;
	}
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::begin() -> iterator
{
	NodeP p = firstNode();
	assert(p != nil);
	return iterator{p, this};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::end() -> iterator
{
	return iterator{root, this};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::begin() const -> const_iterator
{
	NodeP p = firstNode();
	assert(p != nil);
	return const_iterator{p, this};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::end() const -> const_iterator
{
	NodeP p = root;
	return const_iterator{p, this};
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::erase(iterator i) -> iterator
{
	auto r = i;
	++r;
	treeDeleteNode(i.node);
	return r;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::erase(iterator b, iterator e)
{
	while (b != e)
	{
		b = erase(b);
	}
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::unique()
{
	NVec keep, discard;
	keep.reserve(size());

	NodeP last = nullptr;

	auto want = [&](NodeP p) -> bool
	{
		if (last && (last->item() == p->item()))
		{
			return false;
		} else
		{
			last = p;
			return true;
		}
	};

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::remove(const T& t)
{
	NVec keep, discard;
	keep.reserve(size());

	auto want = [&](NodeP n) -> bool { return n->item() != t; };

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::recursiveFlatten(NodeP n, NVec& vec)
{
	if (n == nil)
		return;
	assert(!n->sentry());
	recursiveFlatten(n->left(), vec);
	vec.push_back(n);
	recursiveFlatten(n->right(), vec);
}

template<typename T, template<typename...> class A>
template<typename Op>
void TreeVector<T, A>::recursiveFlatten(NodeP n, NVec& keep, NVec& discard, Op&& want)
{
	if (n == nil)
		return;
	assert(!n->sentry());
	recursiveFlatten(n->left(), keep, discard, std::forward<Op>(want));
	if (want(n))
		keep.push_back(n);
	else
		discard.push_back(n);
	recursiveFlatten(n->right(), keep, discard, std::forward<Op>(want));
}

template<typename T, template<typename...> class A>
template<typename Op>
void TreeVector<T, A>::remove_if(Op&& op)
{
	NVec keep, discard;
	keep.reserve(size());

	auto want = [&](NodeP n) -> bool { return !op(n->item()); };

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template<typename T, template<typename...> class A>
std::size_t TreeVector<T, A>::size() const
{
	return head()->weight();
}

template<typename T, template<typename...> class A>
std::size_t TreeVector<T, A>::max_size() const
{
	return std::numeric_limits<int>::max();
}

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::empty() const
{
	return root->left() == nil;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::helperUnlink(NodeP node)
{
	auto relink = [&](NodeP n1, NodeP n2)
	{
		if (isLeft(n1))
			linkL(n1->parent(), n2);
		else
			linkR(n1->parent(), n2);
	};

	bool ln = node->left() == nil;
	bool rn = node->right() == nil;

	if (ln && rn)
	{
		relink(node, nil);
		treeBalance(node->parent());
	} else if (ln)
	{
		relink(node, node->right());
		treeBalance(node->parent());
	} else if (rn)
	{
		relink(node, node->left());
		treeBalance(node->parent());
	} else
	{
		NodeP succ = nextNode(node);

		assert(succ->left() == nil);

		if (succ->parent() == node)
		{
			relink(node, succ);
			linkL(succ, node->left());
			treeBalance(succ);
		} else
		{
			linkL(succ->parent(), succ->right());
			treeBalance(succ->parent());
			relink(node, succ);
			linkL(succ, node->left());
			linkR(succ, node->right());
		}
		fixHW(succ);
	}
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::treeDeleteNode(NodeP node)
{
	assert(!node->sentry());
	helperUnlink(node);
	unmakeNode(node);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::treeInsert(NodeP where, NodeP newnode)
{
	newnode->weight() = newnode->height() = 1;
	newnode->left() = newnode->right() = nil;

	if (where->left() == nil)
	{
		where->left()     = newnode;
		newnode->parent() = where;
		treeBalance(where);
		return;
	}
	where = where->left();
	while (where->right() != nil)
		where = where->right();

	where->right()    = newnode;
	newnode->parent() = where;
	treeBalance(where);
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::treeBalance(NodeP node)
{
	while (node != root)
	{
		fixHW(node);

		int balance = node->balance();

		assert((balance >= -2) && (balance <= +2));

		if (balance == -2)
		{
			int lb = node->left()->balance();
			if (lb == -1)
				treeRotateRight(node);
			else
				treeRotateLeftRight(node);
		} else if (balance == +2)
		{
			int rb = node->right()->balance();
			if (rb == +1)
				treeRotateLeft(node);
			else
				treeRotateRightLeft(node);
		}

		node = node->parent();
		if (node == nil)
			break;
	}
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::treeRotateLeft(NodeP node) -> NodeP
{
	NodeP right     = node->right();
	NodeP rightLeft = right->left();
	NodeP parent    = node->parent();

	if (isLeft(node))
		linkL(parent, right);
	else
		linkR(parent, right);

	linkL(right, node);
	linkR(node, rightLeft);

	fixHW(node);
	fixHW(right);

	return right;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::treeRotateRight(NodeP node) -> NodeP
{
	NodeP left      = node->left();
	NodeP leftRight = left->right();
	NodeP parent    = node->parent();

	if (isLeft(node))
		linkL(parent, left);
	else
		linkR(parent, left);

	linkR(left, node);
	linkL(node, leftRight);

	fixHW(node);
	fixHW(left);

	return left;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::treeRotateLeftRight(NodeP node) -> NodeP
{
	linkL(node, treeRotateLeft(node->left()));
	return treeRotateRight(node);
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::treeRotateRightLeft(NodeP node) -> NodeP
{
	linkR(node, treeRotateRight(node->right()));
	return treeRotateLeft(node);
}

#ifndef NDEBUG

#define LOG(y) std::cerr << y << std::endl
#define TEST(x) if (x) { LOG(#x); return false; }

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::integrity(NodeP node) const
{
	if (node == nil)
		return true;
	TEST (node == root)

	bool amleft  = node->parent()->left() == node;
	bool amright = node->parent()->right() == node;

	TEST (node->weight() < 1)
	TEST (node->height() < 1)
	TEST (amleft == amright)

	uint lh = node->left()->height();
	uint rh = node->right()->height();

	auto df = (lh>rh) ? (lh-rh) : (rh-lh);
	TEST (df > 1)

	uint lw = node->left()->weight();
	uint rw = node->right()->weight();

	uint h = std::max(lh, rh) + 1;
	uint w = lw + rw + 1;

	TEST (h != node->height())
	TEST (w != node->weight())

	bool lrn = (node->left() == nil) && (node->right() == nil);

	if ((w == 1) || (h == 1) || lrn)
	{
		TEST (w != 1)
		TEST (h != 1)
		TEST (!lrn)
	}

	if (node->left() != nil)
	{
		TEST (node->left()->parent() != node)
		TEST (!integrity(node->left()))
	}

	if (node->right() != nil)
	{
		TEST (node->right()->parent() != node)
		TEST (!integrity(node->right()))
	}

	return true;
}

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::integrity() const
{
	TEST (!nil)
	TEST (nil->height() != 0)
	TEST (nil->weight() != 0)
	TEST (nil->left() != nil)
	TEST (nil->right() != nil)
	TEST (!root)
	TEST (root->parent() != nil)
	TEST (root->right() != nil)
	TEST (root->height() != 0)
	TEST (root->weight() != 0)
	TEST (root->left() != head())
	TEST (!integrity(head()))

	size_t i = 0, n = size();
	auto iter = begin();
	while (iter != end())
	{
		TEST (iter != nth(i))
		TEST (index_of(iter) != i)
		++i;
		++iter;
	}
	TEST (i != n)

	return true;
}

#undef TEST
#undef LOG


#endif

