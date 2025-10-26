#include "vector.h"  
#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;

class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部
public:
    // 构造函数，默认初始化为(0, 0)
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

    // 计算复数的模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 重载==运算符：比较实部和虚部是否都相等
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // 重载!=运算符：与==相反
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    // 重载<运算符：先比较模，模相等时比较实部
    bool operator<(const Complex& other) const {
        if (modulus() != other.modulus()) return modulus() < other.modulus();
        return real < other.real;
    }

    // 重载输出运算符：格式化输出复数
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

// 生成包含重复元素的复数向量（按需求生成随机复数并包含重复元素）
Vector<Complex> generateRandomComplexVec(int baseSize) {
    Vector<Complex> vec;
    // 生成随机复数元素
    for (int i = 0; i < baseSize; ++i) {
        double real = (rand() % 200) - 100;  // 范围：[-100, 100)
        double imag = (rand() % 200) - 100;
        vec.insert(vec.size(), Complex(real, imag));
    }
    // 添加重复元素（每5个元素添加1个重复元素）
    for (int i = 0; i < baseSize / 5; ++i) {
        int randPos = rand() % baseSize;
        vec.insert(vec.size(), vec[randPos]);
    }
    return vec;
}

// 向量的创建、查找、插入、删除、去重等操作（对应题目1-(1)）
int main() {
    #ifdef _WIN32
    system("chcp 65001");  
    #endif
    srand(time(0));  // 初始化随机数种子
    cout << "=== exp1.pdf 第一小题(1)：复数向量基本操作 ===" << endl;
    
    // 生成包含重复元素的向量
    Vector<Complex> vec = generateRandomComplexVec(10);
    cout << "1. 初始向量（含重复元素），大小为 " << vec.size() << " 个" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl << endl;

    // 1. 向量乱序
    vec.unsort();
    cout << "2. 乱序后的向量" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl << endl;

    // 2. 查找向量中特定元素
    Complex target = vec[3];  // 以第3个元素为目标
    int findPos = vec.find(target);
    cout << "3. 查找元素 " << target << endl;
    if (findPos != -1) {
        cout << "   找到位置：" << findPos << endl;
    } else {
        cout << "   未找到" << endl;
    }
    cout << endl;

    // 3. 插入元素
    Complex insertVal(99.9, 88.8);  // 待插入的复数
    vec.insert(2, insertVal);  // 插入到索引2的位置
    cout << "4. 插入 " << insertVal << " 后向量大小为 " << vec.size() << " 个" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl << endl;

    // 4. 删除元素
    Complex deleteVal = vec[5];
    vec.remove(5);
    cout << "5. 删除位置5的元素 " << deleteVal << " 后向量大小为 " << vec.size() << " 个" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl << endl;

    // 5. 去重操作
    int delCount = vec.uniquify();
    cout << "6. 去重操作后" << endl;
    cout << "   删除的重复元素数量：" << delCount << endl;
    cout << "   去重后向量大小为 " << vec.size() << " 个" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    return 0;
}