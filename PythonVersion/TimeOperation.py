"""
Time Operation
时间操作
包含UI界面

"""

from TimeOperationLib import *
from tkinter import *
from tkinter.ttk import *
from tkinter.simpledialog import *
import webbrowser
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))


def register_browser() -> None:
    """ 注册Edge浏览器 """
    try:
        webbrowser.register('edge', None, webbrowser.BackgroundBrowser("msedge.exe"))
        logging.info("Edge浏览器注册成功")
    except Exception as e:
        logging.error(f"Edge浏览器注册失败: {e}")
        raise e





def get_path(relative_path: str) -> str:
    """ 获取资源的路径，以适配pyinstaller打包 """
    try:
        base_path = sys._MEIPASS # pyinstaller打包后的路径
    except AttributeError:
        base_path = os.path.abspath(".") # 当前工作目录的路径
 
    return os.path.normpath(os.path.join(base_path, relative_path)) # 返回实际路径



def update_time_info() -> None:
    """ 按更新间隔更新时间信息"""
    global after_id
    now_time_info['text'] = get_time_info()
    after_id = root.after(sleep_time, update_time_info)


def synchronize_time() -> None:
    """ 同步时间 """
    if sync_time_with_ntp():
        showinfo('时间操作 - 提示', '时间同步成功！')
    else:
        showerror('时间操作 - 错误', '时间同步失败！详情请见日志！')


def update_sleep() -> None:
    """ 修改时间更新间隔 """
    global sleep_time, after_id
    n = askinteger('时间操作 - 提示', f'请输入更新间隔（毫秒，必须为正整数，当前更新间隔：{sleep_time} 毫秒，默认更新间隔：100毫秒）')
    if n == None:
        return
    try:
        if n < 1:
            showerror('时间操作 - 错误', '输入的更新间隔必须为正整数！')
        else:
            sleep_time = n
            if after_id:
                root.after_cancel(after_id)
            after_id = root.after(sleep_time, update_time_info)
    except Exception as e:
        logging.error(f"输入的更新间隔有误！详情请见日志！\n{e}")
        showerror('时间操作 - 错误', f'输入的更新间隔有误！详情请见日志！\n{e}')





def try_set_time_zone() -> None:
    """ 尝试根据组合框内容设置系统时区 """
    try:
        flag = False
        for i in zone_list:
            if flag:
                if set_time_zone(i):
                    showinfo('时间操作 - 提示', f'时区设置成功！当前时区：\n{cmb.get()}\n{i}\n')
                else:
                    logging.error(f"时区设置失败！")
                    showerror('时间操作 - 错误', f'时区设置失败！详情请见日志！')
                    raise Exception('时区设置失败！请检查你的输入！')
                return
            else:
                if i == cmb.get():
                    flag = True
    except Exception as e:
        logging.error(f"时区设置失败！请检查你的输入！\n{e}\n")
        showerror('时间操作 - 错误', f'时区设置失败！请检查你的输入！详情请见日志！')
        return
    logging.error(f'时区设置失败！请检查你的输入！')
    showerror('时间操作 - 错误', f'时区设置失败！请检查你的输入！')
    raise Exception('时区设置失败！请检查你的输入！')
    


def main() -> None:
    global root, zone_list, now_time_info, sleep_time, after_id, cmb
    register_browser()
    sleep_time = 100
    after_id = None

    # root = Tk()
    root = Toplevel()
    root.title(f'YFY工具 - 时间操作')
    root.geometry('700x600')
    root.iconphoto(False, PhotoImage(file=get_path('assets/icon.ico')))

    time_info_frame = LabelFrame(root, text="时间信息")
    time_info_frame.pack()

    now_time_info = Label(time_info_frame, text=get_time_info())
    now_time_info.pack()

    sleep_btn = Button(root, text='调整更新间隔', command=update_sleep)
    sleep_btn.pack()

    update_btn = Button(root, text='同步时间', command=synchronize_time)
    update_btn.pack()

    try:
        base_path = sys._MEIPASS # pyinstaller打包后的路径
        clock_btn = Button(root, text='打开时钟', command=lambda: webbrowser.get('edge').open(f'file://{get_path("assets/clock.html")}'))
        del base_path
    except AttributeError:
        clock_btn = Button(root, text='打开时钟', command=lambda: webbrowser.get('edge').open(f'file://{get_path(os.getcwd() + "/assets/clock.html")}'))
    clock_btn.pack()

    time_zone_frame = LabelFrame(root, text="时区信息")
    time_zone_frame.pack()

    time_zone_info = Label(time_zone_frame, text=get_time_zone_info())
    time_zone_info.pack()

    set_time_zone_lb = Label(time_zone_frame, text='设置系统时区：')
    set_time_zone_lb.pack()

    op = StringVar()
    op.set('')
    zone_list = get_time_zone_list()
    cmb_list = []
    current_timezone = os.popen('tzutil /g').read().strip()
    for i in range(len(zone_list)):
        if zone_list[i] == current_timezone:
            op.set(zone_list[i - 1])
        if zone_list[i] != '':
            if zone_list[i][0] == '(':
                cmb_list.append(zone_list[i])

    cmb = Combobox(time_zone_frame, textvariable=op, values=cmb_list, height=15, width=60)
    cmb.pack()

    del current_timezone, cmb_list # zone_list 保留

    set_time_zone_btn = Button(time_zone_frame, text='设置时区', command=try_set_time_zone)
    set_time_zone_btn.pack()

    set_display_time_zone_btn = Button(time_zone_frame, text='设置显示在本软件上的时区', command=())
    set_display_time_zone_btn.pack()



    update_time_info()
    
    root.mainloop()



if __name__ == '__main__':
    main()