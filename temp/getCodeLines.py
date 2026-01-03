import os
import sys
from pathlib import Path

def count_code_lines_in_folder(folder_path):
    """
    统计指定文件夹（不包含子文件夹）中.h和.cpp文件的总行数
    
    参数:
        folder_path: 要统计的文件夹路径
        
    返回:
        total_lines: 总行数
        file_count: 文件数量
        file_details: 每个文件的详细信息列表
    """
    folder = Path(folder_path)
    
    if not folder.exists():
        print(f"错误: 文件夹 '{folder_path}' 不存在")
        return 0, 0, []
    
    if not folder.is_dir():
        print(f"错误: '{folder_path}' 不是文件夹")
        return 0, 0, []
    
    # 支持的扩展名
    extensions = {'.h', '.cpp', '.hpp'}
    
    total_lines = 0
    file_count = 0
    file_details = []
    
    print(f"正在统计文件夹: {folder_path}")
    print("仅统计当前文件夹，不包含子文件夹")
    print("-" * 50)
    
    # 遍历文件夹下的所有文件（不递归子文件夹）
    for item in folder.iterdir():
        if item.is_file():
            # 检查文件扩展名
            if item.suffix.lower() in extensions:
                try:
                    # 尝试多种编码读取文件
                    encodings = ['utf-8', 'gbk', 'latin-1']
                    lines = 0
                    
                    for encoding in encodings:
                        try:
                            with open(item, 'r', encoding=encoding) as f:
                                lines = sum(1 for _ in f)
                            break
                        except UnicodeDecodeError:
                            continue
                    
                    total_lines += lines
                    file_count += 1
                    
                    # 记录文件详情
                    file_details.append({
                        'filename': item.name,
                        'path': str(item),
                        'lines': lines
                    })
                    
                    print(f"  ✓ {item.name:30} - {lines:6} 行")
                    
                except Exception as e:
                    print(f"  ✗ {item.name:30} - 读取失败: {e}")
    
    return total_lines, file_count, file_details

def print_statistics(folder_path, total_lines, file_count, file_details):
    """打印统计结果"""
    print("\n" + "=" * 60)
    print("统计结果")
    print("=" * 60)
    print(f"文件夹路径: {folder_path}")
    print(f"统计范围: 仅当前文件夹（不包含子文件夹）")
    print(f"文件类型: .h, .cpp, .hpp")
    print("-" * 60)
    
    if file_count == 0:
        print("未找到符合条件的.h/.cpp/.hpp文件")
        return
    
    print(f"文件总数: {file_count}")
    print(f"总行数: {total_lines:,}")
    print(f"平均每个文件: {total_lines/file_count:.1f} 行")
    
    # 按行数排序显示文件
    if file_details:
        print("-" * 60)
        print("文件详情（按行数排序）:")
        sorted_details = sorted(file_details, key=lambda x: x['lines'], reverse=True)
        
        for i, file_info in enumerate(sorted_details, 1):
            print(f"{i:2d}. {file_info['filename']:30} - {file_info['lines']:6,} 行")
    
    print("=" * 60)

def save_results_to_file(folder_path, total_lines, file_count, file_details):
    """将结果保存到文件"""
    timestamp = Path(__file__).stem
    output_file = f"code_lines_{timestamp}.txt"
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("代码行数统计报告\n")
        f.write("=" * 60 + "\n\n")
        f.write(f"统计时间: {timestamp}\n")
        f.write(f"文件夹路径: {folder_path}\n")
        f.write(f"统计范围: 仅当前文件夹（不包含子文件夹）\n")
        f.write(f"文件类型: .h, .cpp, .hpp\n\n")
        f.write(f"文件总数: {file_count}\n")
        f.write(f"总行数: {total_lines}\n")
        f.write(f"平均行数: {total_lines/max(file_count, 1):.1f}\n\n")
        
        if file_details:
            f.write("详细文件列表:\n")
            f.write("-" * 60 + "\n")
            
            # 按文件名排序
            sorted_details = sorted(file_details, key=lambda x: x['filename'].lower())
            
            for file_info in sorted_details:
                f.write(f"{file_info['filename']:30} - {file_info['lines']:6} 行\n")
    
    print(f"\n结果已保存到: {output_file}")

def main():
    """主函数"""
    # 检查命令行参数
    if len(sys.argv) > 1:
        folder_path = sys.argv[1]
    else:
        # 交互式输入
        if os.name == 'nt':  # Windows
            print("提示: 可以直接将文件夹拖拽到命令行窗口")
        print("提示：E:\YFY\VoidCrawler\VoidCrawler")
        folder_path = input("请输入文件夹路径: ").strip()
    
    # 清理路径（去除可能的引号）
    folder_path = folder_path.strip('"\'')
    
    # 统计代码行数
    total_lines, file_count, file_details = count_code_lines_in_folder(folder_path)
    
    # 打印结果
    print_statistics(folder_path, total_lines, file_count, file_details)
    
    # 询问是否保存结果
    if file_count > 0:
        save_choice = input("\n是否保存结果到文件? (y/n): ").lower()
        if save_choice == 'y':
            save_results_to_file(folder_path, total_lines, file_count, file_details)

if __name__ == "__main__":
    main()