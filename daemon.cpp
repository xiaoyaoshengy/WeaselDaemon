#include "daemon.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <tlhelp32.h>
#include <QFileDialog>
#include <QProcess>

Daemon::Daemon(QWidget *parent)
    : QWidget{parent}
{
    // 窗口设置
    this->setWindowFlags(Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_QuitOnClose);

    // 系统托盘
    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/icon/icon.png"));
    m_systemTray->setToolTip(QString("小狼毫算法服务守护进程"));

    m_menu = new QMenu(this);
    m_autoRunAction = new QAction(QString("开机自启"), m_menu);
    m_autoRunAction->setCheckable(true);
    m_exitAction = new QAction(QString("退出"), m_menu);
    m_menu->addAction(m_autoRunAction);
    m_menu->addAction(m_exitAction);

    // 检查是否已经设置开机自启
    if (this->isAutoRun()) m_autoRunAction->setChecked(true);

    m_systemTray->setContextMenu(m_menu);
    m_systemTray->show();

    connect(m_autoRunAction, &QAction::triggered, this, &Daemon::setAutoRun);
    connect(m_exitAction, &QAction::triggered, this, &QApplication::quit);

    // 计时器
    m_timer = new QTimer(this);
    m_timer->setInterval(3000);
    m_timer->start();
    connect(m_timer, &QTimer::timeout, this, &Daemon::detectProcess);
}

void Daemon::setAutoRun(bool isAutoRun)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString appPath = QApplication::applicationFilePath();
    QFileInfo fInfo(appPath);
    QString name = fInfo.baseName();
    QString oldPath = settings.value(name).toString();
    QString newPath = QDir::toNativeSeparators(appPath);

    if (isAutoRun)
    {
        if (oldPath != newPath) settings.setValue(name, newPath);
    }
    else
        settings.remove(name);
}

void Daemon::detectProcess()
{
    QString parentPath = QApplication::applicationDirPath();
    QSettings settings(parentPath + QString("/sys.ini"), QSettings::IniFormat);
    QString targetExePath = settings.value(QString("target")).toString();
    if ("" == targetExePath)
    {
        // 弹出提示信息并退出
        this->m_systemTray->showMessage(QString("错误"), QString("sys.ini文件缺失或格式错误"), QSystemTrayIcon::Critical);
        QApplication::quit();
    }
    bool isExist = checkProcessByName(targetExePath.split("/").last());  // 查询该进程
    if (!isExist)
    {
        if (!QProcess::startDetached(targetExePath))
        {
            this->m_systemTray->showMessage(QString("错误"), QString("未能成功运行「WeaselServer.exe」"), QSystemTrayIcon::Critical);
            QApplication::quit();
        }
    }
    m_timer->start();
}

void Daemon::Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
    wchar_t *wText = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
    char *psText;  // psText为char*的临时数组，作为赋值给std::string的中间变量
    psText = new char[dwNum];
    WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
    szDst = psText;
    delete [] psText;
}

bool Daemon::checkProcessByName(const QString &strExe)
{
    bool bResult = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return NULL;
    PROCESSENTRY32 pe = { sizeof(pe) };
    for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
    {
        wchar_t *process_str = pe.szExeFile;
        std::string current_process_name;
        Wchar_tToString(current_process_name, process_str);
        if (current_process_name == strExe.toStdString())
        {
            bResult = true;
            break;
        }
    }
    CloseHandle(hSnapshot);
    return bResult;
}

bool Daemon::isAutoRun()
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString appPath = QApplication::applicationFilePath();
    QFileInfo fInfo(appPath);
    QString name = fInfo.baseName();
    return settings.contains(name);
}
