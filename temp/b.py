from fractions import Fraction
import re

def parse_fraction_input(input_str):
    """解析用户输入的分数或整数"""
    # 移除所有空格
    input_str = input_str.replace(' ', '')
    
    # 匹配分数格式 a/b 或整数
    if re.match(r'^-?\d+/?\d*$', input_str):
        if '/' in input_str:
            return Fraction(input_str)
        else:
            return Fraction(int(input_str), 1)
    else:
        raise ValueError("无效的输入格式")

def calculate(expr):
    """计算分数表达式"""
    # 支持的操作符
    operators = {'+': lambda x, y: x + y,
                 '-': lambda x, y: x - y,
                 '*': lambda x, y: x * y,
                 '/': lambda x, y: x / y,
                 '×': lambda x, y: x * y,  # 支持乘法符号×
                 '÷': lambda x, y: x / y}  # 支持除法符号÷
    
    # 查找操作符
    operator_pos = -1
    operator = None
    
    for op in operators:
        if op in expr:
            operator_pos = expr.find(op)
            operator = op
            break
    
    if operator_pos == -1:
        raise ValueError("未找到有效的操作符")
    
    # 分割表达式
    left_part = expr[:operator_pos]
    right_part = expr[operator_pos + 1:]
    
    # 解析分数
    left_fraction = parse_fraction_input(left_part)
    right_fraction = parse_fraction_input(right_part)
    
    # 执行计算
    if operator == '/' and right_fraction == 0:
        raise ValueError("除数不能为零")
    
    result = operators[operator](left_fraction, right_fraction)
    
    return left_fraction, operator, right_fraction, result

def format_fraction(frac):
    """格式化分数显示"""
    if frac.denominator == 1:
        return str(frac.numerator)
    else:
        return f"{frac.numerator}/{frac.denominator}"

def display_result(left, operator, right, result):
    """显示计算结果"""
    # 定义操作符显示符号
    display_operator = {
        '*': '×',
        '/': '÷',
        '×': '×',
        '÷': '÷'
    }.get(operator, operator)
    
    print(f"\n{'='*40}")
    print(f"计算: {format_fraction(left)} {display_operator} {format_fraction(right)}")
    print(f"{'='*40}")
    print(f"结果: {format_fraction(result)}")
    
    # 显示小数形式（可选）
    if result.denominator != 1:
        print(f"小数: {float(result):.4f}")
    print(f"{'='*40}\n")

def main():
    print("分数计算器")
    print("=" * 40)
    print("支持的操作:")
    print("  + : 加法    (例如: 1/2 + 3/4)")
    print("  - : 减法    (例如: 2/3 - 1/6)")
    print("  * 或 × : 乘法 (例如: 2/3 * 4/5)")
    print("  / 或 ÷ : 除法 (例如: 3/4 ÷ 2/3)")
    print("输入 'quit' 或 'q' 退出程序")
    print("=" * 40)
    
    while True:
        try:
            # 获取用户输入
            user_input = input("\n请输入分数表达式: ").strip()
            
            # 检查退出命令
            if user_input.lower() in ['quit', 'q', 'exit']:
                print("感谢使用分数计算器！")
                break
            
            # 处理空输入
            if not user_input:
                continue
            
            # 计算并显示结果
            left, operator, right, result = calculate(user_input)
            display_result(left, operator, right, result)
            
        except ValueError as e:
            print(f"错误: {e}")
            print("请使用正确的格式，例如: 1/2 + 3/4 或 2 * 3/4")
        except ZeroDivisionError:
            print("错误: 除数不能为零")
        except Exception as e:
            print(f"发生错误: {e}")

def batch_calculate():
    """批量计算模式"""
    print("\n批量计算模式")
    print("输入多个表达式，每行一个，输入 'end' 结束")
    print("例如:")
    print("  1/2 + 1/4")
    print("  3/4 * 2/3")
    print("  end")
    
    expressions = []
    while True:
        expr = input().strip()
        if expr.lower() in ['end', 'done']:
            break
        if expr:
            expressions.append(expr)
    
    print("\n计算结果:")
    print("=" * 40)
    
    for expr in expressions:
        try:
            left, operator, right, result = calculate(expr)
            display_result(left, operator, right, result)
        except Exception as e:
            print(f"表达式 '{expr}' 计算失败: {e}")

if __name__ == "__main__":
    import sys
    
    # 检查是否通过命令行参数传递表达式
    if len(sys.argv) > 1:
        # 命令行模式
        expr = ' '.join(sys.argv[1:])
        try:
            left, operator, right, result = calculate(expr)
            display_result(left, operator, right, result)
        except Exception as e:
            print(f"计算失败: {e}")
    else:
        # 交互模式
        print("选择模式:")
        print("1. 交互式计算")
        print("2. 批量计算")
        
        choice = input("请输入选择 (1 或 2): ").strip()
        
        if choice == '2':
            batch_calculate()
        else:
            main()