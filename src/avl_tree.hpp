
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
			return weight&&height;
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

	/// Insert new data in the tree, before n
	template<typename... Args>
	NodeP _AVL_insert(NodeP n, Args&&... args)
	{
		if (n->left == core.nil)
		{
			n->left = _AVL_node_new(n, std::forward<Args>(args)...);
			_AVL_balance(n);
			return n;
		}
		n = n->left;
		while (n->right != core.nil)
			n = n->right;
		n->right = _AVL_node_new(n, std::forward<Args>(args)...);
		_AVL_balance(n);
		return n;
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
			node->left = _AVL_node_new(node, data);
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
					node->left = _AVL_node_new(node, data);
					_AVL_balance(node);
					return node;
				}
				node = node->left;
			} else {
				if (node->right == core.nil)
				{
					node->right = _AVL_node_new(node, data);
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


	/// Removes a given node from the tree.
	void _AVL_delete_node(NodeP node)
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
				return _AVL_delete_node(node);
			} else {
				NodeP pred = _AVL_prev_node(node);
				_AVL_full_node_swap(node, pred);
				return _AVL_delete_node(node);
			}
		}

		delete node;
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
	NodeP _AVL_node_new(NodeP parent, Args&&... args)
	{
		NodeP p = allocator_type{}.allocate(1);
		new (p) Node(payload_tag{}, std::forward<Args>(args)...);
		p->parent = parent;
		p->left = p->right = core.nil;
		p->weight = p->height = 1;
		return p;
	}

	bool _AVL_integrity(NodeP node) const
	{
		if (node == core.nil) return true;

		int lh = node->left->height;
		int rh = node->right->height;

		if (std::abs(lh - rh) > 1) return false;

		int h = std::max(lh, rh) + 1;
		int w = node->left->weight + node->right->weight + 1;

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
		bool operator==(const iterator& other) const { assert(avl == other.avl); return node == other.node; }
		bool operator!=(const iterator& other) const { assert(avl == other.avl); return node != other.node; }
	friend
		class avl_vector;
	private:
		iterator(avl_vector* avl, Node* node) : avl(avl), node(node) {}
		avl_vector* avl = nullptr;
		Node* node = nullptr;
	};

	iterator begin() { return {this, _AVL_first_node()}; }
	iterator end() { return {this, _AVL_last_node()}; }

	iterator nth(std::size_t idx) { return {this, _AVL_nth(idx)}; }

	iterator insert(iterator node, const T& item)
	{
		auto p = _AVL_insert(node.node, item);
		return {this, p};
	}
	template<typename... Args>
	iterator emplace(iterator node, Args&&... args)
	{
		auto p = _AVL_insert(node.node, std::forward<Args>(args)...);
		return {this, p};
	}
	iterator erase(iterator node)
	{
		auto p = _AVL_next_node(node.node);
		_AVL_delete_node(node.node);
		return {this, p};
	}

	T& operator[](std::size_t idx) { return _AVL_nth(idx)->item; }
	const T& operator[](std::size_t idx) const { return _AVL_nth(idx)->item; }

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

};


