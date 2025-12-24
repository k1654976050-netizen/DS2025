#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <windows.h>
#include <climits>
#include <algorithm>
#include <set>
#include <map>
#include <string>
#include <functional>  // 新增：std::function依赖
#include <utility>     // 新增：pair辅助
using namespace std;


// ===================== （1）图1：带权无向图（邻接矩阵） =====================
class WeightedGraph {
private:
    vector<string> vertices;       // 顶点列表（A-H）
    map<string, int> v2i;          // 顶点→索引映射
    vector<vector<int>> adjMatrix; // 邻接矩阵
    int vertexCount;               // 顶点数
    const int INF = INT_MAX;       // 无穷大（不可达）

public:
    // 构造函数：初始化顶点
    WeightedGraph(const vector<string>& vs) {
        vertices = vs;
        vertexCount = vs.size();
        for (int i = 0; i < vertexCount; ++i) {
            v2i[vs[i]] = i;
        }
        // 初始化邻接矩阵：0表示无边，INF表示不可达（Dijkstra/Prim用）
        adjMatrix.resize(vertexCount, vector<int>(vertexCount, 0));
    }

    // 添加无向带权边
    void addEdge(const string& v1, const string& v2, int weight) {
        int i1 = v2i[v1];
        int i2 = v2i[v2];
        adjMatrix[i1][i2] = weight;
        adjMatrix[i2][i1] = weight; // 无向图对称
    }

    // 打印邻接矩阵
    void printAdjMatrix() {
        cout << "=== 图1的邻接矩阵 ===" << endl;
        // 打印列标题
        cout << "   ";
        for (const auto& v : vertices) {
            cout << v << "   ";
        }
        cout << endl;
        // 打印每行
        for (int i = 0; i < vertexCount; ++i) {
            cout << vertices[i] << "  ";
            for (int j = 0; j < vertexCount; ++j) {
                if (adjMatrix[i][j] == 0 && i != j) {
                    cout << "0   "; // 无边显示0
                } else {
                    printf("%-3d ", adjMatrix[i][j]); // 对齐输出
                }
            }
            cout << endl;
        }
    }

    // ===================== （2）BFS和DFS =====================
    // BFS遍历（从start顶点出发）
    vector<string> BFS(const string& start) {
        int startIdx = v2i[start];
        vector<bool> visited(vertexCount, false);
        queue<int> q;
        vector<string> result;

        q.push(startIdx);
        visited[startIdx] = true;

        while (!q.empty()) {
            int currIdx = q.front();
            q.pop();
            result.push_back(vertices[currIdx]);

            // 遍历所有邻接顶点（按索引顺序，保证结果稳定）
            for (int i = 0; i < vertexCount; ++i) {
                if (adjMatrix[currIdx][i] != 0 && !visited[i]) {
                    visited[i] = true;
                    q.push(i);
                }
            }
        }
        return result;
    }

    // DFS遍历（递归版）
    void DFSRecursiveHelper(int currIdx, vector<bool>& visited, vector<string>& result) {
        visited[currIdx] = true;
        result.push_back(vertices[currIdx]);

        // 遍历所有邻接顶点（按索引顺序）
        for (int i = 0; i < vertexCount; ++i) {
            if (adjMatrix[currIdx][i] != 0 && !visited[i]) {
                DFSRecursiveHelper(i, visited, result);
            }
        }
    }

    vector<string> DFS(const string& start) {
        int startIdx = v2i[start];
        vector<bool> visited(vertexCount, false);
        vector<string> result;
        DFSRecursiveHelper(startIdx, visited, result);
        return result;
    }

    // ===================== （3）Dijkstra最短路径 =====================
    // 返回：<顶点, <最短距离, 路径>>
    map<string, pair<int, string>> Dijkstra(const string& start) {
        int startIdx = v2i[start];
        vector<int> dist(vertexCount, INF);   // 最短距离
        vector<bool> visited(vertexCount, false); // 访问标记
        vector<int> prev(vertexCount, -1);    // 前驱顶点
        dist[startIdx] = 0;

        // 迭代n次（n=顶点数）
        for (int i = 0; i < vertexCount; ++i) {
            // 步骤1：找未访问的距离最小顶点
            int minDist = INF;
            int u = -1;
            for (int j = 0; j < vertexCount; ++j) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }
            if (u == -1) break; // 无可达顶点
            visited[u] = true;

            // 步骤2：更新邻接顶点的距离
            for (int v = 0; v < vertexCount; ++v) {
                if (!visited[v] && adjMatrix[u][v] != 0 
                    && dist[u] != INF 
                    && dist[u] + adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjMatrix[u][v];
                    prev[v] = u;
                }
            }
        }

        // 整理结果：距离 + 路径
        map<string, pair<int, string>> result;
        for (int i = 0; i < vertexCount; ++i) {
            string vertex = vertices[i];
            if (dist[i] == INF) {
                result[vertex] = {INF, "无路径"};
            } else {
                // 回溯路径
                stack<string> pathStack;
                int curr = i;
                while (curr != -1) {
                    pathStack.push(vertices[curr]);
                    curr = prev[curr];
                }
                // 拼接路径
                string path;
                while (!pathStack.empty()) {
                    path += pathStack.top();
                    pathStack.pop();
                    if (!pathStack.empty()) path += "->";
                }
                result[vertex] = {dist[i], path};
            }
        }
        return result;
    }

    // ===================== （3）Prim最小生成树 =====================
    // 返回：<边列表(起点,终点,权重), 总权重>
    pair<vector<tuple<string, string, int>>, int> Prim(const string& start) {
        int startIdx = v2i[start];
        vector<int> key(vertexCount, INF);    // 顶点到生成树的最小权值
        vector<int> parent(vertexCount, -1);  // 父节点
        vector<bool> inMST(vertexCount, false); // 是否在MST中
        key[startIdx] = 0;

        for (int i = 0; i < vertexCount; ++i) {
            // 找未加入MST的key最小顶点
            int minKey = INF;
            int u = -1;
            for (int j = 0; j < vertexCount; ++j) {
                if (!inMST[j] && key[j] < minKey) {
                    minKey = key[j];
                    u = j;
                }
            }
            inMST[u] = true;

            // 更新邻接顶点的key
            for (int v = 0; v < vertexCount; ++v) {
                if (adjMatrix[u][v] != 0 && !inMST[v] && adjMatrix[u][v] < key[v]) {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }

        // 整理生成树边和总权重
        vector<tuple<string, string, int>> mstEdges;
        int totalWeight = 0;
        for (int i = 0; i < vertexCount; ++i) {
            if (parent[i] != -1) {
                string u = vertices[parent[i]];
                string v = vertices[i];
                int w = adjMatrix[parent[i]][i];
                mstEdges.emplace_back(u, v, w);
                totalWeight += w;
            }
        }
        return make_pair(mstEdges, totalWeight); // C++11兼容：替换emplace
    }
};

// ===================== （4）图2：无向无权图（邻接表）- 双连通分量+关节点 =====================
class UnweightedGraph {
private:
    vector<string> vertices;       // 顶点列表（A-L）
    map<string, int> v2i;          // 顶点→索引映射
    vector<vector<int>> adjList;   // 邻接表
    int vertexCount;
    int time;                      // DFS时间戳
    set<int> articulationPoints;   // 关节点（索引）
    vector<vector<pair<int, int>>> bccEdges; // 点双连通分量（边集合）
    stack<pair<int, int>> edgeStack; // 存储边的栈

    // Tarjan算法核心（找关节点和双连通分量）
    void tarjan() { // 移除多余参数，简化逻辑
        articulationPoints.clear();
        bccEdges.clear();
        time = 0;
        vector<int> dfn(vertexCount, -1);
        vector<int> low(vertexCount, -1);

        // 定义DFS闭包（显式std::function，C++11兼容）
        std::function<void(int, int)> dfs = [&](int u, int p) {
            dfn[u] = low[u] = ++time;
            int children = 0; // 子节点数（根节点专用）

            for (int v : adjList[u]) {
                if (v == p) continue; // 跳过父节点

                if (dfn[v] == -1) { // 未访问
                    edgeStack.push(make_pair(u, v)); // C++11兼容
                    children++;
                    dfs(v, u);

                    // 更新low[u]
                    low[u] = min(low[u], low[v]);

                    // 判断关节点
                    // 1. 根节点且子节点>1
                    if (p == -1 && children > 1) {
                        articulationPoints.insert(u);
                        // 提取双连通分量
                        vector<pair<int, int>> component;
                        while (!edgeStack.empty()) {
                            auto edge = edgeStack.top();
                            edgeStack.pop();
                            component.push_back(edge);
                            if ((edge.first == u && edge.second == v) || (edge.first == v && edge.second == u)) {
                                break;
                            }
                        }
                        bccEdges.push_back(component);
                    }
                    // 2. 非根节点，low[v] >= dfn[u]
                    if (p != -1 && low[v] >= dfn[u]) {
                        articulationPoints.insert(u);
                        // 提取双连通分量
                        vector<pair<int, int>> component;
                        while (!edgeStack.empty()) {
                            auto edge = edgeStack.top();
                            edgeStack.pop();
                            component.push_back(edge);
                            if ((edge.first == u && edge.second == v) || (edge.first == v && edge.second == u)) {
                                break;
                            }
                        }
                        bccEdges.push_back(component);
                    }
                } else if (dfn[v] < dfn[u]) { // 回边（避免重复）
                    edgeStack.push(make_pair(u, v)); // C++11兼容
                    low[u] = min(low[u], dfn[v]);
                }
            }
        };

        // 遍历所有连通分量
        for (int i = 0; i < vertexCount; ++i) {
            if (dfn[i] == -1) {
                dfs(i, -1);
                // 处理栈中剩余的边（最后一个连通分量）
                vector<pair<int, int>> component;
                while (!edgeStack.empty()) {
                    component.push_back(edgeStack.top());
                    edgeStack.pop();
                }
                if (!component.empty()) {
                    bccEdges.push_back(component);
                }
            }
        }
    }

public:
    UnweightedGraph(const vector<string>& vs) {
        vertices = vs;
        vertexCount = vs.size();
        for (int i = 0; i < vertexCount; ++i) {
            v2i[vs[i]] = i;
        }
        adjList.resize(vertexCount);
    }

    // 添加无向边
    void addEdge(const string& v1, const string& v2) {
        int i1 = v2i[v1];
        int i2 = v2i[v2];
        adjList[i1].push_back(i2);
        adjList[i2].push_back(i1);
    }

    // 获取关节点（顶点名）和双连通分量（顶点集合）
    pair<set<string>, vector<set<string>>> getArticulationAndBCC() {
        tarjan(); // 调用修正后的tarjan函数

        // 转换关节点为顶点名
        set<string> apNames;
        for (int idx : articulationPoints) {
            apNames.insert(vertices[idx]);
        }

        // 转换双连通分量为顶点集合
        vector<set<string>> bccVertices;
        for (const auto& edges : bccEdges) {
            set<string> comp;
            for (const auto& e : edges) {
                comp.insert(vertices[e.first]);
                comp.insert(vertices[e.second]);
            }
            if (!comp.empty()) {
                bccVertices.push_back(comp);
            }
        }

        return make_pair(apNames, bccVertices); // C++11兼容
    }
};

// ===================== 主函数：测试所有功能 =====================
int main() {
    // 解决中文乱码（Windows）
    SetConsoleOutputCP(CP_UTF8); // 如需中文显示，取消注释

    // ===================== （1）图1：邻接矩阵 =====================
    vector<string> g1Vertices = {"A", "B", "C", "D", "E", "F", "G", "H"};
    WeightedGraph g1(g1Vertices);
    // 添加图1的边
    g1.addEdge("A", "B", 4);
    g1.addEdge("A", "D", 6);
    g1.addEdge("A", "G", 7);
    g1.addEdge("B", "C", 12);
    g1.addEdge("C", "D", 9);
    g1.addEdge("C", "E", 1);
    g1.addEdge("C", "F", 2);
    g1.addEdge("C", "H", 10);
    g1.addEdge("D", "E", 13);
    g1.addEdge("D", "G", 2);
    g1.addEdge("E", "F", 5);
    g1.addEdge("E", "G", 11);
    g1.addEdge("E", "H", 8);
    g1.addEdge("F", "H", 3);
    g1.addEdge("G", "H", 14);
    // 打印邻接矩阵
    g1.printAdjMatrix();

    // ===================== （2）图1：BFS和DFS =====================
    cout << "\n=== 图1从A出发的BFS遍历结果 ===" << endl;
    vector<string> bfsRes = g1.BFS("A");
    for (const auto& v : bfsRes) {
        cout << v << " ";
    }
    cout << endl;

    cout << "\n=== 图1从A出发的DFS遍历结果 ===" << endl;
    vector<string> dfsRes = g1.DFS("A");
    for (const auto& v : dfsRes) {
        cout << v << " ";
    }
    cout << endl;

    // ===================== （3）图1：Dijkstra最短路径 =====================
    cout << "\n=== 图1从A出发的最短路径（Dijkstra） ===" << endl;
    auto shortestPaths = g1.Dijkstra("A");
    for (const auto& p : shortestPaths) {
        string v = p.first;
        int dist = p.second.first;
        string path = p.second.second;
        cout << "A -> " << v << ": 距离=";
        if (dist == INT_MAX) {
            cout << "∞";
        } else {
            cout << dist;
        }
        cout << ", 路径=" << path << endl;
    }

    // ===================== （3）图1：Prim最小生成树 =====================
    cout << "\n=== 图1的最小生成树（Prim，从A出发） ===" << endl;
    // C++11兼容：替换结构化绑定为pair取值
    pair<vector<tuple<string, string, int>>, int> primResult = g1.Prim("A");
    vector<tuple<string, string, int>> mstEdges = primResult.first;
    int totalWeight = primResult.second;
    cout << "生成树边：" << endl;
    for (const auto& e : mstEdges) {
        cout << get<0>(e) << " - " << get<1>(e) << " (权重：" << get<2>(e) << ")" << endl;
    }
    cout << "生成树总权重：" << totalWeight << endl;

    // ===================== （4）图2：双连通分量+关节点 =====================
    vector<string> g2Vertices = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
    UnweightedGraph g2(g2Vertices);
    // 添加图2的边
    g2.addEdge("A", "B");
    g2.addEdge("A", "E");
    g2.addEdge("B", "F");
    g2.addEdge("C", "D");
    g2.addEdge("C", "F");
    g2.addEdge("D", "H");
    g2.addEdge("E", "F");
    g2.addEdge("E", "I");
    g2.addEdge("F", "G");
    g2.addEdge("F", "J");
    g2.addEdge("F", "K");
    g2.addEdge("G", "K");
    g2.addEdge("J", "K");
    g2.addEdge("K", "L");

    // 第一次计算（默认起点）
    cout << "\n=== 图2的关节点和双连通分量（初始起点） ===" << endl;
    // C++11兼容：替换结构化绑定为pair取值
    pair<set<string>, vector<set<string>>> bccResult1 = g2.getArticulationAndBCC();
    set<string> ap1 = bccResult1.first;
    vector<set<string>> bcc1 = bccResult1.second;
    cout << "关节点（割点）：";
    for (const auto& v : ap1) {
        cout << v << " ";
    }
    cout << endl;

    cout << "点双连通分量：" << endl;
    for (int i = 0; i < bcc1.size(); ++i) {
        cout << "  分量" << i+1 << ": ";
        for (const auto& v : bcc1[i]) {
            cout << v << " ";
        }
        cout << endl;
    }

    // 验证：换起点（K）重新计算
    cout << "\n=== 验证：换起点K重新计算 ===" << endl;
    UnweightedGraph g2_again(g2Vertices);
    // 重新添加边
    g2_again.addEdge("A", "B");
    g2_again.addEdge("A", "E");
    g2_again.addEdge("B", "F");
    g2_again.addEdge("C", "D");
    g2_again.addEdge("C", "F");
    g2_again.addEdge("D", "H");
    g2_again.addEdge("E", "F");
    g2_again.addEdge("E", "I");
    g2_again.addEdge("F", "G");
    g2_again.addEdge("F", "J");
    g2_again.addEdge("F", "K");
    g2_again.addEdge("G", "K");
    g2_again.addEdge("J", "K");
    g2_again.addEdge("K", "L");

    // C++11兼容：替换结构化绑定为pair取值
    pair<set<string>, vector<set<string>>> bccResult2 = g2_again.getArticulationAndBCC();
    set<string> ap2 = bccResult2.first;
    vector<set<string>> bcc2 = bccResult2.second;
    cout << "关节点（割点）：";
    for (const auto& v : ap2) {
        cout << v << " ";
    }
    cout << endl;
    cout << "关节点结果是否一致：" << (ap1 == ap2 ? "是" : "否") << endl;

    return 0;
}