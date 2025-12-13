from pynput import keyboard
from pynput.keyboard import Key, KeyCode
from typing import *
import os
import shutil
from pathlib import Path
import send2trash
import logging
import multiprocessing
import sys
import time
import psutil
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
import ctypes
from ctypes import wintypes

from Prompt import new_prompt


class KeyConverter:
    @staticmethod
    def string_to_key(key_string):
        """
        将字符串转换为对应的 Key 或 KeyCode 对象
        """
        key_string = key_string.lower().strip()
        
        # 特殊功能键映射
        special_keys = {
            'esc': Key.esc, 'escape': Key.esc,
            'f1': Key.f1, 'f2': Key.f2, 'f3': Key.f3, 'f4': Key.f4,
            'f5': Key.f5, 'f6': Key.f6, 'f7': Key.f7, 'f8': Key.f8,
            'f9': Key.f9, 'f10': Key.f10, 'f11': Key.f11, 'f12': Key.f12,
            'enter': Key.enter, 'return': Key.enter,
            'shift': Key.shift, 'ctrl': Key.ctrl, 'alt': Key.alt,
            'tab': Key.tab, 'caps_lock': Key.caps_lock,
            'space': Key.space, 'backspace': Key.backspace,
            'delete': Key.delete, 'insert': Key.insert,
            'home': Key.home, 'end': Key.end,
            'page_up': Key.page_up, 'page_down': Key.page_down,
            'up': Key.up, 'down': Key.down, 'left': Key.left, 'right': Key.right,
            'cmd': Key.cmd, 'super': Key.cmd, 'win': Key.cmd,
        }
        
        if key_string in special_keys:
            return special_keys[key_string]
        elif len(key_string) == 1:
            # 单个字符
            return KeyCode(char=key_string)
        else:
            raise ValueError(f"不支持的键: {key_string}")

converter = KeyConverter()


class GlobalHotkey:
    def __init__(self, keys: list[str], function: Any):
        if keys is None:
            raise ValueError("GlobalHotkey Class key value is not None!")
        self.currently_pressed = set()
        self.keys = keys
        self.function = function
        self.listener = None
        self.running = False
        self.triggered = False  # 新增：防止重复触发的标志
        

    def on_press(self, key):
        """
        按键按下时的处理
        """
        self.currently_pressed.add(key)
        flag = True
        for i in self.keys:
            if (not converter.string_to_key(i) in self.currently_pressed):
                flag = False
                break
        
        if flag and not self.triggered:  # 新增条件：未触发过
            self.triggered = True  # 标记为已触发
            (self.function)()
    
    def on_release(self, key):
        """
        按键释放时的处理
        - 从已按下的键集合中移除释放的键
        - 重置触发标志
        """
        if key in self.currently_pressed:
            self.currently_pressed.remove(key)
        
        # 新增：当所有热键都释放时，重置触发标志
        if not self.currently_pressed:
            self.triggered = False
    

    def start(self):
        """
        开始监听热键
        """
        self.running = True
        logging.debug("全局热键监听已启动...")
        # 创建键盘监听器
        with keyboard.Listener(
            on_press=self.on_press,
            on_release=self.on_release) as listener:
            
            self.listener = listener
            listener.join()
    

    def stop(self):
        """
        停止监听
        """
        self.running = False
        if self.listener:
            self.listener.stop()

            


def delete_file_folder(file: str | Path, permanent: bool = False):
    """
    删除指定文件或文件夹

    Args:
        file: 要删除的文件路径
        permanent: 是否永久删除，默认放入回收站
    """
    # 转换为Path对象
    path = Path(file)
    
    # 检查路径是否存在
    if not path.exists():
        raise FileNotFoundError(f"路径不存在: {path}")
    
    try:
        if permanent:
            # 永久删除
            if path.is_file():
                os.remove(path)
            elif path.is_dir():
                shutil.rmtree(path)
        else:
            # 放入回收站
            send2trash(str(path))
            
    except PermissionError as e:
        raise PermissionError(f"没有权限删除: {path}") from e
    except Exception as e:
        raise RuntimeError(f"删除失败: {path}") from e


def clear_log(config: dict):
    """
    清除日志文件

    Args:
        config: 配置文件的内容
    """
    log_file = config['logging']['json']['handlers']['file']['filename']
    permanent = config['clear_log']['permanent']
    
    try:
        # 尝试清空文件内容而不是删除文件
        with open(log_file, 'w', encoding='utf-8') as f:
            f.truncate(0)
        new_prompt("Clear Log", f"已清空日志文件: {log_file}")
    except (PermissionError, FileNotFoundError) as e:
        new_prompt("Clear Log", f"无法清空日志文件 {log_file}: {e}\n尝试删除...")
        # 如果清空失败，尝试删除（可能需要重启程序后才能生效）
        if permanent:
            try:
                delete_file_folder(log_file, permanent)
                print("Clear Log", f"已删除日志文件: {log_file}")
            except Exception as delete_error:
                print("Clear Log", f"删除日志文件也失败: {delete_error}")


def toggle_window_invisibility(window_id: int, dwAffinity: int = 0x11):
    """
    切换窗口的不可见性

    参考: https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-setwindowdisplayaffinity

    Args:
        window_id: 要修改的窗口Id
        dwAffinity: 指定窗口内容的显示位置的显示相关性设置
    """
    # 定义 Windows API 常量
    WDA_NONE = 0x0

    # 定义 SetWindowDisplayAffinity 函数
    user32 = ctypes.windll.user32
    user32.SetWindowDisplayAffinity.argtypes = [wintypes.HWND, wintypes.DWORD]
    user32.SetWindowDisplayAffinity.restype = wintypes.BOOL
    success = user32.SetWindowDisplayAffinity(window_id, dwAffinity)