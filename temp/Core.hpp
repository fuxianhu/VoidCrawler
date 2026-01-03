#pragma once

#include <new>
#include <random>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <algorithm>


// 空指针异常
inline void raiseNullptrException()
{
    int* p = nullptr;
    int n = *p; // 触发 segmentation fault
}

// 内存越界写：向空指针或极低地址写入（必定崩溃）
inline void writeInvalidMemory()
{
    volatile int* ptr = reinterpret_cast<volatile int*>(0x1); // 地址 1
    *ptr = 999; // 写入受保护内存区域，触发 segmentation fault
}

// 内存越界读：从极低地址读取（必定崩溃）
inline void readInvalidMemory()
{
    volatile int* ptr = reinterpret_cast<volatile int*>(0x1); // 地址 1
    int value = *ptr; // 读取非法内存，触发 segmentation fault
}

// 内存泄漏
inline void memoryLeak(size_t bytes = 1, bool fill = true)
{
    // 申请内存
    char* ptr = new char[bytes];
    if (fill)
    {
        // 填充任意值，以占用实际内存
        for (size_t i = 0; i < bytes; i++)
        {
            ptr[i] = i;
        }
    }
    // 没有 delete 导致内存泄漏
}

// inline void GetPrefixSum()
// {
//     const int MAXN = 1001;
//     int n, m, q, x1, y1, x2, y2;
//     int a[MAXN][MAXN], p[MAXN][MAXN];
//     void main()
//     {
//         std::cin >> n >> m >> q;
//         for (int i = 0; i <= n; i++)
//         {
//             p[0][i] = 0;
//             p[i][0] = 0;
//         }
//         for (int i = 1; i <= n; i++)
//         {
//             for (int j = 1; j <= m; j++)
//             {
//                 std::cin >> a[i][j];
//                 p[i][j] = p[i][j - 1] + p[i - 1][j] + a[i][j] - p[i - 1][j - 1];
//             }
//         }

//         for (int i = 1; i <= q; i++)
//         {
//             std::cin >> x1 >> y1 >> x2 >> y2;
//             x1--;
//             y1--;
//             std::cout << p[x2][y2] - p[x2][y1] - p[x1][y2] + p[x1][y1] << std::endl;
//         }
//     }
// }