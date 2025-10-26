#include "stack.h"
#include <iostream>
using namespace std;

int main() {
    #ifdef _WIN32
    system("chcp 65001");
    #endif

    cout << "=== 栈数据结构实现测试 ===" << endl;
    
    Stack<int> intStack;
    
    // 测试栈的基本操作
    cout << "1. 测试入栈操作:" << endl;
    for (int i = 1; i <= 5; i++) {
        intStack.push(i * 10);
        cout << "  入栈: " << i * 10 << ", 栈大小: " << intStack.size() << endl;
    }
    
    cout << "\n2. 测试查看栈顶操作:" << endl;
    cout << "  栈顶元素: " << intStack.top() << endl;
    
    cout << "\n3. 测试出栈操作:" << endl;
    while (!intStack.empty()) {
        cout << "  出栈: " << intStack.pop() << ", 剩余大小: " << intStack.size() << endl;
    }
    
    // 测试字符栈
    cout << "\n4. 测试字符栈:" << endl;
    Stack<char> charStack;
    
    // 将字符串"DS2025"入栈
    char str[] = "DS2025";
    cout << "  将字符串 \"" << str << "\" 的字符依次入栈:" << endl;
    for (int i = 0; str[i] != '\0'; i++) {
        charStack.push(str[i]);
        cout << "    入栈: " << str[i] << endl;
    }
    
    cout << "  依次出栈所有字符: ";
    while (!charStack.empty()) {
        cout << charStack.pop();
    }
    cout << endl;
    
    // 测试栈的其他功能
    cout << "\n5. 测试栈的其他功能:" << endl;
    Stack<double> doubleStack;
    
    // 入栈一些浮点数
    doubleStack.push(3.14);
    doubleStack.push(2.71);
    doubleStack.push(1.41);
    
    cout << "  栈大小: " << doubleStack.size() << endl;
    cout << "  是否为空: " << (doubleStack.empty() ? "是" : "否") << endl;
    
    // 清空栈
    cout << "  清空栈:" << endl;
    while (!doubleStack.empty()) {
        cout << "    弹出 " << doubleStack.pop() << endl;
    }
    cout << "  清空后是否为空: " << (doubleStack.empty() ? "是" : "否") << endl;
    
    cout << "\n=== 栈数据结构测试完成 ===" << endl;
    
    return 0;
}