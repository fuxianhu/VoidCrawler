import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from tkinter.filedialog import *
import json
import os
import subprocess
from ctypes import wintypes
import ctypes, logging
import platform
import psutil
import socket
import getpass
from datetime import datetime
import GPUtil
from typing import *


def disable_redirection():
    """禁用WOW64文件系统重定向"""
    old_value = wintypes.DWORD()
    success = ctypes.WinDLL('kernel32', use_last_error=True).Wow64DisableWow64FsRedirection(ctypes.byref(old_value))
    if not success:
        print("禁用重定向失败")
        return None
    logging.debug("禁用WOW64文件系统重定向")
    return old_value

def revert_redirection(old_value):
    """恢复WOW64文件系统重定向"""
    if old_value is not None:
        success = ctypes.WinDLL('kernel32', use_last_error=True).Wow64RevertWow64FsRedirection(old_value)
        if not success:
            print("恢复重定向失败")
    logging.debug("恢复WOW64文件系统重定向")



class ComputerInfoWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.setupAnimations()
        self.info = ""
        
    def initUI(self):
        # 设置窗口属性
        self.setWindowTitle('Get Computer Info')
        self.setGeometry(300, 300, 1000, 1000)
        
        # 设置黑色主题
        self.setDarkTheme()
        
        # 创建中央部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建主布局
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(20, 20, 20, 20)
        main_layout.setSpacing(15)
        
        # 创建文本编辑框（设置为只读，但可以选择和复制）
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)  # 设置为只读，但允许选择和复制
        self.text_edit.setFont(QFont("Arial", 10))
        
        # 设置长文本内容
        long_text = self.get_info()
        self.text_edit.setText(long_text)
        
        # 设置文本编辑框的样式
        self.text_edit.setStyleSheet("""
            QTextEdit {
                background-color: #2b2b2b;
                color: #e0e0e0;
                border: 2px solid #555;
                border-radius: 8px;
                padding: 10px;
                selection-background-color: #3daee9;
            }
            QScrollBar:vertical {
                background-color: #2b2b2b;
                width: 15px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background-color: #555;
                border-radius: 7px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background-color: #777;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                border: none;
                background: none;
            }
        """)
        
        # 创建底部按钮
        # self.save_json_button = QPushButton("Save Info Json")
        # self.save_json_button.setFixedHeight(40)
        # self.save_json_button.setStyleSheet("""
        #     QPushButton {
        #         background-color: #e74c3c;
        #         color: white;
        #         border: none;
        #         border-radius: 8px;
        #         font-size: 14px;
        #         font-weight: bold;
        #     }
        #     QPushButton:hover {
        #         background-color: #c0392b;
        #     }
        #     QPushButton:pressed {
        #         background-color: #a93226;
        #     }
        # """)
        # self.save_md_button = QPushButton("Save Info Markdown")
        # self.save_md_button.setFixedHeight(40)
        # self.save_md_button.setStyleSheet("""
        #     QPushButton {
        #         background-color: #e74c3c;
        #         color: white;
        #         border: none;
        #         border-radius: 8px;
        #         font-size: 14px;
        #         font-weight: bold;
        #     }
        #     QPushButton:hover {
        #         background-color: #c0392b;
        #     }
        #     QPushButton:pressed {
        #         background-color: #a93226;
        #     }
        # """)
        # 连接按钮信号
        # self.save_json_button.clicked.connect(self.save_info_json)
        # self.save_md_button.clicked.connect(self.save_info_markdown)
        
        # 将部件添加到布局
        main_layout.addWidget(self.text_edit)
        # main_layout.addWidget(self.save_json_button)
        # main_layout.addWidget(self.save_md_button)
        
    def setDarkTheme(self):
        # 设置应用程序的暗色主题
        dark_palette = QPalette()
        dark_palette.setColor(QPalette.Window, QColor(53, 53, 53))
        dark_palette.setColor(QPalette.WindowText, Qt.white)
        dark_palette.setColor(QPalette.Base, QColor(25, 25, 25))
        dark_palette.setColor(QPalette.AlternateBase, QColor(53, 53, 53))
        dark_palette.setColor(QPalette.ToolTipBase, Qt.white)
        dark_palette.setColor(QPalette.ToolTipText, Qt.white)
        dark_palette.setColor(QPalette.Text, Qt.white)
        dark_palette.setColor(QPalette.Button, QColor(53, 53, 53))
        dark_palette.setColor(QPalette.ButtonText, Qt.white)
        dark_palette.setColor(QPalette.BrightText, Qt.red)
        dark_palette.setColor(QPalette.Link, QColor(42, 130, 218))
        dark_palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
        dark_palette.setColor(QPalette.HighlightedText, Qt.black)
        
        self.setPalette(dark_palette)
        self.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }")
    
    def setupAnimations(self):
        # 创建打开动画
        self.open_animation = QPropertyAnimation(self, b"geometry")
        self.open_animation.setDuration(500)
        self.open_animation.setEasingCurve(QEasingCurve.OutCubic)
    
    def showEvent(self, event):
        super().showEvent(event)
        self.animateOpen()
    
    def animateOpen(self):
        # 获取屏幕尺寸和窗口尺寸
        screen_geo = QApplication.primaryScreen().availableGeometry()
        window_geo = self.geometry()
        
        # 设置动画起始位置（从屏幕上方滑入）
        start_geo = QRect(
            (screen_geo.width() - window_geo.width()) // 2,
            -window_geo.height(),
            window_geo.width(),
            window_geo.height()
        )
        
        # 设置动画结束位置（屏幕中央）
        end_geo = QRect(
            (screen_geo.width() - window_geo.width()) // 2,
            (screen_geo.height() - window_geo.height()) // 2,
            window_geo.width(),
            window_geo.height()
        )
        
        self.open_animation.setStartValue(start_geo)
        self.open_animation.setEndValue(end_geo)
        self.open_animation.start()
    
    def get_info(self):
        self.info = get_info_text()
        return self.info
    
    # def save_info_json(self):
    #     """
    #     保存信息到 json 文件
    #     """
    #     path = asksaveasfilename(
    #         # mode='w',  # 写入模式
    #         filetypes=[                  # 文件类型过滤器
    #             ("Json Data File", "*.json"),
    #         ],
    #         initialfile="info.json",   # 初始文件名
    #         title="Save Computer Info - Json mode"
    #     )
    #     with open(file=path, mode="w", encoding="utf-8") as f:
    #         json.dump(get_info_json(), f, ensure_ascii=True, indent=4)
        


    # def save_info_markdown(self):
    #     """
    #     保存信息到 md 文件
    #     """
    #     path = asksaveasfilename(
    #         # mode='w',  # 写入模式
    #         filetypes=[                  # 文件类型过滤器
    #             ("Markdown File", "*.md"),
    #         ],
    #         initialfile="info.md",   # 初始文件名
    #         title="Save Computer Info - Markdown mode"
    #     )
    #     with open(file=path, mode="w", encoding="utf-8") as f:
    #         f.write(get_info_markdown())


def get_comprehensive_system_info() -> str:
    """
    获取全面的计算机系统信息
    返回格式化的字符串信息
    """
    info_dict = {}
    
    try:
        # 1. 系统基本信息
        info_dict['system_info'] = get_system_info()
        
        # 2. CPU信息
        info_dict['cpu_info'] = get_cpu_info()
        
        # 3. 内存信息
        info_dict['memory_info'] = get_memory_info()
        
        # 4. 磁盘信息
        info_dict['disk_info'] = get_disk_info()
        
        # 5. 网络信息
        info_dict['network_info'] = get_network_info()
        
        # 6. 显卡信息
        info_dict['gpu_info'] = get_gpu_info()
        revert_redirection(old_value)
        
        # 7. 环境信息
        info_dict['environment_info'] = get_environment_info()
        
        # 8. 进程信息
        info_dict['process_info'] = get_process_info()
        
        # 9. 启动和用户信息
        info_dict['boot_user_info'] = get_boot_user_info()
        
        # 10. Python环境信息
        info_dict['python_info'] = get_python_info()
        
        # 格式化输出
        return format_info_to_string(info_dict)
        
    except Exception as e:
        return f"获取系统信息时出错: {str(e)}"

def get_system_info() -> Dict[str, Any]:
    """获取系统基本信息"""
    try:
        system_info = {}
        uname = platform.uname()
        
        system_info['system'] = uname.system
        system_info['node_name'] = uname.node
        system_info['release'] = uname.release
        system_info['version'] = uname.version
        system_info['machine'] = uname.machine
        system_info['processor'] = uname.processor
        
        # 系统架构
        system_info['architecture'] = platform.architecture()[0]
        
        # 平台信息
        system_info['platform'] = platform.platform()
        
        # Windows特定信息
        if hasattr(platform, 'win32_ver'):
            system_info['windows_version'] = platform.win32_ver()
        
        # macOS特定信息
        if hasattr(platform, 'mac_ver'):
            system_info['mac_version'] = platform.mac_ver()
        
        return system_info
    except Exception as e:
        return {'error': f'获取系统信息失败: {str(e)}'}

def get_cpu_info() -> Dict[str, Any]:
    """获取CPU信息"""
    try:
        cpu_info = {}
        
        # CPU物理核心数
        cpu_info['physical_cores'] = psutil.cpu_count(logical=False)
        
        # CPU逻辑核心数
        cpu_info['logical_cores'] = psutil.cpu_count(logical=True)
        
        # CPU频率
        cpu_freq = psutil.cpu_freq()
        if cpu_freq:
            cpu_info['max_frequency'] = f"{cpu_freq.max:.2f} MHz"
            cpu_info['current_frequency'] = f"{cpu_freq.current:.2f} MHz"
        
        # CPU使用率
        cpu_info['cpu_usage_per_core'] = [f"{x}%" for x in psutil.cpu_percent(percpu=True, interval=1)]
        cpu_info['total_cpu_usage'] = f"{psutil.cpu_percent(interval=1)}%"
        
        # CPU统计信息
        cpu_stats = psutil.cpu_stats()
        cpu_info['ctx_switches'] = cpu_stats.ctx_switches
        cpu_info['interrupts'] = cpu_stats.interrupts
        cpu_info['soft_interrupts'] = cpu_stats.soft_interrupts
        cpu_info['syscalls'] = cpu_stats.syscalls
        
        return cpu_info
    except Exception as e:
        return {'error': f'获取CPU信息失败: {str(e)}'}

def get_memory_info() -> Dict[str, Any]:
    """获取内存信息"""
    try:
        memory_info = {}
        
        # 虚拟内存
        svmem = psutil.virtual_memory()
        memory_info['total'] = f"{svmem.total / (1024**3):.2f} GB"
        memory_info['available'] = f"{svmem.available / (1024**3):.2f} GB"
        memory_info['used'] = f"{svmem.used / (1024**3):.2f} GB"
        memory_info['percentage'] = f"{svmem.percent}%"
        
        # 交换内存
        swap = psutil.swap_memory()
        memory_info['swap_total'] = f"{swap.total / (1024**3):.2f} GB"
        memory_info['swap_used'] = f"{swap.used / (1024**3):.2f} GB"
        memory_info['swap_free'] = f"{swap.free / (1024**3):.2f} GB"
        memory_info['swap_percentage'] = f"{swap.percent}%"
        
        return memory_info
    except Exception as e:
        return {'error': f'获取内存信息失败: {str(e)}'}

def get_disk_info() -> Dict[str, Any]:
    """获取磁盘信息"""
    try:
        disk_info = {}
        
        # 磁盘分区
        partitions = psutil.disk_partitions()
        disk_info['partitions'] = []
        
        for partition in partitions:
            try:
                partition_usage = psutil.disk_usage(partition.mountpoint)
                partition_info = {
                    'device': partition.device,
                    'mountpoint': partition.mountpoint,
                    'fstype': partition.fstype,
                    'total_size': f"{partition_usage.total / (1024**3):.2f} GB",
                    'used': f"{partition_usage.used / (1024**3):.2f} GB",
                    'free': f"{partition_usage.free / (1024**3):.2f} GB",
                    'percentage': f"{partition_usage.percent}%"
                }
                disk_info['partitions'].append(partition_info)
            except PermissionError:
                # 有些分区可能无法访问
                continue
        
        # 磁盘IO统计
        disk_io = psutil.disk_io_counters()
        if disk_io:
            disk_info['read_count'] = disk_io.read_count
            disk_info['write_count'] = disk_io.write_count
            disk_info['read_bytes'] = f"{disk_io.read_bytes / (1024**2):.2f} MB"
            disk_info['write_bytes'] = f"{disk_io.write_bytes / (1024**2):.2f} MB"
        
        return disk_info
    except Exception as e:
        return {'error': f'获取磁盘信息失败: {str(e)}'}

def get_network_info() -> Dict[str, Any]:
    """获取网络信息"""
    try:
        network_info = {}
        
        # 网络接口
        net_if_addrs = psutil.net_if_addrs()
        network_info['interfaces'] = {}
        
        for interface_name, interface_addresses in net_if_addrs.items():
            network_info['interfaces'][interface_name] = []
            for address in interface_addresses:
                addr_info = {
                    'family': str(address.family),
                    'address': address.address,
                    'netmask': address.netmask,
                    'broadcast': address.broadcast
                }
                network_info['interfaces'][interface_name].append(addr_info)
        
        # 网络IO统计
        net_io = psutil.net_io_counters()
        if net_io:
            network_info['bytes_sent'] = f"{net_io.bytes_sent / (1024**2):.2f} MB"
            network_info['bytes_recv'] = f"{net_io.bytes_recv / (1024**2):.2f} MB"
            network_info['packets_sent'] = net_io.packets_sent
            network_info['packets_recv'] = net_io.packets_recv
        
        # 主机名和IP
        network_info['hostname'] = socket.gethostname()
        try:
            network_info['local_ip'] = socket.gethostbyname(socket.gethostname())
        except:
            network_info['local_ip'] = '无法获取'
        
        return network_info
    except Exception as e:
        return {'error': f'获取网络信息失败: {str(e)}'}

def get_gpu_info() -> Dict[str, Any]:
    """获取显卡信息"""
    global old_value
    try:
        # gpu_info = {}
        
        # # 使用GPUtil获取NVIDIA GPU信息
        # try:
        #     gpus = GPUtil.getGPUs()
        #     gpu_info['gpus'] = []
            
        #     for gpu in gpus:
        #         gpu_details = {
        #             'id': gpu.id,
        #             'name': gpu.name,
        #             'load': f"{gpu.load * 100}%",
        #             'free_memory': f"{gpu.memoryFree} MB",
        #             'used_memory': f"{gpu.memoryUsed} MB",
        #             'total_memory': f"{gpu.memoryTotal} MB",
        #             'temperature': f"{gpu.temperature} °C"
        #         }
        #         gpu_info['gpus'].append(gpu_details)
        # except Exception as e:
        #     gpu_info['gpu_error'] = f"GPU信息获取失败: {str(e)}"f
        # return gpu_info
        old_value = disable_redirection()
        return subprocess.run(["nvidia-smi"], capture_output=True, text=True, check=False).stdout
    except Exception as e:
        return {'error': f'获取显卡信息失败: {str(e)}'}

def get_environment_info() -> Dict[str, Any]:
    """获取环境信息"""
    try:
        env_info = {}
        
        # 当前用户
        env_info['current_user'] = getpass.getuser()
        
        # 环境变量（只显示部分重要的）
        important_env_vars = ['PATH', 'TEMP', 'TMP', 'USERNAME', 'COMPUTERNAME', 
                             'OS', 'PROCESSOR_ARCHITECTURE', 'NUMBER_OF_PROCESSORS']
        
        env_info['environment_variables'] = {}
        for var in important_env_vars:
            if var in os.environ:
                env_info['environment_variables'][var] = os.environ[var]
        
        # 当前工作目录
        env_info['current_working_directory'] = os.getcwd()
        
        # 可执行文件路径
        env_info['executable_path'] = sys.executable
        
        return env_info
    except Exception as e:
        return {'error': f'获取环境信息失败: {str(e)}'}

def get_process_info() -> Dict[str, Any]:
    """获取进程信息"""
    try:
        process_info = {}
        
        # 当前进程信息
        current_process = psutil.Process()
        process_info['current_pid'] = current_process.pid
        process_info['current_name'] = current_process.name()
        process_info['current_status'] = current_process.status()
        process_info['current_cpu_percent'] = current_process.cpu_percent()
        process_info['current_memory_percent'] = f"{current_process.memory_percent():.2f}%"
        
        # 系统总进程数
        process_info['total_processes'] = len(psutil.pids())
        
        return process_info
    except Exception as e:
        return {'error': f'获取进程信息失败: {str(e)}'}

def get_boot_user_info() -> Dict[str, Any]:
    """获取启动和用户信息"""
    try:
        boot_user_info = {}
        
        # 系统启动时间
        boot_time = psutil.boot_time()
        boot_user_info['boot_time'] = datetime.fromtimestamp(boot_time).strftime("%Y-%m-%d %H:%M:%S")
        
        # 当前登录用户
        users = psutil.users()
        boot_user_info['logged_in_users'] = []
        
        for user in users:
            user_info = {
                'name': user.name,
                'terminal': user.terminal,
                'host': user.host,
                'started': datetime.fromtimestamp(user.started).strftime("%Y-%m-%d %H:%M:%S")
            }
            boot_user_info['logged_in_users'].append(user_info)
        
        return boot_user_info
    except Exception as e:
        return {'error': f'获取启动用户信息失败: {str(e)}'}

def get_python_info() -> Dict[str, Any]:
    """获取Python环境信息"""
    try:
        python_info = {}
        
        python_info['python_version'] = sys.version
        python_info['python_implementation'] = platform.python_implementation()
        python_info['python_compiler'] = platform.python_compiler()
        python_info['python_build'] = platform.python_build()
        
        # 检查是否在PyInstaller打包环境中运行
        python_info['frozen'] = getattr(sys, 'frozen', False)
        if python_info['frozen']:
            python_info['frozen_path'] = sys._MEIPASS if hasattr(sys, '_MEIPASS') else 'Unknown'
        
        return python_info
    except Exception as e:
        return {'error': f'获取Python信息失败: {str(e)}'}

def format_info_to_string(info_dict: Dict[str, Any]) -> str:
    """将信息字典格式化为字符串"""
    output = []
    output.append("=" * 60)
    output.append("COMPREHENSIVE SYSTEM INFORMATION")
    output.append("=" * 60)
    
    # 系统信息
    output.append("\n1. SYSTEM INFORMATION:")
    output.append("-" * 40)
    for key, value in info_dict['system_info'].items():
        output.append(f"  {key}: {value}")
    
    # CPU信息
    output.append("\n2. CPU INFORMATION:")
    output.append("-" * 40)
    for key, value in info_dict['cpu_info'].items():
        if key == 'cpu_usage_per_core':
            output.append(f"  {key}:")
            for i, usage in enumerate(value):
                output.append(f"    Core {i}: {usage}")
        else:
            output.append(f"  {key}: {value}")
    
    # 内存信息
    output.append("\n3. MEMORY INFORMATION:")
    output.append("-" * 40)
    for key, value in info_dict['memory_info'].items():
        output.append(f"  {key}: {value}")
    
    # 磁盘信息
    output.append("\n4. DISK INFORMATION:")
    output.append("-" * 40)
    disk_info = info_dict['disk_info']
    if 'partitions' in disk_info:
        output.append("  Partitions:")
        for partition in disk_info['partitions']:
            output.append(f"    Device: {partition['device']}")
            output.append(f"    Mountpoint: {partition['mountpoint']}")
            output.append(f"    File System: {partition['fstype']}")
            output.append(f"    Total Size: {partition['total_size']}")
            output.append(f"    Used: {partition['used']} ({partition['percentage']})")
            output.append(f"    Free: {partition['free']}")
            output.append("    " + "-" * 20)
    
    # 网络信息
    output.append("\n5. NETWORK INFORMATION:")
    output.append("-" * 40)
    for key, value in info_dict['network_info'].items():
        if key == 'interfaces':
            output.append("  Network Interfaces:")
            for iface, addresses in value.items():
                output.append(f"    {iface}:")
                for addr in addresses:
                    output.append(f"      {addr['family']}: {addr['address']}")
        else:
            output.append(f"  {key}: {value}")
    
    # 显卡信息
    output.append("\n6. GPU INFORMATION:")
    output.append("-" * 40)
    output.append(info_dict['gpu_info'])
    
    # 其他信息
    sections = [
        ('7. ENVIRONMENT INFORMATION:', 'environment_info'),
        ('8. PROCESS INFORMATION:', 'process_info'),
        ('9. BOOT & USER INFORMATION:', 'boot_user_info'),
        ('10. PYTHON ENVIRONMENT:', 'python_info')
    ]
    
    for title, key in sections:
        output.append(f"\n{title}")
        output.append("-" * 40)
        section_data = info_dict.get(key, {})
        for k, v in section_data.items():
            if isinstance(v, list):
                output.append(f"  {k}:")
                for item in v:
                    output.append(f"    {item}")
            elif isinstance(v, dict):
                output.append(f"  {k}:")
                for sub_k, sub_v in v.items():
                    output.append(f"    {sub_k}: {sub_v}")
            else:
                output.append(f"  {k}: {v}")
    
    output.append("\n" + "=" * 60)
    output.append("END OF SYSTEM INFORMATION")
    output.append("=" * 60)
    
    return "\n".join(output)



def get_info_text() -> str:
    """
    获取计算机信息（ **文本** 模式）
    """
    return get_comprehensive_system_info()


# def get_info_json() -> dict:
#     """
#     获取计算机信息（ **Json** 模式）
#     """
#     return {}


# def get_info_markdown() -> str:
#     """
#     获取计算机信息（ **Markdown** 模式）
#     """
#     return ""


def main(*args, **kwargs) -> NoReturn:
    app = QApplication(sys.argv)
    
    # 设置应用程序字体
    app.setFont(QFont("Arial", 10))

    window = ComputerInfoWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()