# WeaselDaemon
自用「小狼毫」进程守护程序

## 基本原理
- 每隔 3 秒搜索是否有「WeaselServer.exe」进程运行

  - 如果存在该进程，则什么也不做

  - 如果不存在该进程，则运行「小狼毫」安装目录中的「WeaselServer.exe」程序

- 「开机自启」功能采用在注册表路径注册该程序的方法实现

  - 注册表路径 `HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run`

  - 注册表键值 `WeaselDaemon: D:\path\to\WeaselDaemon.exe`

## 注意事项⚠
- 「Release」中的压缩包直接解压，运行「WeaselDaemon.exe」即可，将在任务栏中出现「齿轮」图标；卸载建议先取消「开机自启」删除注册表项，然后直接删除所有解压文件即可

- 在「WeaselDaemon.exe」同级目录的「sys.ini」文件中，需要修改 `target=D:\path\to\WeaselServer.exe` 一行，将 `target=` 后的路径修改为你的「WeaselServer.exe」路径，一般在「小狼毫」的安装目录下

- 程序启动后会检查「sys.ini」文件是否存在以及格式是否正确，若存在问题，将会在提示后退出程序；程序若未能成功启动「WeaselServer.exe」，会提示并退出程序

  - PS：注意不要随意设置 `target=` 后的程序路径，否则很可能出现每 3 秒给你运行一次程序的问题，然后内存占用飞速上升，甚至出现死机！！！

## 构建
- 本程序基于「Qt-6.5.3」，使用「MinGW-11.2.0 64-bit」编译，使用「windeployqt.exe」部署
