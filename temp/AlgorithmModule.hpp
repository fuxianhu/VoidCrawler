/*
开发说明：
对于简短的函数使用 inline 关键字以提高执行效率。
*/

#pragma once

// 导入标准库
#include <algorithm>
#include <cmath>
#include <bitset>
#include <string>
#include <stdexcept>
#include <vector>
#include <stack>
#include <queue>


namespace AlgorithmModule
{
    // 算法模块命名空间
    class StringManipulation
    {
        /*
        字符串操作
        */
    public:
        // 将字符传唤为对应的数字
        inline int charToValue(char c) {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
            if (c >= 'a' && c <= 'z') return c - 'a' + 10;
            throw std::invalid_argument("Invalid character in input number");
        }

        // 将数值转换为对应的字符
        inline char valueToChar(int value) {
            if (value >= 0 && value <= 9) return '0' + value;
            if (value >= 10 && value <= 35) return 'A' + (value - 10);
            throw std::invalid_argument("Value out of range for conversion to character");
        }
    };


    class BaseConversion
    {
        /*
        进制转换
        */
        // 将单个字符转换为对应的数值
    private:
        StringManipulation stringManipulation;

        // 将任意进制转换为十进制（支持小数）
        inline double toDecimal(const std::string& number, int fromBase) {
            if (fromBase < 2 || fromBase > 36) {
                throw std::invalid_argument("Base must be between 2 and 36");
            }
            
            // 检查符号
            bool isNegative = false;
            std::string num = number;
            if (!num.empty() && num[0] == '-') {
                isNegative = true;
                num = num.substr(1);
            } else if (!num.empty() && num[0] == '+') {
                num = num.substr(1);
            }
            
            if (num.empty()) {
                throw std::invalid_argument("Empty number after sign removal");
            }
            
            // 查找小数点
            size_t dotPos = num.find('.');
            std::string integerPart, fractionalPart;
            
            if (dotPos == std::string::npos) {
                integerPart = num;
                fractionalPart = "";
            } else {
                integerPart = num.substr(0, dotPos);
                fractionalPart = num.substr(dotPos + 1);
            }
            
            // 转换整数部分
            double decimalValue = 0.0;
            
            // 整数部分转换
            for (char c : integerPart) {
                int value = stringManipulation.charToValue(c);
                if (value >= fromBase) {
                    throw std::invalid_argument("Digit exceeds source base");
                }
                decimalValue = decimalValue * fromBase + value;
            }
            
            // 小数部分转换
            if (!fractionalPart.empty()) {
                double fractionalValue = 0.0;
                for (int i = fractionalPart.length() - 1; i >= 0; i--) {
                    int value = stringManipulation.charToValue(fractionalPart[i]);
                    if (value >= fromBase) {
                        throw std::invalid_argument("Digit exceeds source base");
                    }
                    fractionalValue = (fractionalValue + value) / fromBase;
                }
                decimalValue += fractionalValue;
            }
            
            return isNegative ? -decimalValue : decimalValue;
        }

        // 将十进制转换为任意进制（支持小数，最多保留15位小数）
        inline std::string fromDecimal(double decimalValue, int toBase, int maxFractionalDigits = 15) {
            if (toBase < 2 || toBase > 36) {
                throw std::invalid_argument("Base must be between 2 and 36");
            }
            
            if (decimalValue == 0.0) return "0";
            
            // 处理符号
            bool isNegative = false;
            if (decimalValue < 0) {
                isNegative = true;
                decimalValue = -decimalValue;
            }
            
            // 分离整数和小数部分
            long long integerPart = static_cast<long long>(decimalValue);
            double fractionalPart = decimalValue - integerPart;
            
            // 转换整数部分
            std::string result;
            
            if (integerPart == 0) {
                result = "0";
            } else {
                while (integerPart > 0) {
                    int remainder = integerPart % toBase;
                    result += stringManipulation.valueToChar(remainder);
                    integerPart /= toBase;
                }
                std::reverse(result.begin(), result.end());
            }
            
            // 转换小数部分
            if (fractionalPart > 0) {
                result += '.';
                int digitsConverted = 0;
                
                while (fractionalPart > 0 && digitsConverted < maxFractionalDigits) {
                    fractionalPart *= toBase;
                    int digit = static_cast<int>(fractionalPart);
                    result += stringManipulation.valueToChar(digit);
                    fractionalPart -= digit;
                    digitsConverted++;
                }
            }
            
            return isNegative ? "-" + result : result;
        }
    public:
        // 主转换函数
        inline std::string convertBase(const std::string& number, int fromBase, int toBase) {
            if (fromBase == toBase) {
                return number; // 如果进制相同，直接返回原数字
            }
            
            // 转换为十进制
            double decimalValue = toDecimal(number, fromBase);
            
            // 从十进制转换为目标进制
            return fromDecimal(decimalValue, toBase);
        }
    };

    class DigitalEncodingConverter
    {
        /*
        数字编码转换器类
        
        编码定义：
        - 原码(Sign-Magnitude)：最高位表示符号位(0为正，1为负)，其余位表示数值的绝对值
        - 反码(Ones' Complement)：正数的反码与原码相同，负数的反码是原码除符号位外按位取反
        - 补码(Two's Complement)：正数的补码与原码相同，负数的补码是其反码加1
        
        转换规则：
        - 反码：原码除符号位外的所有位取反
        - 补码：正数的补码等于其原码，负数的补码等于其反码加1
        */

    public:
        // 数字编码类型枚举定义
        enum class DigitalEncoding
        {
            SignMagnitude = 0,  // 原码：符号位 + 绝对值表示
            OnesComplement = 1, // 反码：正数同原码，负数符号位不变其余取反
            TwosComplement = 2  // 补码：正数同原码，负数为反码加1
        };
        
    private:
        // 验证二进制字符串的格式有效性
        inline void validateBinaryString(const std::string& number) const
        {
            // 检查字符串是否为空
            if (number.empty()) {
                throw std::invalid_argument("Binary string cannot be empty");
            }
            
            // 检查字符串长度是否足够（至少包含符号位和一个数据位）
            if (number.length() < 2) {
                throw std::invalid_argument("Binary string must have at least 2 bits (sign bit + data bits)");
            }
            
            // 检查字符串是否只包含'0'和'1'字符
            for (char c : number) {
                if (c != '0' && c != '1') {
                    throw std::invalid_argument("Binary string can only contain '0' and '1'");
                }
            }
        }
        
        // 检查二进制数是否表示零值（所有数据位都为0）
        inline bool isZero(const std::string& number) const
        {
            // 从第1位开始检查（跳过符号位），所有数据位必须为0
            for (int i = 1; i < number.length(); i++) {
                if (number[i] != '0') {
                    return false;  // 发现非零数据位，不是零值
                }
            }
            return true;  // 所有数据位都是0，是零值
        }
        
        // 检查是否为反码表示的负零（所有位都是1）
        inline bool isNegativeZeroOnesComplement(const std::string& number) const
        {
            // 检查字符串中的每个字符是否都是'1'
            for (char c : number) {
                if (c != '1') {
                    return false;  // 发现非1位，不是负零的反码表示
                }
            }
            return true;  // 所有位都是1，是反码的负零表示
        }
        
    public:
        // 二进制数加1操作（处理进位）
        inline std::string AddOne(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            // 从最低位（最右边）向最高位（最左边）遍历处理进位
            for (int i = number.length() - 1; i >= 0; i--)
            {
                if (number[i] == '0')
                {
                    // 当前位为0，加1后变为1，没有进位，操作完成
                    number[i] = '1';
                    break;  // 结束循环
                }
                else
                {
                    // 当前位为1，加1后变为0，产生进位继续处理下一位
                    number[i] = '0';
                }
            }
            
            return number;  // 返回加1后的结果
        }

        // 二进制数减1操作（处理借位）
        inline std::string SubtractOne(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            // 从最低位（最右边）向最高位（最左边）遍历处理借位
            for (int i = number.length() - 1; i >= 0; i--)
            {
                if (number[i] == '1')
                {
                    // 当前位为1，减1后变为0，没有借位，操作完成
                    number[i] = '0';
                    break;  // 结束循环
                }
                else
                {
                    // 当前位为0，减1后变为1，需要向上一位借位
                    number[i] = '1';
                }
            }
            
            return number;  // 返回减1后的结果
        }

        // 原码转换为反码
        inline std::string trueFormToOnesComplement(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            // 正数：反码与原码相同，直接返回
            if (number[0] == '0') {
                return number;
            }
            
            // 负数且不是-0：需要对数据位（除符号位外）按位取反
            if (!isZero(number)) {
                for (int i = 1; i < number.length(); i++)
                {
                    // 按位取反：0变1，1变0
                    number[i] = number[i] == '0' ? '1' : '0';
                }
            }
            // 负数-0（原码1000...0）在反码中变为1111...1
            return number;
        }

        // 反码转换为原码
        inline std::string onesComplementToTrueForm(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            // 正数：原码与反码相同，直接返回
            if (number[0] == '0') {
                return number;
            }
            
            // 负数：反码取反得到原码（包括负零的特殊情况处理）
            // 负0的反码（全1）变为原码的1000...0
            for (int i = 1; i < number.length(); i++)
            {
                // 按位取反：0变1，1变0
                number[i] = number[i] == '0' ? '1' : '0';
            }
            return number;
        }

        // 反码转换为补码
        inline std::string onesComplementToTwosComplement(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            if (number[0] == '0')
            {
                // 正数的补码与其原码相同，直接返回反码（正数反码同原码）
                return number;
            }
            else
            {
                // 负数的补码是在反码的基础上加1
                // 特殊情况：反码的负零（全1）加1会变成全零（补码的零）
                if (isNegativeZeroOnesComplement(number)) {
                    return std::string(number.length(), '0');  // 返回全零字符串
                }
                return AddOne(number);  // 一般情况：反码加1得到补码
            }
        }

        // 补码转换为反码
        inline std::string twosComplementToOnesComplement(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            if (number[0] == '0')
            {
                // 正数的反码与补码相同，直接返回
                return number;
            }
            else
            {
                // 负数的补码转反码需要减1
                // 特殊情况：补码的零（全0）减1会变成全1（反码的负零）
                if (isZero(number)) {
                    return std::string(number.length(), '1');  // 返回全1字符串
                }
                return SubtractOne(number);  // 一般情况：补码减1得到反码
            }
        }

        // 原码转换为补码
        std::string trueFormToTwosComplement(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            if (number[0] == '0')
            {
                // 正数的补码与其原码相同，直接返回
                return number;
            }
            else
            {
                // 负数原码转换为补码的两步过程：
                // 1. 原码 -> 反码（按位取反）
                // 2. 反码 -> 补码（加1）
                return onesComplementToTwosComplement(trueFormToOnesComplement(number));
            }
        }

        // 补码转换为原码
        inline std::string twosComplementToTrueForm(std::string number)
        {
            validateBinaryString(number);  // 首先验证输入格式
            
            if (number[0] == '0')
            {
                // 正数的原码与补码相同，直接返回
                return number;
            }
            else
            {
                // 检查是否为补码的最小值特殊情况（1000...0）
                bool onlySignBit = true;  // 标志位，初始假设只有符号位为1
                for (int i = 1; i < number.length(); i++) {
                    if (number[i] != '0') {
                        onlySignBit = false;  // 发现数据位有1，不是特殊情况
                        break;
                    }
                }
                if (onlySignBit) {
                    // 补码1000...0（最小值）对应的原码是1111...1
                    // 例如8位补码10000000对应原码11111111
                    std::string result(number.length(), '1');  // 创建全1字符串
                    result[0] = '1';  // 确保符号位为负
                    return result;
                }
                
                // 一般负数补码转换为原码的两步过程：
                // 1. 补码 -> 反码（减1）
                // 2. 反码 -> 原码（按位取反）
                return onesComplementToTrueForm(twosComplementToOnesComplement(number));
            }
        }
        
        // 便捷转换函数：在不同编码格式之间进行转换的统一接口
        std::string convert(const std::string& number, DigitalEncoding from, DigitalEncoding to)
        {
            // 如果源格式和目标格式相同，直接返回原字符串
            if (from == to) {
                return number;
            }
            
            // 根据源格式和目标格式选择相应的转换函数
            switch (from) {
                case DigitalEncoding::SignMagnitude:  // 从原码转换
                    switch (to) {
                        case DigitalEncoding::OnesComplement:   // 原码 -> 反码
                            return trueFormToOnesComplement(number);
                        case DigitalEncoding::TwosComplement:   // 原码 -> 补码
                            return trueFormToTwosComplement(number);
                        default:
                            throw std::invalid_argument("Invalid target encoding");  // 无效目标格式
                    }
                    break;
                    
                case DigitalEncoding::OnesComplement:  // 从反码转换
                    switch (to) {
                        case DigitalEncoding::SignMagnitude:   // 反码 -> 原码
                            return onesComplementToTrueForm(number);
                        case DigitalEncoding::TwosComplement:   // 反码 -> 补码
                            return onesComplementToTwosComplement(number);
                        default:
                            throw std::invalid_argument("Invalid target encoding");  // 无效目标格式
                    }
                    break;
                    
                case DigitalEncoding::TwosComplement:  // 从补码转换
                    switch (to) {
                        case DigitalEncoding::SignMagnitude:   // 补码 -> 原码
                            return twosComplementToTrueForm(number);
                        case DigitalEncoding::OnesComplement:   // 补码 -> 反码
                            return twosComplementToOnesComplement(number);
                        default:
                            throw std::invalid_argument("Invalid target encoding");  // 无效目标格式
                    }
                    break;
                    
                default:
                    throw std::invalid_argument("Invalid source encoding");  // 无效源格式
            }
        }
    };

    class DistanceCalculation
    {
        /*
        距离计算
        */
    public:
        // 求两点间的欧氏距离（欧几里得距离）
        double getEuclideanMetric(const std::vector<double>& pointA, 
                                const std::vector<double>& pointB)
        {
            if (pointA.size() != pointB.size())
            {
                throw std::invalid_argument(
                    "Points must have the same dimension. Got: " + 
                    std::to_string(pointA.size()) + " and " + 
                    std::to_string(pointB.size())
                );
            }
            
            if (pointA.empty()) {
                throw std::invalid_argument("Points cannot be empty");
            }
            
            double sum = 0.0;
            for (size_t i = 0; i < pointA.size(); i++)
            {
                double diff = pointA[i] - pointB[i];
                sum += diff * diff;  // 比 pow(diff, 2) 更好
            }
            return std::sqrt(sum);
        }

        // 求曼哈顿距离
        // https://blog.csdn.net/weixin_43961909/article/details/132388832
    };
}