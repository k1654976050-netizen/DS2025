#ifndef BINTREE_H
#define BINTREE_H

#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

#define BinNodePosi(T) BinNode<T>*
#define stature(p) ((p) ? (p)->height : -1)

typedef enum { RB_RED, RB_BLACK } RBColor;

template <typename T>
struct BinNode {
    T data;
    BinNodePosi(T) parent;
    BinNodePosi(T) lchild;
    BinNodePosi(T) rchild;
    int height;
    int npl;
    RBColor color;

    BinNode() : parent(NULL), lchild(NULL), rchild(NULL), height(0), npl(1), color(RB_RED) {}
    BinNode(T e, BinNodePosi(T) p = NULL, BinNodePosi(T) lc = NULL, BinNodePosi(T) rc = NULL, 
            int h = 0, int l = 1, RBColor c = RB_RED) 
        : data(e), parent(p), lchild(lc), rchild(rc), height(h), npl(l), color(c) {}

    int size();
    BinNodePosi(T) insertAsLC(T const& e);
    BinNodePosi(T) insertAsRC(T const& e);
    BinNodePosi(T) succ();
    template <typename VST> 
    void travIn(VST& visit) {
        if (lchild) lchild->travIn(visit);
        visit(data);
        if (rchild) rchild->travIn(visit);
    }
};

template <typename T>
class BinTree {
protected:
    int _size;
    BinNodePosi(T) _root;
    virtual int updateHeight(BinNodePosi(T) x);
    void updateHeightAbove(BinNodePosi(T) x);
    int removeAt(BinNodePosi(T) x); // 辅助函数：删除以x为根的子树
public:
    BinTree() : _size(0), _root(NULL) {}
    ~BinTree() { if (_root) removeAt(_root); _root = NULL; }
    
    int size() const { return _size; }
    bool empty() const { return !_root; }
    BinNodePosi(T) root() const { return _root; }
    BinNodePosi(T) insertAsRoot(T const& e);
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const& e);
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const& e);
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree*& S);
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree*& S);
    int remove(BinNodePosi(T) x);
    BinTree<T>* secede(BinNodePosi(T) x);
    template <typename VST>
    void travIn(VST& visit) { if (_root) _root->travIn(visit); }
};

template <typename T>
int BinNode<T>::size() {
    int s = 1;
    if (lchild) s += lchild->size();
    if (rchild) s += rchild->size();
    return s;
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const& e) {
    return lchild = new BinNode(e, this);
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const& e) {
    return rchild = new BinNode(e, this);
}

template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x) {
    return x->height = 1 + max(stature(x->lchild), stature(x->rchild));
}

template <typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi(T) x) {
    while (x) {
        updateHeight(x);
        x = x->parent;
    }
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(T const& e) {
    _size = 1;
    return _root = new BinNode<T>(e);
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, T const& e) {
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lchild;
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const& e) {
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rchild;
}

template <typename T>
BinNodePosi(T) BinTree<T>::attachAsLC(BinNodePosi(T) x, BinTree*& S) {
    if (x->lchild = S->_root) x->lchild->parent = x;
    _size += S->_size;
    updateHeightAbove(x);
    S->_root = NULL;
    S->_size = 0;
    delete S;
    S = NULL;
    return x;
}

template <typename T>
BinNodePosi(T) BinTree<T>::attachAsRC(BinNodePosi(T) x, BinTree*& S) {
    if (x->rchild = S->_root) x->rchild->parent = x;
    _size += S->_size;
    updateHeightAbove(x);
    S->_root = NULL;
    S->_size = 0;
    delete S;
    S = NULL;
    return x;
}

template <typename T>
int BinTree<T>::removeAt(BinNodePosi(T) x) {
    if (!x) return 0;
    int n = 1 + removeAt(x->lchild) + removeAt(x->rchild);
    delete x;
    _size -= n;
    return n;
}

template <typename T>
int BinTree<T>::remove(BinNodePosi(T) x) {
    if (!x) return 0;
    // 切断与父节点的连接
    if (x->parent) {
        if (x == x->parent->lchild) 
            x->parent->lchild = NULL;
        else 
            x->parent->rchild = NULL;
    } else { // 如果是根节点
        _root = NULL;
    }
    updateHeightAbove(x->parent);
    int n = removeAt(x);
    _size -= n;
    return n;
}

template <typename T>
BinTree<T>* BinTree<T>::secede(BinNodePosi(T) x) {
    if (!x) return NULL;
    // 切断与原树的连接
    if (x->parent) {
        if (x == x->parent->lchild) 
            x->parent->lchild = NULL;
        else 
            x->parent->rchild = NULL;
    }
    updateHeightAbove(x->parent);
    // 创建新树
    BinTree<T>* S = new BinTree<T>;
    S->_root = x;
    x->parent = NULL;
    S->_size = x->size();
    _size -= S->_size;
    return S;
}

#endif // BINTREE_H