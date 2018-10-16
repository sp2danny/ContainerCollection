
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

// -------------------------------------------------------------------------------------------------------------

template<typename It>
using iterator_category_t = typename std::iterator_traits<It>::iterator_category;

// -------------------------------------------------------------------------------------------------------------

template<typename T>
class splice_list
{
	union Sentry_or_Node {
		struct Node;
		typedef Node* NodeP;
		struct Sentry
		{
			NodeP prev, next;
		};
		struct Node
		{
			NodeP prev, next;
			T     value;
			template<typename... Args>
			Node(Args&&... args) : value(std::forward<Args>(args)...)
			{
			}
		};
	};
	typedef typename Sentry_or_Node::Node   Node;
	typedef typename Sentry_or_Node::NodeP  NodeP;
	typedef typename Sentry_or_Node::Sentry Sentry;

public:
	typedef T              value_type;
	typedef T&             reference;
	typedef T*             pointer;
	typedef const T&       const_reference;
	typedef const T*       const_pointer;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;
	struct iterator;
	struct const_iterator;

	splice_list();
	splice_list(const splice_list&);
	splice_list(splice_list&&);
	splice_list& operator=(const splice_list&);
	splice_list& operator=(splice_list&&) noexcept;
	~splice_list();

	splice_list(std::initializer_list<T> il);
	splice_list& operator=(std::initializer_list<T> il);

	template<typename It, typename = iterator_category_t<It>>
	splice_list(It b, It e);

	splice_list(std::size_t n, const T& val);

	template<typename It, typename = iterator_category_t<It>>
	void assign(It b, It e);

	void assign(std::size_t n, const T& val);

	void assign(std::initializer_list<T> il);

	void assign(const splice_list& other);

	void assign(splice_list&& other);

	void swap(splice_list&) noexcept;
	void swap(splice_list&& other) noexcept;

	void clear();

	std::size_t size() const noexcept;
	bool        empty() const noexcept;

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
		iterator& operator++()
		{
			node = node->next;
			return *this;
		}
		iterator& operator--()
		{
			node = node->prev;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			node         = node->next;
			return tmp;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			node         = node->prev;
			return tmp;
		}
		T&   operator*() const { return node->value; }
		T*   operator->() const { return std::addressof(node->value); }
		bool operator==(iterator rhs) const { return node == rhs.node; }
		bool operator!=(iterator rhs) const { return node != rhs.node; }
		friend class splice_list;
		friend struct const_iterator;

	private:
		iterator(NodeP p) : node(p) {}
		NodeP node = nullptr;
	};
	struct const_iterator : std::iterator<std::bidirectional_iterator_tag, const T>
	{
		const_iterator() = default;
		const_iterator(typename splice_list::iterator other) : node(other.node) {}
		const_iterator& operator++()
		{
			node = node->next;
			return *this;
		}
		const_iterator& operator--()
		{
			node = node->prev;
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			node               = node->next;
			return tmp;
		}
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			node               = node->prev;
			return tmp;
		}
		const T& operator*() { return node->value; }
		const T* operator->() { return std::addressof(node->value); }
		bool     operator==(const_iterator rhs) const { return node == rhs.node; }
		bool     operator!=(const_iterator rhs) const { return node != rhs.node; }
		friend class splice_list;

	private:
		const_iterator(NodeP p) : node(p) {}
		NodeP node = nullptr;
	};

	typedef std::reverse_iterator<iterator>       reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	iterator               begin() { return {sentinel->next}; }
	iterator               end() { return {sentinel}; }
	const_iterator         begin() const { return {sentinel->next}; }
	const_iterator         end() const { return {sentinel}; }
	const_iterator         cbegin() const { return {sentinel->next}; }
	const_iterator         cend() const { return {sentinel}; }
	reverse_iterator       rbegin() { return reverse_iterator{end()}; }
	reverse_iterator       rend() { return reverse_iterator{begin()}; }
	const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
	const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{end()}; }
	const_reverse_iterator crend() const { return const_reverse_iterator{begin()}; }

	iterator insert(iterator, const T&);
	iterator insert(iterator, T&&);
	template<typename... Args>
	iterator emplace(iterator, Args&&... args);
	iterator erase(iterator);
	iterator erase(iterator, iterator);

	void splice(iterator pos, splice_list& other);
	void splice(iterator pos, splice_list&& other);
	void splice(iterator pos, iterator it);
	void splice(iterator pos, iterator first, iterator last);

	void splice(iterator pos, splice_list&, iterator it) { splice(pos, it); }
	void splice(iterator pos, splice_list&&, iterator it) { splice(pos, it); }
	void splice(iterator pos, splice_list&, iterator first, iterator last) { splice(pos, first, last); }
	void splice(iterator pos, splice_list&&, iterator first, iterator last) { splice(pos, first, last); }

	// default sort is stable
	void sort() { sort(std::less<T>{}); }
	template<typename Op>
	void sort(Op op);

	void merge(splice_list& other) { merge(other, std::less<T>{}); }
	void merge(splice_list&& other) { merge(other); }
	template<typename Op>
	void merge(splice_list& other, Op op);
	template<typename Op>
	void merge(splice_list&& other, Op op)
	{
		merge(other, op);
	}

	void unique() { unique(std::equal_to<T>{}); }
	template<class Eq>
	void unique(Eq eq);

	void reverse();

	void remove(const T& val);
	template<typename Pred>
	Pred remove_if(Pred pred);

	template<typename Stream>
	friend Stream& operator<<(Stream& out, const splice_list& lst)
	{
		return helper_print(out, *(Sentry*)lst.sentinel);
	}

	int compare(const splice_list& other) const;

	bool operator==(const splice_list& other) const;
	bool operator!=(const splice_list& other) const;
	bool operator<(const splice_list& other) const;
	bool operator<=(const splice_list& other) const;
	bool operator>(const splice_list& other) const;
	bool operator>=(const splice_list& other) const;

private:
	template<typename Stream>
	static Stream& helper_print(Stream&, Sentry&);

	template<typename Op>
	static void helper_merge(Sentry&, Sentry&, Sentry&, Op);

	static void helper_split(Sentry&, Sentry&, Sentry&);

	template<typename Op>
	static void helper_sort(Sentry&, Op);

	template<typename It, typename = iterator_category_t<It>>
	void helper_assign(It b, It e);

	void helper_assign(std::size_t n, const T& val);

	template<typename... Args>
	NodeP helper_makenode(Args&&...);

	NodeP       sentinel;
	static void link(NodeP, NodeP);
};

// -------------------------------------------------------------------------------------------------------------

template<typename T>
splice_list<T>::splice_list(std::initializer_list<T> il) : splice_list(il.begin(), il.end())
{
}

template<typename T>
auto splice_list<T>::operator=(std::initializer_list<T> il) -> splice_list&
{
	assign(il.begin(), il.end());
	return *this;
}

template<typename T>
void splice_list<T>::assign(std::initializer_list<T> il)
{
	assign(il.begin(), il.end());
}

template<typename T>
splice_list<T>::splice_list()
{
	sentinel = (NodeP) new Sentry;
	link(sentinel, sentinel);
}

template<typename T>
splice_list<T>::splice_list(const splice_list& other) : splice_list()
{
	helper_assign(other.begin(), other.end());
}

template<typename T>
splice_list<T>::splice_list(splice_list&& other) : splice_list()
{
	swap(other);
}

template<typename T>
auto splice_list<T>::operator=(const splice_list& other) -> splice_list&
{
	assign(other);
	return *this;
}

template<typename T>
auto splice_list<T>::operator=(splice_list&& other) noexcept -> splice_list&
{
	swap(other);
	return *this;
}

template<typename T>
splice_list<T>::~splice_list()
{
	clear();
	delete (Sentry*)sentinel;
}

template<typename T>
template<typename It, typename>
splice_list<T>::splice_list(It b, It e) : splice_list()
{
	helper_assign(b, e);
}

template<typename T>
splice_list<T>::splice_list(std::size_t n, const T& val) : splice_list()
{
	helper_assign(n, val);
}

template<typename T>
template<typename It, typename>
void splice_list<T>::helper_assign(It b, It e)
{
	while (b != e)
	{
		push_back(*b);
		++b;
	}
}

template<typename T>
void splice_list<T>::helper_assign(std::size_t n, const T& val)
{
	while (n--)
		push_back(val);
}

template<typename T>
template<typename It, typename>
void splice_list<T>::assign(It b, It e)
{
	clear();
	helper_assign(b, e);
}

template<typename T>
void splice_list<T>::assign(std::size_t n, const T& val)
{
	clear();
	helper_assign(n, val);
}

template<typename T>
void splice_list<T>::assign(const splice_list& other)
{
	assign(other.begin(), other.end());
}

template<typename T>
void splice_list<T>::assign(splice_list&& other)
{
	swap(other);
}

template<typename T>
void splice_list<T>::swap(splice_list& other) noexcept
{
	using std::swap;
	swap(sentinel, other.sentinel);
}

template<typename T>
void splice_list<T>::swap(splice_list&& other) noexcept
{
	swap(other);
}

template<typename T>
void splice_list<T>::clear()
{
	while (!empty())
		pop_back();
}

/// <summary>
/// size is O(n) due to splice being O(1)
/// </summary>
template<typename T>
std::size_t splice_list<T>::size() const noexcept
{
	NodeP       p  = sentinel->next;
	std::size_t sz = 0;
	while (p != sentinel)
	{
		p = p->next;
		++sz;
	}
	return sz;
}

template<typename T>
bool splice_list<T>::empty() const noexcept
{
	return sentinel->next == sentinel;
}

template<typename T>
template<typename... Args>
auto splice_list<T>::helper_makenode(Args&&... args) -> NodeP
{
	NodeP p = new Node(std::forward<Args>(args)...);
	return p;
}

template<typename T>
void splice_list<T>::push_back(const T& t)
{
	NodeP p = helper_makenode(t);
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T>
void splice_list<T>::push_back(T&& t)
{
	NodeP p = helper_makenode(std::move(t));
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T>
void splice_list<T>::push_front(const T& t)
{
	NodeP p = helper_makenode(t);
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T>
void splice_list<T>::push_front(T&& t)
{
	NodeP p = helper_makenode(std::move(t));
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T>
template<typename... Args>
void splice_list<T>::emplace_back(Args&&... args)
{
	NodeP p = helper_makenode(std::forward<Args>(args)...);
	link(sentinel->prev, p);
	link(p, sentinel);
}

template<typename T>
template<typename... Args>
void splice_list<T>::emplace_front(Args&&... args)
{
	NodeP p = helper_makenode(std::forward<Args>(args)...);
	link(p, sentinel->next);
	link(sentinel, p);
}

template<typename T>
void splice_list<T>::pop_back()
{
	NodeP p = sentinel->prev;
	link(p->prev, p->next);
	delete p;
}

template<typename T>
void splice_list<T>::pop_front()
{
	NodeP p = sentinel->next;
	link(p->prev, p->next);
	delete p;
}

template<typename T>
auto splice_list<T>::insert(iterator where, const T& item) -> iterator
{
	NodeP p = helper_makenode(item);
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T>
auto splice_list<T>::insert(iterator where, T&& item) -> iterator
{
	NodeP p = helper_makenode(std::move(item));
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T>
template<typename... Args>
auto splice_list<T>::emplace(iterator where, Args&&... args) -> iterator
{
	NodeP p = helper_makenode(std::forward<Args>(args)...);
	link(where.node->prev, p);
	link(p, where.node);
	return {p};
}

template<typename T>
auto splice_list<T>::erase(iterator what) -> iterator
{
	NodeP p = what.node;
	NodeP n = p->next;
	link(p->prev, p->next);
	delete p;
	return {n};
}

template<typename T>
auto splice_list<T>::erase(iterator b, iterator e) -> iterator
{
	while (b != e)
		b = erase(b);
	return b;
}

template<typename T>
void splice_list<T>::link(NodeP n1, NodeP n2)
{
	n1->next = n2;
	n2->prev = n1;
}

template<typename T>
void splice_list<T>::splice(iterator pos, splice_list& other)
{
	if (other.empty())
		return;
	link(pos.node->prev, other.sentinel->next);
	link(other.sentinel->prev, pos.node);
	link(other.sentinel, other.sentinel);
}

template<typename T>
void splice_list<T>::splice(iterator pos, splice_list&& other)
{
	splice(pos, other);
}

template<typename T>
void splice_list<T>::splice(iterator pos, iterator it)
{
	link(it.node->prev, it.node->next);
	link(pos.node->prev, it.node);
	link(it.node, pos.node);
}

template<typename T>
void splice_list<T>::splice(iterator pos, iterator first, iterator last)
{
	if (first == last)
		return;
	NodeP f = first.node;
	NodeP l = last.node->prev;
	link(f->prev, l->next);
	link(pos.node->prev, f);
	link(l, pos.node);
}

template<typename T>
template<typename Op>
void splice_list<T>::sort(Op op)
{
	helper_sort(*(Sentry*)sentinel, op);
}

template<typename T>
template<typename Op>
void splice_list<T>::merge(splice_list& other, Op op)
{
	if (other.empty())
		return;
	if (empty())
		return swap(other);
	helper_merge(*(Sentry*)sentinel, *(Sentry*)sentinel, *(Sentry*)other.sentinel, op);
	link(other.sentinel, other.sentinel);
}

template<typename T>
template<typename Stream>
auto splice_list<T>::helper_print(Stream& out, Sentry& s) -> Stream&
{
	out << "[";
	NodeP p     = s.next;
	bool  first = true;
	while (p != (NodeP)&s)
	{
		if (!first)
			out << ", ";
		out << p->value;
		first = false;
		p     = p->next;
	}
	out << "]";
	return out;
}

template<typename T>
template<typename Op>
void splice_list<T>::helper_merge(Sentry& dst, Sentry& s1, Sentry& s2, Op op)
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
		if (op(f1->value, f2->value))
		{
			link(dp, f1);
			f1 = f1->next;
		} else
		{
			link(dp, f2);
			f2 = f2->next;
		}
		dp = dp->next;
	}
}

template<typename T>
void splice_list<T>::helper_split(Sentry& src, Sentry& d1, Sentry& d2)
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
		if (f == l)
			break;
		l = l->prev;
		if (f == l)
			break;
		f = f->next;
	}
	NodeP m  = f;
	NodeP m2 = f->next;
	f        = src.next;
	l        = src.prev;
	link((NodeP)&d1, f);
	link(m, (NodeP)&d1);
	link((NodeP)&d2, m2);
	link(l, (NodeP)&d2);
}

template<typename T>
template<typename Op>
void splice_list<T>::helper_sort(Sentry& src, Op op)
{
	if (src.next == (NodeP)&src)
		return;
	if (src.next->next == (NodeP)&src)
		return;

	Sentry lft, rgt;
	helper_split(src, lft, rgt);
	helper_sort(lft, op);
	helper_sort(rgt, op);
	helper_merge(src, lft, rgt, op);
}

template<typename T>
template<class Eq>
void splice_list<T>::unique(Eq eq)
{
	iterator i = begin();
	if (i == end())
		return;
	iterator curr = i;
	++i;
	while (true)
	{
		if (i == end())
			break;
		if (eq(*i, *curr))
		{
			i = erase(i);
		} else
		{
			curr = i;
			++i;
		}
	}
}

template<typename T>
void splice_list<T>::reverse()
{
	if (empty())
		return;
	NodeP p1 = sentinel;
	NodeP p2 = p1->next;
	while (true)
	{
		NodeP p3 = p2->next;
		link(p2, p1);
		if (p2 == sentinel)
			break;
		p1 = p2;
		p2 = p3;
	}
}

template<typename T>
void splice_list<T>::remove(const T& val)
{
	// using namespace std::placeholders;
	// remove_if( std::bind( std::equal_to<T>(), _1, val ) );
	remove_if([&val](const T& v) -> bool { return v == val; });
}

template<typename T>
template<typename Pred>
Pred splice_list<T>::remove_if(Pred pred)
{
	auto i = begin();
	while (i != end())
	{
		if (pred(*i))
			i = erase(i);
		else
			++i;
	}
	return pred;
}

template<typename T>
int splice_list<T>::compare(const splice_list& other) const
{
	auto me_iter = begin();
	auto ot_iter = other.begin();
	while (true)
	{
		bool me_ate = (me_iter == end());
		bool ot_ate = (ot_iter == other.end());
		if (me_ate && ot_ate)
			return 0;
		if (me_ate) return -1;
		if (ot_ate) return +1;
		if ( (*me_iter) < (*ot_iter) ) return -1;
		if ( (*ot_iter) < (*me_iter) ) return +1;
		++me_iter; ++ot_iter;
	}
}

template<typename T>
bool splice_list<T>::operator == (const splice_list& other) const
{ return compare(other)       == 0; }

template<typename T>
bool splice_list<T>::operator != (const splice_list& other) const
{ return compare(other)       != 0; }

template<typename T>
bool splice_list<T>::operator  < (const splice_list& other) const
{ return compare(other)        < 0; }

template<typename T>
bool splice_list<T>::operator <= (const splice_list& other) const
{ return compare(other)       <= 0; }

template<typename T>
bool splice_list<T>::operator  > (const splice_list& other) const
{ return compare(other)        > 0; }

template<typename T>
bool splice_list<T>::operator >= (const splice_list& other) const
{ return compare(other)       >= 0; }

