import sys
import re
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from playsound import playsound
import threading
import time



class PromptWindow(QWidget):

    def __init__(self):
        super().__init__()
        
        self.is_closing = False  # 标记是否正在关闭
        self.is_moving = False   # 标记是否正在移动
        self.final_x = 0         # 记录最终的X坐标
        self.current_y = 0       # 记录当前Y坐标
        
        # 修改窗口标志：添加ToolTip和透明属性
        self.setWindowFlags(
            Qt.FramelessWindowHint | 
            Qt.WindowStaysOnTopHint | 
            Qt.Tool |  # 添加这个，让窗口不获取焦点
            Qt.WindowTransparentForInput  # 添加这个，让窗口不接收输入
        )
        
        # 设置窗口背景为半透明黑色
        self.setStyleSheet("background-color: rgb(0, 0, 0); border-radius: 8px;")  # 黑色，添加圆角
        self.setWindowOpacity(0.6) # 半透明
        
        # 关键：让鼠标事件穿透整个窗口
        self.setAttribute(Qt.WA_TransparentForMouseEvents, True)
        
        # 可选：设置窗口不获取焦点
        self.setAttribute(Qt.WA_ShowWithoutActivating, True)
        
        # 添加这个属性，确保窗口不会阻止其他窗口
        self.setAttribute(Qt.WA_X11DoNotAcceptFocus, True)
        
        # 获取屏幕信息
        self.screen_rect = QGuiApplication.primaryScreen().availableGeometry()
        
        # 设置固定宽度，高度自动调整
        self.fixed_width = 350  # 在这里初始化 fixed_width
        self.setFixedWidth(self.fixed_width)
        
        # 计算固定的X坐标（屏幕右侧）
        self.final_x = self.screen_rect.width() - self.fixed_width - 20
        
        # 创建布局和内容
        self.initUI()
        
        # 初始化动画
        self.initAnimation()
        
        # 初始隐藏窗口
        self.hide()
    
    def initUI(self):
        # 创建水平布局
        layout = QHBoxLayout()
        layout.setContentsMargins(15, 15, 15, 15)  # 设置边距
        layout.setSpacing(15)  # 设置组件间距
        
        # 左侧图标
        icon_label = QLabel()
        # 创建一个简单的图标（如果文件不存在则创建默认图标）
        try:
            pixmap = QPixmap("./assets/tip.png").scaled(32, 32)
        except:
            # 创建默认图标
            pixmap = QPixmap(32, 32)
            pixmap.fill(QColor(255, 255, 255))
        icon_label.setPixmap(pixmap)
        icon_label.setFixedSize(32, 32)  # 固定图标大小
        
        # 右侧多行文字
        text_widget = QWidget()
        text_layout = QVBoxLayout(text_widget)
        text_layout.setSpacing(8)
        text_layout.setContentsMargins(0, 0, 0, 0)
        
        title_label = QLabel("通知")
        title_label.setStyleSheet("color: white; font-size: 16px; font-weight: bold;")
        title_label.setAlignment(Qt.AlignTop)  # 顶部对齐
        
        # 创建内容标签 - 添加自动换行功能
        self.content_label = QLabel()
        self.content_label.setWordWrap(True)  # 启用自动换行
        self.content_label.setStyleSheet("color: white; font-size: 14px;")
        self.content_label.setAlignment(Qt.AlignTop | Qt.AlignLeft)
        
        # 设置默认文本内容
        default_text = "这是一个通知消息。"
        self.setContentText(default_text)
        
        # 将文字标签添加到垂直布局
        text_layout.addWidget(title_label)
        text_layout.addWidget(self.content_label)
        
        # 将图标和文字添加到水平布局
        layout.addWidget(icon_label)
        layout.addWidget(text_widget)
        
        # 设置布局
        self.setLayout(layout)
    
    def initAnimation(self):
        """初始化动画效果"""
        # 创建位置动画
        self.animation = QPropertyAnimation(self, b"pos")
        self.animation.setDuration(500)  # 动画持续时间500ms
        self.animation.setEasingCurve(QEasingCurve.OutCubic)  # 平滑的缓动曲线
        
        # 创建移动动画
        self.move_animation = QPropertyAnimation(self, b"pos")
        self.move_animation.setEasingCurve(QEasingCurve.OutCubic)
        
        # 创建关闭定时器
        self.close_timer = QTimer()
        self.close_timer.setSingleShot(True)
        self.close_timer.timeout.connect(self.hideAnimated)
    
    def preprocessText(self, text):
        """预处理文本，在长连续字符中插入零宽空格以便换行"""
        # 匹配长连续字母数字（至少15个字符）
        pattern = r'[a-zA-Z0-9]{15,}'
        
        def insertBreakOpportunities(match):
            word = match.group()
            # 在长单词中每8-12个字符插入一个零宽空格
            chunk_size = 10
            chunks = [word[i:i+chunk_size] for i in range(0, len(word), chunk_size)]
            return '\u200B'.join(chunks)  # 零宽空格
        
        processed_text = re.sub(pattern, insertBreakOpportunities, text)
        return processed_text
    
    def setContentText(self, text):
        """设置内容文本并启用自动换行"""
        # 预处理文本，为长连续字符添加换行机会
        processed_text = self.preprocessText(text)
        self.content_label.setText(processed_text)
        
        # 立即调整高度，不延迟
        self.adjustWindowHeight()
    
    def setTitleAndContent(self, title, content):
        """同时设置标题和内容"""
        # 找到标题标签并设置文本
        for i in range(self.layout().count()):
            widget = self.layout().itemAt(i).widget()
            if isinstance(widget, QWidget) and widget.layout():
                for j in range(widget.layout().count()):
                    child_widget = widget.layout().itemAt(j).widget()
                    if isinstance(child_widget, QLabel) and child_widget != self.content_label:
                        child_widget.setText(title)
                        break
        
        # 设置内容
        self.setContentText(content)
    
    def adjustWindowHeight(self):
        """根据文本内容自动调整窗口高度"""
        try:
            # 确保 fixed_width 已经初始化
            if not hasattr(self, 'fixed_width'):
                self.fixed_width = 350
                
            # 计算内容标签的理想高度
            available_width = self.fixed_width - 85  # 总宽度减去边距、图标、间距等
            content_height = self.content_label.heightForWidth(available_width)
            
            if content_height <= 0:
                # 备用方案：手动计算文本高度
                font_metrics = self.content_label.fontMetrics()
                text_rect = font_metrics.boundingRect(
                    0, 0, available_width, 0,
                    Qt.AlignLeft | Qt.AlignTop | Qt.TextWordWrap,
                    self.content_label.text()
                )
                content_height = text_rect.height()
            
            # 计算总高度：标题高度 + 内容高度 + 所有边距和间距
            title_height = 30  # 标题大致高度
            vertical_margins = 50  # 上下边距各15 + 内部间距
            spacing = 8  # 标签间间距
            
            total_height = title_height + content_height + vertical_margins + spacing
            
            # 设置最小高度
            min_height = 100
            final_height = max(total_height, min_height)
            
            # 设置窗口高度
            self.setFixedHeight(final_height)
            
        except Exception as e:
            print(f"调整高度时出错: {e}")
            # 设置默认高度
            self.setFixedHeight(150)
    
    def resizeEvent(self, event):
        """重写resize事件，确保宽度不变并重新调整高度"""
        if hasattr(self, 'fixed_width'):
            self.setFixedWidth(self.fixed_width)
        super().resizeEvent(event)
        self.adjustWindowHeight()
    
    def getWindowHeight(self):
        """返回窗口当前高度"""
        return self.height()
    
    def showAnimated(self, final_y, title="通知", content="这是一个通知消息。", auto_close=False, close_delay=5000):
        """显示窗口并执行动画
        
        Args:
            final_y: 最终的Y坐标
            title: 标题文本
            content: 内容文本
            auto_close: 是否自动关闭
            close_delay: 自动关闭延迟时间（毫秒）
        """
        # 停止之前的关闭定时器
        self.close_timer.stop()
        
        # 设置内容
        self.setTitleAndContent(title, content)
        
        # 计算最终位置
        final_pos = QPoint(self.final_x, final_y)
        self.current_y = final_y
        
        # 计算起始位置（屏幕右侧外部）
        start_pos = QPoint(self.screen_rect.width() + 10, final_y)
        
        # 设置动画
        self.animation.setStartValue(start_pos)
        self.animation.setEndValue(final_pos)
        
        # 断开之前的连接，避免重复连接
        try:
            self.animation.finished.disconnect()
        except:
            pass
        
        # 重置关闭状态
        self.is_closing = False
        
        # 显示窗口并开始动画
        self.show()
        self.animation.start()
        
        # 如果需要自动关闭，启动定时器
        if auto_close:
            self.close_timer.start(close_delay)
    
    def hideAnimated(self):
        """隐藏窗口并执行动画"""
        # 停止关闭定时器
        self.close_timer.stop()
        
        # 标记为正在关闭
        self.is_closing = True
        
        # 计算结束位置（屏幕右侧外部）
        final_x = self.screen_rect.width() + 10
        final_y = self.pos().y()
        end_pos = QPoint(final_x, final_y)
        
        # 设置动画
        self.animation.setStartValue(self.pos())
        self.animation.setEndValue(end_pos)
        
        # 断开之前的连接，避免重复连接
        try:
            self.animation.finished.disconnect()
        except:
            pass
        
        # 动画结束后隐藏窗口
        self.animation.finished.connect(self._onHideAnimationFinished)
        self.animation.start()
    
    def _onHideAnimationFinished(self):
        """隐藏动画完成后的处理"""
        self.hide()
        # 发送关闭完成信号
        self.windowHidden.emit()
    
    def moveUpAnimated(self, distance, duration=300):
        """向上移动窗口的动画
        
        Args:
            distance: 移动距离
            duration: 动画持续时间
        """
        # 如果正在播放关闭动画，则不执行上移动画
        if self.is_closing or self.is_moving:
            return
            
        self.is_moving = True
        
        # 计算新位置（保持X坐标不变）
        current_pos = self.pos()
        new_y = current_pos.y() - distance
        new_pos = QPoint(self.final_x, new_y)
        self.current_y = new_y
        
        # 设置移动动画
        self.move_animation.setDuration(duration)
        self.move_animation.setStartValue(current_pos)
        self.move_animation.setEndValue(new_pos)
        
        # 动画完成后重置移动状态
        def on_move_finished():
            self.is_moving = False
            
        self.move_animation.finished.connect(on_move_finished)
        self.move_animation.start()
    
    def getCurrentY(self):
        """获取当前Y坐标"""
        return self.current_y
    
    def closeWindow(self):
        """关闭窗口（完全退出）"""
        self.close_timer.stop()
        self.hide()
        self.close()
    
    # 定义信号
    windowHidden = pyqtSignal()


class PromptManager:
    def __init__(self):
        self.active_windows = []
        self._app = None
        self._should_quit = False
        self.last_prompt_time = 0  # 记录上次弹出时间
        self.vertical_spacing = 5  # 进一步减小间距为5像素
        self.pending_prompts = []  # 待处理的提示队列
        self.processing_lock = False  # 处理锁，防止并发处理
        self.screen_height = None  # 延迟初始化
        
        # 创建定时器来处理队列
        self.process_timer = QTimer()
        self.process_timer.setSingleShot(True)
        self.process_timer.timeout.connect(self.process_next_prompt)
    
    def get_screen_height(self):
        """获取屏幕高度（延迟初始化）"""
        if self.screen_height is None:
            app = QApplication.instance()
            if app is not None:
                self.screen_height = QGuiApplication.primaryScreen().availableGeometry().height()
            else:
                # 如果没有应用实例，返回默认值
                self.screen_height = 1080
        return self.screen_height
    
    def calculate_animation_duration(self):
        """根据弹出频率计算动画持续时间"""
        current_time = time.time() * 1000  # 转换为毫秒
        time_diff = current_time - self.last_prompt_time
        
        # 动态调整动画速度 - 大幅加快动画速度
        if time_diff < 100:  # 100ms内连续弹出，使用最快速度
            duration = 80  # 最快80ms
        elif time_diff < 300:  # 300ms内连续弹出，使用较快速度
            duration = 120
        elif time_diff < 600:  # 600ms内连续弹出，使用中等速度
            duration = 180
        else:  # 正常速度
            duration = 250
        
        self.last_prompt_time = current_time
        return duration
    
    def check_and_remove_out_of_screen_windows(self):
        """检查并移除超出屏幕上方的窗口"""
        screen_height = self.get_screen_height()
        
        # 从后往前遍历，避免删除时索引问题
        for i in range(len(self.active_windows) - 1, -1, -1):
            window = self.active_windows[i]
            window_bottom = window.getCurrentY() + window.height()
            
            # 如果窗口底部超出屏幕顶部，则关闭它
            if window_bottom < 0:
                # 不播放动画，直接关闭
                window.close_timer.stop()
                window.hide()
                window.close()
                
                # 从列表中移除
                self.active_windows.pop(i)
    
    def move_all_windows_up(self, distance, duration):
        """将所有现有窗口上移
        
        Args:
            distance: 移动距离
            duration: 动画持续时间
        """
        for window in self.active_windows:
            window.moveUpAnimated(distance, duration)
    
    def calculate_new_window_position(self, window_height):
        """计算新窗口的位置"""
        screen_rect = QGuiApplication.primaryScreen().availableGeometry()
        
        # 计算Y坐标（从屏幕底部开始）
        bottom_margin = 20  # 距离底部20像素
        final_y = screen_rect.height() - window_height - bottom_margin
        
        return final_y
    
    def process_next_prompt(self):
        """处理下一个待处理的提示"""
        if not self.pending_prompts or self.processing_lock:
            return
        
        # 加锁防止并发处理
        self.processing_lock = True
        
        try:
            title, content, close_delay = self.pending_prompts.pop(0)
            
            # 计算动画持续时间
            animation_duration = self.calculate_animation_duration()
            
            # 检查并移除超出屏幕的窗口
            self.check_and_remove_out_of_screen_windows()
            
            # 创建新窗口
            window = PromptWindow()
            
            # 设置窗口内容并获取实际高度
            window.setTitleAndContent(title, content)
            # 强制处理事件以确保高度计算正确
            QApplication.processEvents()
            window_height = window.getWindowHeight()
            
            # 计算移动距离（窗口高度 + 减小的间距）
            move_distance = window_height + self.vertical_spacing
            
            # 在显示新窗口前，将所有现有窗口上移
            self.move_all_windows_up(move_distance, animation_duration)
            
            # 立即显示新窗口，不等待上移动画完成
            self.active_windows.append(window)
            
            def on_window_hidden():
                # 从活动窗口列表中移除
                if window in self.active_windows:
                    self.active_windows.remove(window)
                    window.deleteLater()  # 确保窗口被正确清理
                
                # 如果没有活动窗口且是我们创建的app，则退出
                if not self.active_windows and self._should_quit:
                    QTimer.singleShot(100, self._app.quit)
            
            window.windowHidden.connect(on_window_hidden)
            
            # 计算新窗口的Y坐标
            final_y = self.calculate_new_window_position(window_height)
            
            # 显示新窗口
            window.showAnimated(
                final_y=final_y,
                title=title, 
                content=content,
                auto_close=True,
                close_delay=close_delay
            )
            
        except Exception as e:
            print(f"创建提示窗口时出错: {e}")
        finally:
            # 释放锁
            self.processing_lock = False
            
            # 如果还有待处理的提示，继续处理
            if self.pending_prompts:
                QTimer.singleShot(10, self.process_next_prompt)
    
    def new_prompt(self, title: str, content: str, close_delay: int):
        # 获取或创建应用实例
        self._app = QApplication.instance()
        if self._app is None:
            self._app = QApplication(sys.argv)
            self._should_quit = True  # 只有我们创建的app才需要退出
        
        # 播放声音
        threading.Thread(target=lambda: playsound('./assets/prompt.mp3'), daemon=True).start()
        
        # 将提示加入待处理队列
        self.pending_prompts.append((title, content, close_delay))
        
        # 如果没有正在处理，开始处理队列
        if not self.processing_lock and not self.process_timer.isActive():
            self.process_timer.start(0)
        
        # 启动事件循环（如果是我们创建的app）
        if self._should_quit and not getattr(self._app, 'executing', False):
            self._app.executing = True
            self._app.exec_()
            self._app.executing = False

# 创建全局管理器实例
_manager = None

def get_manager():
    """延迟初始化管理器"""
    global _manager
    if _manager is None:
        _manager = PromptManager()
    return _manager

def new_prompt(title: str = "Title", content: str = "Message. ", close_delay: int = 3000):
    """对外接口，保持原有函数签名"""
    manager = get_manager()
    return manager.new_prompt(title, content, close_delay)

# 初始化executing属性
_app = QApplication.instance()
if _app is not None and not hasattr(_app, 'executing'):
    _app.executing = False