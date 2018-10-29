
#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <iomanip>
#include <utility>
#include <cassert>
#include <algorithm>
#include <memory>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <iterator>

template<typename T, typename A = std::allocator<T>>
class avl_vector
{
	struct Node;
	typedef Node* NodeP;
	struct Core;

	struct payload_tag {};
	struct sentry_tag {};

	struct Node
	{
		Node(sentry_tag) : dummy(0) {}
		template<typename... Args>
		Node(payload_tag, Args&&... args)
			: item(std::forward<Args>(args)...)
		{ }
		NodeP parent, left, right;
		std::size_t weight, height;
		union {
			T item;
			char dummy;
		};
		int balance() const
		{
			auto rh = right->height;
			auto lh = left->height;
			if (rh > lh)
				return +int(rh-lh);
			else
				return -int(lh-rh);
		}
		void setnil(NodeP nil) { parent = left = right = nil; weight = height = 0; }
		~Node()
		{
			if (weight||height)
			{
				assert(weight&&height);
				item.~T();
			}
		}
		bool sentry() const
		{
			if (weight||height)
				assert(weight&&height);
			return !(weight&&height);
		}
	};

	struct Core
	{
		NodeP root;
		NodeP nil;
	};

	Core core;

	std::size_t _AVL_indexof(const Node* p) const
	{
		std::size_t idx = 0;
		idx += p->left->weight;
		while (p != core.root)
		{
			if (_AVL_is_right(p))
			{
				idx += p->parent->left->weight + 1;
			}
			p = p->parent;
		}
		return idx;
	}

	static void _AVL_link_r(NodeP par, NodeP r)
	{
		par->right = r;
		r->parent = par;
	}

	static void _AVL_link_l(NodeP par, NodeP l)
	{
		par->left = l;
		l->parent = par;
	}

	static bool _AVL_is_left(const Node* n)
	{
		return n->parent->left == n;
	}

	static bool _AVL_is_right(const Node* n)
	{
		return n->parent->right == n;
	}

	/// Insert existing node in the tree, before at
	NodeP _AVL_insert_node(NodeP at, NodeP newn)
	{
		if (at->left == core.nil)
		{
			_AVL_link_l(at, newn);
			//at->left = newn;
			_AVL_balance(at);
			return newn;
		}
		at = at->left;
		while (at->right != core.nil)
			at = at->right;
		_AVL_link_r(at, newn);
		//at->right = newn;
		_AVL_balance(at);
		return newn;
	}
	/// Insert new data in the tree, before at
	template<typename... Args>
	NodeP _AVL_insert(NodeP at, Args&&... args)
	{
		return _AVL_insert_node(at, _AVL_node_new(std::forward<Args>(args)...));
	}
	
	NodeP _AVL_nth(std::size_t idx)
	{
		if (idx==size()) return _AVL_last_node();
		assert( idx < size() );
		
		static NodeP (*fndi)(Core& c, NodeP, std::size_t) = [](Core& c, NodeP p, std::size_t idx) -> NodeP
		{
			assert (p != c.nil);
			assert (p != c.root);
			auto lw = p->left->weight;
			if (lw == idx) return p;
			if (idx < lw) return fndi(c, p->left, idx);
			assert(idx >= (lw+1));
			idx -= (lw+1);
			return fndi(c, p->right, idx);
		};
		
		return fndi(core, core.root->left, (int)idx);
	}
	const Node* _AVL_nth(std::size_t idx) const
	{
		return ((avl_vector*)this)->_AVL_nth(idx);
	}

	/// Insert data in its sorted position
	/// Only works if container is sorted (binary search)
	NodeP _AVL_insert_sorted(const T& data)
	{
		NodeP node = core.root;
		if (node->left == core.nil)
		{
			_AVL_link_l(node, _AVL_node_new(data));
			_AVL_balance(node);
			return node;
		}
		node = node->left;
		while (true)
		{
			if (data < node->item)
			{
				if (node->left == core.nil)
				{
					_AVL_link_l(node, _AVL_node_new(data));
					_AVL_balance(node);
					return node;
				}
				node = node->left;
			} else {
				if (node->right == core.nil)
				{
					_AVL_link_r(node, _AVL_node_new(data));
					_AVL_balance(node);
					return node;
				}
				node = node->right;
			}
		}
	}

	void _AVL_full_node_swap(NodeP n1, NodeP n2)
	{
		NodeP  new_n1p = n2->parent;
		NodeP  new_n1l = n2->left;
		NodeP  new_n1r = n2->right;
		NodeP*  n1_lnk = _AVL_is_left(n2) ? &new_n1p->left : &new_n1p->right;

		NodeP  new_n2p = n1->parent;
		NodeP  new_n2l = n1->left;
		NodeP  new_n2r = n1->right;
		NodeP*  n2_lnk = _AVL_is_left(n1) ? &new_n2p->left : &new_n2p->right;

		if (n2->parent == n1)
		{
			new_n1p = n2;
			n1_lnk = nullptr;
			if (n1->left == n2)
				new_n2l = n1;
			else
				new_n2r = n1;
		}
		else if (n1->parent == n2)
		{
			new_n2p = n1;
			n2_lnk = nullptr;
			if (n2->left == n1)
				new_n1l = n2;
			else
				new_n1r = n2;
		}

		n1->parent          = new_n1p;
		n1->left            = new_n1l;
		n1->left->parent    = n1;
		n1->right           = new_n1r;
		n1->right->parent   = n1;
		if (n1_lnk) *n1_lnk = n1;

		n2->parent          = new_n2p;
		n2->left            = new_n2l;
		n2->left->parent    = n2;
		n2->right           = new_n2r;
		n2->right->parent   = n2;
		if (n2_lnk) *n2_lnk = n2;

		std::swap(n1->weight, n2->weight);
		std::swap(n1->height, n2->height);
	}

	auto _AVL_relink(NodeP n1, NodeP n2)
	{
		if (_AVL_is_left(n1))
			_AVL_link_l(n1->parent, n2);
		else
			_AVL_link_r(n1->parent, n2);
	};

	NodeP _AVL_unlink_node(NodeP node)
	{
		bool ln = node->left == core.nil;
		bool rn = node->right == core.nil;

		if (ln&&rn)
		{
			_AVL_relink(node, core.nil);
			_AVL_balance(node->parent);
		}
		else if (ln)
		{
			_AVL_relink(node, node->right);
			_AVL_balance(node->right);
		}
		else if (rn)
		{
			_AVL_relink(node, node->left);
			_AVL_balance(node->left);
		}
		else
		{
			auto bal = node->balance();
			if (bal >= 0)
			{
				NodeP succ = _AVL_next_node(node);
				_AVL_full_node_swap(node, succ);
				return _AVL_unlink_node(node);
			}
			else {
				NodeP pred = _AVL_prev_node(node);
				_AVL_full_node_swap(node, pred);
				return _AVL_unlink_node(node);
			}
		}
		return node;
	}

	/// Removes a given node from the tree.
	void _AVL_delete_node(NodeP node)
	{
		delete _AVL_unlink_node(node);
	}

	/// Searches the tree for a node containing the given data.
	/// Only works if container is sorted (binary search)
	NodeP _AVL_search_node(const T& what)
	{
		NodeP node = core.root->left;

		while (node != core.nil)
		{
			if (what < node->item)
				node = node->left;
			else if (node->item < what)
				node = node->right;
			else
				return node;
		}

		return core.root;
	}

	/// Searches the tree for earliest node larger than data
	NodeP _AVL_sorted_insert_position(const T& data)
	{
		NodeP node = core.root->left;
		NodeP lwl = core.root;

		if (node == core.nil)
			return lwl;

		while (true)
		{
			if (data < node->item)
			{
				if (node->left == core.nil)
					return node;
				lwl = node;
				node = node->left;
			} else {
				if (node->right == core.nil)
					return lwl;
				node = node->right;
			}
		}
	}

	/// Returns the first node, or sentry.
	NodeP _AVL_first_node()
	{
		NodeP node = core.root->left;

		if (node == core.nil)
			return core.root;

		while (node->left != core.nil)
			node = node->left;

		return node;
	}

	/// Returns the node after the last. (end marker)
	NodeP _AVL_last_node()
	{
		return core.root;
	}

	/// Returns the last actual node
	NodeP _AVL_last_payload_node()
	{
		NodeP n = core.root->left;
		assert(n && n!=core.nil);
		while (n->right != core.nil)
			n = n->right;
		return n;
	}

	/// Returns the predecessor of the given node.
	NodeP _AVL_prev_node(NodeP n)
	{
		if (n == core.root)
		{
			n = n->left;
			while (n->right != core.nil)
				n = n->right;
			return n;
		}

		NodeP nTemp;

		if (n->left != core.nil)
		{
			n = n->left;
			while (n->right != core.nil)
				n = n->right;
		} else {
			nTemp = n;
			n = n->parent;
			while ((n != core.nil) && (n->left == nTemp))
			{
				nTemp = n;
				n = n->parent;
			}
		}
		return n;
	}

	/// Returns the follower of the given node.
	NodeP _AVL_next_node(NodeP n)
	{
		if (n == core.root)
		{
			while (n->left != core.nil)
				n = n->left;
			return n;
		}

		Node* nTemp;

		if (n->right != core.nil)
		{
			n = n->right;
			while (n->left != core.nil)
				n = n->left;
		}
		else {
			nTemp = n;
			n = n->parent;
			while ((n != core.nil) && (n->right == nTemp))
			{
				nTemp = n;
				n = n->parent;
			}
		}

		return n;
	}

	void _AVL_balance(NodeP node)
	{
		while (node != core.root)
		{
			NodeP n, par = node->parent;

			_AVL_updHW(node);

			int balance = node->balance();

			assert((balance >= -2) && (balance <= +2));

			if (balance <= -2)
			{
				par = node->left;
				int lb = node->left->balance();
				if (lb <= -1)
					n = _AVL_rotate_right(node);
				else
					n = _AVL_rotate_left_right(node);
			}
			else if (balance >= +2)
			{
				par = node->right;
				int rb = node->right->balance();
				if (rb >= +1)
					n = _AVL_rotate_left(node);
				else
					n = _AVL_rotate_right_left(node);
			}
			node = par;
			assert(node != core.nil);
		}
	}

	NodeP _AVL_rotate_left(NodeP node)
	{
		NodeP right = node->right;
		NodeP rightLeft = right->left;
		NodeP parent = node->parent;

		_AVL_relink(node, right);
		_AVL_link_l(right, node);
		_AVL_link_r(node, rightLeft);

		_AVL_updHW(node);

		return right;
	}

	NodeP _AVL_rotate_right(NodeP node)
	{
		NodeP left = node->left;
		NodeP leftRight = left->right;
		NodeP parent = node->parent;

		_AVL_relink(node, left);
		_AVL_link_r(left, node);
		_AVL_link_l(node, leftRight);
		_AVL_updHW(node);

		return left;
	}

	NodeP _AVL_rotate_left_right(NodeP node)
	{
		_AVL_link_l(node, _AVL_rotate_left(node->left));
		return _AVL_rotate_right(node);
	}

	NodeP _AVL_rotate_right_left(NodeP node)
	{
		_AVL_link_r(node, _AVL_rotate_right(node->right));
		return _AVL_rotate_left(node);
	}

	static void _AVL_updHW(NodeP node)
	{
		node->height = std::max(node->left->height, node->right->height) + 1;
		node->weight = node->left->weight + node->right->weight + 1;
	}

	struct SR // SortResult
	{
		T* min;
		T* max;
		bool sorted;
	};

	SR _AVL_is_sub_sorted(NodeP node) const
	{
		bool ln = node->left == core.nil;
		bool rn = node->right == core.nil;

		SR result{ &node->item, &node->item, true };

		if (ln&&rn)
		{
			return result;
		}

		if (!ln)
		{
			SR lft = _AVL_is_sub_sorted(node->left);
			if (!lft.sorted)
				result.sorted = false;
			if (node->item < *lft.max)
				result.sorted = false;
			result.min = lft.min;
		}
		if (!rn)
		{
			SR rgt = _AVL_is_sub_sorted(node->right);
			if (!rgt.sorted)
				result.sorted = false;
			if (*rgt.min < node->item)
				result.sorted = false;
			result.max = rgt.max;
		}

		return result;
	}

	template<typename... Args>
	NodeP _AVL_node_new(Args&&... args)
	{
		NodeP p = allocator_type{}.allocate(1);
		new (p) Node(payload_tag{}, std::forward<Args>(args)...);
		p->parent = p->left = p->right = core.nil;
		p->weight = p->height = 1;
		return p;
	}

	bool _AVL_integrity(NodeP node) const
	{
		if (node == core.nil) return true;

		long long lh = node->left->height;
		long long rh = node->right->height;

		if (std::abs(lh - rh) > 1) return false;

		long long h = std::max(lh, rh) + 1;
		long long w = node->left->weight + node->right->weight + 1;

		if (h != node->height) return false;
		if (w != node->weight) return false;

		bool lrn = (node->left == core.nil) && (node->right == core.nil);

		if ((w == 1) || (h == 1) || lrn)
		{
			if ((w*h) != 1) return false;
			if (!lrn) return false;
		}

		if (node->left != core.nil)
		{
			if (node->left->parent != node) return false;
			if (!_AVL_integrity(node->left)) return false;
		}

		if (node->right != core.nil)
		{
			if (node->right->parent != node) return false;
			if (!_AVL_integrity(node->right)) return false;
		}

		return true;
	}

	struct Trunk {
		Trunk *prev;
		std::string str;
	};

	void _AVL_print_trunks(std::ostream& out, Trunk *p) const
	{
		if (!p) {
			return;
		}
		_AVL_print_trunks(out, p->prev);
		out << p->str;
	}

	void _AVL_print_tree(std::ostream& out, NodeP n, Trunk *prev, bool is_left) const
	{
		if (n == core.nil)
			return;

		Trunk this_disp = { prev, "     " };
		std::string prev_str = this_disp.str;
		_AVL_print_tree(out, n->right, &this_disp, true);

		if (!prev) {
			this_disp.str = "---";
		}
		else if (is_left) {
			this_disp.str = ".--";
			prev_str = "    |";
		}
		else {
			this_disp.str = "`--";
			prev->str = prev_str;
		}

		_AVL_print_trunks(out, &this_disp);
		out << " " << n->item;
		out << " [" << _AVL_indexof(n) << "] ";
		out << " (" << std::showpos << n->balance() << std::noshowpos
			<< "," << n->weight << "," << n->height << ")\n";

		if (prev) {
			prev->str = prev_str;
		}
		this_disp.str = "    |";

		_AVL_print_tree(out, n->left, &this_disp, false);
		if (!prev) {
			out << ("");
		}
	}

	typedef std::vector<NodeP> VNP;

	NodeP _AVL_hang(NodeP* ap, NodeP* bp)
	{
		auto sz = bp - ap;
		if (!sz)
			return core.nil;
		if (sz==1)
		{
			NodeP n = *ap;
			n->weight = n->height = 1;
			n->left = n->right = core.nil;
			return n;
		}
		auto center = sz/2;
		NodeP* cp = ap+center;
		_AVL_link_l(*cp, _AVL_hang(ap, cp));
		_AVL_link_r(*cp, _AVL_hang(cp+1, bp));
		_AVL_updHW(*cp);
		return *cp;
	}
	NodeP _AVL_hang(VNP& vnp)
	{
		NodeP* ptr = vnp.data();
		std::size_t sz = vnp.size();
		return _AVL_hang(ptr, ptr+sz);
	}
	std::size_t _AVL_flatten(VNP& vnp)
	{
		vnp.clear();
		auto sz = core.root->left->weight;
		vnp.reserve(sz);
		NodeP n = _AVL_first_node();
		NodeP e = _AVL_last_node();
		while (n!=e)
		{
			vnp.push_back(n);
			n = _AVL_next_node(n);
		}
		return sz;
	}
	std::size_t _AVL_flatten_insert(VNP& target, NodeP breakp, VNP& inserted)
	{
		target.clear();
		auto sz = core.root->left->weight + inserted.size();
		target.reserve(sz);
		NodeP n = _AVL_first_node();
		NodeP e = _AVL_last_node();
		while (true)
		{
			if (n == breakp)
				target.insert(target.end(), inserted.begin(), inserted.end());
			if (n == e) break;
			target.push_back(n);
			n = _AVL_next_node(n);
		}
		return sz;
	}

	void _AVL_insert_range(NodeP n, VNP& vnp)
	{
		if (size() > vnp.size())
		{
			for (NodeP p : vnp)
			{
				n = _AVL_insert_node(n, p);
				n = _AVL_next_node(n);
			}
		} else {
			VNP vnp_new;
			_AVL_flatten_insert(vnp_new, n, vnp);
			_AVL_link_l(core.root, _AVL_hang(vnp_new));
		}
	}



friend
	struct iterator;

public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;
	struct iterator;
	struct const_iterator;
	typedef typename A::template rebind<Node>::other allocator_type;

	/// Creates a new empty tree.
	avl_vector()
	{
		NodeP p = allocator_type{}.allocate(2);
		core.root = new (p+0) Node{ sentry_tag{} };
		core.nil  = new (p+1) Node{ sentry_tag{} };
		core.root ->setnil(core.nil);
		core.nil  ->setnil(core.nil);
	}
	template<typename It>
	avl_vector(It b, It e)
		: avl_vector()
	{
		VNP vpn;
		while (b != e)
			vpn.push_back(_AVL_node_new(*b++));
		_AVL_link_l(core.root, _AVL_hang(vpn));
	}
	avl_vector(std::initializer_list<T> il)
		: avl_vector(il.begin(), il.end())
	{}
	avl_vector(std::size_t sz, const T& val)
		: avl_vector()
	{
		VNP vpn;
		while (sz--)
			vpn.push_back(_AVL_node_new(val));
		_AVL_link_l(core.root, _AVL_hang(vpn));
	}
	avl_vector(const avl_vector& other)
		: avl_vector(other.begin(), other.end())
	{}
	avl_vector(avl_vector&& other)
		: avl_vector()
	{
		swap(other);
	}
	avl_vector& operator=(const avl_vector& other)
	{
		assign(other.begin(), other.end());
		return *this;
	}
	avl_vector& operator=(std::initializer_list<T> il)
	{
		assign(il.begin(), il.end());
		return *this;
	}
	template<typename It>
	void assign(It b, It e)
	{
		clear();
		VNP vpn;
		while (b != e)
			vpn.push_back(_AVL_node_new(*b++));
		_AVL_link_l(core.root, _AVL_hang(vpn));
	}
	void assign(std::initializer_list<T> il)
	{
		assign(il.begin(), il.end());
	}
	void assign(std::size_t n, const T& val)
	{
		VNP vnp;
		while (n--)
			vnp.push_back(_AVL_node_new(val));
		_AVL_link_l(core.root, _AVL_hang(vnp));
	}
	avl_vector& operator=(avl_vector&& other) noexcept
	{
		swap(other);
		return *this;
	}
	void swap(avl_vector& other) noexcept
	{
		using std::swap;
		swap(core.root, other.core.root);
		swap(core.nil, other.core.nil);
	}
	std::size_t size() const
	{
		return core.root->left->weight;
	}

	bool empty() const { return core.root->left->weight == 0; }

	bool sorted() const
	{
		if (core.root->left->weight <= 1) return true;
		return _AVL_is_sub_sorted(core.root->left).sorted;
	}

	void clear()
	{
		static void (*rec_clr)(NodeP, NodeP)
		 = [](NodeP node, NodeP nil)
		{
			if (node==nil) return;
			rec_clr(node->left,  nil);
			rec_clr(node->right, nil);
			node->~Node();
			allocator_type{}.deallocate(node, 1);
		};

		rec_clr(core.root->left, core.nil);
		core.root->left = core.nil;
	}

	~avl_vector()
	{
		clear();
		assert(core.root->sentry() && core.nil->sentry());
		allocator_type{}.deallocate(core.root, 2);
		core.root = core.nil = nullptr;
	}

	bool integrity() const
	{
		if (!core.nil) return false;
		if (core.nil->height != 0) return false;
		if (core.nil->weight != 0) return false;

		if (core.nil->left != core.nil) return false;
		if (core.nil->right != core.nil) return false;

		if (!core.root) return false;
		if (core.root->parent != core.nil) return false;
		if (core.root->right != core.nil) return false;

		return _AVL_integrity(core.root->left);
	}

	void print_tree(std::ostream& out) const
	{
		_AVL_print_tree(out, core.root->left, nullptr, true);
	}

	struct iterator
	{
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef std::ptrdiff_t difference_type;
		iterator() = default;
		T& operator*() { return node->item; }
		T* operator->() { return &node->item; }
		iterator& operator++() { node = avl->_AVL_next_node(node); return *this; }
		iterator& operator--() { node = avl->_AVL_next_node(node); return *this; }
		iterator operator++(int) { auto tmp = *this; node = avl->_AVL_next_node(node); return tmp; }
		iterator operator--(int) { auto tmp = *this; node = avl->_AVL_next_node(node); return tmp; }
		bool operator==(const iterator& other) const { assert(avl == other.avl); return node == other.node; }
		bool operator!=(const iterator& other) const { assert(avl == other.avl); return node != other.node; }
	friend
		class avl_vector;
	friend
		struct const_iterator;
	private:
		iterator(avl_vector* avl, Node* node) : avl(avl), node(node) {}
		avl_vector* avl = nullptr;
		Node* node = nullptr;
	};

	struct const_iterator
	{
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef const T value_type;
		typedef const T* pointer;
		typedef const T& reference;
		typedef std::ptrdiff_t difference_type;
		const_iterator() = default;
		const_iterator(iterator i) : avl(i.avl), node(i.node) {}
		const T& operator*() { return node->item; }
		const T* operator->() { return &node->item; }
		const_iterator& operator++() { node = avl->_AVL_next_node(node); return *this; }
		const_iterator& operator--() { node = avl->_AVL_next_node(node); return *this; }
		const_iterator operator++(int) { auto tmp = *this; node = avl->_AVL_next_node(node); return tmp; }
		const_iterator operator--(int) { auto tmp = *this; node = avl->_AVL_next_node(node); return tmp; }
		bool operator==(const const_iterator& other) const { assert(avl == other.avl); return node == other.node; }
		bool operator!=(const const_iterator& other) const { assert(avl == other.avl); return node != other.node; }
	friend
		class avl_vector;
	private:
		const_iterator(avl_vector* avl, Node* node) : avl(avl), node(node) {}
		avl_vector* avl = nullptr;
		Node* node = nullptr;
	};

	iterator begin() { return {this, _AVL_first_node()}; }
	iterator end()   { return {this, _AVL_last_node()}; }

	iterator begin()  const { avl_vector* me = (avl_vector*)this; return {me, me->_AVL_first_node()}; }
	iterator end()    const { avl_vector* me = (avl_vector*)this; return {me, me->_AVL_last_node()}; }
	iterator cbegin() const { return begin(); }
	iterator cend()   const { return end(); }

	iterator nth(std::size_t idx) { return {this, _AVL_nth(idx)}; }

	iterator insert(iterator itr, const T& item)
	{
		auto p = _AVL_insert(itr.node, item);
		return {this, p};
	}
	template<typename It>
	void insert(iterator itr, It b, It e)
	{
		VNP vnp;
		typedef std::iterator_traits<It>::iterator_category ItCat;
		if constexpr (std::is_same_v<ItCat, std::random_access_iterator_tag>)
		{
			vnp.reserve(e - b);
		}
		while (b != e)
		{
			vnp.push_back( _AVL_node_new(*b) );
			++b;
		}
		_AVL_insert_range(itr.node, vnp);
	}
	template<typename... Args>
	iterator emplace(iterator itr, Args&&... args)
	{
		auto p = _AVL_insert(itr.node, std::forward<Args>(args)...);
		return {this, p};
	}
	iterator erase(iterator itr)
	{
		auto p = _AVL_next_node(itr.node);
		_AVL_delete_node(itr.node);
		return {this, p};
	}
	iterator erase(iterator b, iterator e)
	{
		while (b != e)
		{
			b = erase(b);
		}
		return b;
	}

	T& operator[](std::size_t idx) { return _AVL_nth(idx)->item; }
	const T& operator[](std::size_t idx) const { return _AVL_nth(idx)->item; }

	T& front() { return _AVL_first_node()->item; }
	T& back() { return _AVL_last_payload_node()->item; }
	const T& front() const { return ((avl_vector*)this)->_AVL_first_node()->item; }
	const T& back() const { return ((avl_vector*)this)->_AVL_last_payload_node()->item; }

	T& push_back(const T& item)
	{
		auto p = _AVL_insert(_AVL_last_node(), item);
		return p->item;
	}
	T& push_back(T&& item)
	{
		auto p = _AVL_insert(_AVL_last_node(), std::move(item));
		return p->item;
	}
	T& push_front(const T& item)
	{
		auto p = _AVL_insert(_AVL_first_node(), item);
		return p->item;
	}
	T& push_front(T&& item)
	{
		auto p = _AVL_insert(_AVL_first_node(), std::move(item));
		return p->item;
	}
	template<typename... Args>
	T& emplace_back(Args&&... args)
	{
		auto p = _AVL_insert(_AVL_last_node(), std::forward<Args>(args)...);
		return p->item;
	}
	template<typename... Args>
	T& emplace_front(Args&&... args)
	{
		auto p = _AVL_insert(_AVL_first_node(), std::forward<Args>(args)...);
		return p->item;
	}

	void pop_back() { _AVL_delete_node(_AVL_last_payload_node()); }
	void pop_front() { _AVL_delete_node(_AVL_first_node()); }

	template<typename Op = std::less<T>>
	void sort(Op op = Op{})
	{
		VNP vnp;
		_AVL_flatten(vnp);
		auto nless = [&op](NodeP lhs, NodeP rhs) -> bool
		{
			return op(lhs->item, rhs->item);
		};
		std::sort(vnp.begin(), vnp.end(), nless);
		_AVL_link_l(core.root, _AVL_hang(vnp));
	}
	template<typename Op = std::less<T>>
	void stable_sort(Op op = Op{})
	{
		VNP vnp;
		_AVL_flatten(vnp);
		auto nless = [&op](NodeP lhs, NodeP rhs) -> bool
		{
			return op(lhs->item, rhs->item);
		};
		std::stable_sort(vnp.begin(), vnp.end(), nless);
		_AVL_link_l(core.root, _AVL_hang(vnp));
	}

	template<typename Op = std::less<T>>
	void unique(Op op = Op{})
	{
		VNP vnp;
		_AVL_flatten(vnp);
		auto nless = [&op](NodeP lhs, NodeP rhs) -> bool
		{
			return op(lhs->item, rhs->item);
		};
		auto ptr = vnp.data();
		auto sz = vnp.size();
		auto p = std::unique(ptr, ptr+sz, nless);
		_AVL_link_l(core.root, _AVL_hang(ptr, p));
		while (p != (ptr+sz))
		{
			_AVL_delete_node(*p);
			++p;
		}
	}

	void reverse()
	{
		VNP vnp;
		_AVL_flatten(vnp);
		std::reverse(vnp.begin(), vnp.end());
		_AVL_link_l(core.root, _AVL_hang(vnp));
	}

	void reserve(std::size_t) {}
	void shrink_to_fit() {}
	std::size_t capacity() const { return max_size(); }
	std::size_t max_size() const { return (unsigned long)-1; }

	std::size_t remove(const T& value)
	{
		auto op = [&value](const T& itm)
		{
			return itm == value;
		};
		return remove_if(op);
	}
	template<typename Op>
	std::size_t remove_if(Op op)
	{
		std::size_t cnt = 0;
		NodeP n = _AVL_first_node();
		NodeP e = _AVL_last_node();
		while (n != e)
		{
			NodeP t = _AVL_next_node(n);
			if (op(n->item))
			{
				_AVL_delete_node(n);
				++cnt;
			}
			n = t;
		}
		return cnt;
	}

	void splice(iterator pos, avl_vector& other)
	{
		VNP vnp_me, vnp_ot;

		other._AVL_flatten(vnp_ot);
		other.core.root->left = other.core.nil;
		_AVL_flatten_insert(vnp_me, pos.node, vnp_ot);
		_AVL_link_l(core.root, _AVL_hang(vnp_me));
	}
	void splice(iterator pos, avl_vector&& other) { splice(pos, other); }

	void splice(iterator pos, avl_vector& other, iterator it)
	{
		NodeP n = other._AVL_unlink_node(it.node);
		_AVL_insert_node(pos.node, n);
	}
	void splice(iterator pos, avl_vector&& other, iterator it) { splice(pos, other, it); }
};

