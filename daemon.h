#ifndef DAEMON_H
#define DAEMON_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <windows.h>

class Daemon : public QWidget
{
    Q_OBJECT
public:
    explicit Daemon(QWidget *parent = nullptr);

protected:
    void setAutoRun(bool isAutoRun);
    void detectProcess();

private:
    void Wchar_tToString(std::string& szDst, wchar_t *wchar);
    bool checkProcessByName(const QString &strExe);
    bool isAutoRun();

    QSystemTrayIcon *m_systemTray;
    QMenu *m_menu;
    QAction *m_autoRunAction;
    QAction *m_exitAction;
    QTimer *m_timer;
};

#endif // DAEMON_H
