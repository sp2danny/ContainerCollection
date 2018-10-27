
#pragma once

#include <utility>
#include <cstddef>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <sstream>

template<typename Key, typename Val>
class hb_map
{
public:
	typedef Key key_type;
	typedef Val mapped_type;
	typedef std::pair</*const*/ Key, Val> value_type;
	struct hb_node {
		hb_node*       parent;
		hb_node*       left;
		hb_node*       right;
		signed char    balance;
		value_type     value;
		hb_node(const Key& key, const Val& val)
			: value(key, val)
		{
			parent = left = right = nullptr;
			balance = 0;
		}
	};
	typedef hb_node node_type;

	hb_map() noexcept
	{
		root = nullptr;
		count = 0;
	}
	~hb_map()
	{
		clear();
	}
	hb_map(const hb_map&) = delete;
	hb_map& operator=(const hb_map&) = delete;
	void swap(hb_map& other) noexcept
	{
		using std::swap;
		swap(root, other.root);
		swap(count, other.count);
	}
	hb_map(hb_map&& other) noexcept
		: hb_map()
	{
		swap(other);
	}
	hb_map& operator=(hb_map&& other) noexcept
	{
		swap(other);
		return *this;
	}
	std::size_t clear()
	{
		std::size_t old_count = count;

		hb_node* node = root;
		while (node) {
			if (node->left)
			{
				node = node->left;
				continue;
			}
			if (node->right)
			{
				node = node->right;
				continue;
			}

			hb_node* parent = node->parent;
			mylink(node) = nullptr;
			delete node;
			node = parent;
		}
		assert(!root);
		count = 0;
		return old_count;
	}

private:

	static bool isleft(const hb_node* node) { return node->parent->left == node; }

	hb_node*& mylink(hb_node* node)
	{
		if (!node->parent) return root;
		return isleft(node) ? node->parent->left : node->parent->right;
	}

	// old parent re link
	void oprl(hb_node* opar, hb_node* node, hb_node* newn)
	{
		if (!opar)
			root = newn;
		else
			if (opar->left == node)
				opar->left = newn;
			else
				opar->right = newn;
	}
	bool rotate_l(hb_node* q)
	{
		hb_node* qp = q->parent;
		hb_node* qr = q->right;

		assert(q->balance > 0);

		hb_node* qrl = qr->left;
		int qr_bal = qr->balance;

		q->parent = qr;
		q->balance = (qr_bal == 0) ? +1 : 0;
		q->right = qrl;
		if (qrl)
			qrl->parent = q;

		qr->parent = qp;
		qr->balance = (qr_bal == 0) ? -1 : 0;
		qr->left = q;
		oprl(qp, q, qr);

		return (qr_bal == 0);
	}

	bool rotate_r(hb_node* q)
	{
		hb_node* qp = q->parent;
		hb_node* ql = q->left;

		assert(q->balance < 0);

		hb_node* qlr = ql->right;
		const int ql_bal = ql->balance;

		q->parent = ql;
		q->balance = (ql_bal == 0) ? -1 : 0;
		q->left = qlr;
		if (qlr)
			qlr->parent = q;

		ql->parent = qp;
		ql->balance = (ql_bal == 0) ? +1 : 0;
		ql->right = q;
		oprl(qp, q, ql);

		return (ql_bal == 0);
	}

	void rotate_rl(hb_node* q)
	{
		hb_node* qp = q->parent;
		hb_node* qr = q->right;
		hb_node* qrl = qr->left;

		assert(q->balance > 0);
		assert(qr->balance < 0);

		int qrl_bal = qrl->balance;
		hb_node* qrll = qrl->left;
		hb_node* qrlr = qrl->right;

		qrl->parent = qp;
		qrl->balance = 0;
		oprl(qp, q, qrl);
		qrl->left = q;
		qrl->right = qr;

		q->parent = qrl;
		q->balance = (qrl_bal == +1) ? -1 : 0;
		q->right = qrll;
		if (qrll)
			qrll->parent = q;

		qr->parent = qrl;
		qr->balance = (qrl_bal == -1) ? +1 : 0;
		qr->left = qrlr;
		if (qrlr)
			qrlr->parent = qr;
	}

	void rotate_lr(hb_node* q)
	{
		hb_node* qp = q->parent;
		hb_node* ql = q->left;
		hb_node* qlr = ql->right;

		assert(q->balance < 0);
		assert(ql->balance > 0);

		int qlr_bal = qlr->balance;
		hb_node* qlrl = qlr->left;
		hb_node* qlrr = qlr->right;

		qlr->parent = qp;
		qlr->balance = 0;
		oprl(qp, q, qlr);
		qlr->left = ql;
		qlr->right = q;

		q->parent = qlr;
		q->balance = (qlr_bal == -1) ? +1 : 0;
		q->left = qlrr;
		if (qlrr)
			qlrr->parent = q;

		ql->parent = qlr;
		ql->balance = (qlr_bal == +1) ? -1 : 0;
		ql->right = qlrl;
		if (qlrl)
			qlrl->parent = ql;
	}

	static hb_node* node_prev(hb_node* node)
	{
		if (node->left)
			return node_max(node->left);
		hb_node* parent = node->parent;
		while (parent && parent->left == node) {
			node = parent;
			parent = parent->parent;
		}
		return parent;
	}

	static hb_node* node_next(hb_node* node)
	{
		if (node->right)
			return node_min(node->right);
		hb_node* parent = node->parent;
		while (parent && parent->right == node) {
			node = parent;
			parent = parent->parent;
		}
		return parent;
	}

	static hb_node* node_min(hb_node* node)
	{
		if (!node)
			return nullptr;
		while (node->left)
			node = node->left;
		return node;
	}

	static hb_node* node_max(hb_node* node)
	{
		if (!node)
			return nullptr;
		while (node->right)
			node = node->right;
		return node;
	}

	bool node_verify(
		const hb_node* parent,
		const hb_node* node,
		int* height,
		std::size_t *count) const
	{
		if (!parent) {
			if(root != node)
				return false;
		} else {
			if (parent->left == node) {
				if (node)
					if (!(node->value.first < parent->value.first))
						return false;
			} else {
				if(parent->right != node)
					return false;
				if (node)
					if (!(parent->value.first < node->value.first))
						return false;
			}
		}
		if (node) {
			int balance = node->balance;
			if (balance < -1) return false;
			if (balance > +1) return false;
			if (balance == -1) {
				if (node->left == nullptr) return false;
			} else if (balance == +1) {
				if (node->right == nullptr) return false;
			}
			if (node->parent != parent) return false;
			int lheight, rheight;
			if (!node_verify(node, node->left, &lheight, count) ||
				!node_verify(node, node->right, &rheight, count))
				return false;
			if (balance != (rheight - lheight))
				return false;
			if (height)
				*height = std::max(lheight, rheight) + 1;
			*count += 1;
		} else {
			if (height)
				*height = 0;
		}
		return true;
	}

	//void full_node_swap(hb_node* n1, hb_node* n2)
	//{
	//	assert(n1 && n2);
	//
	//	hb_node* n1p = n1->parent;
	//	hb_node* n1l = n1->left;
	//	hb_node* n1r = n1->right;
	//	int n1b = n1->balance;
	//
	//	hb_node* n2p = n2->parent;
	//	hb_node* n2l = n2->left;
	//	hb_node* n2r = n2->right;
	//	int n2b = n2->balance;
	//	
	//	if (n2p == n1)
	//	{
	//		if (n1l == n2) // left case
	//		{
	//			n1->parent = n2;
	//			n1->left = n2l;
	//			n1->right = n2r;
	//			n1->balance = n2b;
	//			if (n2l) n2l->parent = n1;
	//			if (n2r) n2r->parent = n1;
	//
	//			n2->right = n1r;
	//			if (n1r) n1r->parent = n2;
	//			oprl(n1p, n1, n2);
	//			n2->left = n1;
	//			n2->balance = n1b;
	//		}
	//		else if (n1r == n2) // left case
	//		{
	//			n1->parent = n2;
	//			n1->left = n2l;
	//			n1->right = n2r;
	//			n1->balance = n2b;
	//			if (n2l) n2l->parent = n1;
	//			if (n2r) n2r->parent = n1;
	//
	//			n2->right = n1;
	//			if (n1l) n1l->parent = n2;
	//			oprl(n1p, n1, n2);
	//			n2->left = n1l;
	//			n2->balance = n1b;
	//		}
	//		else
	//		{
	//			assert(false && "link error");
	//		}
	//	}
	//	else if (n1p == n2)
	//	{
	//		full_node_swap(n2, n1);
	//	}
	//	else
	//	{
	//		// not related
	//		n2->parent = n1p;
	//		oprl(n1p, n1, n2);
	//		n2->left = n1l;
	//		if (n1l) n1l->parent = n2;
	//		n2->right = n1r;
	//		if (n1r) n1r->parent = n2;
	//		n2->balance = n1b;
	//
	//		n1->parent = n2p;
	//		oprl(n2p, n2, n1);
	//		n1->left = n2l;
	//		if (n2l) n2l->parent = n1;
	//		n1->right = n2r;
	//		if (n2r) n2r->parent = n1;
	//		n1->balance = n2b;
	//	}
	//}

	void full_node_swap(hb_node* aP,hb_node* aQ)
	{
		hb_node* new_p_parent = aQ->parent;
		hb_node* new_p_left = aQ->left;
		hb_node* new_p_right = aQ->right;
		hb_node** new_p_link = &root;
		if (aQ->parent)
			new_p_link = aQ->parent->left == aQ ? &aQ->parent->left : &aQ->parent->right;

		hb_node* new_q_parent = aP->parent;
		hb_node* new_q_left = aP->left;
		hb_node* new_q_right = aP->right;
		hb_node** new_q_link = &root;
		if (aP->parent)
			new_q_link = aP->parent->left == aP ? &aP->parent->left : &aP->parent->right;

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
		if (aP->left)
			aP->left->parent = aP;
		aP->right = new_p_right;
		if (aP->right)
			aP->right->parent = aP;
		if (new_p_link)
			*new_p_link = aP;

		aQ->parent = new_q_parent;
		aQ->left = new_q_left;
		if (aQ->left)
			aQ->left->parent = aQ;
		aQ->right = new_q_right;
		if (aQ->right)
			aQ->right->parent = aQ;
		if (new_q_link)
			*new_q_link = aQ;

		std::swap(aP->balance, aQ->balance);
	}


public:

	bool verify() const
	{
		size_t cnt = 0;
		bool ok = node_verify(nullptr, root, nullptr, &cnt);
		if (count != cnt)
			ok = false;
		return ok;
	}

	typedef std::pair<bool, node_type*> insert_result_type;
	insert_result_type insert(const Key& key, const Val& val)
	{
		int cmp = 0;
		hb_node* node = root;
		hb_node* parent = nullptr;
		hb_node* q = nullptr;
		while (node) {
			if (key < node->value.first) {
				cmp = -1; parent = node; node = node->left;
			} else if (node->value.first < key) {
				cmp = +1; parent = node; node = node->right;
			} else
				return { false, node };
			if (parent->balance)
				q = parent;
		}

		hb_node* add = node = new hb_node(key, val);
		if (!node)
			return { false, nullptr };

		node->parent = parent;
		if (!parent) {
			assert(count == 0);
			assert(root == nullptr);
			root = node;
		} else {
			if (cmp < 0)
				parent->left = node;
			else
				parent->right = node;

			while (parent != q) {
				assert((parent->balance) == 0);
				if (parent->left == node)
					parent->balance = -1;
				else
					parent->balance = +1;
				node = parent;
				parent = parent->parent;
			}
			if (q) {
				assert(q->balance);
				if (q->left == node) {
					if (q->balance < 0) {
						if (q->left->balance > 0) {
							//tree->rotation_count += 2;
							rotate_lr(q);
						} else {
							//tree->rotation_count += 1;
							bool rotr = rotate_r(q); 
							assert(!rotr);
						}
					} else {
						assert(q->balance > 0);
						q->balance = 0;
					}
				} else {
					assert(q->right == node);
					if (q->balance > 0) {
						if (q->right->balance < 0) {
							//tree->rotation_count += 2;
							rotate_rl(q);
						} else {
							//tree->rotation_count += 1;
							bool rotl = rotate_l(q);
							assert(!rotl);
						}
					} else {
						assert(q->balance < 0);
						q->balance = 0;
					}
				}
			}
		}
		++count;
		return { true, add };
	}

	void remove_node(hb_node* node)
	{
		assert(node);
		if (node->left && node->right) {
			hb_node* out =
				(node->balance>0) ? node_min(node->right) : node_max(node->left);
			full_node_swap(node, out);
			//std::swap( node->value, out->value );
			//node = out;
		}

		hb_node* p = node->parent;
		hb_node* child = node->left ? node->left : node->right;
		delete node;
		--count;
		if (child)
			child->parent = p;
		if (!p) {
			root = child;
			return;
		}

		bool left = (p->left == node);
		if (left) {
			p->left = child;
		} else {
			assert(p->right == node);
			p->right = child;
		}
		node = child;

		unsigned rotations = 0;
		while (true) {
			if (left) {
				assert(p->left == node);
				if (p->balance > 0) {
					if (p->right->balance < 0) {
						rotations += 2;
						rotate_rl(p);
					} else {
						rotations += 1;
						if (rotate_l(p))
							break;
					}
					node = p->parent;
				} else if (p->balance < 0) {
					p->balance = 0;
					node = p;
				} else {
					assert(p->balance == 0);
					p->balance = 1;
					break;
				}
			} else {
				assert(p->right == node);
				if (p->balance < 0) {
					if (p->left->balance > 0) {
						rotations += 2;
						rotate_lr(p);
					} else {
						rotations += 1;
						if (rotate_r(p))
							break;
					}
					node = p->parent;
				} else if (p->balance > 0) {
					p->balance = 0;
					node = p;
				} else {
					assert((p->balance) == 0);
					p->balance = -1;
					break;
				}
			}

			p = node->parent;
			if (!p)
				break;
			if (p->left == node) {
				left = true;
			} else {
				assert(p->right == node);
				left = false;
			}
		}
		//tree->rotation_count += rotations;
	}

	hb_node* lookup(hb_node* from, const Key& key)
	{
		if (!from) return nullptr;
		if (key < from->value.first)
			return lookup(from->left, key);
		if (from->value.first < key)
			return lookup(from->right, key);
		return from;
	}

	bool remove(const Key& key)
	{
		hb_node* n = lookup(root, key);
		if (n) {
			remove_node(n);
			return true;
		}
		return false;
	}

	std::size_t size() const { return count; }

	void print(std::ostream& out)
	{
		unsigned i;
		prnt(out, root, i, 75);
		out << "\n";
	}

private:
	void prnt(std::ostream& out, hb_node* node, unsigned& at, unsigned w)
	{
		if (!node) return;
		prnt(out, node->left, at, w);
		std::stringstream ss;
		ss << node->value.first << ":" << node->value.second << " ";
		if ((at + ss.str().size()) > w)
		{
			out << std::endl;
			at = 0;
		}
		at += ss.str().size();
		out << ss.str();
		prnt(out, node->right, at, w);
	}

	hb_node* root;
	std::size_t count;
};





