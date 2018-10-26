
#pragma once

#include <utility>
#include <cstddef>
#include <cassert>

template<typename Key, typename Val>
class hb_map
{
public:
	typedef Key key_type;
	typedef Val mapped_type;
	typedef std::pair<const Key, Val> value_type;
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

	static bool isleft(const hb_node* node) { return node->parent->left == node;}
	hb_node*& mylink(hb_node* node)
	{
		if (!node->parent) return root;
		return isleft(node) ? node->left : node->right;
	}

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
		mylink(q) = qr;

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
		mylink(q) = ql;

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
		mylink(q) = qrl;
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
		mylink(q) = qlr;
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

public:
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

private:
	hb_node*    root;
	std::size_t count;
};





