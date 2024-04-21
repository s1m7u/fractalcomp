#include "stree.h"
#include <algorithm>

struct node_point {
        int pstart;
        int pend;
        node_point(stree_t *st) {
                pstart = st->start;
                pend = st->end;
        }
        ~node_point() {}
};

static stree_t *jumpdown(stree_t *node, node_point beta, const u8 *T) {
        stree_t *current = node;
        int i = 0;
        while (beta.pstart <= beta.pend) {
                i = 0;
                while (T[current->children[i]->start] != T[beta.pstart]) {
                        ++i;
                }
                current = current->children[i];
                beta.pstart = beta.pstart + (current->end - current->start) + 1;
        }
        if (beta.pstart == beta.pend + 1) {
                return current;
        } else {
                stree_t *w = new stree_t();
                w->start = current->start;
                w->end = current->end - (beta.pstart - beta.pend) + 1;
                w->parent = current->parent;
                current->parent->children[i] = w;
                current->parent = w;
                current->start = w->end + 1;
                w->children[0] = current;
                return w;
        }
}

static stree_t *walkdown(stree_t *node, node_point &gamma, int &p,
                         const u8 *T) {
        stree_t *current = node;
        stree_t *return_node = NULL;
        while (gamma.pstart <= gamma.pend) {
                int i = 0;
                while (current->children[i] != NULL &&
                       T[current->children[i]->start] != T[gamma.pstart]) {
                        ++i;
                }
                if (!current->children[i]) {
                        p = i;
                        return_node = current;
                        break;
                } else {

                        int intermediate =
                            gamma.pstart + (current->children[i]->end -
                                            current->children[i]->start);
                        int dist = 0;
                        while (T[current->children[i]->start + dist] ==
                                   T[gamma.pstart] &&
                               gamma.pstart <=
                                   std::min(intermediate, gamma.pend)) {
                                ++dist;
                                ++(gamma.pstart);
                        }
                        if (gamma.pstart > std::min(intermediate, gamma.pend)) {
                                current = current->children[i];
                        } else {
                                stree_t *new_node_v = new stree_t();
                                new_node_v->start = current->children[i]->start;
                                new_node_v->end =
                                    current->children[i]->start + dist - 1;
                                current->children[i]->start =
                                    current->children[i]->start + dist;
                                current->children[i]->parent = new_node_v;
                                new_node_v->children[0] = current->children[i];
                                new_node_v->parent = current;
                                p = 1;
                                current->children[i] = new_node_v;
                                return_node = new_node_v;
                                break;
                        }
                }
        }
        return return_node;
}

void stree_build(stree_t *root, const u8 *data, size_t n) {
        *root = {};
        root->parent = root;
        root->suffix = root;
        stree_t *leafnode0 = new stree_t();
        root->children[0] = leafnode0;
        leafnode0->parent = root;
        leafnode0->start = 0;
        leafnode0->end = n;

        stree_t *node_v_j;
        node_v_j = root;
        stree_t *w;

        node_point beta = node_point(root);
        node_point gamma = node_point(leafnode0);

        for (int j = 1; j <= n; ++j) {

                stree_t *current_leafnode = new stree_t();
                current_leafnode->start = j;
                current_leafnode->end = n;

                int I = 1;
                if (node_v_j == root) {
                        ++gamma.pstart;
                        I = 0;
                } else if (node_v_j->parent == root) {
                        if (beta.pstart == beta.pend) {
                                I = 0;
                        } else {
                                ++beta.pstart;
                        }
                }

                if (I == 0) {
                        int p = 0;
                        stree_t *new_node_v;
                        new_node_v = walkdown(root, gamma, p, data);
                        new_node_v->children[p] = current_leafnode;
                        current_leafnode->parent = new_node_v;
                        current_leafnode->start = gamma.pstart;
                        if (beta.pstart == beta.pend && beta.pstart != -1) {
                                node_v_j->suffix = root;
                        }

                } else {
                        w = jumpdown(node_v_j->parent->suffix, beta, data);
                        node_v_j->suffix = w;

                        if (!w->children[1]) {
                                w->children[1] = current_leafnode;
                                current_leafnode->start = gamma.pstart;
                                current_leafnode->parent = w;
                        } else {
                                int p = 0;
                                stree_t *new_node_v;
                                new_node_v = walkdown(w, gamma, p, data);
                                new_node_v->children[p] = current_leafnode;
                                current_leafnode->parent = new_node_v;
                                current_leafnode->start = gamma.pstart;
                        }
                }

                gamma = node_point(current_leafnode);
                beta = node_point(current_leafnode->parent);
                node_v_j = current_leafnode->parent;
        }
}

// does not delete itself
void stree_burn(stree_t *st) {
        if (st == NULL)
                return;
        for (int i = 0; i < (1 << 8); i++) {
                if (st->children[i]) {
                        stree_burn(st->children[i]);
                        delete st->children[i];
                }
        }
}
