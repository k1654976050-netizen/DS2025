#include <iostream>
#include "vector.h"
#include <stack>
using namespace std;

int largestRectangleArea(Vector<int>& heights) {
    stack<int> stk;
    int maxArea = 0;
    int n = heights.size();
    for (int i = 0; i <= n; ++i) {
        while (!stk.empty() && (i == n || heights[stk.top()] >= heights[i])) {
            int height = heights[stk.top()];
            stk.pop();
            int width = stk.empty() ? i : (i - stk.top() - 1);
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);
    }
    return maxArea;
}

int main() {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    int len;
    cout << "请输入柱状图柱子数量（1<=长度<=105）：";
    cin >> len;
    Vector<int> heights;
    cout << "请依次输入" << len << "个柱子的高度（0<=高度<=104），用空格分隔：" << endl;
    for (int i = 0; i < len; ++i) {
        int h;
        cin >> h;
        heights.insert(i, h);
    }
    int area = largestRectangleArea(heights);
    cout << area << endl;
    return 0;
}