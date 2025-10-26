#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <functional>  
using namespace std;

// 复数类定义
class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部
public:
    // 构造函数
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

    // 计算复数的模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 重载<：先按模排序，模相同则按实部排序
    bool operator<(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() < other.modulus();
        }
        return real < other.real;
    }

    // 友元输出：显示复数及模
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ") [模: " << c.modulus() << "]";
        return os;
    }
};

// 向量类定义
template<typename T>
class Vector {
private:
    T* _elem;   // 存储元素的动态数组
    int _size;  // 当前元素个数
    int _cap;   // 数组容量

    // 扩容操作
    void expand() {
        if (_size < _cap) return;
        _cap = (_cap == 0) ? 4 : _cap * 2;
        T* newElem = new T[_cap];
        for (int i = 0; i < _size; ++i) newElem[i] = _elem[i];
        delete[] _elem;
        _elem = newElem;
    }

    // 二分查找左边界：找到第一个模 ≥ target_mod 的元素位置
    int binarySearchLeft(double target_mod) const {
        int lo = 0, hi = _size;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (_elem[mid].modulus() < target_mod) {
                lo = mid + 1;  // 中间元素模太小，去右半区找
            } else {
                hi = mid;      // 中间元素模达标，去左半区找更左的位置
            }
        }
        return lo;  // 循环结束时，lo=hi即为左边界
    }

public:
    // 构造函数
    Vector() : _elem(nullptr), _size(0), _cap(0) {}

    // 析构函数
    ~Vector() { delete[] _elem; }

    // 获取元素个数
    int size() const { return _size; }

    // 重载[]：只读访问
    const T& operator[](int idx) const { return _elem[idx]; }

    // 重载[]：可修改访问
    T& operator[](int idx) { return _elem[idx]; }

    // 向末尾添加元素
    void push_back(const T& e) { expand(); _elem[_size++] = e; }

    // 归并排序（确保向量按模有序）
    void mergeSort() {
        // 先声明合并函数和递归函数（修复未声明的问题）
        auto merge = [&](int lo, int mid, int hi) {
            T* temp = new T[hi - lo];
            int i = lo, j = mid, k = 0;
            while (i < mid && j < hi) {
                temp[k++] = (_elem[i] < _elem[j]) ? _elem[i++] : _elem[j++];
            }
            while (i < mid) temp[k++] = _elem[i++];
            while (j < hi) temp[k++] = _elem[j++];
            for (k = 0; k < hi - lo; ++k) _elem[lo + k] = temp[k];
            delete[] temp;
        };

        // 用function声明递归函数，解决内部依赖问题
        function<void(int, int)> mergeSortRecur;
        mergeSortRecur = [&](int lo, int hi) {
            if (hi - lo <= 1) return;
            int mid = (lo + hi) / 2;
            mergeSortRecur(lo, mid);    // 递归排序左半区
            mergeSortRecur(mid, hi);    // 递归排序右半区
            merge(lo, mid, hi);         // 合并两个有序区
        };

        // 调用递归函数开始排序
        mergeSortRecur(0, _size);
    }

    // 生成指定状态的向量（顺序）
    static Vector<Complex> generateVec(int n, const string& state) {
        Vector<Complex> vec;
        // 生成随机复数（范围[-1000, 1000)）
        for (int i = 0; i < n; ++i) {
            double real = (rand() % 2000) - 1000;
            double imag = (rand() % 2000) - 1000;
            vec.push_back(Complex(real, imag));
        }
        if (state == "sorted") {
            vec.mergeSort();  // 顺序
        }
        return vec;
    }

    // 核心功能：查找模介于[m1, m2)的所有元素，返回子向量
    Vector<Complex> findModInRange(double m1, double m2) const {
        Vector<Complex> result;
        if (m1 >= m2 || _size == 0) return result;  // 无效区间或空向量

        // 1. 找左边界：第一个模 ≥ m1 的位置
        int left = binarySearchLeft(m1);
        // 2. 找右边界：第一个模 ≥ m2 的位置（复用左边界查找逻辑）
        int right = binarySearchLeft(m2);

        // 3. 提取[left, right-1]的元素到结果向量
        for (int i = left; i < right; ++i) {
            result.push_back(_elem[i]);
        }
        return result;
    }
};

// 测试区间查找功能
void testRangeSearch() {
    // 解决中文显示问题（Windows系统）
    #ifdef _WIN32
    system("chcp 65001");  // 设置控制台编码为UTF-8
    #endif

    cout << "=== 测试：查找模介于[m1, m2)的复数 ===" << endl;
    const int VEC_SIZE = 15;  // 小规模向量，便于观察
    Vector<Complex> sortedVec = Vector<Complex>::generateVec(VEC_SIZE, "sorted");

    // 1. 打印原始有序向量（带模值）
    cout << "\n原始有序向量（" << sortedVec.size() << "个元素）：" << endl;
    for (int i = 0; i < sortedVec.size(); ++i) {
        cout << "索引" << i << "：" << sortedVec[i] << endl;
    }

    // 2. 定义目标区间[m1, m2)
    double m1 = 300.0, m2 = 800.0;
    cout << "\n目标区间：模 ∈ [" << m1 << ", " << m2 << ")" << endl;

    // 3. 执行区间查找
    Vector<Complex> result = sortedVec.findModInRange(m1, m2);

    // 4. 打印查找结果
    cout << "\n符合条件的元素（共" << result.size() << "个）：" << endl;
    if (result.size() == 0) {
        cout << "无符合条件的元素" << endl;
        return;
    }
    for (int i = 0; i < result.size(); ++i) {
        cout << "结果" << i + 1 << "：" << result[i] << endl;
    }
}

int main() {
    srand(time(0));  // 初始化随机种子
    testRangeSearch();  // 执行区间查找测试
    return 0;
}