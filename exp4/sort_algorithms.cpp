#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#ifdef _WIN32
#include <windows.h>
#endif
#include "bbox_tools.h"

// 1.1 冒泡排序（按置信度降序）
std::vector<BBox> bubbleSort(std::vector<BBox> bboxes) {
    int n = bboxes.size();
    bool swapped;
    for (int i = 0; i < n - 1; ++i) {
        swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (bboxes[j].confidence < bboxes[j+1].confidence) {
                std::swap(bboxes[j], bboxes[j+1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    return bboxes;
}

// 1.2 插入排序（按置信度降序）
std::vector<BBox> insertionSort(std::vector<BBox> bboxes) {
    int n = bboxes.size();
    for (int i = 1; i < n; ++i) {
        BBox key = bboxes[i];
        int j = i - 1;
        while (j >= 0 && bboxes[j].confidence < key.confidence) {
            bboxes[j+1] = bboxes[j];
            --j;
        }
        bboxes[j+1] = key;
    }
    return bboxes;
}

// 归并排序辅助函数
void merge(std::vector<BBox>& bboxes, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<BBox> left_arr(bboxes.begin() + left, bboxes.begin() + mid + 1);
    std::vector<BBox> right_arr(bboxes.begin() + mid + 1, bboxes.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (left_arr[i].confidence >= right_arr[j].confidence) {
            bboxes[k++] = left_arr[i++];
        } else {
            bboxes[k++] = right_arr[j++];
        }
    }
    while (i < n1) bboxes[k++] = left_arr[i++];
    while (j < n2) bboxes[k++] = right_arr[j++];
}

// 归并排序递归函数
void mergeSortRecursive(std::vector<BBox>& bboxes, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortRecursive(bboxes, left, mid);
    mergeSortRecursive(bboxes, mid + 1, right);
    merge(bboxes, left, mid, right);
}

// 1.3 归并排序（按置信度降序）
std::vector<BBox> mergeSort(std::vector<BBox> bboxes) {
    mergeSortRecursive(bboxes, 0, bboxes.size() - 1);
    return bboxes;
}

// 快速排序分区函数
int partition(std::vector<BBox>& bboxes, int low, int high) {
    BBox pivot = bboxes[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (bboxes[j].confidence >= pivot.confidence) {
            ++i;
            std::swap(bboxes[i], bboxes[j]);
        }
    }
    std::swap(bboxes[i+1], bboxes[high]);
    return i + 1;
}

// 快速排序递归函数
void quickSortRecursive(std::vector<BBox>& bboxes, int low, int high) {
    if (low < high) {
        int pi = partition(bboxes, low, high);
        quickSortRecursive(bboxes, low, pi - 1);
        quickSortRecursive(bboxes, pi + 1, high);
    }
}

// 1.4 快速排序（按置信度降序）
std::vector<BBox> quickSort(std::vector<BBox> bboxes) {
    quickSortRecursive(bboxes, 0, bboxes.size() - 1);
    return bboxes;
}

// 主函数：兼容C++11，移除结构化绑定
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // ========== 配置测试参数（兼容C++11） ==========
    std::vector<int> test_sizes = {100, 500, 1000, 5000, 10000};
    
    // 排序算法列表（C++11兼容写法）
    typedef std::vector<BBox> (*SortFunc)(std::vector<BBox>);
    std::vector<std::pair<std::string, SortFunc>> sort_algs;
    sort_algs.push_back(std::make_pair("冒泡排序", bubbleSort));
    sort_algs.push_back(std::make_pair("插入排序", insertionSort));
    sort_algs.push_back(std::make_pair("归并排序", mergeSort));
    sort_algs.push_back(std::make_pair("快速排序", quickSort));
    
    // 数据生成函数列表（修复函数指针类型匹配问题）
    typedef std::vector<BBox> (*DataGenFunc)(int, float, float);
    std::vector<std::pair<std::string, DataGenFunc>> data_gens;
    data_gens.push_back(std::make_pair("随机分布", generateRandomBBoxes));
    data_gens.push_back(std::make_pair("聚集分布", generateClusteredBBoxes));

    // ========== 测试1：纯排序性能（不同分布+不同规模） ==========
    std::cout << "========================================" << std::endl;
    std::cout << "【纯排序性能测试】" << std::endl;
    std::cout << "========================================" << std::endl;
    for (size_t d = 0; d < data_gens.size(); ++d) {
        std::string dist_name = data_gens[d].first;
        DataGenFunc gen_func = data_gens[d].second;
        std::cout << "\n---------- " << dist_name << " ----------" << std::endl;
        
        for (size_t s = 0; s < test_sizes.size(); ++s) {
            int size = test_sizes[s];
            // 修正：显式传递后两个参数（640x640图像尺寸）
            std::vector<BBox> bboxes = gen_func(size, 640.0f, 640.0f);
            std::cout << "规模：" << size << " 个边界框" << std::endl;
            
            for (size_t a = 0; a < sort_algs.size(); ++a) {
                std::string alg_name = sort_algs[a].first;
                SortFunc alg_func = sort_algs[a].second;
                double time = measureSortTime(alg_func, bboxes);
                std::cout << "  " << alg_name << "：" << time << " ms" << std::endl;
            }
        }
    }

    // ========== 测试2：NMS+排序性能（不同分布+不同规模） ==========
    std::cout << "\n========================================" << std::endl;
    std::cout << "【NMS+排序性能测试（IoU=0.5）】" << std::endl;
    std::cout << "========================================" << std::endl;
    for (size_t d = 0; d < data_gens.size(); ++d) {
        std::string dist_name = data_gens[d].first;
        DataGenFunc gen_func = data_gens[d].second;
        std::cout << "\n---------- " << dist_name << " ----------" << std::endl;
        
        for (size_t s = 0; s < test_sizes.size(); ++s) {
            int size = test_sizes[s];
            // 修正：显式传递后两个参数（640x640图像尺寸）
            std::vector<BBox> bboxes = gen_func(size, 640.0f, 640.0f);
            std::cout << "规模：" << size << " 个边界框" << std::endl;
            
            for (size_t a = 0; a < sort_algs.size(); ++a) {
                std::string alg_name = sort_algs[a].first;
                SortFunc alg_func = sort_algs[a].second;
                double time = measureNmsTime(alg_func, bboxes);
                std::cout << "  " << alg_name << "+NMS：" << time << " ms" << std::endl;
            }
        }
    }

    // ========== 验证排序正确性 ==========
    std::cout << "\n========================================" << std::endl;
    std::cout << "【排序正确性验证】" << std::endl;
    std::cout << "========================================" << std::endl;
    std::vector<BBox> test_bboxes = {
        BBox(10,20,30,40,0.5f), BBox(15,25,35,45,0.8f),
        BBox(5,15,25,35,0.3f),  BBox(20,30,40,50,0.9f),
        BBox(8,18,28,38,0.6f)
    };
    std::cout << "原置信度："; printBBoxConfidences(test_bboxes);
    std::cout << "冒泡排序后："; printBBoxConfidences(bubbleSort(test_bboxes));
    std::cout << "插入排序后："; printBBoxConfidences(insertionSort(test_bboxes));
    std::cout << "归并排序后："; printBBoxConfidences(mergeSort(test_bboxes));
    std::cout << "快速排序后："; printBBoxConfidences(quickSort(test_bboxes));

    return 0;
}