#include "vector.h"
#define N_OPTR 9
#include <time.h>
#include <ctype.h>
#include <string.h>

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

void readNumber(char *&p, Stack<float> &stk)
{
    stk.push((float)(*p - '0'));
    while (isdigit(*(++p)))
    {
        stk.push(stk.pop() * 10 + (*p - '0'));
        if ('.' != *p)
            return;
        float fraction = 1;
        while (isdigit(*(++p)))
        {
            stk.push(stk.pop() + (*p - '0') * (fraction /= 10));
        }
    }
}
void append(char *&rpn, float opnd)
{
    int n = strlen(rpn);
    char buf[64];
    if (opnd != (float)(int)opnd)
        sprintf(buf, "%.2f ", opnd);
    else
        rpn = (char *)realloc(rpn, sizeof(char) * (n + strlen(buf) + 1));
    strcat(rpn, buf);
}

void append(char *&rpn, char optr)
{
    int n = strlen(rpn);
    rpn = (char *)realloc(rpn, sizeof(char) * (n + 3));
    sprintf(rpn + n, "%c ", optr);
    rpn[n + 2] = '\0';
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
        return f2 - f1;
    if (s == '*')
        return f1 * f2;
    if (s == '/')
        return f2 / f1;
    if (s == '^')
    {
        float temp = f1;
        for (int i = 0; i < f2; i++)
        {
            f1 *= temp;
        }
        return f1;
    }
    return 0;
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
