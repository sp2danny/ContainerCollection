
// #pragma once

// -------------------------------------------------------------------------------------------------------------

#include <utility>
#include <cstddef>
#include <iterator>
#include <functional>
#include <algorithm>
#include <initializer_list>
#include <limits>
#include <memory>
#include <cassert>

// -------------------------------------------------------------------------------------------------------------

template<typename It>
using iterator_category_t = typename std::iterator_traits<It>::iterator_category;

// -------------------------------------------------------------------------------------------------------------

template<typename T, typename A = std::allocator<T>>
class splice_list_allocator
{
	struct Node;
	typedef Node* NodeP;
	struct Node
	{
		NodeP prev, next;
		Node() = default;
		template<typename... Args>
		void make(Args&&... args) { new (ptr()) T( std::forward<Args>(args)... ); }
		void unmake()
		{
			T* p = ptr();
			p->~T();
		}
		alignas(T) unsigned char value[ sizeof(T) ];
		T* ptr() const { return (T*)&value; }
	};
	typedef typename A::template rebind<Node>::other Alloc;
	Alloc alloc;
public:
	typedef A allocator_type;
	typedef typename A::value_type value_type;
	typedef typename A::value_type& reference;
	typedef typename A::pointer pointer;
	typedef typename A::value_type const& const_reference;
	typedef typename A::const_pointer const_pointer;
	typedef typename A::size_type size_type;
	typedef typename A::difference_type difference_type;
	struct iterator;
	struct const_iterator;

	splice_list_allocator();
	splice_list_allocator(const splice_list_allocator&);
	splice_list_allocator(splice_list_allocator&&);
	splice_list_allocator& operator=(const splice_list_allocator&);
	splice_list_allocator& operator=(splice_list_allocator&&) noexcept;
	~splice_list_allocator();

	splice_list_allocator(std::initializer_list<T> il);
	splice_list_allocator& operator=(std::initializer_list<T> il);

	template<typename It, typename = iterator_category_t<It>>
	splice_list_allocator(It b, It e);

	splice_list_allocator(std::size_t n, const T& val);

	template<typename It, typename = iterator_category_t<It>>
	void assign(It b, It e);

	void assign(std::size_t n, const T& val);

	void assign(std::initializer_list<T> il);

	void assign(const splice_list_allocator& other);

	void assign(splice_list_allocator&& other);

	void swap(splice_list_allocator&) noexcept;
	void swap(splice_list_allocator&& other) noexcept;

	void clear();

	std::size_t size() const noexcept;
	bool empty() const noexcept;

	constexpr static std::size_t max_size() { return std::numeric_limits<std::size_t>::max(); }

	void push_back(const T&);
	void push_back(T&&);
	void push_front(const T&);
	void push_front(T&&);
	void pop_back();
	void pop_front();

	template<typename... Args>
	void emplace_back(Args&&... args);
	template<typename... Args>
	void emplace_front(Args&&... args);

	struct iterator : std::iterator<std::bidirectional_iterator_tag, T>
	{
		iterator() = default;
		iterator& operator++() { node = node->next; return *this; }
		iterator& operator--() { node = node->prev; return *this; }
		iterator operator++(int) { iterator tmp = *this; node = node->next; return tmp; }
		iterator operator--(int) { iterator tmp = *this; node = node->prev; return tmp; }
		T& operator*() { return *(node->ptr()); }
		T* operator->() { return node->ptr(); }
		bool operator==(iterator rhs) const { return node == rhs.node; }
		bool operator!=(iterator rhs) const { return node != rhs.node; }
		friend
			class splice_list_allocator;
		friend
			struct const_iterator;
	private:
		iterator(NodeP p) : node(p) {}
		NodeP node = nullptr;
	};
	struct const_iterator : std::iterator<std::bidirectional_iterator_tag, const T>
	{
		const_iterator() = default;
		const_iterator(typename splice_list_allocator::iterator other) : node(other.node) {}
		const_iterator& operator++() { node = node->next; return *this; }
		const_iterator& operator--() { node = node->prev; return *this; }
		const_iterator operator++(int) { const_iterator tmp = *this; node = node->next; return tmp; }
		const_iterator operator--(int) { const_iterator tmp = *this; node = node->prev; return tmp; }
		const T& operator*() { return *(node->ptr()); }
		const T* operator->() { return node->ptr(); }
		bool operator==(const_iterator rhs) const { return node == rhs.node; }
		bool operator!=(const_iterator rhs) const { return node != rhs.node; }
		friend
			class splice_list_allocator;
	private:
		const_iterator(NodeP p) : node(p) {}
		NodeP node = nullptr;
	};

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	iterator begin() { return {sentinel->next}; }
	iterator end() { return {sentinel}; }
	const_iterator begin() const { return {sentinel->next}; }
	const_iterator end() const { return {sentinel}; }
	const_iterator cbegin() const { return {sentinel->next}; }
	const_iterator cend() const { return {sentinel}; }
	reverse_iterator rbegin() { return reverse_iterator{end()}; }
	reverse_iterator rend() { return reverse_iterator{begin()}; }
	const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
	const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{end()}; }
	const_reverse_iterator crend() const { return const_reverse_iterator{begin()}; }

	iterator insert(iterator, const T&);
	iterator insert(iterator, T&&);
	template<typename... Args>
	iterator emplace(iterator, Args&&... args);
	iterator erase(iterator);

	void splice(iterator pos, splice_list_allocator& other);
	void splice(iterator pos, splice_list_allocator&& other);
	void splice(iterator pos, iterator it);
	void splice(iterator pos, iterator first, iterator last);

	void splice(iterator pos, splice_list_allocator&,  iterator it) { splice(pos,it); }
	void splice(iterator pos, splice_list_allocator&&, iterator it) { splice(pos,it); }
	void splice(iterator pos, splice_list_allocator&,  iterator first, iterator last) { splice(pos,first,last); }
	void splice(iterator pos, splice_list_allocator&&, iterator first, iterator last) { splice(pos,first,last); }

	// default sort is stable
	void sort() { sort( std::less<T>{} ); }
	template<typename Op>
	void sort(Op op);

	void merge(splice_list_allocator& other) { merge( other, std::less<T>{} ); }
	void merge(splice_list_allocator&& other) { merge( other ); }
	template<typename Op>
	void merge(splice_list_allocator& other, Op op);
	template<typename Op>
	void merge(splice_list_allocator&& other, Op op) { merge(other,op); }

	void unique() { unique( std::equal_to<T>{} ); }
	template<class Eq>
	void unique(Eq eq);

	void reverse();

	void remove(const T& val);
	template<typename Pred>
	void remove_if(Pred pred);

	template<typename Stream>
	friend Stream& operator<<(Stream& out, const splice_list_allocator& lst) { return helper_print(out, *lst.sentinel); }

private:

	template<typename Stream>
	static Stream& helper_print(Stream&, Node&);

	template<typename Op>
	static void helper_merge(Node&, Node&, Node&, Op);

	static void helper_split(Node&, Node&, Node&);

	template<typename Op>
	static void helper_sort(Node&, Op);

	template<typename It, typename = iterator_category_t<It>>
	void helper_assign(It b, It e);

	void helper_assign(std::size_t n, const T& val);

	template<typename... Args>
	NodeP helper_makenode(Args&&...);

	void helper_unmakenode(NodeP);

	NodeP sentinel;
	static void link(NodeP, NodeP);
};

// -------------------------------------------------------------------------------------------------------------

template<typename T,typename A>
splice_list_allocator<T, A>::splice_list_allocator(std::initializer_list<T> il)
	: splice_list_allocator(il.begin(), il.end())
{
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::operator=(std::initializer_list<T> il) -> splice_list_allocator&
{
	assign(il.begin(), il.end());
	return *this;
}

template<typename T,typename A>
void splice_list_allocator<T, A>::assign(std::initializer_list<T> il)
{
	assign(il.begin(), il.end());
}

template<typename T,typename A>
splice_list_allocator<T, A>::splice_list_allocator()
{
	sentinel = (NodeP) new Node;
	link(sentinel, sentinel);
}

template<typename T,typename A>
splice_list_allocator<T, A>::splice_list_allocator(const splice_list_allocator& other)
	: splice_list_allocator()
{
	helper_assign(other.begin(), other.end());
}

template<typename T,typename A>
splice_list_allocator<T, A>::splice_list_allocator(splice_list_allocator&& other)
	: splice_list_allocator()
{
	swap(other);
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::operator=(const splice_list_allocator& other) -> splice_list_allocator&
{
	assign(other);
	return *this;
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::operator=(splice_list_allocator&& other) noexcept -> splice_list_allocator&
{
	swap(other);
	return *this;
}

template<typename T,typename A>
splice_list_allocator<T, A>::~splice_list_allocator()
{
	clear();
	delete sentinel;
}

template<typename T,typename A>
template<typename It, typename>
splice_list_allocator<T, A>::splice_list_allocator(It b, It e)
	: splice_list_allocator()
{
	helper_assign(b, e);
}

template<typename T,typename A>
splice_list_allocator<T, A>::splice_list_allocator(std::size_t n, const T& val)
	: splice_list_allocator()
{
	helper_assign(n, val);
}

template<typename T,typename A>
template<typename It, typename>
void splice_list_allocator<T, A>::helper_assign(It b, It e)
{
	while (b != e)
	{
		push_back(*b);
		++b;
	}
}

template<typename T,typename A>
void splice_list_allocator<T, A>::helper_assign(std::size_t n, const T& val)
{
	while (n--)
		push_back(val);
}

template<typename T,typename A>
template<typename It, typename>
void splice_list_allocator<T, A>::assign(It b, It e)
{
	clear();
	helper_assign(b, e);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::assign(std::size_t n, const T& val)
{
	clear();
	helper_assign(n, val);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::assign(const splice_list_allocator& other)
{
	assign(other.begin(), other.end());
}

template<typename T,typename A>
void splice_list_allocator<T, A>::assign(splice_list_allocator&& other)
{
	swap(other);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::swap(splice_list_allocator& other) noexcept
{
	using std::swap;
	swap(sentinel, other.sentinel);
	swap(alloc, other.alloc);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::swap(splice_list_allocator&& other) noexcept
{
	swap(other);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::clear()
{
	while (!empty())
		pop_back();
}

/// <summary>
/// size is O(n) due to splice being O(1)
/// </summary>
template<typename T,typename A>
std::size_t splice_list_allocator<T, A>::size() const noexcept
{
	NodeP p = sentinel->next;
	std::size_t sz = 0;
	while (p != sentinel)
	{
		p = p->next;
		++sz;
	}
	return sz;
}

template<typename T,typename A>
bool splice_list_allocator<T, A>::empty() const noexcept
{
	return sentinel->next == sentinel;
}

template<typename T,typename A>
void splice_list_allocator<T, A>::helper_unmakenode(NodeP p)
{
	p->unmake();
	alloc.deallocate(p,1);
	//delete p;
}

template<typename T,typename A>
template<typename... Args>
auto splice_list_allocator<T, A>::helper_makenode(Args&&... args) -> NodeP
{
	NodeP p = alloc.allocate(1);
	p->make( std::forward<Args>(args)... );
	return p;
}

template<typename T,typename A>
void splice_list_allocator<T, A>::push_back(const T& t)
{
	NodeP p = helper_makenode(t);
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::push_back(T&& t)
{
	NodeP p = helper_makenode(std::move(t));
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::push_front(const T& t)
{
	NodeP p = helper_makenode(t);
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::push_front(T&& t)
{
	NodeP p = helper_makenode(std::move(t));
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T,typename A>
template<typename... Args>
void splice_list_allocator<T, A>::emplace_back(Args&&... args)
{
	NodeP p = helper_makenode(std::forward<Args>(args)... );
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T,typename A>
template<typename... Args>
void splice_list_allocator<T, A>::emplace_front(Args&&... args)
{
	NodeP p = helper_makenode(std::forward<Args>(args)...);
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::pop_back()
{
	NodeP p = sentinel->prev;
	link(p->prev, p->next);
	helper_unmakenode(p);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::pop_front()
{
	NodeP p = sentinel->next;
	link(p->prev, p->next);
	helper_unmakenode(p);
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::insert(iterator where, const T& item) -> iterator
{
	NodeP p = helper_makenode(item);
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::insert(iterator where, T&& item) -> iterator
{
	NodeP p = helper_makenode(std::move(item));
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T,typename A>
template<typename... Args>
auto splice_list_allocator<T, A>::emplace(iterator where, Args&&... args) -> iterator
{
	NodeP p = helper_makenode(std::forward<Args>(args)...);
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T,typename A>
auto splice_list_allocator<T, A>::erase(iterator what) -> iterator
{
	NodeP p = what.node;
	NodeP n = p->next;
	link(p->prev, p->next);
	helper_unmakenode(p);
	return {n};
}

template<typename T,typename A>
void splice_list_allocator<T, A>::link(NodeP n1, NodeP n2)
{
	n1->next = n2;
	n2->prev = n1;
}

template<typename T,typename A>
void splice_list_allocator<T, A>::splice(iterator pos, splice_list_allocator& other)
{
	if (other.empty()) return;
	link(pos.node->prev, other.sentinel->next);
	link(other.sentinel->prev, pos.node);
	link(other.sentinel, other.sentinel);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::splice(iterator pos, splice_list_allocator&& other)
{
	splice(pos, other);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::splice(iterator pos, iterator it)
{
	link(it.node->prev, it.node->next);
	link(pos.node->prev, it.node);
	link(it.node, pos.node);
}

template<typename T,typename A>
void splice_list_allocator<T, A>::splice(iterator pos, iterator first, iterator last)
{
	if (first == last) return;
	NodeP f = first.node;
	NodeP l = last.node->prev;
	link(f->prev, l->next);
	link(pos.node->prev, f);
	link(l, pos.node);
}

template<typename T,typename A>
template<typename Op>
void splice_list_allocator<T, A>::sort(Op op)
{
	helper_sort(*sentinel, op);
}

template<typename T,typename A>
template<typename Op>
void splice_list_allocator<T, A>::merge(splice_list_allocator& other, Op op)
{
	if (other.empty()) return;
	if (empty()) return swap(other);
	helper_merge(*sentinel, *sentinel, *other.sentinel, op);
	link(other.sentinel, other.sentinel);
}

template<typename T,typename A>
template<typename Stream>
auto splice_list_allocator<T, A>::helper_print(Stream& out, Node& s) -> Stream&
{
	out << "[";
	NodeP p = s.next;
	bool first = true;
	while (p != (NodeP)&s)
	{
		if(!first) out << ", ";
		out << *p->ptr();
		first = false;
		p = p->next;
	}
	out << "]";
	return out;
}

template<typename T,typename A>
template<typename Op>
void splice_list_allocator<T, A>::helper_merge(Node& dst, Node& s1, Node& s2, Op op)
{
	NodeP f1 = s1.next;
	NodeP l1 = s1.prev;
	NodeP f2 = s2.next;
	NodeP l2 = s2.prev;
	NodeP dp = (NodeP)&dst;

	while (true)
	{
		if (f1 == (NodeP)&s1)
		{
			link(dp, f2);
			link(l2, (NodeP)&dst);
			break;
		}
		if (f2 == (NodeP)&s2)
		{
			link(dp, f1);
			link(l1, (NodeP)&dst);
			break;
		}
		if (op(*f1->ptr(), *f2->ptr()))
		{
			link(dp, f1);
			f1 = f1->next;
		}
		else
		{
			link(dp, f2);
			f2 = f2->next;
		}
		dp = dp->next;
	}
}

template<typename T,typename A>
void splice_list_allocator<T, A>::helper_split(Node& src, Node& d1, Node& d2)
{
	if (src.next == (NodeP)&src)
	{
		d1.next = d1.prev = (NodeP)&d1;
		d2.next = d2.prev = (NodeP)&d2;
		return;
	}
	if (src.next->next == (NodeP)&src)
	{
		link((NodeP)&d1, src.next);
		link(src.next, (NodeP)&d1);
		d2.next = d2.prev = (NodeP)&d2;
		return;
	}
	if (src.next->next->next == (NodeP)&src)
	{
		link((NodeP)&d1, src.next);
		link(src.next, (NodeP)&d1);
		link((NodeP)&d2, src.prev);
		link(src.prev, (NodeP)&d2);
		return;
	}

	NodeP f = src.next;
	NodeP l = src.prev;

	while (true)
	{
		if (f == l) break;
		l = l->prev;
		if (f == l) break;
		f = f->next;
	}
	NodeP m = f;
	NodeP m2 = f->next;
	f = src.next;
	l = src.prev;
	link((NodeP)&d1, f);
	link(m, (NodeP)&d1);
	link((NodeP)&d2, m2);
	link(l, (NodeP)&d2);
}

template<typename T,typename A>
template<typename Op>
void splice_list_allocator<T, A>::helper_sort(Node& src, Op op)
{
	if (src.next == (NodeP)&src) return;
	if (src.next->next == (NodeP)&src) return;

	Node lft, rgt;
	helper_split(src, lft, rgt);
	helper_sort(lft, op);
	helper_sort(rgt, op);
	helper_merge(src, lft, rgt, op);
}

template<typename T,typename A>
template<class Eq>
void splice_list_allocator<T, A>::unique(Eq eq)
{
	iterator i = begin();
	if (i == end()) return;
	iterator curr = i;
	++i;
	while (true)
	{
		if (i == end()) break;
		if (eq(*i, *curr))
		{
			i = erase(i);
		} else {
			curr = i;
			++i;
		}
	}
}

template<typename T,typename A>
void splice_list_allocator<T, A>::reverse()
{
	if (empty()) return;
	NodeP p1 = sentinel;
	NodeP p2 = p1->next;
	while (true)
	{
		NodeP p3 = p2->next;
		link(p2, p1);
		if (p2 == sentinel) break;
		p1 = p2;
		p2 = p3;
	}
}

template<typename T,typename A>
void splice_list_allocator<T, A>::remove(const T& val)
{
	using namespace std::placeholders;
	remove_if( std::bind( std::equal_to<T>(), _1, val ) );
}

template<typename T,typename A>
template<typename Pred>
void splice_list_allocator<T, A>::remove_if(Pred pred)
{
	auto i = begin();
	while (i != end())
	{
		if (pred(*i))
			i = erase(i);
		else
			++i;
	}
}

// -------------------------------------------------------------------------------------------------------------

template class splice_list_allocator<int>;

// -------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <string>

using namespace std;

struct Reporter
{
	Reporter(std::string name)
		: name(name)
	{
		cout << name << " Default constructor\n";
		state = ok;
		++defc;
	}
	Reporter(const Reporter& r)
		: name(r.name)
	{
		assert(r.state==ok);
		cout << name << " Copy constructor\n";
		state = ok;
		++copc;
	}
	Reporter(Reporter&& r) noexcept
		: name(r.name)
	{
		assert(r.state==ok);
		cout << name << " Move constructor\n";
		state = ok;
		r.state = moved;
		++movc;
	}
	Reporter& operator=(const Reporter& r)
	{
		assert(state==ok || state==moved);
		assert(r.state==ok);
		cout << name << " Assignment operator\n";
		state = ok;
		++copa;
		return *this;
	}
	Reporter& operator=(Reporter&& r) noexcept
	{
		assert(state==ok || state==moved);
		assert(r.state==ok);
		cout << name << " Move Assignment operator\n";
		state = ok;
		r.state = moved;
		++mova;
		return *this;
	}
	~Reporter()
	{
		assert(state==ok || state==moved);
		cout << name << " Destructor\n";
		state = destroyed;
		++dest;
	}
	bool isok() const { return state==ok; }
	bool issemi() const { return state==ok || state==moved; }
	void swap(Reporter& r) noexcept
	{
		using std::swap;
		swap(state, r.state);
		swap(name, r.name);
	}
	static void Report()
	{
		cout << "total default : " << defc << endl;
		cout << "total copy    : " << copc << endl;
		cout << "total move    : " << movc << endl;
		cout << "total assign  : " << copa << endl;
		cout << "total mv-ass  : " << mova << endl;
		cout << "total alloc   : " << (defc+copc+movc) << endl;
		cout << "total destroy : " << dest << endl;
	}
private:
	enum { zero=0, pre, ok, moved, destroyed } state = pre;
	std::string name;

	/*inline*/ static long long defc, copc, movc, dest, copa, mova;
};

long long Reporter::defc = 0;
long long Reporter::copc = 0;
long long Reporter::movc = 0;
long long Reporter::dest = 0;
long long Reporter::copa = 0;
long long Reporter::mova = 0;

struct MyInt
{
	MyInt(int i=0) : i(i) {}
	operator int() { return i; }
	MyInt& operator=(int j) { i=j; return *this; }
	bool operator == (const MyInt& other) const { return i == other.i; }
	bool operator  < (const MyInt& other) const { return i  < other.i; }
	~MyInt()
	{
		i = 0;
	}
private:
	int i;
	Reporter rep = "MyInt"s;
};


int main()
{
	{
		int arr[] = { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6 };

		splice_list_allocator<MyInt> sli_1( arr, arr+(sizeof(arr)/sizeof(*arr)) );
		sli_1.sort();

		splice_list_allocator<MyInt> sli_2( 7, 2 );

		sli_1.merge(sli_2);

		sli_1.remove_if( [](int i)->bool{return i%2;} );

		sli_1.unique();
		sli_1.reverse();

		cout << sli_1 << endl;
	}

	Reporter::Report();

	cout << endl;
	cout << "\n\nDone.\n";
}

