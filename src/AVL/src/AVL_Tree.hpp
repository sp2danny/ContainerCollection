
#pragma once

namespace AVL
{

typedef int T;

typedef unsigned sz_t;

struct Node;
typedef Node* NodeP;

struct Node
{
	NodeP parent, left, right;
	int weight, height;
	T item;
	int balance() const { return right->height - left->height; }
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

//----------------------------------------------------------------------------

// Tree_New --
//
//     Creates a new empty tree.
//
void Tree_New(Core& tree)
{
	Sentry* nil  = new Sentry;
	Sentry* root = new Sentry;
	tree.root = (Node*)root;
	tree.nil  = (Node*)nil;
	(*nil) = (*root) = Sentry{tree.nil,tree.nil,tree.nil,0,0};

}

void LinkR( NodeP par, NodeP r )
{
	par->right = r;
	r->parent = par;
}

void LinkL( NodeP par, NodeP l )
{
	par->left = l;
	l->parent = par;
}

bool isLeft( NodeP n )
{
	return n->parent->left == n;
}

bool isRight( NodeP n )
{
	return n->parent->right == n;
}


NodeP Node_New(Core&,const T&,NodeP);
void Tree_Balance (Core&, NodeP);
void Tree_Propagate(Core&, NodeP);

// Tree_Insert --
//
//     Insert new data in the tree, before where
//
void Tree_Insert(Core& tree, NodeP where, const T& data)
{

	if( where->left == tree.nil )
	{
		where->left = Node_New(tree, data, where);
		Tree_Balance(tree, where);
		return;
	}
	where = where->left;
	while( where->right != tree.nil )
		where = where->right;

	where->right = Node_New(tree, data, where);
	Tree_Balance(tree, where);

}

void Tree_Insert_Sorted(Core& tree, const T& data)
{
	NodeP node = tree.root;
	if (node->left == tree.nil)
	{
		node->left = Node_New(tree, data, node);
		Tree_Balance(tree, node);
		return;
	}
	node = node->left;
	while (true)
	{
		if (data < node->item)
		{
			if (node->left == tree.nil)
			{
				node->left = Node_New(tree, data, node);
				Tree_Balance(tree, node);
				return;
			}
			node = node->left;
		} else {
			if(node->right == tree.nil)
			{
				node->right = Node_New(tree, data, node);
				Tree_Balance(tree, node);
				return;
			}
			node = node->right;
		}
	}
}

NodeP Tree_NextNode(Core&,NodeP);

void UpdHW(NodeP);

// Tree_DeleteNode --
//
//     Removes a given node from the tree.
//
void Tree_DeleteNode(Core& tree, NodeP node)
{
	auto relink = [&](NodeP n1,NodeP n2)
	{
		if( isLeft(n1) )
			LinkL( n1->parent, n2 );
		else
			LinkR( n1->parent, n2 );
	};

	bool ln = node->left == tree.nil;
	bool rn = node->right == tree.nil;

	if(ln&&rn)
	{
		relink(node, tree.nil);
		Tree_Balance(tree, node->parent);
	}
	else if(ln)
	{
		relink(node, node->right);
		Tree_Balance(tree, node->parent);
	}
	else if(rn)
	{
		relink(node, node->left);
		Tree_Balance(tree, node->parent);
	}
	else
	{

		NodeP succ = Tree_NextNode(tree,node);

		assert( succ->left == tree.nil );

		if( succ->parent == node )
		{
			relink(node, succ);
			LinkL( succ, node->left );
			Tree_Balance(tree, succ);
		} else {
			LinkL( succ->parent, succ->right );
			Tree_Balance(tree, succ->parent);
			relink(node, succ);
			LinkL( succ, node->left );
			LinkR( succ, node->right );
		}
		UpdHW(succ);
	}

	delete node;
}

// Tree_SearchNode --
//
//     Searches the tree for a node containing the given data.
//     Only works if container is sorted (binary search)
//
NodeP Tree_SearchNode(Core& tree, const T& what)
{
	NodeP node = tree.root->left;

	while (node != tree.nil)
	{
		if (what < node->item)
			node = node->left;
		else if (node->item < what)
			node = node->right;
		else
			return node;
	}

	return tree.root;
}

NodeP Tree_NextNode(Core&, NodeP);

// Tree_SortedInsertPosition --
//
//     Searches the tree for earliest node larger than data
//
NodeP Tree_SortedInsertPosition(Core& tree, const T& data)
{
	NodeP node = tree.root->left;
	NodeP lwl = tree.root;

	if(node==tree.nil)
		return lwl;

	while(true)
	{
		if (data < node->item)
		{
			if (node->left == tree.nil)
				return node;
			lwl = node;
			node = node->left;
		} else {
			if (node->right == tree.nil)
				return lwl;
			node = node->right;
		}
	}
}

// Tree_FirstNode --
//
//     Returns the first node, or sentry.
//
NodeP Tree_FirstNode(Core& tree)
{
	NodeP node = tree.root->left;

	if (node == tree.nil)
		return tree.root;

	while (node->left != tree.nil)
		node = node->left;

	return node;
}

// Tree_LastNode --
//
//     Returns the node after the last. (end marker)
//
NodeP Tree_LastNode(Core& tree)
{
	return tree.root;
}

// Tree_PrevNode --
//
//     Returns the predecessor of the given node.
//
NodeP Tree_PrevNode(Core& tree, NodeP n)
{
	if (n == tree.root)
	{
		while(n->right != tree.nil)
			n = n->right;
		return n;
	}

	NodeP nTemp;

	if (n->left != tree.nil)
	{
		n = n->left;
		while (n->right != tree.nil)
			n = n->right;
	} else {
		nTemp = n;
		n = n->parent;
		while ((n != tree.nil) && (n->left == nTemp))
		{
			nTemp = n;
			n = n->parent;
		}
	}
	return n;
}

// Tree_NextNode --
//
//     Returns the follower of the given node.
//
NodeP Tree_NextNode (Core& tree, NodeP n)
{
	if (n==tree.root)
	{
		while (n->left != tree.nil)
			n = n->left;
		return n;
	}

	Node* nTemp;

	if (n->right != tree.nil)
	{
		n = n->right;
		while (n->left != tree.nil)
			n = n->left;
	} else {
		nTemp = n;
		n = n->parent;
		while ((n != tree.nil) && (n->right == nTemp))
		{
			nTemp = n;
			n = n->parent;
		}
	}

	return n;
}

// Node_GetData --
//
//     Returns the data in a node.
//
T& Node_GetData(NodeP n)
{
	return n->item;
}

//----------------------------------------------------------------------------
//
// Internal functions.
//

NodeP Tree_RotateRight     (Core&, NodeP);
NodeP Tree_RotateLeftRight (Core&, NodeP);
NodeP Tree_RotateLeft      (Core&, NodeP);
NodeP Tree_RotateRightLeft (Core&, NodeP);

void UpdHW(NodeP node)
{
	node->height = std::max( node->left->height, node->right->height ) + 1;
	node->weight = node->left->weight + node->right->weight + 1;
}

void Tree_Balance(Core& tree, NodeP node)
{
	while (node != tree.root)
	{
		UpdHW(node);

		int balance = node->balance();

		//assert( (balance >= -2) && (balance <= +2) );

		if (balance == -2)
		{
			int lb = node->left->balance();
			if (lb == -1)
				Tree_RotateRight (tree, node);
			else
				Tree_RotateLeftRight(tree, node);
		}
		else
		if (balance == +2)
		{
			int rb = node->right->balance();
			if (rb == +1)
				Tree_RotateLeft(tree, node);
			else
				Tree_RotateRightLeft(tree, node);
		}
		/* */

		node = node->parent;
		if(node == tree.nil) break;
	}
}

NodeP Tree_RotateLeft(Core& tree, NodeP node)
{
	NodeP right = node->right;
	NodeP rightLeft = right->left;
	NodeP parent = node->parent;

	if (isLeft(node))
		LinkL( parent, right );
	else
		LinkR( parent, right );

	LinkL( right, node );
	LinkR( node, rightLeft );

	UpdHW(node);
	UpdHW(right);

	return right;
}

NodeP Tree_RotateRight(Core& tree, NodeP node)
{
	NodeP left = node->left;
	NodeP leftRight = left->right;
	NodeP parent = node->parent;

	if (isLeft(node))
		LinkL( parent, left );
	else
		LinkR( parent, left );

	LinkR( left, node );
	LinkL( node, leftRight );

	UpdHW(node);
	UpdHW(left);

	return left;
}

NodeP Tree_RotateLeftRight(Core& tree, NodeP node)
{
	LinkL( node, Tree_RotateLeft(tree, node->left) );
	return Tree_RotateRight(tree, node);
}

NodeP Tree_RotateRightLeft(Core& tree, NodeP node)
{
	LinkR( node, Tree_RotateRight(tree, node->right) );
	return Tree_RotateLeft(tree, node);
}

sz_t Tree_Size(Core& tree)
{
	return (sz_t) tree.root->left->weight;
}

struct SR
{
	T* min;
	T* max;
	bool sorted;
};

SR is_sub_sorted( Core& tree, NodeP node )
{
	bool ln = node->left  == tree.nil;
	bool rn = node->right == tree.nil;

	SR result { &node->item, &node->item, true };

	if (ln&&rn)
	{
		return result;
	}

	if (!ln)
	{
		SR lft = is_sub_sorted(tree,node->left);
		if (!lft.sorted)
			result.sorted = false;
		if ( node->item < *lft.max )
			result.sorted = false;
		result.min = lft.min;
	}
	if (!rn)
	{
		SR rgt = is_sub_sorted(tree,node->right);
		if (!rgt.sorted)
			result.sorted = false;
		if ( *rgt.min < node->item )
			result.sorted = false;
		result.max = rgt.max;
	}

	return result;
}

bool Tree_Sorted(Core& tree)
{
	if(tree.root->left->weight <= 1) return true;
	return is_sub_sorted(tree,tree.root->left).sorted;
}

void Tree_Clear(Core& tree)
{
	static void (*rec_clr)(NodeP,NodeP)
	 = [](NodeP node,NodeP nil)
	{
		if(node==nil) return;
		rec_clr(node->left,nil);
		rec_clr(node->right,nil);
		delete node;
	};

	rec_clr(tree.root->left,tree.nil);
	tree.root->left = tree.nil;
}

void Tree_Delete(Core& tree)
{
	Tree_Clear(tree);
	delete (Sentry*) tree.root;
	delete (Sentry*) tree.nil;
	tree.root = tree.nil = nullptr;
}

NodeP Node_New(Core& tree, const T& data, NodeP parent)
{
	return new Node {
		parent,
		tree.nil,
		tree.nil,
		1,
		1,
		data
	};
}

NodeP Node_New(Core& tree, T&& data, NodeP parent)
{
	return new Node {
		parent,
		tree.nil,
		tree.nil,
		1,
		1,
		(T&&)data
	};
}

bool integrity(Core& tree, NodeP node)
{
	if( node == tree.nil ) return true;

	int lh = node->left->height;
	int rh = node->right->height;

	if( std::abs(lh-rh) > 1 ) return false;

	int h = std::max( lh, rh ) + 1;
	int w = node->weight = node->left->weight + node->right->weight + 1;

	if( h != node->height ) return false;
	if( w != node->weight ) return false;

	bool lrn = (node->left == tree.nil) && (node->right == tree.nil) ;

	if( (w==1) || (h==1) || lrn )
	{
		if( (w*h) != 1 ) return false;
		if( ! lrn ) return false;
	}

	if( node->left != tree.nil )
	{
		if( node->left->parent != node ) return false;
		if( ! integrity(tree,node->left) ) return false;
	}

	if( node->right != tree.nil )
	{
		if( node->right->parent != node ) return false;
		if( ! integrity(tree,node->right) ) return false;
	}

	return true;
}

bool integrity(Core& tree)
{
	if( ! tree.nil ) return false;
	if( tree.nil->height != 0 ) return false;
	if( tree.nil->weight != 0 ) return false;
	//if( tree.nil->parent != tree.nil ) return false;
	if( tree.nil->left != tree.nil ) return false;
	if( tree.nil->right != tree.nil ) return false;

	if( ! tree.root ) return false;
	if( tree.root->parent != tree.nil ) return false;
	if( tree.root->right != tree.nil ) return false;

	return integrity(tree, tree.root->left );
}

#include <iostream>
#include <string>
#include <iomanip>

struct Trunk {
	Trunk *prev;
	std::string str;
};


void print_trunks (std::ostream& out, Trunk *p)
{
	if (!p) {
		return;
	}
	print_trunks (out, p->prev);
	out << p->str;
}

void print_tree (Core& tree, std::ostream& out, Node* n, Trunk *prev, bool is_left)
{
	if (n == tree.nil)
		return;

	Trunk this_disp = { prev, "     " };
	std::string prev_str = this_disp.str;
	print_tree (tree, out, n->right, &this_disp, true);

	if (!prev) {
		this_disp.str = "---";
	} else if (is_left) {
		this_disp.str = ".--";
		prev_str = "    |";
	} else {
		this_disp.str = "`--";
		prev->str = prev_str;
	}

	print_trunks (out, &this_disp);
	out << " " << n->item;
	out << " (" << std::showpos << n->balance() << std::noshowpos << "," << n->weight << "," << n->height << ")\n";

	if (prev) {
		prev->str = prev_str;
	}
	this_disp.str = "    |";

	print_tree (tree, out, n->left, &this_disp, false);
	if (!prev) {
		out << ("");
	}
}

void print_tree (Core& tree, std::ostream& out)
{
	print_tree( tree, out, tree.root->left, nullptr, true );
}
































}



