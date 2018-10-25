
#pragma once

#include <cassert>
#include <algorithm>
#include <utility>

template<typename T>
class hb_tree
{
public:

	struct sentry_tag {};
	struct payload_tag {};

	struct hb_node {
		int height;
		int weight;
		hb_node* pptr;
		hb_node* llink;
		hb_node* rlink;
		union {
			T data;
			char dummy;
		};
		hb_node(sentry_tag)
			: dummy(0)
		{
			weight = height = 0;
		}
		template<typename... Args>
		hb_node(payload_tag, Args&&... args)
			: data(std::forward<Args>(args)...)
		{
			weight = height = 1;
		}
		~hb_node()
		{
			if (weight||height)
				data.~T();
		}
	};

	bool sentry(const hb_node* n) const
	{
		assert(n);
		if (n->height || n->weight)
		{
			assert(n->height && n->weight);
			return false;
		}
		assert(n->dummy==0);
		return true;
	}

	hb_tree()
	{
		root = new hb_node(sentry_tag{});
		nil = new hb_node(sentry_tag{});
		root->pptr  = nil;
		root->llink = nil;
		root->rlink = nil;
		nil->pptr  = nil;
		nil->llink = nil;
		nil->rlink = nil;
		count = 0;
		rotation_count = 0;
    }
	~hb_tree()
	{
		hb_tree_clear();
		assert(sentry(root));
		assert(sentry(nil));
		delete root;
		delete nil;
	}

	static bool isleft(const hb_node* n)
	{
		return n->pptr->llink == n;
	}
	static bool isright(const hb_node* n)
	{
		return n->pptr->rlink == n;
	}

	void link_left(hb_node* par, hb_node* lft)
	{
		par->llink = lft;
		lft->pptr = par;
	}
	void link_right(hb_node* par, hb_node* rgt)
	{
		par->rlink = rgt;
		rgt->pptr = par;
	}

	void par_relink(hb_node* n1, hb_node* n2)
	{
		if (isleft(n1))
			link_left(n1->pptr, n2);
		else
			link_right(n1->pptr, n2);
	}

	void hb_tree_clear()
	{
		hb_node* node = root;
		while (true)
		{
			if (node->llink != nil)
			{
				node = node->llink;
				continue;
			}
			if (node->rlink != nil)
			{
				node = node->rlink;
				continue;
			}

			hb_node* parent = node->pptr;
			if (parent != root)
			{
				bool isleft = (parent->llink == node);
				assert(!sentry(node));
				delete node;
				if (isleft)
					parent->llink = nil;
				else
					parent->rlink = nil;
				node = parent;
			} else {
				assert(!sentry(node));
				delete node;
				root->llink = nil;
				break;
			}
		}
		count = 0;
	}
	
	int balance(const hb_node* q) const
	{
		assert(q);
		assert(q != nil);
		assert(q != root);
		assert(q->rlink);
		assert(q->llink);
		int rh = q->rlink->height;
		int lh = q->llink->height;
		return rh-lh;
	}
	void update(hb_node* q)
	{
		assert(q);
		assert(q != nil);
		assert(q != root);
		assert(q->rlink);
		assert(q->llink);
		int rh = q->rlink->height;
		int lh = q->llink->height;
		int newh = std::max(rh, lh) + 1;
		bool didchange = (newh != q->height);
		q->height = newh;
		int rw = q->rlink->weight;
		int lw = q->llink->weight;
		int neww = rw + lw + 1;
		didchange = didchange || (neww != q->weight);
		q->weight = neww;
		if (didchange && (q->pptr != root))
			update(q->pptr);
	}

	bool rotate_l(hb_node* q)
	{
		assert(q);
		assert(q != nil);
		assert(q != root);

		hb_node* qr = q->rlink;

		assert(balance(q) > 0);

		hb_node* qrl = qr->llink;
		int old_qr_bal = balance(qr);

		assert(qr != nil);
		q->pptr = qr;
		link_right(q, qrl);
		qr->llink = q;
		par_relink(q, qr);

		update(qr);

		return (old_qr_bal == 0);
	}

	bool rotate_r(hb_node* q)
	{
		assert(q);
		assert(q != nil);
		assert(q != root);

		hb_node* qp = q->pptr;
		hb_node* ql = q->llink;

		assert(balance(q) < 0);

		hb_node* qlr = ql->rlink;
		int old_ql_bal = balance(ql);

		q->pptr = ql;
		q->llink = qlr;
		if (qlr)
			qlr->pptr = q;
		ql->pptr = qp;
		ql->rlink = q;
		par_relink(q, ql);

		update(ql);

		return (old_ql_bal == 0);
	}

	void rotate_rl(hb_node* q)
	{
		assert(q);
		assert(q != nil);
		assert(q != root);

		hb_node* qp = q->pptr;
		hb_node* qr = q->rlink;
		hb_node* qrl = qr->llink;

		assert(balance(q) > 0);
		assert(balance(qr) < 0);

		hb_node* qrll = qrl->llink;
		hb_node* qrlr = qrl->rlink;

		/* qrl->parent <- qp; qrl->bal <- 0; */
		qrl->pptr = qp;
		par_relink(q, qrl);
		qrl->llink = q;
		qrl->rlink = qr;

		/* q->parent <- qrll; q->bal <- -(qrl_bal == 1); */
		q->pptr = qrl;
		q->rlink = qrll;
		if (qrll != nil)
			qrll->pptr = q;

		/* qr->parent <- qrl; qr->bal <- (qrl_bal == -1); */
		qr->pptr = qrl;
		qr->llink = qrlr;
		if (qrlr != nil)
			qrlr->pptr = qr;
		
		update(qrll);
		update(qrlr);
	}


private:
	hb_node* root;
	hb_node* nil;
	std::size_t count;
	std::size_t rotation_count;

};



