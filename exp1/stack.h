#include "vector.h"
#define N_OPTR 9
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <cmath>

typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    FAC,
    L_P,
    R_P,
    EOE
} Operator;

template <typename T>
class Stack : public Vector<T>
{
public:
    void push(T const &e)
    {
        this->insert(this->size(), e);
    }

    T pop()
    {
        return this->remove(this->size() - 1);
    }

    T &top()
    {
        return (*this)[this->size() - 1];
    }
};

void convert(Stack<char> &S, __int64 n, int base)
{
    static char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    while (n > 0)
    {
        int remainder = (int)(n % base);
        S.push(digit[remainder]);
        n /= base;
    }
}

bool paren(const char exp[], int lo, int hi)
{
    Stack<char> S;
    for (int i = lo; i <= hi; i++)
    {
        switch (exp[i])
        {
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
    return S.empty();
}

const char pri[N_OPTR][N_OPTR] =
    {
        //  |-------------------- 当 前 运 算 符 --------------------|
        //              +      -      *      /      ^      !      (      )      \0
        //  +
        '>', '>', '<', '<', '<', '<', '<', '>', '>',
        // |
        '>', '>', '<', '<', '<', '<', '<', '>', '>',
        // 栈  *
        '>', '>', '>', '>', '<', '<', '<', '>', '>',
        // 顶  /
        '>', '>', '>', '>', '<', '<', '<', '>', '>',
        // 运  ^
        '>', '>', '>', '>', '>', '<', '<', '>', '>',
        // 算  !
        '>', '>', '>', '>', '>', '>', ' ', '>', '>',
        // 符  (
        '<', '<', '<', '<', '<', '<', '<', '=', ' ',
        // |   )
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        // -- \0
        '<', '<', '<', '<', '<', '<', '<', ' ', '='};

float calcu(char s, float f1)
{
    if (s != '!')
        exit(-1); // 或者返回错误值
    if (f1 == 0 || f1 == 1)
        return 1;
    float sum = 1;
    for (int i = 1; i <= f1; i++)
    {
        sum *= i;
    }
    return sum;
}

float calcu(float f1, char s, float f2)
{
    if (s == '+')
        return f1 + f2;
    if (s == '-')
        return f1 - f2;  // 修复：应该是f1-f2而不是f2-f1
    if (s == '*')
        return f1 * f2;
    if (s == '/')
        return f1 / f2;  // 修复：应该是f1/f2而不是f2/f1
    if (s == '^')
    {
        return pow(f1, f2); // 修复：使用标准库函数计算幂
    }
    return 0;
}

void readNumber(char *&p, Stack<float> &stk)
{
    float num = 0;
    // 读取整数部分
    while (isdigit(*p)) {
        num = num * 10 + (*p - '0');
        p++;
    }
    // 检查是否有小数部分
    if (*p == '.') {
        p++;
        float fraction = 0.1;
        // 读取小数部分
        while (isdigit(*p)) {
            num += (*p - '0') * fraction;
            fraction *= 0.1;
            p++;
        }
    }
    stk.push(num);
}

void append(char *&rpn, float opnd)
{
    int n = strlen(rpn);
    char buf[64];
    if (opnd != (float)(int)opnd)
        sprintf(buf, "%.2f ", opnd);
    else
        sprintf(buf, "%d ", (int)opnd);  // 修复：添加完整sprintf调用
    rpn = (char *)realloc(rpn, sizeof(char) * (n + strlen(buf) + 1));  // 修复：移动realloc到正确位置
    strcat(rpn, buf);
}

void append(char *&rpn, char optr)
{
    int n = strlen(rpn);
    rpn = (char *)realloc(rpn, sizeof(char) * (n + 3));
    sprintf(rpn + n, "%c ", optr);
    // rpn[n + 2] = '\0';  // 修复：sprintf已经添加了终止符，这行是多余的
}

Operator optr2rank(char op)
{
    switch (op)
    {
    case '+':
        return ADD;
        break;
    case '-':
        return SUB;
        break;
    case '*':
        return MUL;
        break;
    case '/':
        return DIV;
        break;
    case '^':
        return POW;
        break;
    case '!':
        return FAC;
        break;
    case '(':
        return L_P;
        break;
    case ')':
        return R_P;
        break;
    case '\0':
        return EOE;
        break;
    default:
        exit(-1);
    }
}

char orderBetween(char op1, char op2)
{
    return pri[optr2rank(op1)][optr2rank(op2)];
}

float evaluate(char *S, char *&RPN)
{
    Stack<float> opnd;
    Stack<char> optr;
    optr.push('\0');
    while (!optr.empty())
    {
        if (isdigit(*S))
        {
            readNumber(S, opnd);
            append(RPN, opnd.top());
        }
        else
            switch (orderBetween(optr.top(), *S))
            {
            case '<':
                optr.push(*S);
                S++;
                break;
            case '=':
                optr.pop();
                S++;
                break;
            case '>':
                {
                    char op = optr.pop();
                    append(RPN, op);
                    if ('!' == op)
                    {
                        float pOpnd = opnd.pop();
                        opnd.push(calcu(op, pOpnd));
                    }
                    else
                    {
                        float pOpnd2 = opnd.pop(), pOpnd1 = opnd.pop();
                        opnd.push(calcu(pOpnd1, op, pOpnd2));
                    }
                    break;
                }
            default:
                exit(-1);
            }
    }
    return opnd.pop();
}
