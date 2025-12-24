#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "BinTree.h"
#include "vector.h"

// Huffman树节点结构
typedef struct HuffChar {
    char ch;       // 字符
    int weight;    // 权重（频率）
    
    HuffChar(char c = '^', int w = 0) : ch(c), weight(w) {}
    
    bool operator< (HuffChar const& hc) { return weight > hc.weight; }  // 优先队列需要的比较规则
    bool operator== (HuffChar const& hc) { return weight == hc.weight; }
} HuffChar;

// Huffman编码类
class HuffTree : public BinTree<HuffChar> {
public:
    HuffTree(HuffChar* hf) { 
        _root = new BinNode<HuffChar>(*hf); 
        _size = 1;
    }
    
    // 通过合并两棵Huffman树构造新的Huffman树
    HuffTree(HuffTree* t1, HuffTree* t2) {
        HuffChar hc('^', (t1->root() ? t1->root()->data.weight : 0) + (t2->root() ? t2->root()->data.weight : 0));
        _root = new BinNode<HuffChar>(hc);
        _size = 1;
        
        if (t1 && t1->_root) {
            _root->lchild = t1->_root;
            t1->_root->parent = _root;
            _size += t1->_size;
        }
        
        if (t2 && t2->_root) {
            _root->rchild = t2->_root;
            t2->_root->parent = _root;
            _size += t2->_size;
        }
        
        // 清理子树，防止被删除
        if (t1) {
            t1->_root = NULL;
            delete t1;
        }
        if (t2) {
            t2->_root = NULL;
            delete t2;
        }
    }
    
    // 获取权重
    int weight() { return _root ? _root->data.weight : 0; }
};

// 构造Huffman森林
Vector<HuffTree*>* generateForest(int* freq) {
    Vector<HuffTree*>* forest = new Vector<HuffTree*>();
    
    // 为每个有频率的字符创建单独的Huffman树
    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            char ch = 'a' + i;
            HuffChar* hc = new HuffChar(ch, freq[i]);
            HuffTree* tree = new HuffTree(hc);
            forest->insert(tree);
        }
    }
    
    return forest;
}

// 根据频率数组构建Huffman树
HuffTree* generateHuffTree(int* freq) {
    Vector<HuffTree*>* forest = generateForest(freq);
    
    // 使用简单选择排序方式模拟优先队列行为
    while (forest->size() > 1) {
        // 找到权重最小的两个树
        int min1 = 0, min2 = 1;
        if (forest->at(min1)->weight() > forest->at(min2)->weight()) 
            swap(min1, min2);
            
        for (int i = 2; i < forest->size(); i++) {
            int weight = forest->at(i)->weight();
            if (weight < forest->at(min1)->weight()) {
                min2 = min1;
                min1 = i;
            } else if (weight < forest->at(min2)->weight()) {
                min2 = i;
            }
        }
        
        // 取出权重最小的两个树
        HuffTree* tree1 = forest->remove(min1);
        // 注意:min2索引可能会因为删除min1而改变
        HuffTree* tree2 = forest->remove(min1 < min2 ? min2 - 1 : min2);
        
        // 合并成新树并加入森林
        HuffTree* newTree = new HuffTree(tree1, tree2);
        forest->insert(newTree);
    }
    
    HuffTree* huffTree = forest->at(0);
    delete forest;
    return huffTree;
}

#endif // HUFFTREE_H