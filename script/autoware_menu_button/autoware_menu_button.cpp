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

#include <QFile>
#include <QTextStream>

// 假设这些函数是你的工具函数，用于读取代理号码和获取网络接口
QString readProxyNumber(const QString &filename);
QMap<QString, QString> getNetworkInterfaces();

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建系统托盘图标
    QSystemTrayIcon trayIcon;

    // 创建菜单和菜单项
    QMainWindow mainWindow;
    QMenu menu;
    
    // 添加自定义的菜单项
    QAction *exitAction = new QAction("exit", &menu);
    QAction *autowareRestartAction = new QAction("AUTOWARE-RESET", &menu);
    QAction *autowareStartAction = new QAction("AUTOWARE-RUN", &menu);
    QAction *autowareStopAction = new QAction("AUTOWARE-STOP", &menu);
    QAction *rosNodeStop = new QAction("ROS-NODE-kill", &menu);
    QAction *ipcStopAction = new QAction("IPC关机", &menu);
    QAction *oneClickDetection = new QAction("ONE-CLICK-DETECTION", &menu);
    
    // 添加自定义的菜单项的脚本处理项
    QProcess *autowareRestartProcess = new QProcess(&menu);
    QProcess *autowareStartProcess = new QProcess(&menu);
    QProcess *autowareStopProcess = new QProcess(&menu);
    QProcess *rosNodeStopProcess = new QProcess(&menu);
    QProcess *ipcStopProcess = new QProcess(&menu);
    QProcess *oneClickDetectionProcess = new QProcess(&menu);

    // 添加进度条弹窗
    QProgressDialog *progressDialog1 = new QProgressDialog("autoware启动脚本正在执行...", "取消", 0, 0, &menu);
    QProgressDialog *progressDialog2 = new QProgressDialog("autoware停止脚本正在执行...", "取消", 0, 0, &menu);
    
    progressDialog1->setWindowModality(Qt::WindowModal);
    progressDialog2->setWindowModality(Qt::WindowModal);
    progressDialog1->close(); // 关闭进度弹窗
    progressDialog2->close(); // 关闭进度弹窗


    // 添加菜单项到菜单
    menu.addAction(autowareRestartAction);
    menu.addAction(autowareStartAction);
    menu.addAction(autowareStopAction);
    menu.addSeparator(); // 添加一个分隔符
    menu.addAction(rosNodeStop);
    menu.addAction(ipcStopAction);
    menu.addSeparator(); // 添加另一个分隔符
    menu.addAction(oneClickDetection);
    menu.addSeparator(); // 添加另一个分隔符
    menu.addAction(exitAction);

    QString homePath = QString(qgetenv("HOME"));
    autowareStartAction->setEnabled(false); // 不可点击
    rosNodeStop->setEnabled(false); // 不可点击
    
    // autowareStopAction 点击次数
    uint8_t autowareStopClickNumber = 0;

    // 连接退出菜单项的触发信号到应用程序的quit槽
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);

    // -----------------------------------------------------------------连接菜单项的信号到具体的槽函数 -- 非阻塞
    QObject::connect(autowareRestartAction, &QAction::triggered, [&](){
        // 实现 autoware 恢复的功能
        autowareRestartProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh");
    });

    // 连接菜单项的信号到具体的槽函数 -- 非阻塞
    QObject::connect(autowareRestartProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            autowareStartAction->setEnabled(true); // 可点击
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            autowareStartAction->setEnabled(false); // 不可点击
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = autowareRestartProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh";
                // 关闭文件
                errorLogFile.close();
            }
        } 
    });
    // -----------------------------------------------------------------

    // -----------------------------------------------------------------连接 autoware 启动的信号到具体的槽函数 -- 非阻塞
    QObject::connect(autowareStartAction, &QAction::triggered, [&](){
        // 显示进度弹窗
        progressDialog1->show();

        // 实现 autoware 启动的功能
        autowareStartAction->setEnabled(false); // 不可点击 

        // 开始执行脚本
        autowareStartProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/robobus.sh");
    });

    // 连接 QProgressDialog1 的 canceled 信号到一个 lambda 表达式
    QObject::connect(progressDialog1, &QProgressDialog::canceled, [&]() {
        if (autowareStartProcess->state() == QProcess::Running) {
            autowareStartProcess->terminate(); // 终止正在运行的进程
        }
    });

    // 连接 autowareStartProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(autowareStartProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        
        progressDialog1->close(); // 关闭进度弹窗
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
            
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = autowareStartProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/robobus.sh";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------连接 autoware 关闭的信号到具体的槽函数
    QObject::connect(autowareStopAction, &QAction::triggered, [&](){
        // 显示进度弹窗
        // progressDialog2->show();

        autowareStopClickNumber++;
        if(autowareStopClickNumber>=3){
            autowareStopClickNumber = 0;
            rosNodeStop->setEnabled(true); // 不可点击
        }

        // 实现 autoware 关闭的功能
        autowareStopProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/close_autoware.sh");
    });

    // 连接 QProgressDialog 的 canceled 信号到一个 lambda 表达式
    // QObject::connect(progressDialog2, &QProgressDialog::canceled, [&]() {
    //     if (autowareStopProcess->state() == QProcess::Running) {
    //         autowareStopProcess->terminate(); // 终止正在运行的进程
    //     }
    // });

    // 连接 autowareStopProcess 的 finished 信号以检查脚本执行结果
    // QObject::connect(autowareStopProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
    //                 [&](int exitCode, QProcess::ExitStatus exitStatus){
    //     progressDialog2->close(); // 关闭进度弹窗
    //     if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    //         // 脚本成功执行
    //         QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
    //     } else {
    //         // 脚本执行失败
    //         QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

    //         // 获取标准错误内容并将其记录到日志或显示给用户
    //         QByteArray errorOutput = autowareStopProcess->readAllStandardError();
    //         QFile errorLogFile(homePath + "/error.log");
    //         if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
    //             QTextStream errorLogStream(&errorLogFile);
    //             // 将标准错误内容写入文件
    //             errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh";
    //             // 关闭文件
    //             errorLogFile.close();
    //         }
    //     }
    // });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------
    // 连接 rosnode kill 的信号到具体的槽函数
    QObject::connect(rosNodeStop, &QAction::triggered, [&](){
        // 实现 rosnode kill 的功能
        rosNodeStopProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/kill_ros.py");
    });
    // 连接 rosNodeStopProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(rosNodeStopProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            rosNodeStop->setEnabled(false); // 不可点击
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            rosNodeStop->setEnabled(true); // 不可点击
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = rosNodeStopProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------连接 IPC 关闭的信号到具体的槽函数
    QObject::connect(ipcStopAction, &QAction::triggered, [&](){
        // 实现 IPC 关机的功能
        ipcStopProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/ipc_poweroff.sh");

        QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
    });
    // 连接 ipcStopProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(ipcStopProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = ipcStopProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_ros_topics.sh";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------
    // 连接一键恢复的信号到具体的槽函数
    QObject::connect(oneClickDetection, &QAction::triggered, [&](){
        // 实现一键恢复的功能
        oneClickDetectionProcess->start("bash", QStringList() 
            << "-c"  
            << homePath + "/pix/scripts/scripts/autoware_menu_button/one_click_detection.sh");
    });
    // 连接 oneClickDetectionProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(oneClickDetectionProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = ipcStopProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/one_click_detection.sh";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


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
