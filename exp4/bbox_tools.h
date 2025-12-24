#ifndef BBOX_TOOLS_H
#define BBOX_TOOLS_H
#include <vector>
#include <chrono>

// 定义边界框结构体（属性：位置、大小、置信度）
struct BBox {
    float x1, y1, x2, y2, confidence;
    BBox(float x1_, float y1_, float x2_, float y2_, float conf_)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), confidence(conf_) {}
};

// 前置声明快速排序（解决NMS默认参数问题）
std::vector<BBox> quickSort(std::vector<BBox> bboxes);

// 时间类型别名
using TimePoint = std::chrono::high_resolution_clock::time_point;
using DurationMs = std::chrono::duration<double, std::milli>;

// 3. 数据生成函数（随机/聚集分布，支持100~10000规模）
std::vector<BBox> generateRandomBBoxes(int count, float img_w = 640, float img_h = 640);
std::vector<BBox> generateClusteredBBoxes(int count, float img_w = 640, float img_h = 640);

// 4. 时间测量函数
double measureSortTime(std::vector<BBox>(*sortFunc)(std::vector<BBox>), const std::vector<BBox>& bboxes);
double measureNmsTime(std::vector<BBox>(*sortFunc)(std::vector<BBox>), 
                     const std::vector<BBox>& bboxes, float iou_thresh = 0.5f);

// 2. NMS相关函数
float calculateIoU(const BBox& a, const BBox& b);
std::vector<BBox> nms(std::vector<BBox> bboxes, float iou_thresh = 0.5f, 
                     std::vector<BBox>(*sortFunc)(std::vector<BBox>) = quickSort);

// 辅助打印函数
void printBBoxConfidences(const std::vector<BBox>& bboxes);

#endif // BBOX_TOOLS_H