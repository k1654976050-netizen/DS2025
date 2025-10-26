#include <iostream>
#include <cstdlib>
#include "stack.h"

using namespace std;

// 检查表达式是否有效
bool isValidExpression(const char* exp) {
    // 检查括号匹配
    if (!paren(exp, 0, strlen(exp) - 1)) {
        return false;
    }
    
    // 检查基本语法错误（简化版）
    int len = strlen(exp);
    for (int i = 0; i < len; i++) {
        char c = exp[i];
        if (!isdigit(c) && c != '+' && c != '-' && c != '*' && c != '/' 
            && c != '^' && c != '!' && c != '(' && c != ')' && c != '.') {
            return false; // 包含非法字符
        }
    }
    return true;
}

int main() {
    #ifdef _WIN32
    system("chcp 65001");
    #endif
    char expression[1024];
    char* rpn = (char*)malloc(sizeof(char) * 1);
    *rpn = '\0'; // 初始化逆波兰表达式字符串
    
    cout << "请输入表达式（支持 +, -, *, /, ^, !, (, ) 和小数）：" << endl;
    cin.getline(expression, 1024);
    
    // 验证表达式有效性
    if (!isValidExpression(expression)) {
        cout << "表达式无效！" << endl;
        free(rpn);
        return 1;
    }
    
    try {
        float result = evaluate(expression, rpn);
        cout << "逆波兰表达式: " << rpn << endl;
        cout << "计算结果: " << result << endl;
    } catch (...) {
        cout << "表达式计算错误！" << endl;
        free(rpn);
        return 1;
    }
    
    free(rpn);
    return 0;
}