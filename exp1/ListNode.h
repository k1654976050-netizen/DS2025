#include <cstddef>

typedef int Rank;
#define ListNodePosi(T) ListNode<T> *

template <typename T>
struct ListNode
{
    T data;  // 修复：应该是T类型而不是T*类型
    ListNodePosi(T) pred;
    ListNodePosi(T) succ;

    ListNode() {}
    ListNode(T e, ListNodePosi(T) p = NULL, ListNodePosi(T) s = NULL) : data(e), pred(p), succ(s) {}

    ListNodePosi(T) insertAsPred(T const &e);
    ListNodePosi(T) insertAsSucc(T const &e);
};