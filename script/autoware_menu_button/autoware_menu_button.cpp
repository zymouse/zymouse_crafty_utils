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
    QAction *hardwareDetectionAction = new QAction("Hardware-Detection", &menu);
    QAction *softwareDetectionAction = new QAction("Software-Detection", &menu);
    
    // 添加自定义的菜单项的脚本处理项
    QProcess *autowareRestartProcess = new QProcess(&menu);
    QProcess *autowareStartProcess = new QProcess(&menu);
    QProcess *autowareStopProcess = new QProcess(&menu);
    QProcess *rosNodeStopProcess = new QProcess(&menu);
    QProcess *hardwareDetectionProcess = new QProcess(&menu);
    QProcess *softwareDetectionProcess = new QProcess(&menu);

    // 添加进度条弹窗
    QProgressDialog *progressDialog1 = new QProgressDialog("autoware启动脚本正在执行...", "取消", 0, 0, &menu);
    QProgressDialog *progressDialog_hardwareDetection = new QProgressDialog("硬件检测脚本正在执行...", "取消", 0, 0, &menu);
    QProgressDialog *progressDialog_softwareDetection = new QProgressDialog("软件检测脚本正在执行...", "取消", 0, 0, &menu);
    
    progressDialog1->setWindowModality(Qt::WindowModal);
    progressDialog_hardwareDetection->setWindowModality(Qt::WindowModal);
    progressDialog_softwareDetection->setWindowModality(Qt::WindowModal);

    progressDialog1->close(); // 关闭进度弹窗
    progressDialog_hardwareDetection->close(); // 关闭进度弹窗
    progressDialog_softwareDetection->close(); // 关闭进度弹窗


    // 添加菜单项到菜单
    menu.addAction(autowareRestartAction);
    menu.addAction(autowareStartAction);
    menu.addAction(autowareStopAction);
    menu.addSeparator(); // 添加一个分隔符
    menu.addAction(rosNodeStop);
    menu.addAction(hardwareDetectionAction);
    menu.addSeparator(); // 添加另一个分隔符
    menu.addAction(softwareDetectionAction);
    menu.addSeparator(); // 添加另一个分隔符
    menu.addAction(exitAction);

    QString homePath = QString(qgetenv("HOME"));
    autowareStartAction->setEnabled(false); // 不可点击
    softwareDetectionAction->setEnabled(false); // 不可点击
    
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
            hardwareDetectionAction->setEnabled(true);  
            softwareDetectionAction->setEnabled(false);  

            autowareStopClickNumber = 0;
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);

        } else {
            // 脚本执行失败
            autowareStartAction->setEnabled(false); // 不可点击
            hardwareDetectionAction->setEnabled(false);  
            softwareDetectionAction->setEnabled(true); 
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = autowareRestartProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/pix/ros-log/autoware_menu_button_error.log");
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
        hardwareDetectionAction->setEnabled(false); // 不可点击
        softwareDetectionAction->setEnabled(true); // 可点击
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
            QFile errorLogFile(homePath + "/pix/ros-log/autoware_menu_button_error.log");
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
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------
    // 连接 rosnode kill 的信号到具体的槽函数
    QObject::connect(rosNodeStop, &QAction::triggered, [&](){
        // 实现 rosnode kill 的功能
        rosNodeStopProcess->start("/usr/bin/python3", QStringList() 
            << homePath + "/pix/scripts/scripts/autoware_menu_button/kill_ros.py");
    });
    // 连接 rosNodeStopProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(rosNodeStopProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = rosNodeStopProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/pix/ros-log/autoware_menu_button_error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/kill_ros.py";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------连接 软件检测的信号到具体的槽函数
    QObject::connect(hardwareDetectionAction, &QAction::triggered, [&](){
        // 显示进度弹窗
        progressDialog_hardwareDetection->show();

        // 实现软件检测的功能
        hardwareDetectionProcess->start("/usr/bin/python3", QStringList() 
            << homePath + "/pix/scripts/scripts/autoware_menu_button/check_hardware_status.py");
    });

    // 连接 progressDialog_hardwareDetection 的 canceled 信号到一个 lambda 表达式
    QObject::connect(progressDialog_hardwareDetection, &QProgressDialog::canceled, [&]() {
        if (hardwareDetectionProcess->state() == QProcess::Running) {
            hardwareDetectionProcess->terminate(); // 终止正在运行的进程
        }
    });

    // 连接 hardwareDetectionProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(hardwareDetectionProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        progressDialog_hardwareDetection->close(); // 关闭进度弹窗
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = hardwareDetectionProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/pix/ros-log/autoware_menu_button_error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_hardware_status.py";
                // 关闭文件
                errorLogFile.close();
            }
        }
    });
    // -----------------------------------------------------------------


    // -----------------------------------------------------------------
    // 连接硬件检测的信号到具体的槽函数
    QObject::connect(softwareDetectionAction, &QAction::triggered, [&](){
        // 显示进度弹窗
        progressDialog_softwareDetection->show();

        // 实现硬件检测的功能
        softwareDetectionProcess->start("/usr/bin/python3", QStringList() 
            << homePath + "/pix/scripts/scripts/autoware_menu_button/check_software_status.py");
    });

    // 连接 progressDialog_softwareDetection 的 canceled 信号到一个 lambda 表达式
    QObject::connect(progressDialog_softwareDetection, &QProgressDialog::canceled, [&]() {
        if (softwareDetectionProcess->state() == QProcess::Running) {
            softwareDetectionProcess->terminate(); // 终止正在运行的进程
        }
    });

    // 连接 softwareDetectionProcess 的 finished 信号以检查脚本执行结果
    QObject::connect(softwareDetectionProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [&](int exitCode, QProcess::ExitStatus exitStatus){
        progressDialog_softwareDetection->close(); // 关闭进度弹窗
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            // 脚本成功执行
            QMessageBox::information(nullptr, "成功", "脚本执行成功。", QMessageBox::Ok);
        } else {
            // 脚本执行失败
            QMessageBox::critical(nullptr, "错误", "脚本执行失败。", QMessageBox::Ok);

            // 获取标准错误内容并将其记录到日志或显示给用户
            QByteArray errorOutput = softwareDetectionProcess->readAllStandardError();
            QFile errorLogFile(homePath + "/pix/ros-log/autoware_menu_button_error.log");
            if (errorLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { // 使用Append标志
                QTextStream errorLogStream(&errorLogFile);
                // 将标准错误内容写入文件
                errorLogStream << errorOutput << homePath + "/pix/scripts/scripts/autoware_menu_button/check_software_status.py";
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
