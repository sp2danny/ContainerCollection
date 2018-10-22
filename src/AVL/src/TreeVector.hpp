
#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <utility>
#include <iostream>
#include <functional>

template<typename T, template<typename...> class Alloc = std::allocator>
class TreeVector
{
	union Node;
	typedef Node* NodeP;

public:
	// types
	struct                  iterator;
	struct                  const_iterator;
	typedef T               value_type;
	typedef T&              reference;
	typedef const T&        const_reference;
	typedef T*              pointer;
	typedef const T&        const_pointer;
	typedef std::size_t     size_type;
	typedef std::ptrdiff_t  difference_type;
	typedef Alloc<Node>     allocator_type;

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
		T& operator*();
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
		NodeP node;
		TreeVector* owner;
		friend class TreeVector;
		friend struct const_iterator;
	};

	struct const_iterator
		: std::iterator<std::random_access_iterator_tag, const T>
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

		const_iterator(typename TreeVector::iterator i)
			: node(i.node), owner(i.owner)
		{
		}

	private:
		const_iterator(NodeP n, const TreeVector* o) : node(n), owner(o) {}
		NodeP node;
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

	const_iterator begin()  const;
	const_iterator end()    const;
	const_iterator cbegin() const { return begin(); }
	const_iterator cend()   const { return end();   }

	typedef std::reverse_iterator<iterator> reverse_iterator;

	reverse_iterator rbegin() { return reverse_iterator{end()  }; }
	reverse_iterator rend()   { return reverse_iterator{begin()}; }

	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	const_reverse_iterator rbegin()  const { return const_reverse_iterator{cend()  }; }
	const_reverse_iterator rend()    const { return const_reverse_iterator{cbegin()}; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()  }; }
	const_reverse_iterator crend()   const { return const_reverse_iterator{cbegin()}; }

	// capacity
	std::size_t size() const;
	std::size_t max_size() const;
	bool empty() const;

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
	iterator unstable_insert_position(const T&); // slightly faster than upper_bound

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
	void merge(TreeVector&& other, Op&& op) { merge(other, op); }

	bool is_sorted() const { return is_sorted(std::less<T>()); }
	template<typename Op>
	bool is_sorted(Op&&) const;

	void unique();
	void remove(const T&);
	template<typename Op>
	void remove_if(Op&&);

	template<typename Op>
	void for_each(Op&&);

	void reverse();

	void resize(std::size_t, const T& = T{});

	void splice(iterator pos, TreeVector& other, iterator item);
	void splice(iterator pos, TreeVector& other);
	void splice(iterator pos, TreeVector&& other) { splice(pos, other); }
	void splice(iterator pos, TreeVector& other, iterator b, iterator e);

	int compare(const TreeVector&) const;

	TreeVector(std::initializer_list<T>);
	TreeVector(std::size_t, const T&);
	template<
		typename It,
		typename Cat = typename std::iterator_traits<It>::iterator_category>
	TreeVector(It, It);

	template<
		typename It,
		typename Cat = typename std::iterator_traits<It>::iterator_category>
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

	struct NodeS
	{
		int   weight, height;
		NodeP parent, left, right;
		T     item;
		template<typename... Args>
		NodeS(Args&&... args) : item(std::forward<Args>(args)...) {}
	};

	struct SentryS
	{
		int   weight, height;
		NodeP parent, left, right;
	};

	struct sentry_tag {};
	struct node_tag   {};

	union Node {
		NodeS   n;
		SentryS s;
		Node(sentry_tag);
		template<typename... Args>
		Node(node_tag, Args&&...);
		~Node();

		int    balance();
		bool   sentry();
		int&   weight();
		int&   height();
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

	template<
		typename It,
		typename Cat = typename std::iterator_traits<It>::iterator_category>
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

#include "TreeVector_Nonmember.cpp"

