#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib> 
using namespace std;

// 复数类定义
class Complex {
private:
    double real;  
    double imag;  
public:
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

    // 计算复数的模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 重载<：先按模排序，模相同按实部排序
    bool operator<(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() < other.modulus();
        }
        return real < other.real;
    }

    // 重载==：实部和虚部均相同才相等
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // 友元输出：格式化打印复数
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ")";
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

    // 扩容操作：当元素满时扩容为原来的2倍
    void expand() {
        if (_size < _cap) return;
        _cap = (_cap == 0) ? 4 : _cap * 2;  // 初始容量为4
        T* newElem = new T[_cap];
        for (int i = 0; i < _size; ++i) {
            newElem[i] = _elem[i];
        }
        delete[] _elem;
        _elem = newElem;
    }

public:
    // 构造函数
    Vector() : _elem(nullptr), _size(0), _cap(0) {}

    // 析构函数
    ~Vector() {
        delete[] _elem;
    }

    // 获取当前元素个数
    int size() const {
        return _size;
    }

    // 重载[]：只读访问
    const T& operator[](int idx) const {
        return _elem[idx];
    }

    // 重载[]：可修改访问
    T& operator[](int idx) {
        return _elem[idx];
    }

    // 向末尾添加元素
    void push_back(const T& e) {
        expand();
        _elem[_size++] = e;
    }

    // 置乱向量（生成乱序）
    void unsort() {
        for (int i = _size - 1; i > 0; --i) {
            swap(_elem[i], _elem[rand() % (i + 1)]);
        }
    }

    // 反转向量（生成逆序）
    void reverse() {
        for (int i = 0; i < _size / 2; ++i) {
            swap(_elem[i], _elem[_size - 1 - i]);
        }
    }

    // 起泡排序
    void bubbleSort() {
        bool swapped;  // 标记是否发生交换
        for (int i = 0; i < _size - 1; ++i) {
            swapped = false;
            // 内层循环条件修复：j < _size-1-i
            for (int j = 0; j < _size - 1 - i; ++j) {
                if (_elem[j + 1] < _elem[j]) {  // 按重载的<比较
                    swap(_elem[j], _elem[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) break;  // 无交换则提前退出
        }
    }

private:
    // 归并排序：合并两个有序区间[lo, mid)和[mid, hi)
    void merge(int lo, int mid, int hi) {
        T* temp = new T[hi - lo];  // 临时存储合并结果
        int i = lo, j = mid, k = 0;
        // 修复三元运算符逻辑
        while (i < mid && j < hi) {
            temp[k++] = (_elem[i] < _elem[j]) ? _elem[i++] : _elem[j++];
        }
        // 复制剩余元素
        while (i < mid) temp[k++] = _elem[i++];
        while (j < hi) temp[k++] = _elem[j++];
        // 写回原数组
        for (k = 0; k < hi - lo; ++k) {
            _elem[lo + k] = temp[k];
        }
        delete[] temp;
    }

    // 归并排序递归函数：排序[lo, hi)区间
    void mergeSortRecur(int lo, int hi) {
        if (hi - lo <= 1) return;  // 单个元素无需排序
        int mid = (lo + hi) / 2;
        mergeSortRecur(lo, mid);
        mergeSortRecur(mid, hi);   // 修复分号缺失
        merge(lo, mid, hi);
    }

public:
    // 归并排序对外接口
    void mergeSort() {
        mergeSortRecur(0, _size);
    }

    // 生成指定状态的向量（顺序/乱序/逆序）
    static Vector<Complex> generateVec(int n, const string& state) {
        Vector<Complex> vec;
        // 生成随机复数（范围：[-1000, 1000)）
        for (int i = 0; i < n; ++i) {
            double real = (rand() % 2000) - 1000;  // -1000到999
            double imag = (rand() % 2000) - 1000;
            vec.push_back(Complex(real, imag));
        }
        // 调整为指定状态（修复条件判断==）
        if (state == "sorted") {
            vec.mergeSort();  // 顺序
        } else if (state == "unsorted") {
            vec.mergeSort();
            vec.unsort();     // 乱序
        } else if (state == "reversed") {
            vec.mergeSort();
            vec.reverse();    // 逆序
        }
        return vec;
    }
};

// 计算排序耗时（毫秒）
template<typename T>
double calculateSortTime(Vector<T>& vec, void (Vector<T>::*sortFunc)()) {
    // 复制向量，避免原向量被修改
    Vector<T> vecCopy;
    for (int i = 0; i < vec.size(); ++i) {
        vecCopy.push_back(vec[i]);
    }
    // 计时
    clock_t start = clock();
    (vecCopy.*sortFunc)();  // 调用排序函数
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC * 1000;
}

int main() {
    srand(time(0));  // Initialize random seed
    const int VEC_SIZE = 10000;  // Vector size (adjustable)
    string states[] = {"sorted", "unsorted", "reversed"};

    // 1. Test Complex Output
    cout << "=== Complex Output Test ===" << endl;
    Complex c1(3.5, -2.0);
    Complex c2(0.0, 5.7);
    cout << "Complex 1: " << c1 << ", Modulus: " << c1.modulus() << endl;
    cout << "Complex 2: " << c2 << ", Modulus: " << c2.modulus() << endl << endl;

    // 2. Test Vector States (small size for visualization)
    cout << "=== Vector State Examples (5 elements) ===" << endl;
    Vector<Complex> smallSorted = Vector<Complex>::generateVec(5, "sorted");
    cout << "Sorted Vector: ";
    for (int i = 0; i < smallSorted.size(); ++i) {
        cout << smallSorted[i] << " ";
    }
    cout << endl;

    Vector<Complex> smallReversed = Vector<Complex>::generateVec(5, "reversed");
    cout << "Reversed Vector: ";
    for (int i = 0; i < smallReversed.size(); ++i) {
        cout << smallReversed[i] << " ";
    }
    cout << endl << endl;

    // 3. Sorting Efficiency Comparison
    cout << "=== Sorting Efficiency Comparison (Size: " << VEC_SIZE << ") ===" << endl;
    cout << "Algorithm\\State   |  Sorted (ms) |  Unsorted (ms) |  Reversed (ms) |" << endl;
    cout << "---------------------------------------------------------------" << endl;

    // Test Bubble Sort
    double bubbleSorted, bubbleUnsorted, bubbleReversed;
    for (int i = 0; i < 3; ++i) {
        Vector<Complex> vec = Vector<Complex>::generateVec(VEC_SIZE, states[i]);
        if (i == 0) bubbleSorted = calculateSortTime(vec, &Vector<Complex>::bubbleSort);
        if (i == 1) bubbleUnsorted = calculateSortTime(vec, &Vector<Complex>::bubbleSort);
        if (i == 2) bubbleReversed = calculateSortTime(vec, &Vector<Complex>::bubbleSort);
    }
    printf("Bubble Sort       |  %8.2f  |  %10.2f  |  %10.2f  |\n",
           bubbleSorted, bubbleUnsorted, bubbleReversed);

    // Test Merge Sort
    double mergeSorted, mergeUnsorted, mergeReversed;
    for (int i = 0; i < 3; ++i) {
        Vector<Complex> vec = Vector<Complex>::generateVec(VEC_SIZE, states[i]);
        if (i == 0) mergeSorted = calculateSortTime(vec, &Vector<Complex>::mergeSort);
        if (i == 1) mergeUnsorted = calculateSortTime(vec, &Vector<Complex>::mergeSort);
        if (i == 2) mergeReversed = calculateSortTime(vec, &Vector<Complex>::mergeSort);
    }
    printf("Merge Sort        |  %8.2f  |  %10.2f  |  %10.2f  |\n",
           mergeSorted, mergeUnsorted, mergeReversed);

    return 0;
}