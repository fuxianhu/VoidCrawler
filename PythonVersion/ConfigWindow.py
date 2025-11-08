import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import threading


class CustomTitleBar(QWidget):

    def __init__(self, parent, title : str):
        super().__init__(parent)
        self.title = title
        self.parent = parent
        self.init_ui()
        
    def init_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(15, 0, 15, 0)
        layout.setSpacing(0)
        
        # 标题 - 居中且放大
        self.title_label = QLabel(self.title)
        self.title_label.setAlignment(Qt.AlignCenter)
        self.title_label.setStyleSheet("""
            QLabel {
                color: #ffffff;
                font-size: 28px;
                font-weight: bold;
                padding: 0px;
                background: transparent;
            }
        """)
        self.title_label.setFont(QFont("Microsoft YaHei", 20, QFont.Bold))
        
        # 空白区域（左侧）
        left_spacer = QWidget()
        left_spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        
        # 空白区域（右侧）- 为了平衡关闭按钮的宽度
        right_spacer = QWidget()
        right_spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        
        # 关闭按钮
        self.close_btn = QPushButton("✘")
        self.close_btn.setFixedSize(28, 28)
        self.close_btn.setStyleSheet("""
            QPushButton {
                background-color: transparent;
                border: none;
                border-radius: 14px;
                color: #ffffff;
                font-size: 16px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #e74c3c;
                color: white;
            }
            QPushButton:pressed {
                background-color: #c0392b;
            }
        """)
        self.close_btn.clicked.connect(self.parent.close_with_animation)
        
        layout.addWidget(left_spacer)
        layout.addWidget(self.title_label)
        layout.addWidget(right_spacer)
        layout.addWidget(self.close_btn)
        
        self.setFixedHeight(40)
        self.setStyleSheet("""
            CustomTitleBar {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                    stop: 0 #2c3e50, stop: 0.5 #34495e, stop: 1 #2c3e50);
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                border-bottom: 2px solid #34495e;
            }
        """)
    
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.parent.drag_start_position = event.globalPos() - self.parent.frameGeometry().topLeft()
            event.accept()
    
    def mouseMoveEvent(self, event):
        if event.buttons() == Qt.LeftButton and hasattr(self.parent, 'drag_start_position'):
            self.parent.move(event.globalPos() - self.parent.drag_start_position)
            event.accept()

class SeparatorWidget(QWidget):
    """分割线控件"""
    def __init__(self, text=""):
        super().__init__()
        self.text = text
        self.init_ui()
        
    def init_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 10, 0, 10)
        layout.setSpacing(8)
        
        # 左侧分割线
        left_line = QFrame()
        left_line.setFrameShape(QFrame.HLine)
        left_line.setFrameShadow(QFrame.Sunken)
        left_line.setStyleSheet("background-color: #5d6d7e;")
        left_line.setFixedHeight(2)
        
        # 文本标签
        text_label = QLabel(self.text)
        text_label.setAlignment(Qt.AlignCenter)
        text_label.setStyleSheet("""
            QLabel {
                color: #aab7b8;
                font-size: 20px;
                font-weight: bold;
                background: transparent;
                padding: 0px 8px;
            }
        """)
        text_label.setFont(QFont("Microsoft YaHei", 9, QFont.Bold))
        
        # 右侧分割线
        right_line = QFrame()
        right_line.setFrameShape(QFrame.HLine)
        right_line.setFrameShadow(QFrame.Sunken)
        right_line.setStyleSheet("background-color: #5d6d7e;")
        right_line.setFixedHeight(2)
        
        # 添加拉伸因子，让分割线占据剩余空间
        layout.addWidget(left_line, 1)
        layout.addWidget(text_label, 0)
        layout.addWidget(right_line, 1)

class ColorPickerWidget(QWidget):
    colorChanged = pyqtSignal(str)
    
    def __init__(self, default_color="#3498db"):
        super().__init__()
        self.current_color = default_color
        self.init_ui()
        
    def init_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(8)
        
        # 颜色显示按钮
        self.color_btn = QPushButton()
        self.color_btn.setFixedSize(40, 30)
        self.color_btn.setStyleSheet(f"""
            QPushButton {{
                background-color: {self.current_color};
                border: 2px solid #5d6d7e;
                border-radius: 6px;
            }}
            QPushButton:hover {{
                border: 2px solid #3498db;
            }}
        """)
        self.color_btn.clicked.connect(self.pick_color)
        
        # 颜色输入框
        self.color_input = QLineEdit(self.current_color)
        self.color_input.setPlaceholderText("#RRGGBB")
        self.color_input.textChanged.connect(self.validate_color_input)
        self.color_input.setStyleSheet("""
            QLineEdit {
                background-color: #2c3e50;
                border: 2px solid #5d6d7e;
                border-radius: 6px;
                color: #ecf0f1;
                padding: 5px 10px;
                font-size: 12px;
            }
            QLineEdit:focus {
                border-color: #3498db;
            }
        """)
        
        layout.addWidget(self.color_btn)
        layout.addWidget(self.color_input)
        
    def pick_color(self):
        color = QColorDialog.getColor(QColor(self.current_color))
        if color.isValid():
            self.set_color(color.name())
            
    def validate_color_input(self, text):
        if QColor(text).isValid():
            self.set_color(text)
            
    def set_color(self, color):
        self.current_color = color
        self.color_btn.setStyleSheet(f"""
            QPushButton {{
                background-color: {color};
                border: 2px solid #3498db;
                border-radius: 6px;
            }}
        """)
        self.color_input.setText(color)
        self.colorChanged.emit(color)
        
    def get_color(self):
        return self.current_color

class ListConfigWidget(QWidget):
    def __init__(self, item_type, default_values=None):
        super().__init__()
        self.item_type = item_type
        self.default_values = default_values or []
        self.init_ui()
        
    def init_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(5)
        
        # 列表显示
        self.list_widget = QListWidget()
        self.list_widget.setMaximumHeight(80)
        self.list_widget.setStyleSheet("""
            QListWidget {
                border: 2px solid #5d6d7e;
                border-radius: 6px;
                background-color: #2c3e50;
                padding: 5px;
                color: #ecf0f1;
            }
            QListWidget:focus {
                border-color: #3498db;
            }
            QListWidget::item {
                padding: 5px;
                border-bottom: 1px solid #34495e;
                color: #ecf0f1;
                background-color: #2c3e50;
            }
            QListWidget::item:selected {
                background-color: #3498db;
                color: white;
            }
            QListWidget::item:hover {
                background-color: #34495e;
            }
        """)
        
        # 添加/删除按钮
        btn_layout = QHBoxLayout()
        
        self.add_btn = QPushButton("添加")
        self.delete_btn = QPushButton("删除")
        
        for btn in [self.add_btn, self.delete_btn]:
            btn.setFixedHeight(25)
            btn.setStyleSheet("""
                QPushButton {
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                        stop: 0 #3498db, stop: 1 #2980b9);
                    border: none;
                    border-radius: 5px;
                    color: white;
                    font-weight: bold;
                    padding: 5px 10px;
                }
                QPushButton:hover {
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                        stop: 0 #2980b9, stop: 1 #1f618d);
                }
                QPushButton:pressed {
                    background: #1f618d;
                }
            """)
        
        self.add_btn.clicked.connect(self.add_item)
        self.delete_btn.clicked.connect(self.delete_item)
        
        btn_layout.addWidget(self.add_btn)
        btn_layout.addWidget(self.delete_btn)
        btn_layout.addStretch()
        
        layout.addWidget(self.list_widget)
        layout.addLayout(btn_layout)
        
        # 添加默认值
        for value in self.default_values:
            self.add_item_with_value(value)
            
    def add_item(self):
        if self.item_type == str:
            text, ok = QInputDialog.getText(self, "添加项目", "请输入文本:")
            if ok and text:
                self.add_item_with_value(text)
        elif self.item_type == int:
            value, ok = QInputDialog.getInt(self, "添加项目", "请输入整数:")
            if ok:
                self.add_item_with_value(value)
        elif self.item_type == float:
            value, ok = QInputDialog.getDouble(self, "添加项目", "请输入浮点数:")
            if ok:
                self.add_item_with_value(value)
        elif self.item_type == bool:
            items = ["True", "False"]
            item, ok = QInputDialog.getItem(self, "添加项目", "选择布尔值:", items, 0, False)
            if ok and item:
                self.add_item_with_value(item == "True")
        
    def add_item_with_value(self, value):
        item = QListWidgetItem(str(value))
        item.setData(Qt.UserRole, value)
        self.list_widget.addItem(item)
        
    def delete_item(self):
        current_row = self.list_widget.currentRow()
        if current_row >= 0:
            self.list_widget.takeItem(current_row)
            
    def get_values(self):
        values = []
        for i in range(self.list_widget.count()):
            item = self.list_widget.item(i)
            value = item.data(Qt.UserRole)
            values.append(value)
        return values

class ConfigWindow(QMainWindow):
    # 添加关闭信号
    windowClosed = pyqtSignal(dict)

    def __init__(self, title: str, config_list : list, parent=None):
        super().__init__(parent)
        self.title = title
        self.config_list = config_list
        self.drag_start_position = None
        self.final_values = None
        self.init_ui()
        
    def init_ui(self):
        # 移除默认标题栏
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint)
        
        # 设置窗口样式 - 黑色主题
        self.setStyleSheet("""
            QMainWindow {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #2c3e50, stop: 1 #34495e);
                border: 1px solid #1c2833;
                border-radius: 8px;
            }
        """)
        
        # 创建主窗口部件
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        
        # 主布局
        main_layout = QVBoxLayout(main_widget)
        main_layout.setSpacing(0)
        main_layout.setContentsMargins(1, 1, 1, 1)
        
        # 添加自定义标题栏
        self.title_bar = CustomTitleBar(self, self.title)
        main_layout.addWidget(self.title_bar)
        
        # 创建内容区域
        content_widget = QWidget()
        content_layout = QVBoxLayout(content_widget)
        content_layout.setSpacing(8)
        content_layout.setContentsMargins(15, 12, 15, 15)
        
        # 设置内容区域背景色
        content_widget.setStyleSheet("background: transparent;")
        
        # 创建滚动区域
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        scroll_area.setWidget(content_widget)
        scroll_area.setFrameShape(QFrame.NoFrame)
        scroll_area.setStyleSheet("""
            QScrollArea { 
                border: none; 
                background: transparent;
            }
            QScrollBar:vertical {
                background: #2c3e50;
                width: 12px;
                margin: 0px;
            }
            QScrollBar::handle:vertical {
                background: #5d6d7e;
                border-radius: 6px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background: #85929e;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                border: none;
                background: none;
            }
        """)
        
        main_layout.addWidget(scroll_area)
        
        # 存储所有控件的引用
        self.controls = {}
        self.slider_connections = {}
        
        # 为每个配置项创建控件
        for i, item in enumerate(self.config_list):
            name = item[0]
            
            # 检查是否是分割线
            if len(item) >= 2 and item[1] == "separator":
                # 创建分割线
                separator = SeparatorWidget(name)
                content_layout.addWidget(separator)
                continue
            
            # 特殊处理颜色和列表类型
            if item[1] == "color":
                value_type = "color"
                default_value = item[2] if len(item) > 2 else "#3498db"
            elif item[1] == "list":
                value_type = "list"
                default_values = item[2] if len(item) > 2 else []
                item_type = item[3] if len(item) > 3 else str
            else:
                value_type = type(item[1])
            
            # 创建配置项框架 - 黑色主题
            item_frame = QFrame()
            item_frame.setFrameShape(QFrame.StyledPanel)
            item_frame.setStyleSheet("""
                QFrame {
                    background: #2c3e50;
                    border: 2px solid transparent;
                    border-radius: 8px;
                    padding: 0px;
                }
                QFrame:hover {
                    border: 2px solid #3498db;
                    background: #34495e;
                }
            """)
            
            # 创建水平布局用于名称和输入控件
            h_layout = QHBoxLayout(item_frame)
            h_layout.setContentsMargins(12, 10, 12, 10)
            h_layout.setSpacing(12)
            
            # 名称标签
            label = QLabel(name)
            label.setMinimumWidth(100)
            label.setMaximumWidth(150)
            label.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Preferred)
            label.setFont(QFont("Microsoft YaHei", 10, QFont.Bold))
            label.setStyleSheet("QLabel { color: #ecf0f1; background: transparent; padding: 0px; }")
            h_layout.addWidget(label)
            
            # 根据类型创建不同的控件
            if value_type == bool:
                checkbox = QCheckBox()
                checkbox.setChecked(item[1])
                checkbox.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
                checkbox.setStyleSheet("""
                    QCheckBox {
                        color: #ecf0f1;
                        background: transparent;
                    }
                    QCheckBox::indicator {
                        width: 16px;
                        height: 16px;
                        border: 2px solid #5d6d7e;
                        border-radius: 3px;
                        background: #2c3e50;
                    }
                    QCheckBox::indicator:checked {
                        background: #3498db;
                        border-color: #3498db;
                    }
                    QCheckBox::indicator:hover {
                        border-color: #3498db;
                    }
                """)
                h_layout.addWidget(checkbox)
                h_layout.addStretch()
                self.controls[name] = checkbox
                
            elif value_type == int:
                int_layout = QHBoxLayout()
                int_layout.setSpacing(8)
                
                spinbox = QSpinBox()
                spinbox.setValue(item[1])
                spinbox.setRange(item[2], item[3]) if len(item) >= 4 else spinbox.setRange(-10000, 10000)
                spinbox.setStyleSheet("""
                    QSpinBox {
                        background-color: #2c3e50;
                        border: 2px solid #5d6d7e;
                        border-radius: 6px;
                        color: #ecf0f1;
                        padding: 5px 10px;
                        font-size: 12px;
                    }
                    QSpinBox:focus {
                        border-color: #3498db;
                    }
                    QSpinBox::up-button, QSpinBox::down-button {
                        background-color: #34495e;
                        border: none;
                        border-radius: 3px;
                        width: 16px;
                    }
                    QSpinBox::up-button:hover, QSpinBox::down-button:hover {
                        background-color: #3498db;
                    }
                """)
                
                slider = QSlider(Qt.Horizontal)
                slider.setRange(spinbox.minimum(), spinbox.maximum())
                slider.setValue(item[1])
                slider.setStyleSheet("""
                    QSlider::groove:horizontal {
                        border: 1px solid #5d6d7e;
                        height: 6px;
                        background: #34495e;
                        border-radius: 3px;
                    }
                    QSlider::handle:horizontal {
                        background: #3498db;
                        border: 2px solid #2c3e50;
                        width: 16px;
                        margin: -5px 0;
                        border-radius: 8px;
                    }
                    QSlider::handle:horizontal:hover {
                        background: #2980b9;
                    }
                    QSlider::sub-page:horizontal {
                        background: #3498db;
                        border-radius: 3px;
                    }
                """)
                
                spinbox.valueChanged.connect(slider.setValue)
                slider.valueChanged.connect(spinbox.setValue)
                
                int_layout.addWidget(spinbox)
                int_layout.addWidget(slider)
                int_layout.setStretchFactor(slider, 1)
                h_layout.addLayout(int_layout)
                
                self.controls[name] = spinbox
                
            elif value_type == float:
                float_layout = QHBoxLayout()
                float_layout.setSpacing(8)
                
                doublespinbox = QDoubleSpinBox()
                doublespinbox.setValue(item[1])
                doublespinbox.setDecimals(4)
                doublespinbox.setSingleStep(0.01)
                doublespinbox.setRange(item[2], item[3]) if len(item) >= 4 else doublespinbox.setRange(-100.0, 100.0)
                doublespinbox.setStyleSheet("""
                    QDoubleSpinBox {
                        background-color: #2c3e50;
                        border: 2px solid #5d6d7e;
                        border-radius: 6px;
                        color: #ecf0f1;
                        padding: 5px 10px;
                        font-size: 12px;
                    }
                    QDoubleSpinBox:focus {
                        border-color: #3498db;
                    }
                    QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                        background-color: #34495e;
                        border: none;
                        border-radius: 3px;
                        width: 16px;
                    }
                    QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
                        background-color: #3498db;
                    }
                """)
                
                slider = QSlider(Qt.Horizontal)
                scale_factor = 1000
                min_range = int(item[2] * scale_factor) if len(item) >= 4 else int(doublespinbox.minimum() * scale_factor)
                max_range = int(item[3] * scale_factor) if len(item) >= 4 else int(doublespinbox.maximum() * scale_factor)
                slider.setRange(min_range, max_range)
                slider.setValue(int(item[1] * scale_factor))
                slider.setStyleSheet("""
                    QSlider::groove:horizontal {
                        border: 1px solid #5d6d7e;
                        height: 6px;
                        background: #34495e;
                        border-radius: 3px;
                    }
                    QSlider::handle:horizontal {
                        background: #3498db;
                        border: 2px solid #2c3e50;
                        width: 16px;
                        margin: -5px 0;
                        border-radius: 8px;
                    }
                    QSlider::handle:horizontal:hover {
                        background: #2980b9;
                    }
                    QSlider::sub-page:horizontal {
                        background: #3498db;
                        border-radius: 3px;
                    }
                """)
                
                def create_slider_update_func(spinbox, slider, factor):
                    return lambda value: slider.setValue(int(value * factor))
                
                def create_spinbox_update_func(spinbox, slider, factor):
                    return lambda value: spinbox.setValue(value / factor)
                
                spinbox_to_slider = create_slider_update_func(doublespinbox, slider, scale_factor)
                slider_to_spinbox = create_spinbox_update_func(doublespinbox, slider, scale_factor)
                
                doublespinbox.valueChanged.connect(spinbox_to_slider)
                slider.valueChanged.connect(slider_to_spinbox)
                
                self.slider_connections[name] = (spinbox_to_slider, slider_to_spinbox)
                
                float_layout.addWidget(doublespinbox)
                float_layout.addWidget(slider)
                float_layout.setStretchFactor(slider, 1)
                h_layout.addLayout(float_layout)
                
                self.controls[name] = doublespinbox
                
            elif value_type == str:
                line_edit = QLineEdit()
                line_edit.setText(item[1])
                line_edit.setStyleSheet("""
                    QLineEdit {
                        background-color: #2c3e50;
                        border: 2px solid #5d6d7e;
                        border-radius: 6px;
                        color: #ecf0f1;
                        padding: 5px 10px;
                        font-size: 12px;
                    }
                    QLineEdit:focus {
                        border-color: #3498db;
                    }
                    QLineEdit[invalid="true"] {
                        border-color: #e74c3c;
                    }
                """)
                
                if len(item) >= 3 and not item[2]:
                    self.validate_non_empty(line_edit.text(), line_edit)
                    line_edit.textChanged.connect(lambda text, le=line_edit: self.validate_non_empty(text, le))
                
                h_layout.addWidget(line_edit)
                self.controls[name] = line_edit
                
            elif value_type == "color":
                color_picker = ColorPickerWidget(default_value)
                h_layout.addWidget(color_picker)
                self.controls[name] = color_picker
                
            elif value_type == "list":
                list_widget = ListConfigWidget(item_type, default_values)
                h_layout.addWidget(list_widget)
                self.controls[name] = list_widget
                
            else:
                unknown_label = QLabel(f"未知类型: {value_type}")
                unknown_label.setStyleSheet("color: #ecf0f1;")
                h_layout.addWidget(unknown_label)
            
            content_layout.addWidget(item_frame)
        
        content_layout.addStretch()
        
        # 禁止调整窗口大小
        self.setFixedSize(self.calculate_window_size())
        
    def calculate_window_size(self):
        base_width = 600
        # 计算分割线的数量
        separator_count = sum(1 for item in self.config_list if len(item) >= 2 and item[1] == "separator")
        # 分割线高度较小，所以减少每个分割线的高度影响
        base_height = min(400 + (len(self.config_list) - separator_count) * 65 + separator_count * 25, 700)
        return QSize(base_width, base_height)
        
    def validate_non_empty(self, text, line_edit):
        if not text.strip():
            line_edit.setProperty("invalid", "true")
        else:
            line_edit.setProperty("invalid", "false")
        line_edit.setStyleSheet(line_edit.styleSheet())
    
    def showEvent(self, event):
        super().showEvent(event)
        self.animate_window(show=True)

    def close_with_animation(self):
        # 在关闭前保存最终值
        self.final_values = self.get_values()
        # 发射关闭信号
        self.windowClosed.emit(self.final_values)
        self.animate_window(show=False)

    def closeEvent(self, event):
        # 确保有最终值
        if self.final_values is None:
            self.final_values = self.get_values()
        # 发射关闭信号
        self.windowClosed.emit(self.final_values)
        event.accept()

    def animate_window(self, show=True):
        if show:
            # 显示动画
            self.fade_animation = QPropertyAnimation(self, b"windowOpacity")
            self.fade_animation.setDuration(400)
            self.fade_animation.setStartValue(0.0)
            self.fade_animation.setEndValue(1.0)
            self.fade_animation.setEasingCurve(QEasingCurve.OutCubic)
            
            screen_geo = QApplication.primaryScreen().availableGeometry()
            start_rect = QRect(
                screen_geo.center().x() - 100,
                screen_geo.center().y() - 75,
                200, 150
            )
            
            self.geometry_animation = QPropertyAnimation(self, b"geometry")
            self.geometry_animation.setDuration(500)
            self.geometry_animation.setStartValue(start_rect)
            self.geometry_animation.setEndValue(self.geometry())
            self.geometry_animation.setEasingCurve(QEasingCurve.OutBack)
            
            self.fade_animation.start()
            self.geometry_animation.start()
        else:
            # 关闭动画
            self.fade_animation = QPropertyAnimation(self, b"windowOpacity")
            self.fade_animation.setDuration(300)
            self.fade_animation.setStartValue(1.0)
            self.fade_animation.setEndValue(0.0)
            self.fade_animation.setEasingCurve(QEasingCurve.InCubic)
            
            screen_geo = QApplication.primaryScreen().availableGeometry()
            end_rect = QRect(
                screen_geo.center().x() - 50,
                screen_geo.center().y() - 40,
                100, 80
            )
            
            self.geometry_animation = QPropertyAnimation(self, b"geometry")
            self.geometry_animation.setDuration(400)
            self.geometry_animation.setStartValue(self.geometry())
            self.geometry_animation.setEndValue(end_rect)
            self.geometry_animation.setEasingCurve(QEasingCurve.InBack)
            
            self.fade_animation.finished.connect(self.close)
            
            self.fade_animation.start()
            self.geometry_animation.start()
    
    def get_values(self):
        """获取所有配置项的当前值"""
        values = {}
        for name, control in self.controls.items():
            if isinstance(control, QCheckBox):
                values[name] = control.isChecked()
            elif isinstance(control, (QSpinBox, QDoubleSpinBox)):
                values[name] = control.value()
            elif isinstance(control, QLineEdit):
                values[name] = control.text()
            elif isinstance(control, ColorPickerWidget):
                values[name] = control.get_color()
            elif isinstance(control, ListConfigWidget):
                values[name] = control.get_values()
        return values
    
    def get_result(self):
        """获取配置结果，用于非阻塞模式"""
        return self.final_values
    
    def wait_for_result(self):
        """等待窗口关闭并返回结果（非阻塞方式）"""
        loop = QEventLoop()
        self.windowClosed.connect(loop.quit)
        loop.exec_()
        return self.final_values


def show_config_window(title: str = "Config", config_list: list = [], parent=None):
    """显示配置窗口并返回用户配置"""
    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
    
    font = QFont("Microsoft YaHei", 9)
    app.setFont(font)
    
    window = ConfigWindow(title, config_list, parent)
    window.show()
    
    # 确保窗口显示出来
    QApplication.processEvents()
    
    # 如果是直接运行此脚本，则使用事件循环阻塞
    if __name__ == "__main__":
        app.exec_()
        return window.final_values
    else:
        # 作为模块被导入时，显示窗口但不阻塞
        return window


# 测试代码
if __name__ == "__main__":
    # 测试配置列表
    config_list = [
        ["启用功能", True],
        ["音量设置", 1000, 0, 2025],
        ["透明度", 0.5, 0.0, 1.0],
        ["用户名", "默认用户", False],
        ["主题颜色", "color", "#3498db"],  # 颜色配置
        ["数字列表", "list", [1, 2, 3], int],  # 整数列表
        ["字符串列表", "list", ["a", "b", "c"], str],  # 字符串列表
        ["这是一条分割线", "separator"], 
        ["浮点数列表", "list", [0.1, 0.2, 0.3], float]  # 浮点数列表
        
    ]
    
    # 显示窗口并获取配置
    final_config = show_config_window("title", config_list)
    print('x')
    # print("用户最终配置:", list(map(list, final_config.items())))