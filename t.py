import ctypes
import ctypes.wintypes
import time
import threading

# Windows API常量
WM_XBUTTONDOWN = 0x020B
WM_XBUTTONUP = 0x020C
XBUTTON1 = 0x0001
XBUTTON2 = 0x0002

# Windows API函数
user32 = ctypes.windll.user32

# 钩子过程回调函数类型
HOOKPROC = ctypes.WINFUNCTYPE(
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_void_p)
)

class MouseHook:
    def __init__(self):
        self.hook_id = None
        self.running = False
        
    def hook_proc(self, nCode, wParam, lParam):
        """鼠标钩子处理函数"""
        if nCode >= 0:
            # 检查是否是侧键消息
            if wParam == WM_XBUTTONDOWN or wParam == WM_XBUTTONUP:
                # 获取鼠标信息
                mouse_info = ctypes.cast(lParam, ctypes.POINTER(ctypes.c_void_p)).contents
                
                # 提取高位字判断是哪个侧键
                xbutton = (ctypes.c_uint.from_address(mouse_info.value + 12).value >> 16)
                
                if xbutton == XBUTTON1:
                    if wParam == WM_XBUTTONDOWN:
                        print("鼠标侧键1 按下")
                        # 执行你的操作
                    else:
                        print("鼠标侧键1 释放")
                        
                elif xbutton == XBUTTON2:
                    if wParam == WM_XBUTTONDOWN:
                        print("鼠标侧键2 按下")
                        # 执行你的操作
                    else:
                        print("鼠标侧键2 释放")
        
        # 传递给下一个钩子
        return user32.CallNextHookEx(self.hook_id, nCode, wParam, lParam)
    
    def start(self):
        """启动鼠标钩子"""
        # 设置钩子过程
        hook_proc_ptr = HOOKPROC(self.hook_proc)
        
        # 安装鼠标钩子
        self.hook_id = user32.SetWindowsHookExW(
            14,  # WH_MOUSE_LL 低级别鼠标钩子
            hook_proc_ptr,
            None,
            0
        )
        
        if not self.hook_id:
            print("无法安装鼠标钩子")
            return False
            
        self.running = True
        print("鼠标侧键监听已启动...")
        print("按Ctrl+C退出")
        
        # 消息循环
        msg = ctypes.wintypes.MSG()
        while self.running and user32.GetMessageW(ctypes.byref(msg), None, 0, 0):
            user32.TranslateMessage(ctypes.byref(msg))
            user32.DispatchMessageW(ctypes.byref(msg))
            time.sleep(0.01)
            
        return True
    
    def stop(self):
        """停止鼠标钩子"""
        self.running = False
        if self.hook_id:
            user32.UnhookWindowsHookEx(self.hook_id)
            self.hook_id = None
            print("鼠标钩子已卸载")

if __name__ == "__main__":
    import sys
    
    hook = MouseHook()
    
    try:
        hook.start()
    except KeyboardInterrupt:
        print("\n程序被用户中断")
    except Exception as e:
        print(f"错误: {e}")
    finally:
        hook.stop()