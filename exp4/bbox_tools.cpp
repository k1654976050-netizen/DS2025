#include "bbox_tools.h"
#include <iostream>
#include <random>
#include <algorithm>

// 3.1 生成随机分布边界框
std::vector<BBox> generateRandomBBoxes(int count, float img_w, float img_h) {
    std::vector<BBox> bboxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> conf_dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> coord_dist(0.0f, img_w);
    std::uniform_real_distribution<float> size_dist(10.0f, 100.0f);

    for (int i = 0; i < count; ++i) {
        float x1 = coord_dist(gen);
        float x2 = std::min(x1 + size_dist(gen), img_w);
        float y1 = coord_dist(gen);
        float y2 = std::min(y1 + size_dist(gen), img_h);
        bboxes.emplace_back(x1, y1, x2, y2, conf_dist(gen));
    }
    return bboxes;
}

// 3.2 生成聚集分布边界框（移除结构化绑定，兼容C++11）
std::vector<BBox> generateClusteredBBoxes(int count, float img_w, float img_h) {
    std::vector<BBox> bboxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> conf_dist(0.0f, 1.0f);
    std::normal_distribution<float> offset_dist(0.0f, 50.0f);
    std::uniform_real_distribution<float> size_dist(10.0f, 100.0f);

    // 定义3个聚集中心点
    std::vector<std::pair<float, float>> centers;
    centers.push_back(std::make_pair(img_w * 0.25f, img_h * 0.25f));
    centers.push_back(std::make_pair(img_w * 0.75f, img_h * 0.25f));
    centers.push_back(std::make_pair(img_w * 0.5f,  img_h * 0.75f));

    for (int i = 0; i < count; ++i) {
        // C++11兼容写法：不使用结构化绑定
        int center_idx = gen() % centers.size();
        float cx = centers[center_idx].first;
        float cy = centers[center_idx].second;
        
        float x1 = std::max(0.0f, cx + offset_dist(gen));
        float x2 = std::min(x1 + size_dist(gen), img_w);
        float y1 = std::max(0.0f, cy + offset_dist(gen));
        float y2 = std::min(y1 + size_dist(gen), img_h);
        bboxes.emplace_back(x1, y1, x2, y2, conf_dist(gen));
    }
    return bboxes;
}

// 4.1 测量纯排序耗时
double measureSortTime(std::vector<BBox>(*sortFunc)(std::vector<BBox>), const std::vector<BBox>& bboxes) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(bboxes);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// 计算IoU
float calculateIoU(const BBox& a, const BBox& b) {
    float inter_x1 = std::max(a.x1, b.x1);
    float inter_y1 = std::max(a.y1, b.y1);
    float inter_x2 = std::min(a.x2, b.x2);
    float inter_y2 = std::min(a.y2, b.y2);
    float inter_area = std::max(0.0f, inter_x2 - inter_x1) * std::max(0.0f, inter_y2 - inter_y1);
    
    float area_a = (a.x2 - a.x1) * (a.y2 - a.y1);
    float area_b = (b.x2 - b.x1) * (b.y2 - b.y1);
    float union_area = area_a + area_b - inter_area;
    
    return union_area > 0 ? inter_area / union_area : 0.0f;
}

// 2. 基础NMS算法
std::vector<BBox> nms(std::vector<BBox> bboxes, float iou_thresh, 
                     std::vector<BBox>(*sortFunc)(std::vector<BBox>)) {
    if (bboxes.empty()) return {};

    std::vector<BBox> sorted_bboxes = sortFunc(bboxes);
    std::vector<BBox> result;

    while (!sorted_bboxes.empty()) {
        BBox max_conf_box = sorted_bboxes[0];
        result.push_back(max_conf_box);
        sorted_bboxes.erase(sorted_bboxes.begin());

        // 移除高IoU框
        for (auto it = sorted_bboxes.begin(); it != sorted_bboxes.end();) {
            if (calculateIoU(max_conf_box, *it) > iou_thresh) {
                it = sorted_bboxes.erase(it);
            } else {
                ++it;
            }
        }
    }
    return result;
}

// 4.2 测量NMS+排序耗时
double measureNmsTime(std::vector<BBox>(*sortFunc)(std::vector<BBox>), 
                     const std::vector<BBox>& bboxes, float iou_thresh) {
    auto start = std::chrono::high_resolution_clock::now();
    nms(bboxes, iou_thresh, sortFunc);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// 辅助打印函数
void printBBoxConfidences(const std::vector<BBox>& bboxes) {
    for (size_t i = 0; i < bboxes.size(); ++i) {
        std::cout << bboxes[i].confidence << " ";
    }
    std::cout << std::endl;
}