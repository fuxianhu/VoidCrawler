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

            


def safe_delete_log_file(log_file_path: str | Path, permanent: bool = False):
    """
    安全删除日志文件 - 先关闭logging handlers
    """
    log_path = Path(log_file_path) if isinstance(log_file_path, str) else log_file_path
    
    if not log_path.exists():
        print(f"日志文件不存在: {log_path}")
        return True
    
    try:
        # 关键步骤1: 关闭所有logging handlers释放文件锁
        print("正在关闭logging handlers...")
        loggers = [logging.getLogger()]  # root logger
        loggers += [logging.getLogger(name) for name in logging.root.manager.loggerDict]
        
        for logger in loggers:
            for handler in logger.handlers[:]:
                # 如果是文件handler并且指向目标日志文件，关闭它
                if (hasattr(handler, 'baseFilename') and 
                    str(Path(handler.baseFilename).resolve()) == str(log_path.resolve())):
                    logger.removeHandler(handler)
                    handler.close()
                    print(f"已关闭文件handler: {handler.baseFilename}")
        
        # 关键步骤2: 强制刷新并等待文件释放
        logging.shutdown()
        time.sleep(1)  # 给系统时间释放文件锁
        
        # 关键步骤3: 尝试多次删除（重试机制）
        max_retries = 5
        for attempt in range(max_retries):
            try:
                if permanent:
                    os.remove(log_path)
                    print(f"已永久删除日志文件: {log_path}")
                else:
                    send2trash.send2trash(str(log_path))
                    print(f"已放入回收站: {log_path}")
                return True
            except PermissionError:
                if attempt < max_retries - 1:
                    print(f"删除被拒绝，等待重试... ({attempt + 1}/{max_retries})")
                    time.sleep(0.5)
                else:
                    print(f"经过 {max_retries} 次尝试后仍无法删除文件")
                    raise
                    
    except Exception as e:
        print(f"删除日志文件时出错: {e}")
        return False

def monitor_and_restart(config: dict, main_pid: int):
    """监控并重启主进程"""
    
    def is_process_running(pid):
        """安全地检查进程是否存在"""
        try:
            process = psutil.Process(pid)
            return process.is_running()
        except psutil.NoSuchProcess:
            return False
        except Exception as e:
            print(f"检查进程状态时出错: {e}")
            return False
    
    print(f"监控进程启动，监控主进程PID: {main_pid}")
    
    # 等待主进程完全退出
    while True:
        time.sleep(0.5)
        
        if not is_process_running(main_pid):
            print("主进程已完全退出，开始清理和重启...")
            
            # 执行清理 - 使用安全的删除方法
            try:
                log_file = config['logging']['json']['handlers']['file']['filename']
                print(f"准备清理日志文件: {log_file}")
                safe_delete_log_file(log_file, config['clear_log']['permanent'])
            except Exception as e:
                print(f"清理日志文件时出错: {e}")
            
            # 重启程序
            try:
                restart_cmd = config['restart_self_program']
                print(f"执行重启命令: {restart_cmd}")
                # 使用subprocess替代os.system，更可靠
                import subprocess
                subprocess.Popen(restart_cmd, shell=True)
            except Exception as e:
                print(f"重启程序时出错: {e}")
            
            print("监控进程完成任务，退出")
            return

def clear_log_and_restart(config: dict):
    """
    清除日志并重启 - 增强版本
    """
    # 获取当前进程PID
    current_pid = os.getpid()
    print(f"当前主进程PID: {current_pid}")
    
    # 关键步骤：在退出前手动关闭logging
    print("主进程准备退出，清理logging...")
    logging.shutdown()
    
    # 启动监控进程
    monitor_process = multiprocessing.Process(
        target=monitor_and_restart, 
        args=(config, current_pid)
    )
    
    monitor_process.daemon = False
    monitor_process.start()
    
    print(f"监控进程已启动，PID: {monitor_process.pid}")
    
    # 退出当前进程 - 使用QApplication.quit()确保Qt清理
    QApplication.quit()
    # 双重确保进程退出
    sys.exit(0)