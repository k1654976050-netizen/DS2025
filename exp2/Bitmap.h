#include <cstring>   // 包含标准memcpy函数
#include <cstdio>    // 包含fopen/fread等文件操作函数
#include <cstdlib>   // 确保内存操作相关头文件引入

typedef int Rank; // 类型定义：保持与原代码一致

class Bitmap { 
private:
    unsigned char* M;
    Rank N, _sz; // N：缓冲区总字节数；_sz：有效位（值为1的位）数量
protected:
    void init(Rank n) { 
        // n：需要容纳的最大位的索引（如n=7表示需要8位，即1字节）
        N = (n + 7) / 8; // 计算所需字节数（向上取整）
        M = new unsigned char[N](); // 初始化时直接将内存置0（替代memset）
        _sz = 0; 
    }
public:
    Bitmap(Rank n = 8) { init(n); } 

    Bitmap(char* file, Rank n = 8) { 
        init(n);
        FILE* fp = fopen(file, "rb"); // 用"rb"（二进制读）避免文本模式转义
        if (fp != NULL) { // 增加文件打开失败的判断，避免空指针访问
            fread(M, sizeof(char), N, fp); 
            fclose(fp);
            // 重新计算有效位数量（修复原代码中_sz未正确赋值的问题）
            _sz = 0;
            for (Rank k = 0; k < 8 * N; k++) {
                _sz += test(k);
            }
        }
    }

    ~Bitmap() { 
        delete[] M; 
        M = NULL; 
        N = 0; // 析构时重置成员变量，避免野指针/无效值
        _sz = 0; 
    }

    Rank size() { return _sz; }

    void set(Rank k) { 
        if (k < 0) return; // 增加负索引判断，避免越界
        expand(k); 
        if (!test(k)) { // 先判断位是否已为1，避免重复计数
            M[k >> 3] |= (0x80 >> (k & 0x07)); 
            _sz++; 
        }
    }

    void clear(Rank k) { 
        if (k < 0) return; // 增加负索引判断，避免越界
        expand(k); 
        if (test(k)) { // 先判断位是否已为0，避免重复计数
            M[k >> 3] &= ~(0x80 >> (k & 0x07)); 
            _sz--; 
        }
    }

    bool test(Rank k) { 
        if (k < 0 || k >= 8 * N) return false; // 明确越界返回false，避免非法访问
        return M[k >> 3] & (0x80 >> (k & 0x07)); 
    }

    void dump(char* file) { 
        FILE* fp = fopen(file, "wb"); // 用"wb"（二进制写）保持数据完整性
        if (fp != NULL) { // 增加文件打开失败的判断
            fwrite(M, sizeof(char), N, fp); 
            fclose(fp);
        }
    }

    char* bits2string(Rank n) { 
        if (n <= 0) return NULL; // 避免n<=0导致的内存浪费
        expand(n - 1); 
        char* s = new char[n + 1]; 
        s[n] = '\0'; 
        for (Rank i = 0; i < n; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s; 
    }

    void expand(Rank k) { 
        if (k < 8 * N) return; 
        // 扩容至刚好能容纳第k位（替代2*k，减少内存冗余）
        Rank newN = (k + 7) / 8; 
        unsigned char* newM = new unsigned char[newN](); // 新缓冲区初始化为0
        // 用标准 memcpy 替代 memcpy_s，保证跨平台兼容性（如 Linux/macOS）
        memcpy(newM, M, N);

        // 释放旧内存，更新成员变量
        delete[] M; 
        M = newM; 
        N = newN; 
    }
};