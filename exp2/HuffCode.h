#ifndef HUFFCODE_H
#define HUFFCODE_H

#include "HuffTree.h"
#include "Bitmap.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

// Huffman编码表
class HuffTable {
private:
    char** table;  // 存储每个字符对应的编码字符串
    int* lengths;  // 存储每个编码的长度
    
public:
    HuffTable() {
        table = new char*[26];
        lengths = new int[26];
        for (int i = 0; i < 26; i++) {
            table[i] = NULL;
            lengths[i] = 0;
        }
    }
    
    ~HuffTable() {
        for (int i = 0; i < 26; i++) {
            if (table[i]) delete[] table[i];
        }
        delete[] table;
        delete[] lengths;
    }
    
    // 获取字符对应的编码
    char* getCode(char ch) {
        if (ch >= 'a' && ch <= 'z') {
            return table[ch - 'a'];
        } else if (ch >= 'A' && ch <= 'Z') {
            return table[ch - 'A'];
        }
        return NULL;
    }
    
    // 获取编码长度
    int getCodeLength(char ch) {
        if (ch >= 'a' && ch <= 'z') {
            return lengths[ch - 'a'];
        } else if (ch >= 'A' && ch <= 'Z') {
            return lengths[ch - 'A'];
        }
        return 0;
    }
    
    // 设置字符对应的编码
    void setCode(char ch, char* code, int len) {
        int index;
        if (ch >= 'a' && ch <= 'z') {
            index = ch - 'a';
        } else if (ch >= 'A' && ch <= 'Z') {
            index = ch - 'A';
        } else {
            return;
        }
        
        if (table[index]) delete[] table[index];
        table[index] = new char[len + 1];
        strcpy(table[index], code);
        lengths[index] = len;
    }
};

// 递归生成Huffman编码表
void generateTable(BinNodePosi(HuffChar) node, char* code, int depth, HuffTable& table) {
    if (!node) return;
    
    // 如果是叶节点，保存编码
    if (!node->lchild && !node->rchild) {
        code[depth] = '\0';
        table.setCode(node->data.ch, code, depth);
        return;
    }
    
    // 向左遍历，编码添加'0'
    if (node->lchild) {
        code[depth] = '0';
        generateTable(node->lchild, code, depth + 1, table);
    }
    
    // 向右遍历，编码添加'1'
    if (node->rchild) {
        code[depth] = '1';
        generateTable(node->rchild, code, depth + 1, table);
    }
}

// 根据Huffman树生成编码表
HuffTable* generateHuffTable(HuffTree* tree) {
    HuffTable* table = new HuffTable();
    char code[100];  // 假设最大深度不超过100
    
    if (tree && tree->root()) {
        generateTable(tree->root(), code, 0, *table);
    }
    
    return table;
}

// 使用Huffman编码对文本进行编码
Bitmap* encodeText(const string& text, HuffTable* table, int& totalBits) {
    // 首先计算总位数
    totalBits = 0;
    for (char ch : text) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            totalBits += table->getCodeLength(ch);
        }
    }
    
    // 创建足够大的位图
    Bitmap* bmp = new Bitmap(totalBits > 0 ? totalBits : 8);
    
    // 对每个字符进行编码
    int bitIndex = 0;
    for (char ch : text) {
        char* code = NULL;
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            code = table->getCode(ch);
        }
        
        if (code) {
            // 将编码写入位图
            for (int i = 0; code[i] != '\0'; i++) {
                if (code[i] == '1') {
                    bmp->set(bitIndex);
                }
                bitIndex++;
            }
        }
    }
    
    return bmp;
}

// 解码位图数据
string decodeBitmap(Bitmap* bmp, int totalBits, HuffTree* tree) {
    string result = "";
    BinNodePosi(HuffChar) current = tree->root();
    
    for (int i = 0; i < totalBits; i++) {
        // 根据位值选择左右子树
        if (bmp->test(i)) {
            current = current->rchild;
        } else {
            current = current->lchild;
        }
        
        // 如果到达叶节点，输出字符并回到根节点
        if (!current->lchild && !current->rchild) {
            result += current->data.ch;
            current = tree->root();
        }
    }
    
    return result;
}

#endif // HUFFCODE_H