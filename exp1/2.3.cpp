#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include "../MySLQ/stack.h"

using namespace std;

#define OP_ADD     "+"     // 加法
#define OP_SUB     "-"     // 减法
#define OP_MUL     "*"     // 乘法
#define OP_DIV     "/"     // 除法
#define OP_POW     "^"     // 幂运算
#define OP_FAC     "!"     // 阶乘
#define OP_LP      "("     // 左括号
#define OP_RP      ")"     // 右括号
#define OP_SIN     "sin"   // 正弦函数
#define OP_COS     "cos"   // 余弦函数
#define OP_TAN     "tan"   // 正切函数
#define OP_LOG     "log"   // 常用对数
#define OP_LN      "ln"    // 自然对数
#define OP_SQRT    "sqrt"  // 平方根
#define OP_EOE     ""      // 结束符

#define N_EXT_OPTR 15

// 扩展的运算符优先级表
const char ext_pri[N_EXT_OPTR][N_EXT_OPTR] = {
    // 当前运算符
    //    +    -    *    /    ^    !    (    )   sin  cos  tan  log   ln  sqrt  \0
    /* + */ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /* - */ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /* * */ '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /* / */ '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /* ^ */ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /* ! */ '>', '>', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>',
    /* ( */ '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', ' ',
    /* ) */ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    /*sin*/ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*cos*/ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*tan*/ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*log*/ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*ln */ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*sqrt*/'>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>',
    /*\0 */ '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', '<', '<', '<', '='
};

// 函数名列表
const char* functions[] = {OP_SIN, OP_COS, OP_TAN, OP_LOG, OP_LN, OP_SQRT};
const int NUM_FUNCTIONS = 6;

// 获取运算符对应的索引
int getOperatorIndex(const char* op) {
    if (strcmp(op, OP_ADD) == 0)  return 0;
    if (strcmp(op, OP_SUB) == 0)  return 1;
    if (strcmp(op, OP_MUL) == 0)  return 2;
    if (strcmp(op, OP_DIV) == 0)  return 3;
    if (strcmp(op, OP_POW) == 0)  return 4;
    if (strcmp(op, OP_FAC) == 0)  return 5;
    if (strcmp(op, OP_LP) == 0)   return 6;
    if (strcmp(op, OP_RP) == 0)   return 7;
    if (strcmp(op, OP_SIN) == 0)  return 8;
    if (strcmp(op, OP_COS) == 0)  return 9;
    if (strcmp(op, OP_TAN) == 0)  return 10;
    if (strcmp(op, OP_LOG) == 0)  return 11;
    if (strcmp(op, OP_LN) == 0)   return 12;
    if (strcmp(op, OP_SQRT) == 0) return 13;
    if (strcmp(op, OP_EOE) == 0)  return 14;
    return 14; // 默认返回结束符索引
}

// 判断两个运算符的优先级关系
char ext_orderBetween(const char* op1, const char* op2) {
    return ext_pri[getOperatorIndex(op1)][getOperatorIndex(op2)];
}

// 检查是否为函数名
bool isFunction(const char* expr, int pos) {
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        int len = strlen(functions[i]);
        if (strncmp(expr + pos, functions[i], len) == 0 && 
            !isalpha(expr[pos + len])) {  // 确保后面不是字母
            return true;
        }
    }
    return false;
}

// 获取函数名
void getFunction(const char* expr, int pos, char* func) {
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        int len = strlen(functions[i]);
        if (strncmp(expr + pos, functions[i], len) == 0) {
            strncpy(func, functions[i], len);
            func[len] = '\0';
            return;
        }
    }
    func[0] = '\0';
}


float ext_calcu(float opnd, const char* op) {
    // 处理一元运算符
    if (strcmp(op, OP_FAC) == 0) {
        if (opnd < 0 || opnd != (int)opnd) {
            cout << "错误：阶乘操作数必须是非负整数" << endl;
            exit(-1);
        }
        int n = (int)opnd;
        float result = 1;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }
        return result;
    }
    
    // 处理函数
    if (strcmp(op, OP_SIN) == 0) return sin(opnd);
    if (strcmp(op, OP_COS) == 0) return cos(opnd);
    if (strcmp(op, OP_TAN) == 0) return tan(opnd);
    if (strcmp(op, OP_LOG) == 0) {
        if (opnd <= 0) {
            cout << "错误：对数的参数必须大于0" << endl;
            exit(-1);
        }
        return log10(opnd);
    }
    if (strcmp(op, OP_LN) == 0) {
        if (opnd <= 0) {
            cout << "错误：自然对数的参数必须大于0" << endl;
            exit(-1);
        }
        return log(opnd);
    }
    if (strcmp(op, OP_SQRT) == 0) {
        if (opnd < 0) {
            cout << "错误：平方根的参数不能为负数" << endl;
            exit(-1);
        }
        return sqrt(opnd);
    }
    
    exit(-1);
}

float ext_calcu(float op1, const char* op, float op2) {
    if (strlen(op) == 1) {
        return calcu(op1, op[0], op2);
    }
    exit(-1);
}

float evaluate(char* S) {
    Stack<float> opnd;   
    Stack<char*> optr;  
    
    char* null_op = new char[1];
    null_op[0] = '\0';
    optr.push(null_op);
    
    while (true) {
        while (*S == ' ') S++;
        
        if (*S == '\0') {
            char* end_op = new char[1];
            end_op[0] = '\0';
            while (strcmp(optr.top(), end_op) != 0) {
                char* op = optr.pop();
                if (strcmp(op, OP_FAC) == 0 || 
                    strcmp(op, OP_SIN) == 0 || strcmp(op, OP_COS) == 0 || 
                    strcmp(op, OP_TAN) == 0 || strcmp(op, OP_LOG) == 0 || 
                    strcmp(op, OP_LN) == 0 || strcmp(op, OP_SQRT) == 0) {
                    float opn = opnd.pop();
                    opnd.push(ext_calcu(opn, op));
                } else if (strlen(op) == 1) {
                    float opn2 = opnd.pop();
                    float opn1 = opnd.pop();
                    opnd.push(ext_calcu(opn1, op, opn2));
                }
                delete[] op;
            }
            delete[] end_op;
            break;
        }
        
        if (isdigit(*S)) {
            readNumber(S, opnd);
        } 
        else if (isFunction(S, 0)) {
            char func[10];
            getFunction(S, 0, func);
            int func_len = strlen(func);
            char* func_copy = new char[func_len + 1];
            strcpy(func_copy, func);
            optr.push(func_copy);
            S += func_len;
        }

        else {
            char* current_op_copy = new char[2];
            current_op_copy[0] = *S;
            current_op_copy[1] = '\0';
            
            char* top_op = optr.top();
            
            switch (ext_orderBetween(top_op, current_op_copy)) {
                case '<': 
                    optr.push(current_op_copy);
                    S++;
                    break;
                case '=': 
                    {
                        char* popped = optr.pop();
                        delete[] popped;
                        S++;
                        delete[] current_op_copy;
                    }
                    break;
                case '>': 
                    {
                        char* op = optr.pop();
                        if (strcmp(op, OP_FAC) == 0 || 
                            strcmp(op, OP_SIN) == 0 || strcmp(op, OP_COS) == 0 || 
                            strcmp(op, OP_TAN) == 0 || strcmp(op, OP_LOG) == 0 || 
                            strcmp(op, OP_LN) == 0 || strcmp(op, OP_SQRT) == 0) {
                            float opn = opnd.pop();
                            opnd.push(ext_calcu(opn, op));
                        } else if (strlen(op) == 1) {
                            float opn2 = opnd.pop();
                            float opn1 = opnd.pop();
                            opnd.push(ext_calcu(opn1, op, opn2));
                        }
                        delete[] op;
                        delete[] current_op_copy;
                    }
                    break;
                default:
                    cout << "表达式无效" << endl;
                    exit(-1);
            }
        }
    }
    
    return opnd.pop();
}

bool isValidExpression(const char* exp) {
    Stack<char> S;
    int len = strlen(exp);
    for (int i = 0; i < len; i++) {
        switch (exp[i]) {
            case '(':
            case '[':
            case '{':
                S.push(exp[i]);
                break;
            case ')':
                if ((S.empty()) || ('(' != S.pop()))
                    return false;
                break;
            case ']':
                if ((S.empty()) || ('[' != S.pop()))
                    return false;
                break;
            case '}':
                if ((S.empty()) || ('{' != S.pop()))
                    return false;
                break;
            default:
                break;
        }
    }
    if (!S.empty()) return false;
    
    for (int i = 0; i < len; i++) {
        char c = exp[i];
        if (!isdigit(c) && c != '+' && c != '-' && c != '*' && c != '/' 
            && c != '^' && c != '!' && c != '(' && c != ')' && c != '.'
            && c != ' ' && !isalpha(c)) {
            return false;
        }
    }
    
    return true;
}

int main() {
    #ifdef _WIN32
    system("chcp 65001");
    #endif
    
    char expression[1024];
    
    cout << "=== 高级字符串计算器 ===" << endl;
    cout << "支持的运算：" << endl;
    cout << "  基本运算: +, -, *, /, ^(幂), !(阶乘)" << endl;
    cout << "  函数运算: sin, cos, tan, log(常用对数), ln(自然对数), sqrt(平方根)" << endl;
    cout << "  示例：2.5 + sin(0.5) * 3 或 sqrt(16) + log(100)" << endl;
    cout << "请输入表达式（输入 'quit' 退出）：" << endl;
    
    while (true) {
        cout << "> ";
        cin.getline(expression, 1024);
        
        if (strcmp(expression, "quit") == 0) {
            break;
        }
        
        if (!isValidExpression(expression)) {
            cout << "表达式无效！" << endl;
            continue;
        }
        
        try {
            float result = evaluate(expression);
            cout << "= " << result << endl;
        } catch (...) {
            cout << "表达式计算错误！" << endl;
        }
    }
    
    cout << "=== 计算器退出 ===" << endl;
    
    return 0;
}