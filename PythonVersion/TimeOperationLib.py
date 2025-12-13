import os
import sys
import time
import ntplib
from datetime import datetime, timezone, timedelta
import win32api
import logging
import ctypes
import subprocess
from tkinter.messagebox import *


# def is_admin() -> bool:
#     """ 检查当前进程是否为管理员 """
#     return ctypes.windll.shell32.IsUserAnAdmin() != 0


def sync_time_with_ntp() -> bool:
    """
    通过NTP服务器获取网络时间并同步
    返回 True 表示成功，False 表示失败
    """
    try:
        # 创建一个NTPClient
        client = ntplib.NTPClient()
        # 从阿里云NTP服务器请求时间（也可以用 pool.ntp.org 或其他）
        response = client.request('ntp.aliyun.com')
        # 将NTP时间戳转换为UTC时间
        ntp_time_utc = datetime.fromtimestamp(response.tx_time, timezone.utc)
        # 转换为本地时间
        ntp_time_local = ntp_time_utc.astimezone()
        logging.info(f"从NTP服务器获取的时间: {ntp_time_local}")

        # 转换为系统设置时间所需的格式 (YYYY, MM, DD, HH, MM, SS, Weekday, Day-of-year, Is-DST)
        # 注意：win32api.SetSystemTime 使用的是UTC时间
        system_time_utc = ntp_time_utc.replace(tzinfo=None)
        win32api.SetSystemTime(
            system_time_utc.year,
            system_time_utc.month,
            system_time_utc.isoweekday() % 7, # Windows期望0=周一，6=周日。isoweekday()是1=周一，7=周日，取模后对齐。
            system_time_utc.day,
            system_time_utc.hour,
            system_time_utc.minute,
            system_time_utc.second,
            int(system_time_utc.microsecond / 1000) # 毫秒
        )
        logging.info("系统时间已成功同步！")
        return True
    except Exception as e:
        logging.error(f"同步失败: {e}")
        # 备用方案：调用系统的w32tm命令
        logging.debug("正在尝试使用系统命令同步...")
        result = os.system('w32tm /resync /force')
        if result == 0:
            logging.info("系统命令同步成功！")
            return True
        else:
            logging.error("系统命令同步也失败了，请以管理员权限运行。")
            return False


def get_time_info() -> str:
    """ 获取时间信息 """
    return f"当前时间戳: {time.time()}\n时间戳是1970年1月1日 00:00:00 UTC（纪元时间）以来经过的秒数。\nUTC（世界协调时）格式化时间：{datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')}\nUTC+8格式化时间（北京时间）：{datetime.now(timezone(timedelta(hours=8))).strftime('%Y-%m-%d %H:%M:%S UTC+8')}"


def get_time_zone_info() -> str:
    """ 获取时区信息 """
    return f"当前时区：{time.tzname[0]}\n{time.tzname[1]}\n{os.popen('tzutil /g').read().strip()}\n时区偏移量：{time.timezone}秒"


def get_time_zone_list() -> list:
    """ 获取所有时区列表 """
    zone_list = os.popen('tzutil /l').read().splitlines()

    # 格式化，去除末尾空格
    for i in range(len(zone_list)):
        zone_list[i] = zone_list[i].strip()

    return zone_list


def set_time_zone(desired_timezone_id: str) -> bool:
    """ 设置系统时区，成功返回True，失败返回False """
    exit_code = os.system(f'tzutil /s "{desired_timezone_id}"')
    if exit_code == 0:
        logging.info(f"系统时区已成功设置为 {desired_timezone_id}，命令：tzutil /s \"{desired_timezone_id}\"")
        return True
    else:
        logging.error(f"设置系统时区失败，返回代码：{exit_code}，命令：tzutil /s \"{desired_timezone_id}\"")
        return False


# # 检查管理员权限
# if sys.platform == 'win32' and not is_admin():
#     logging.error("请以管理员身份运行此脚本！")
#     raise Exception("Use administrator privileges to run this script.")