
#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <iomanip>
#include <utility>
#include <cassert>
#include <algorithm>

template<typename T>
class avl_tree
{
//protected:
public:
	typedef unsigned sz_t;

	struct Node;
	typedef Node* NodeP;
	struct Core;

	struct Node
	{
		template<typename... Args>
		Node(NodeP par, NodeP nil, Args&&... args)
			: parent(par)
			, left(nil)
			, right(nil)
			, weight(1)
			, height(1)
			, item(std::forward<Args>(args)...)
		{ }
		NodeP parent, left, right;
		int weight, height;
		T item;
		int balance() const { return right->height - left->height; }
		//int myindex(const avl_tree& at) const { return at.indexof(this); }
	};

	struct Sentry
	{
		NodeP parent, left, right;
		int weight, height;
	};

	struct Core
	{
		NodeP root;
		NodeP nil;
	};

	Core core;

	/// Creates a new empty tree.
	avl_tree()
	{
		Sentry* nil  = new Sentry;
		Sentry* root = new Sentry;
		core.root = (Node*)root;
		core.nil  = (Node*)nil;
		(*nil) = (*root) = Sentry{core.nil,core.nil,core.nil, 0,0};
	}

	int indexof(const Node* p) const
	{
		int idx = 0;
		idx += p->left->weight;
		while (p != core.root)
		{
			if (is_right(p))
			{
				idx += p->parent->left->weight + 1;
			}
			p = p->parent;
		}
		return idx;
	}

	static void link_r(NodeP par, NodeP r)
	{
		par->right = r;
		r->parent = par;
	}

	static void link_l(NodeP par, NodeP l)
	{
		par->left = l;
		l->parent = par;
	}

	static bool is_left(const Node* n)
	{
		return n->parent->left == n;
	}

	static bool is_right(const Node* n)
	{
		return n->parent->right == n;
	}

	/// Insert new data in the tree, before where
	template<typename... Args>
	NodeP insert(NodeP where, Args&&... args)
	{
		if (where->left == core.nil)
		{
			where->left = node_new(where, std::forward<Args>(args)...);
			balance(where);
			return where;
		}
		where = where->left;
		while (where->right != core.nil)
			where = where->right;
		where->right = node_new(where, std::forward<Args>(args)...);
		balance(where);
		return where;
	}
	
	NodeP nth(sz_t idx)
	{
		if (idx==size()) return last_node();
		assert( idx < size() );
		
		static NodeP (*fndi)(Core& c, NodeP, int) = [](Core& c, NodeP p, int idx) -> NodeP
		{
			assert (p != c.nil);
			assert (p != c.root);
			auto lw = p->left->weight;
			if (lw == idx) return p;
			if (idx < lw) return fndi(c, p->left, idx);
			idx -= (lw+1);
			return fndi(c, p->right, idx);
		};
		
		return fndi(core, core.root->left, (int)idx);
	}

	/// Insert data in its sorted position
	/// Only works if container is sorted (binary search)
	NodeP insert_sorted(const T& data)
	{
		NodeP node = core.root;
		if (node->left == core.nil)
		{
			node->left = node_new(node, data);
			balance(node);
			return node;
		}
		node = node->left;
		while (true)
		{
			if (data < node->item)
			{
				if (node->left == core.nil)
				{
					node->left = node_new(node, data);
					balance(node);
					return node;
				}
				node = node->left;
			} else {
				if (node->right == core.nil)
				{
					node->right = node_new(node, data);
					balance(node);
					return node;
				}
				node = node->right;
			}
		}
	}

	void full_node_swap(NodeP aP, NodeP aQ)
	{
		NodeP new_p_parent = aQ->parent;
		NodeP new_p_left = aQ->left;
		NodeP new_p_right = aQ->right;
		NodeP* new_p_link = (aQ->parent->left) == aQ ? &aQ->parent->left : &aQ->parent->right;

		NodeP new_q_parent = aP->parent;
		NodeP new_q_left = aP->left;
		NodeP new_q_right = aP->right;
		NodeP* new_q_link = (aP->parent->left) == aP ? &aP->parent->left : &aP->parent->right;

		if (aQ->parent == aP)
		{
			new_p_parent = aQ;
			new_p_link = nullptr;
			if (aP->left == aQ)
				new_q_left = aP;
			else
				new_q_right = aP;
		}
		else if (aP->parent == aQ)
		{
			new_q_parent = aP;
			new_q_link = nullptr;
			if (aQ->left == aP)
				new_p_left = aQ;
			else
				new_p_right = aQ;
		}

		aP->parent = new_p_parent;
		aP->left = new_p_left;
		aP->left->parent = aP;
		aP->right = new_p_right;
		aP->right->parent = aP;
		if (new_p_link)
			*new_p_link = aP;

		aQ->parent = new_q_parent;
		aQ->left = new_q_left;
		aQ->left->parent = aQ;
		aQ->right = new_q_right;
		aQ->right->parent = aQ;
		if (new_q_link)
			*new_q_link = aQ;

		std::swap(aP->weight, aQ->weight);
		std::swap(aP->height, aQ->height);
	}

	/// Removes a given node from the tree.
	void delete_node(NodeP node)
	{
		auto relink = [&](NodeP n1,NodeP n2)
		{
			if (is_left(n1))
				link_l(n1->parent, n2);
			else
				link_r(n1->parent, n2);
		};

		bool ln = node->left == core.nil;
		bool rn = node->right == core.nil;

		if (ln&&rn)
		{
			relink(node, core.nil);
			balance(node->parent);
		}
		else if (ln)
		{
			relink(node, node->right);
			balance(node->right);
		}
		else if (rn)
		{
			relink(node, node->left);
			balance(node->left);
			//balance(node->parent);
		}
		else
		{
			auto bal = node->balance();
			if (bal >= 0)
			{
				NodeP succ = next_node(node);
				full_node_swap(node, succ);
				return delete_node(node);
			} else {
				NodeP pred = prev_node(node);
				full_node_swap(node, pred);
				return delete_node(node);
			}
		}

		delete node;
	}

	/// Searches the tree for a node containing the given data.
	/// Only works if container is sorted (binary search)
	NodeP search_node(const T& what)
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
	NodeP sorted_insert_position(const T& data)
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
	NodeP first_node()
	{
		NodeP node = core.root->left;

		if (node == core.nil)
			return core.root;

		while (node->left != core.nil)
			node = node->left;

		return node;
	}

	/// Returns the node after the last. (end marker)
	NodeP last_node()
	{
		return core.root;
	}

	/// Returns the predecessor of the given node.
	NodeP prev_node(NodeP n)
	{
		if (n == core.root)
		{
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
	NodeP next_node(NodeP n)
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
		} else {
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

	/// Returns the data in a node.
	T& get_data(NodeP n)
	{
		return n->item;
	}

	static void UpdHW(NodeP node)
	{
		node->height = std::max(node->left->height, node->right->height) + 1;
		node->weight = node->left->weight + node->right->weight + 1;
	}

	void balance(NodeP node)
	{
		while (node != core.root)
		{
			NodeP n, par = node->parent;

			UpdHW(node);

			int balance = node->balance();

			assert((balance >= -2) && (balance <= +2));

			if (balance <= -2)
			{
				par = node->left;
				int lb = node->left->balance();
				if (lb <= -1)
					n = rotate_right(node);
				else
					n = rotate_left_right(node);
				UpdHW(n);
			}
			else if (balance >= +2)
			{
				par = node->right;
				int rb = node->right->balance();
				if (rb >= +1)
					n = rotate_left(node);
				else
					n = rotate_right_left(node);
				UpdHW(n);
			}
			node = par;
			assert (node != core.nil);
		}
	}

	NodeP rotate_left(NodeP node)
	{
		NodeP right = node->right;
		NodeP rightLeft = right->left;
		NodeP parent = node->parent;

		if (is_left(node))
			link_l(parent, right);
		else
			link_r(parent, right);

		link_l(right, node);
		link_r(node, rightLeft);

		UpdHW(node);
		UpdHW(right);
		//UpdHW(parent);

		return right;
	}

	NodeP rotate_right(NodeP node)
	{
		NodeP left = node->left;
		NodeP leftRight = left->right;
		NodeP parent = node->parent;

		if (is_left(node))
			link_l(parent, left);
		else
			link_r(parent, left);

		link_r(left, node);
		link_l(node, leftRight);

		UpdHW(node);
		UpdHW(left);
		//UpdHW(parent);

		return left;
	}

	NodeP rotate_left_right(NodeP node)
	{
		link_l(node, rotate_left(node->left));
		return rotate_right(node);
	}

	NodeP rotate_right_left(NodeP node)
	{
		link_r(node, rotate_right(node->right));
		return rotate_left(node);
	}

	sz_t size()
	{
		return (sz_t) core.root->left->weight;
	}

	struct SR
	{
		T* min;
		T* max;
		bool sorted;
	};

	SR is_sub_sorted(NodeP node) const
	{
		bool ln = node->left  == core.nil;
		bool rn = node->right == core.nil;

		SR result { &node->item, &node->item, true };

		if (ln&&rn)
		{
			return result;
		}

		if (!ln)
		{
			SR lft = is_sub_sorted(node->left);
			if (!lft.sorted)
				result.sorted = false;
			if (node->item < *lft.max)
				result.sorted = false;
			result.min = lft.min;
		}
		if (!rn)
		{
			SR rgt = is_sub_sorted(node->right);
			if (!rgt.sorted)
				result.sorted = false;
			if (*rgt.min < node->item)
				result.sorted = false;
			result.max = rgt.max;
		}

		return result;
	}

	bool sorted() const
	{
		if (core.root->left->weight <= 1) return true;
		return is_sub_sorted(core.root->left).sorted;
	}

	void clear()
	{
		static void (*rec_clr)(NodeP, NodeP)
		 = [](NodeP node, NodeP nil)
		{
			if (node==nil) return;
			rec_clr(node->left,  nil);
			rec_clr(node->right, nil);
			delete node;
		};

		rec_clr(core.root->left, core.nil);
		core.root->left = core.nil;
	}

	~avl_tree()
	{
		clear();
		delete (Sentry*) core.root;
		delete (Sentry*) core.nil;
		core.root = core.nil = nullptr;
	}

	template<typename... Args>
	NodeP node_new(NodeP parent, Args&&... args)
	{
		return new Node {
			parent,
			core.nil,
			std::forward<Args>(args)...
		};
	}

	bool integrity(NodeP node) const
	{
		if (node == core.nil) return true;

		int lh = node->left->height;
		int rh = node->right->height;

		if (std::abs(lh-rh) > 1) return false;

		int h = std::max( lh, rh ) + 1;
		int w = node->weight = node->left->weight + node->right->weight + 1;

		if (h != node->height) return false;
		if (w != node->weight) return false;

		bool lrn = (node->left == core.nil) && (node->right == core.nil) ;

		if ((w==1) || (h==1) || lrn)
		{
			if ((w*h) != 1) return false;
			if (!lrn) return false;
		}

		if (node->left != core.nil)
		{
			if (node->left->parent != node) return false;
			if (!integrity(node->left)) return false;
		}

		if (node->right != core.nil)
		{
			if (node->right->parent != node) return false;
			if (!integrity(node->right)) return false;
		}

		return true;
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

		return integrity(core.root->left);
	}

	struct Trunk {
		Trunk *prev;
		std::string str;
	};

	void print_trunks(std::ostream& out, Trunk *p) const
	{
		if (!p) {
			return;
		}
		print_trunks (out, p->prev);
		out << p->str;
	}

	void print_tree(std::ostream& out, NodeP n, Trunk *prev, bool is_left) const
	{
		if (n == core.nil)
			return;

		Trunk this_disp = { prev, "     " };
		std::string prev_str = this_disp.str;
		print_tree(out, n->right, &this_disp, true);

		if (!prev) {
			this_disp.str = "---";
		} else if (is_left) {
			this_disp.str = ".--";
			prev_str = "    |";
		} else {
			this_disp.str = "`--";
			prev->str = prev_str;
		}

		print_trunks(out, &this_disp);
		out << " " << n->item;
		out << " [" << indexof(n) << "] ";
		out << " (" << std::showpos << n->balance() << std::noshowpos
		    << "," << n->weight << "," << n->height << ")\n";

		if (prev) {
			prev->str = prev_str;
		}
		this_disp.str = "    |";

		print_tree(out, n->left, &this_disp, false);
		if (!prev) {
			out << ("");
		}
	}

	void print_tree(std::ostream& out) const
	{
		print_tree(out, core.root->left, nullptr, true);
	}

};


