#include <iostream>
#include <string>
#include <cctype>
#include <windows.h>
#include "HuffTree.h"
#include "HuffCode.h"
using namespace std;

// "I have a dream"演讲的部分内容
const char* SPEECH_TEXT = "I have a dream that one day this nation will rise up and live out the true meaning of its creed We hold these truths to be self evident that all men are created equal I have a dream that one day on the red hills of Georgia the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood I have a dream that one day even the state of Mississippi a state sweltering with the heat of injustice sweltering with the heat of oppression will be transformed into an oasis of freedom and justice";

// 统计字符频率
void countFrequency(const char* text, int* freq) {
    for (int i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        if (ch >= 'A' && ch <= 'Z') {
            freq[ch - 'A']++;
        } else if (ch >= 'a' && ch <= 'z') {
            freq[ch - 'a']++;
        }
    }
}

// 打印字符频率统计
void printFrequency(int* freq) {
    cout << "字符频率统计:" << endl;
    for (int i = 0; i < 26; i++) {
        if (freq[i] > 0) {
            cout << (char)('a' + i) << ": " << freq[i] << endl;
        }
    }
    cout << endl;
}

// 打印Huffman编码表
void printHuffTable(HuffTable* table) {
    cout << "Huffman编码表:" << endl;
    for (int i = 0; i < 26; i++) {
        char ch = 'a' + i;
        char* code = table->getCode(ch);
        if (code) {
            cout << ch << ": " << code << endl;
        }
    }
    cout << endl;
}

int main() {
    // 设置控制台代码页为UTF-8以正确显示中文
    SetConsoleOutputCP(CP_UTF8);
    
    // 初始化频率数组
    int freq[26] = {0};
    
    // 统计文本中各字符频率
    countFrequency(SPEECH_TEXT, freq);
    
    // 打印字符频率
    printFrequency(freq);
    
    // 构建Huffman树
    cout << "正在构建Huffman树..." << endl;
    HuffTree* huffTree = generateHuffTree(freq);
    cout << "Huffman树构建完成!" << endl << endl;
    
    // 生成Huffman编码表
    cout << "正在生成Huffman编码表..." << endl;
    HuffTable* huffTable = generateHuffTable(huffTree);
    cout << "Huffman编码表生成完成!" << endl << endl;
    
    // 打印编码表
    printHuffTable(huffTable);
    
    // 对"dream"进行编码
    string dream = "dream";
    int dreamBits;
    cout << "对单词 \"" << dream << "\" 进行编码:" << endl;
    Bitmap* dreamCode = encodeText(dream, huffTable, dreamBits);
    char* dreamCodeStr = new char[dreamBits + 1];
    for (int i = 0; i < dreamBits; i++) {
        dreamCodeStr[i] = dreamCode->test(i) ? '1' : '0';
    }
    dreamCodeStr[dreamBits] = '\0';
    cout << "编码结果: " << dreamCodeStr << " (" << dreamBits << " 位)" << endl << endl;
    
    // 对其他单词进行编码
    string words[] = {"freedom", "justice", "equality"};
    int wordCount = sizeof(words) / sizeof(words[0]);
    
    cout << "对其他单词进行编码:" << endl;
    for (int i = 0; i < wordCount; i++) {
        int bits;
        Bitmap* code = encodeText(words[i], huffTable, bits);
        char* codeStr = new char[bits + 1];
        for (int j = 0; j < bits; j++) {
            codeStr[j] = code->test(j) ? '1' : '0';
        }
        codeStr[bits] = '\0';
        cout << words[i] << ": " << codeStr << " (" << bits << " 位)" << endl;
        delete[] codeStr;
        delete code;
    }
    cout << endl;
    
    // 对整个演讲片段进行编码
    int speechBits;
    cout << "对演讲片段进行编码..." << endl;
    Bitmap* speechCode = encodeText(SPEECH_TEXT, huffTable, speechBits);
    cout << "演讲片段编码完成，共 " << speechBits << " 位" << endl;
    
    // 解码验证
    cout << "解码验证..." << endl;
    string decodedSpeech = decodeBitmap(speechCode, speechBits, huffTree);
    
    // 转换原始文本为小写以便比较，只保留字母
    string lowerSpeech = "";
    for (int i = 0; SPEECH_TEXT[i] != '\0'; i++) {
        if (isalpha(SPEECH_TEXT[i])) {
            lowerSpeech += tolower(SPEECH_TEXT[i]);
        }
    }
    
    // 提取解码结果中的字母
    string decodedLetters = "";
    for (char c : decodedSpeech) {
        if (isalpha(c)) {
            decodedLetters += c;
        }
    }
    
    if (lowerSpeech == decodedLetters) {
        cout << "解码成功！解码结果与原始文本匹配。" << endl;
    } else {
        cout << "解码失败！解码结果与原始文本不匹配。" << endl;
        cout << "原始文本前50个字符: " << lowerSpeech.substr(0, 50) << "..." << endl;
        cout << "解码结果前50个字符: " << decodedLetters.substr(0, 50) << "..." << endl;
    }
    
    // 清理内存
    delete[] dreamCodeStr;
    delete dreamCode;
    delete speechCode;
    delete huffTable;
    delete huffTree;
    
    return 0;
}