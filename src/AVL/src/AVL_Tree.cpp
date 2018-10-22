
#include "stdafx.h"

#include "AVL_Tree.h"

//
// Private datatypes
//

struct Tree {
	Node* root;
	int (*comp) (void *, void *);
	void (*print) (void *);
};

struct Node {
	Node* parent;
	Node* left;
	Node* right;
	void *data;
	int balance;
};

struct trunk {
	struct trunk *prev;
	char *str;
};

//
// Declaration of private functions.
//
void  Tree_InsertBalance    (Tree* t, Node* node, int balance);
void  Tree_DeleteBalance    (Tree* t, Node* node, int balance);
Node* Tree_RotateLeft       (Tree* t, Node* node);
Node* Tree_RotateRight      (Tree* t, Node* node);
Node* Tree_RotateLeftRight  (Tree* t, Node* node);
Node* Tree_RotateRightLeft  (Tree* t, Node* node);
void  Tree_Replace          (Node* target, Node* source);

Node* Node_New              (void *data, Node* parent);

void  print_tree            (Tree* t, Node* n, struct trunk *prev, int is_left);

//----------------------------------------------------------------------------

// Tree_New --
//
//     Creates a new tree using the parameters 'comp' and 'print' for
//     comparing and printing the data in the nodes.
//
Tree* Tree_New (int (*comp)(void *, void *), void (*print)(void *)) {
	Tree* t;

	t = (Tree*) malloc (sizeof (Tree));
	t->comp = comp;
	t->print = print;
	t->root = nullptr;

	return t;
}

// Tree_Insert --
//
//     Insert new data in the tree. If the data is already in the tree,
//     nothing will be done.
//
void Tree_Insert (Tree* t, void *data) {
	if (t->root == nullptr) {
		t->root = Node_New (data, nullptr);
	} else {
		Node* node = t->root;
		while (node != nullptr) {
			if ((t->comp) (data, node->data) < 0) {
				Node* left = node->left;
				if (left == nullptr) {
					node->left = Node_New (data, node);
					Tree_InsertBalance (t, node, -1);
					return;
				} else {
					node = left;
				}
			} else if ((t->comp) (data, node->data) > 0) {
				Node* right = node->right;
				if (right == nullptr) {
					node->right = Node_New (data, node);
					Tree_InsertBalance (t, node, 1);
					return;
				} else {
					node = right;
				}
			} else {
				node->data = data;
				return;
			}
		}
	}
}

// Tree_DeleteNode --
//
//     Removes a given node from the tree.
//
void Tree_DeleteNode (Tree* t, Node* node) {
	Node* left = node->left;
	Node* right = node->right;
	Node* toDelete = node;

	if (left == nullptr) {
		if (right == nullptr) {
			if (node == t->root) {
				t->root = nullptr;
			} else {
				Node* parent = node->parent;
				if (parent->left == node) {
					parent->left = nullptr;
					Tree_DeleteBalance (t, parent, 1);
				} else {
					parent->right = nullptr;
					Tree_DeleteBalance (t, parent, -1);
				}
			}
		} else {
			Tree_Replace (node, right);
			Tree_DeleteBalance (t, node, 0);
			toDelete = right;
		}
	} else if (right == nullptr) {
		Tree_Replace (node, left);
		Tree_DeleteBalance (t, node, 0);
		toDelete = left;
	} else {
		Node* successor = right;
		if (successor->left == nullptr) {
			Node* parent = node->parent;
			successor->parent = parent;
			successor->left = left;
			successor->balance = node->balance;

			if (left != nullptr) {
				left->parent = successor;
			}
			if (node == t->root) {
				t->root = successor;
			} else {
				if (parent->left == node) {
					parent->left = successor;
				} else {
					parent->right = successor;
				}
			}
			Tree_DeleteBalance (t, successor, -1);
		} else {
			while (successor->left != nullptr) {
				successor = successor->left;
			}
			Node* parent = node->parent;
			Node* successorParent = successor->parent;
			Node* successorRight = successor->right;

			if (successorParent->left == successor) {
				successorParent->left = successorRight;
			} else {
				successorParent->right = successorRight;
			}

			if (successorRight != nullptr) {
				successorRight->parent = successorParent;
			}

			successor->parent = parent;
			successor->left = left;
			successor->balance = node->balance;
			successor->right = right;
			right->parent = successor;

			if (left != nullptr) {
				left->parent = successor;
			}

			if (node == t->root) {
				t->root = successor;
			} else {
				if (parent->left == node) {
					parent->left = successor;
				} else {
					parent->right = successor;
				}
			}
			Tree_DeleteBalance (t, successorParent, 1);
		}
	}

	free (toDelete);
}

// Tree_SearchNode --
//
//     Searches the tree for a node containing the given data.
//
Node* Tree_SearchNode (Tree* t, void *data)
{
	Node* node = t->root;

	while (node != nullptr) {
		if ((t->comp) (data, node->data) < 0) {
			node = node->left;
		} else if ((t->comp) (data, node->data) > 0) {
			node = node->right;
		} else {
			return node;
		}
	}

	return nullptr;
}

// Tree_Print --
//
//     Prints an ASCII representation of the tree on screen.
//
void Tree_Print (Tree* t) {
	print_tree (t, t->root, 0, 0);
	fflush (stdout);
}

// Tree_FirstNode --
//
//     Returns the node containing the smallest key.
//
Node* Tree_FirstNode (Tree* t) {
	Node* node = t->root;

	while ((node != nullptr) && (node->left != nullptr)) {
		node = node->left;
	}

	return node;
}

// Tree_LastNode --
//
//     Returns the node containing the biggest key.
//
Node* Tree_LastNode (Tree* t) {
	Node* node = t->root;

	while ((node != nullptr) && (node->right != nullptr)) {
		node = node->right;
	}

	return node;
}

// Tree_PrevNode --
//
//     Returns the predecessor of the given node.
//
Node* Tree_PrevNode (Tree* t, Node* n) {
	Node* nTemp;

	if (n->left != nullptr) {
		n = n->left;
		while (n->right != nullptr) {
			n = n->right;
		}
	} else {
		nTemp = n;
		n = n->parent;
		while ((n != nullptr) && (n->left == nTemp)) {
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
Node* Tree_NextNode (Tree* t, Node* n) {
	Node* nTemp;

	if (n->right != nullptr) {
		n = n->right;
		while (n->left != nullptr) {
			n = n->left;
		}
	} else {
		nTemp = n;
		n = n->parent;
		while ((n != nullptr) && (n->right == nTemp)) {
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
void *Node_GetData (Node* n) {
	return n->data;
}

//----------------------------------------------------------------------------
//
// Internal functions.
//

void Tree_InsertBalance (Tree* t, Node* node, int balance) {
	while (node != nullptr) {
		balance = (node->balance += balance);
		if (balance == 0) {
			return;
		} else if (balance == -2) {
			if (node->left->balance == -1) {
				Tree_RotateRight (t, node);
			} else {
				Tree_RotateLeftRight (t, node);
			}
			return;
		} else if (balance == 2) {
			if (node->right->balance == 1) {
				Tree_RotateLeft (t, node);
			} else {
				Tree_RotateRightLeft (t, node);
			}
			return;
		}
		Node* parent = node->parent;
		if (parent != nullptr) {
			balance = (parent->left == node) ? -1 : 1;
		}
		node = parent;
	}
}

void Tree_DeleteBalance (Tree* t, Node* node, int balance) {
	while (node != nullptr) {
		balance = (node->balance += balance);

		if (balance == -2) {
			if (node->left->balance <= 0) {
				node = Tree_RotateRight (t, node);

				if (node->balance == 1) {
					return;
				}
			} else {
				node = Tree_RotateLeftRight (t, node);
			}
		} else if (balance == 2) {
			if (node->right->balance >= 0) {
				node = Tree_RotateLeft (t, node);

				if (node->balance == -1) {
					return;
				}
			} else {
				node = Tree_RotateRightLeft (t, node);
			}
		} else if (balance != 0) {
			return;
		}

		Node* parent = node->parent;

		if (parent != nullptr) {
			balance = (parent->left == node) ? 1 : -1;
		}

		node = parent;
	}
}

void Tree_Replace (Node* target, Node* source) {
	Node* left = source->left;
	Node* right = source->right;

	target->balance = source->balance;
	target->data = source->data;
	target->left = left;
	target->right = right;

	if (left != nullptr) {
		left->parent = target;
	}

	if (right != nullptr) {
		right->parent = target;
	}
}

Node* Tree_RotateLeft (Tree* t, Node* node) {
	Node* right = node->right;
	Node* rightLeft = right->left;
	Node* parent = node->parent;

	right->parent = parent;
	right->left = node;
	node->right = rightLeft;
	node->parent = right;

	if (rightLeft != nullptr) {
		rightLeft->parent = node;
	}

	if (node == t->root) {
		t->root = right;
	} else if (parent->right == node) {
		parent->right = right;
	} else {
		parent->left = right;
	}

	right->balance--;
	node->balance = -right->balance;

	return right;
}

Node* Tree_RotateRight (Tree* t, Node* node) {
	Node* left = node->left;
	Node* leftRight = left->right;
	Node* parent = node->parent;

	left->parent = parent;
	left->right = node;
	node->left = leftRight;
	node->parent = left;

	if (leftRight != nullptr) {
		leftRight->parent = node;
	}

	if (node == t->root) {
		t->root = left;
	} else if (parent->left == node) {
		parent->left = left;
	} else {
		parent->right = left;
	}

	left->balance++;
	node->balance = -left->balance;

	return left;
}

Node* Tree_RotateLeftRight (Tree* t, Node* node) {
	Node* left = node->left;
	Node* leftRight = left->right;
	Node* parent = node->parent;
	Node* leftRightRight = leftRight->right;
	Node* leftRightLeft = leftRight->left;

	leftRight->parent = parent;
	node->left = leftRightRight;
	left->right = leftRightLeft;
	leftRight->left = left;
	leftRight->right = node;
	left->parent = leftRight;
	node->parent = leftRight;

	if (leftRightRight != nullptr) {
		leftRightRight->parent = node;
	}

	if (leftRightLeft != nullptr) {
		leftRightLeft->parent = left;
	}

	if (node == t->root) {
		t->root = leftRight;
	} else if (parent->left == node) {
		parent->left = leftRight;
	} else {
		parent->right = leftRight;
	}

	if (leftRight->balance == 1) {
		node->balance = 0;
		left->balance = -1;
	} else if (leftRight->balance == 0) {
		node->balance = 0;
		left->balance = 0;
	} else {
		node->balance = 1;
		left->balance = 0;
	}

	leftRight->balance = 0;

	return leftRight;
}

Node* Tree_RotateRightLeft (Tree* t, Node* node) {
	Node* right = node->right;
	Node* rightLeft = right->left;
	Node* parent = node->parent;
	Node* rightLeftLeft = rightLeft->left;
	Node* rightLeftRight = rightLeft->right;

	rightLeft->parent = parent;
	node->right = rightLeftLeft;
	right->left = rightLeftRight;
	rightLeft->right = right;
	rightLeft->left = node;
	right->parent = rightLeft;
	node->parent = rightLeft;

	if (rightLeftLeft != nullptr) {
		rightLeftLeft->parent = node;
	}

	if (rightLeftRight != nullptr) {
		rightLeftRight->parent = right;
	}

	if (node == t->root) {
		t->root = rightLeft;
	} else if (parent->right == node) {
		parent->right = rightLeft;
	} else {
		parent->left = rightLeft;
	}

	if (rightLeft->balance == -1) {
		node->balance = 0;
		right->balance = 1;
	} else if (rightLeft->balance == 0) {
		node->balance = 0;
		right->balance = 0;
	} else {
		node->balance = -1;
		right->balance = 0;
	}

	rightLeft->balance = 0;

	return rightLeft;
}

void print_trunks (struct trunk *p) {
	if (!p) {
		return;
	}
	print_trunks (p->prev);
	printf ("%s", p->str);
}

void print_tree (Tree* t, Node* n, struct trunk *prev, int is_left) {
	if (n == nullptr) {
		return;
	}

	struct trunk this_disp = { prev, "     " };
	char *prev_str = this_disp.str;
	print_tree (t, n->right, &this_disp, 1);

	if (!prev) {
		this_disp.str = "---";
	} else if (is_left) {
		this_disp.str = ".--";
		prev_str = "    |";
	} else {
		this_disp.str = "`--";
		prev->str = prev_str;
	}

	print_trunks (&this_disp);
	(t->print) (n->data);
	printf (" (%+d)\n", n->balance);

	if (prev) {
		prev->str = prev_str;
	}
	this_disp.str = "    |";

	print_tree (t, n->left, &this_disp, 0);
	if (!prev) {
		puts ("");
	}
}

Node* Node_New (void *data, Node* parent)
{
	Node* n;

	n = (Node*) malloc( sizeof(Node) );
	n->parent = parent;
	n->left = nullptr;
	n->right = nullptr;
	n->data = data;
	n->balance = 0;

	return n;
}

