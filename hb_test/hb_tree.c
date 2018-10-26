
#include "pch.h"

/*
 * libdict -- height-balanced (AVL) tree implementation.
 *
 * Copyright (c) 2001-2014, Farooq Mela
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hb_tree.h"

#include "dict_private.h"
#include "tree_common.h"

typedef struct hb_node hb_node;

struct hb_node {
	void*          key;
	void*          datum;
	hb_node*       parent;
	hb_node*       left;
	hb_node*       right;
	signed char    balance;
};

#define PARENT(node)      ((node)->parent)
#define BAL_POS(node)     ((node)->balance > 0)
#define BAL_NEG(node)     ((node)->balance < 0)
#define BAL_ZERO(node)    ((node)->balance == 0)

struct hb_tree {
	TREE_FIELDS(hb_node);
};

struct hb_itor {
	TREE_ITERATOR_FIELDS(hb_tree, hb_node);
};

static const dict_vtable hb_tree_vtable = {
	true,
	(dict_inew_func)       hb_dict_itor_new,
	(dict_dfree_func)      hb_tree_free,
	(dict_insert_func)     hb_tree_insert,
	(dict_search_func)     tree_search,
	(dict_search_func)     tree_search_le,
	(dict_search_func)     tree_search_lt,
	(dict_search_func)     tree_search_ge,
	(dict_search_func)     tree_search_gt,
	(dict_remove_func)     hb_tree_remove,
	(dict_clear_func)      hb_tree_clear,
	(dict_traverse_func)   hb_tree_traverse,
	(dict_select_func)     hb_tree_select,
	(dict_count_func)      tree_count,
	(dict_verify_func)     hb_tree_verify,
};

static const itor_vtable hb_tree_itor_vtable = {
	(dict_ifree_func)       tree_iterator_free,
	(dict_valid_func)       tree_iterator_valid,
	(dict_invalidate_func)  tree_iterator_invalidate,
	(dict_next_func)        hb_itor_next,
	(dict_prev_func)        hb_itor_prev,
	(dict_nextn_func)       hb_itor_nextn,
	(dict_prevn_func)       hb_itor_prevn,
	(dict_first_func)       tree_iterator_first,
	(dict_last_func)        tree_iterator_last,
	(dict_key_func)         tree_iterator_key,
	(dict_datum_func)       tree_iterator_datum,
	(dict_isearch_func)     tree_iterator_search,
	(dict_isearch_func)     tree_iterator_search_le,
	(dict_isearch_func)     tree_iterator_search_lt,
	(dict_isearch_func)     tree_iterator_search_ge,
	(dict_isearch_func)     tree_iterator_search_gt,
	(dict_iremove_func)     hb_itor_remove,
	(dict_icompare_func)    tree_iterator_compare,
};

static hb_node* node_prev(hb_node* node);
static hb_node* node_next(hb_node* node);
static hb_node* node_new(void* key);
static bool     node_verify(const hb_tree* tree, const hb_node* parent, const hb_node* node,
							unsigned* height, size_t *count);

hb_tree*
hb_tree_new(dict_compare_func cmp_func)
{
	ASSERT(cmp_func != NULL);

	hb_tree* tree = MALLOC(sizeof(*tree));
	if (tree) {
		tree->root = NULL;
		tree->count = 0;
		tree->cmp_func = cmp_func;
		tree->rotation_count = 0;
	}
	return tree;
}

dict*
hb_dict_new(dict_compare_func cmp_func)
{
	dict* dct = MALLOC(sizeof(*dct));
	if (dct) {
		if (!(dct->_object = hb_tree_new(cmp_func))) {
			FREE(dct);
			return NULL;
		}
		dct->_vtable = &hb_tree_vtable;
	}
	return dct;
}

size_t
hb_tree_free(hb_tree* tree, dict_delete_func delete_func) {
	const size_t count = hb_tree_clear(tree, delete_func);
	FREE(tree);
	return count;
}

size_t
hb_tree_clear(hb_tree* tree, dict_delete_func delete_func)
{
	const size_t count = tree->count;

	hb_node* node = tree->root;
	while (node) {
		if (node->left || node->right) {
			node = node->left ? node->left : node->right;
			continue;
		}

		if (delete_func)
			delete_func(node->key, node->datum);

		hb_node* const parent = PARENT(node);
		FREE(node);
		*(parent ? (parent->left == node ? &parent->left : &parent->right) : &tree->root) = NULL;
		node = parent;
	}
	ASSERT(tree->root == NULL);
	tree->count = 0;
	return count;
}

/* L: rotate |q| left */
static bool
rotate_l(hb_tree* restrict const tree, hb_node* restrict const q)
{
	hb_node* restrict const qp = PARENT(q);
	hb_node* restrict const qr = q->right;

	ASSERT(BAL_POS(q));

	hb_node *restrict const qrl = qr->left;
	const int qr_bal = qr->balance;

	/* q->parent <- qr; q->balance <- (qr_bal == 0); */
	q->parent = qr;
	q->balance = (qr_bal == 0) ? +1 : 0;
	//set_par_bal(q, (intptr_t)qr | (qr_bal == 0) );
	if ((q->right = qrl) != NULL)
		qrl->parent = q; //, (intptr_t)q | (qrl->balance & BAL_MASK); /* qrl->parent <- q; */

	/* qr->parent <- qp; qr->balance <- -(qr_bal == 0); */
	//set_par_bal(qr, (intptr_t)qp | ((qr_bal == 0) << 1) );
	qr->parent = qp;
	qr->balance = (qr_bal == 0) ? -1 : 0;
	qr->left = q;
	*(qp == NULL ? &tree->root : qp->left == q ? &qp->left : &qp->right) = qr;

	return (qr_bal == 0);
}

/* R: rotate |q| right. */
static bool
rotate_r(hb_tree* restrict const tree, hb_node* restrict const q)
{
	hb_node* restrict const qp = PARENT(q);
	hb_node* restrict const ql = q->left;

	ASSERT(BAL_NEG(q));

	hb_node* restrict const qlr = ql->right;
	const int ql_bal = ql->balance;// & BAL_MASK;

	/* q->parent <- ql; q->balance <- -(ql_bal == 0); */
	//set_par_bal(q, (intptr_t)ql | ((ql_bal == 0) << 1) );
	q->parent = ql;
	q->balance = (ql_bal == 0) ? -1 : 0;
	if ((q->left = qlr) != NULL)
		qlr->parent = q;
		//qlr->balance = (intptr_t)q | (qlr->balance & BAL_MASK); /* qlr->parent <- q; */

	/* ql->parent <- qp; ql->balance <- (ql_bal == 0); */
	//set_par_bal(ql, (intptr_t)qp | (ql_bal == 0) );
	ql->parent = qp;
	ql->balance = (ql_bal == 0) ? +1 : 0;
	ql->right = q;
	*(qp == NULL ? &tree->root : qp->left == q ? &qp->left : &qp->right) = ql;

	return (ql_bal == 0);
}

static void
rotate_rl(hb_tree* restrict const tree, hb_node* restrict const q)
{
	hb_node* restrict const qp = PARENT(q);
	hb_node* restrict const qr = q->right;
	hb_node* restrict const qrl = qr->left;

	ASSERT(BAL_POS(q));
	ASSERT(BAL_NEG(qr));

	const int qrl_bal = qrl->balance;
	hb_node* restrict const qrll = qrl->left;
	hb_node* restrict const qrlr = qrl->right;

	qrl->parent = qp;
	qrl->balance = 0;
	*(qp == NULL ? &tree->root : qp->left == q ? &qp->left : &qp->right) = qrl;
	qrl->left = q;
	qrl->right = qr;

	q->parent = qrl;
	q->balance = (qrl_bal == +1) ? -1 : 0;
	if ((q->right = qrll) != NULL)
		qrll->parent = q;

	qr->parent = qrl;
	qr->balance = (qrl_bal == -1) ? +1 : 0;
	if ((qr->left = qrlr) != NULL)
		qrlr->parent = qr;
}

static void
rotate_lr(hb_tree* restrict const tree, hb_node* restrict const q)
{
	hb_node* restrict const qp = PARENT(q);
	hb_node* restrict const ql = q->left;
	hb_node* restrict const qlr = ql->right;

	ASSERT(BAL_NEG(q));
	ASSERT(BAL_POS(ql));

	const int qlr_bal = qlr->balance;
	hb_node* restrict const qlrl = qlr->left;
	hb_node* restrict const qlrr = qlr->right;

	qlr->parent = qp;
	qlr->balance = 0;
	*(qp == NULL ? &tree->root : qp->left == q ? &qp->left : &qp->right) = qlr;
	qlr->left = ql;
	qlr->right = q;

	q->parent = qlr;
	q->balance = (qlr_bal == -1) ? +1 : 0;
	if ((q->left = qlrr) != NULL)
		qlrr->parent = q;

	ql->parent = qlr;
	ql->balance = (qlr_bal == +1) ? -1 : 0;
	if ((ql->right = qlrl) != NULL)
		qlrl->parent = ql;
}

dict_insert_result
hb_tree_insert(hb_tree* tree, void* key)
{
	int cmp = 0;
	hb_node* node = tree->root;
	hb_node* parent = NULL;
	hb_node* q = NULL;
	while (node) {
		cmp = tree->cmp_func(key, node->key);
		if (cmp < 0) {
			parent = node; node = node->left;
		} else if (cmp > 0) {
			parent = node; node = node->right;
		} else
			return (dict_insert_result) { &node->datum, false };
		if (parent->balance)
			q = parent;
	}

	hb_node* const add = node = node_new(key);
	if (!node)
		return (dict_insert_result) { NULL, false };

	if (!(node->parent = parent)) {
		ASSERT(tree->count == 0);
		ASSERT(tree->root == NULL);
		tree->root = node;
	} else {
		if (cmp < 0)
			parent->left = node;
		else
			parent->right = node;

		while (parent != q) {
			ASSERT((parent->balance) == 0);
			if (parent->left == node)
				parent->balance = -1;
			else
				parent->balance = +1;
			node = parent;
			parent = PARENT(parent);
		}
		if (q) {
			ASSERT(q->balance);
			if (q->left == node) {
				if (BAL_NEG(q)) {
					if (BAL_POS(q->left)) {
						tree->rotation_count += 2;
						rotate_lr(tree, q);
					} else {
						tree->rotation_count += 1;
						ASSERT(!rotate_r(tree, q));
					}
				} else {
					ASSERT(BAL_POS(q));
					q->balance = 0;
				}
			} else {
				ASSERT(q->right == node);
				if (BAL_POS(q)) {
					if (BAL_NEG(q->right)) {
						tree->rotation_count += 2;
						rotate_rl(tree, q);
					} else {
						tree->rotation_count += 1;
						ASSERT(!rotate_l(tree, q));
					}
				} else {
					ASSERT(BAL_NEG(q));
					q->balance = 0;
				}
			}
		}
	}
	tree->count++;
	return (dict_insert_result) { &add->datum, true };
}

void** hb_tree_search(hb_tree* tree, const void* key) { return tree_search(tree, key); }
void** hb_tree_search_le(hb_tree* tree, const void* key) { return tree_search_le(tree, key); }
void** hb_tree_search_lt(hb_tree* tree, const void* key) { return tree_search_lt(tree, key); }
void** hb_tree_search_ge(hb_tree* tree, const void* key) { return tree_search_ge(tree, key); }
void** hb_tree_search_gt(hb_tree* tree, const void* key) { return tree_search_gt(tree, key); }

static void
remove_node(hb_tree* tree, hb_node* node)
{
	if (node->left && node->right) {
		hb_node* restrict out =
			BAL_POS(node) ? tree_node_min(node->right) : tree_node_max(node->left);
		void* tmp;
		SWAP(node->key, out->key, tmp);
		SWAP(node->datum, out->datum, tmp);
		node = out;
	}

	hb_node* p = PARENT(node);
	hb_node* child = node->left ? node->left : node->right;
	FREE(node);
	tree->count--;
	if (child)
		child->parent = p;
	if (!p) {
		tree->root = child;
		return;
	}

	bool left = (p->left == node);
	if (left) {
		p->left = child;
	} else {
		ASSERT(p->right == node);
		p->right = child;
	}
	node = child;

	unsigned rotations = 0;
	for (;;) {
		if (left) {
			ASSERT(p->left == node);
			if (BAL_POS(p)) {
				if (BAL_NEG(p->right)) {
					rotations += 2;
					rotate_rl(tree, p);
				} else {
					rotations += 1;
					if (rotate_l(tree, p))
						break;
				}
				node = PARENT(p);
			} else if (BAL_NEG(p)) {
				p->balance = 0;
				node = p;
			} else {
				ASSERT((p->balance ) == 0);
				p->balance = 1;
				break;
			}
		} else {
			ASSERT(p->right == node);
			if (BAL_NEG(p)) {
				if (BAL_POS(p->left)) {
					rotations += 2;
					rotate_lr(tree, p);
				} else {
					rotations += 1;
					if (rotate_r(tree, p))
						break;
				}
				node = PARENT(p);
			} else if (BAL_POS(p)) {
				p->balance = 0;
				node = p;
			} else {
				ASSERT((p->balance) == 0);
				p->balance = -1;
				break;
			}
		}

		if (!(p = PARENT(node)))
			break;
		if (p->left == node) {
			left = true;
		} else {
			ASSERT(p->right == node);
			left = false;
		}
	}
	tree->rotation_count += rotations;
}

dict_remove_result
hb_tree_remove(hb_tree* tree, const void* key)
{
	hb_node* node = tree_search_node(tree, key);
	if (!node)
		return (dict_remove_result) { NULL, NULL, false };
	const dict_remove_result result = { node->key, node->datum, true };
	remove_node(tree, node);
	return result;
}

size_t
hb_tree_traverse(hb_tree* tree, dict_visit_func visit, void* user_data)
{
	return tree_traverse(tree, visit, user_data);
}

bool
hb_tree_select(hb_tree *tree, size_t n, const void **key, void **datum)
{
	if (n >= tree->count) {
		if (key)
			*key = NULL;
		if (datum)
			*datum = NULL;
		return false;
	}
	hb_node* node;
	if (n >= tree->count / 2) {
		node = tree_node_max(tree->root);
		n = tree->count - 1 - n;
		while (n--)
			node = node_prev(node);
	} else {
		node = tree_node_min(tree->root);
		while (n--)
			node = node_next(node);
	}
	if (key)
		*key = node->key;
	if (datum)
		*datum = node->datum;
	return true;
}

size_t hb_tree_count(const hb_tree* tree) { return tree_count(tree); }
size_t hb_tree_min_path_length(const hb_tree* tree) { return tree_min_path_length(tree); }
size_t hb_tree_max_path_length(const hb_tree* tree) { return tree_max_path_length(tree); }
size_t hb_tree_total_path_length(const hb_tree* tree) { return tree_total_path_length(tree); }

static hb_node*
node_new(void* key)
{
	hb_node* node = MALLOC(sizeof(*node));
	if (node) {
		//ASSERT((((intptr_t)node) & 3) == 0); /* Ensure malloc returns aligned result. */
		node->key = key;
		node->datum = NULL;
		node->parent = NULL;
		node->balance = 0; /* also initializes parent to NULL */
		node->left = NULL;
		node->right = NULL;
	}
	return node;
}

static hb_node*
node_prev(hb_node* node)
{
	if (node->left)
		return tree_node_max(node->left);
	hb_node* parent = PARENT(node);
	while (parent && parent->left == node) {
		node = parent;
		parent = PARENT(parent);
	}
	return parent;
}

static hb_node*
node_next(hb_node* node)
{
	if (node->right)
		return tree_node_min(node->right);
	hb_node* parent = PARENT(node);
	while (parent && parent->right == node) {
		node = parent;
		parent = PARENT(parent);
	}
	return parent;
}

static bool
node_verify(const hb_tree* tree, const hb_node* parent, const hb_node* node,
			unsigned* height, size_t *count)
{
	if (!parent) {
		VERIFY(tree->root == node);
	} else {
		if (parent->left == node) {
			if (node)
				VERIFY(tree->cmp_func(parent->key, node->key) > 0);
		} else {
			ASSERT(parent->right == node);
			if (node)
				VERIFY(tree->cmp_func(parent->key, node->key) < 0);
		}
	}
	if (node) {
		int balance = node->balance;// & BAL_MASK;
		VERIFY(balance >= -1);
		VERIFY(balance <= +1);
		if (balance == -1) {
			VERIFY(node->left != NULL);
			//balance = -1;
		} else if (balance == 1) {
			VERIFY(node->right != NULL);
		}
		VERIFY(PARENT(node) == parent);
		unsigned lheight, rheight;
		if (!node_verify(tree, node, node->left, &lheight, count) ||
			!node_verify(tree, node, node->right, &rheight, count))
			return false;
		VERIFY(balance == (int)rheight - (int)lheight);
		if (height)
			*height = MAX(lheight, rheight) + 1;
		*count += 1;
	} else {
		if (height)
			*height = 0;
	}
	return true;
}

bool
hb_tree_verify(const hb_tree* tree)
{
	size_t count = 0;
	bool verified = node_verify(tree, NULL, tree->root, NULL, &count);
	VERIFY(tree->count == count);
	return verified;
}

hb_itor*
hb_itor_new(hb_tree* tree)
{
	hb_itor* itor = MALLOC(sizeof(*itor));
	if (itor) {
		itor->tree = tree;
		itor->node = NULL;
	}
	return itor;
}

dict_itor*
hb_dict_itor_new(hb_tree* tree)
{
	dict_itor* itor = MALLOC(sizeof(*itor));
	if (itor) {
		if (!(itor->_itor = hb_itor_new(tree))) {
			FREE(itor);
			return NULL;
		}
		itor->_vtable = &hb_tree_itor_vtable;
	}
	return itor;
}

void hb_itor_free(hb_itor* itor) { tree_iterator_free(itor); }
bool hb_itor_valid(const hb_itor* itor) { return tree_iterator_valid(itor); }
void hb_itor_invalidate(hb_itor* itor) { tree_iterator_invalidate(itor); }

bool hb_itor_next(hb_itor* itor) {
	if (itor->node)
		itor->node = node_next(itor->node);
	return itor->node != NULL;
}

bool hb_itor_prev(hb_itor* itor) {
	if (itor->node)
		itor->node = node_prev(itor->node);
	return itor->node != NULL;
}

bool hb_itor_nextn(hb_itor* itor, size_t count) {
	while (itor->node && count--)
		itor->node = node_next(itor->node);
	return itor->node != NULL;
}

bool hb_itor_prevn(hb_itor* itor, size_t count) {
	while (itor->node && count--)
		itor->node = node_prev(itor->node);
	return itor->node != NULL;
}

bool hb_itor_first(hb_itor* itor) { return tree_iterator_first(itor); }
bool hb_itor_last(hb_itor* itor) { return tree_iterator_last(itor); }
bool hb_itor_search(hb_itor* itor, const void* key) { return tree_iterator_search_ge(itor, key); }
bool hb_itor_search_le(hb_itor* itor, const void* key) { return tree_iterator_search_le(itor, key); }
bool hb_itor_search_lt(hb_itor* itor, const void* key) { return tree_iterator_search_lt(itor, key); }
bool hb_itor_search_ge(hb_itor* itor, const void* key) { return tree_iterator_search_ge(itor, key); }
bool hb_itor_search_gt(hb_itor* itor, const void* key) { return tree_iterator_search_gt(itor, key); }
int hb_itor_compare(const hb_itor* i1, const hb_itor* i2) { return tree_iterator_compare(i1, i2); }
const void* hb_itor_key(const hb_itor* itor) { return tree_iterator_key(itor); }
void** hb_itor_datum(hb_itor* itor) { return tree_iterator_datum(itor); }

bool
hb_itor_remove(hb_itor* itor)
{
	if (!itor->node)
		return false;
	remove_node(itor->tree, itor->node);
	itor->node = NULL;
	return true;
}
