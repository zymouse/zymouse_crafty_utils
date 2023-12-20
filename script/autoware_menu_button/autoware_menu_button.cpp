#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QClipboard>
#include <QMap>

#include <QProcess>
#include <QProgressDialog>
#include <QMessageBox>
#include <QMainWindow>


// 假设这些函数是你的工具函数，用于读取代理号码和获取网络接口
QString readProxyNumber(const QString &filename);
QMap<QString, QString> getNetworkInterfaces();
QProcess *process = nullptr;
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建系统托盘图标
    QSystemTrayIcon trayIcon;

    // 创建菜单和菜单项
    QMenu menu;
    QMainWindow mainWindow;
    
    // 添加自定义的菜单项
    QAction *exitAction = new QAction("退出", &menu);

    QAction *autowareRestartAction = new QAction("autoware恢复", &menu);
    QAction *autowareStartAction = new QAction("autoware启动", &menu);
    QAction *autowareStopAction = new QAction("autoware关闭", &menu);
    QAction *ipcRestartAction = new QAction("IPC重启", &menu);
    QAction *ipcStopAction = new QAction("IPC关机", &menu);
    QAction *quickRecoverAction = new QAction("一键检修", &menu);

    // 添加菜单项到菜单
    menu.addAction(autowareRestartAction);
    menu.addAction(autowareStartAction);
    menu.addAction(autowareStopAction);
    menu.addSeparator(); // 添加一个分隔符
    menu.addAction(ipcRestartAction);
    menu.addAction(ipcStopAction);
    menu.addSeparator(); // 添加另一个分隔符
    menu.addAction(quickRecoverAction);

    menu.addSeparator();
    menu.addAction(exitAction);

    QString homePath = QString(qgetenv("HOME"));

    // 连接退出菜单项的触发信号到应用程序的quit槽
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);

    // 连接菜单项的信号到具体的槽函数（这里需要你实现具体的功能）
    QObject::connect(autowareRestartAction, &QAction::triggered, [&autowareStartAction, homePath](){
        // 实现 autoware 重启的功能
        QProcess *process = new QProcess();

        // 连接 process 的 finished 信号以检查脚本执行结果
        QObject::connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                        [autowareStartAction, process](int exitCode, QProcess::ExitStatus exitStatus){
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                // 脚本成功执行
                autowareStartAction->setEnabled(true); // 可点击
                QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
            } else {
                // 脚本执行失败
                autowareStartAction->setEnabled(false); // 不可点击
                QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);
            }
            process->deleteLater(); // 完成后将 QProcess 对象标记为删除
        });

        // 开始执行脚本
        process->start("sh", QStringList() 
            << "-i" << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh");
    });

    // 连接 autoware 启动的信号到具体的槽函数
    QObject::connect(autowareStartAction, &QAction::triggered, [&autowareStartAction, homePath](){
        // 实现 autoware 启动的功能
        autowareStartAction->setEnabled(false); // 不可点击
        QProcess::execute("sh", QStringList() 
            << "-i" << "-c"  
            << homePath + "/pix/scripts/scripts/robobus.sh");
    });

    // 连接 autoware 关闭的信号到具体的槽函数
    QObject::connect(autowareStopAction, &QAction::triggered, [&homePath](){
        // 实现 autoware 关闭的功能
        QProcess::execute("sh", QStringList() 
            << "-i" << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/close_autoware.sh");
    });

    // 连接 IPC 重启的信号到具体的槽函数
    QObject::connect(ipcRestartAction, &QAction::triggered, [&](){
        // 实现 IPC 重启的功能
        QProcess::execute("sh", QStringList() 
            << "-i" << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/ipc_reboot.sh");
    });

    // 连接 IPC 关闭的信号到具体的槽函数
    QObject::connect(ipcStopAction, &QAction::triggered, [&](){
        // 实现 IPC 关机的功能
        QProcess::execute("sh", QStringList() 
            << "-i" << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/ipc_poweroff.sh");
    });

    // 连接一键恢复的信号到具体的槽函数
    QObject::connect(quickRecoverAction, &QAction::triggered, [&](){
        // 实现一键恢复的功能
    });

    // 设置系统托盘图标的菜单
    trayIcon.setContextMenu(&menu);

    // 设置系统托盘图标（替换为你的图标路径）
    trayIcon.setIcon(QIcon(":/PIXMOVING.png"));
    trayIcon.show();
    // mainWindow.show();
    // 禁止在关闭主窗口时退出应用程序
    app.setQuitOnLastWindowClosed(false);

    return app.exec();
}
