#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QNetworkInterface>
#include <QFile>
#include <QTextStream>

#include <QInputDialog> // 包含QInputDialog的头文件
#include <QLineEdit>    // 包含QLineEdit的头文件
#include <QMessageBox>


// 获取网络接口和IPv4地址的函数
QMap<QString, QString> getNetworkInterfaces() {
    QMap<QString, QString> interfaceAddresses;
    for (const QNetworkInterface& networkInterface : QNetworkInterface::allInterfaces()) {
        // 检查接口是否已激活
        if (networkInterface.flags().testFlag(QNetworkInterface::IsUp) &&
            !networkInterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            // 获取IPv4地址
            for (const QNetworkAddressEntry& entry : networkInterface.addressEntries()) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    interfaceAddresses.insert(networkInterface.humanReadableName(), entry.ip().toString());
                }
            }
        }
    }
    interfaceAddresses.insert("lo", "127.0.0.1");
    return interfaceAddresses;
}

QString readProxyNumber(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 文件打开失败，弹出输入框供用户输入
        bool ok;
        QString text = QInputDialog::getText(nullptr, "输入", 
                                             "无法打开[proxy.txt]文件,请手动输入clash端口号:", 
                                             QLineEdit::Normal,
                                             "7890", &ok);
        if (ok) {
            // 用户输入了内容，将这个内容保存到文件中
            QFile outFile(filePath);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&outFile);
                out << text;
                outFile.close();
            }
            return text;
        } else {
            QApplication::exit(); // 如果用户取消，退出程序
        }
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QSystemTrayIcon trayIcon;
    QMenu menu;
    QAction *exitAction = new QAction("退出", &menu);

    // 读取代理模板文件
    QString proxyNumber = readProxyNumber("proxy.txt");
    QString proxyTemplate = QString("export https_proxy=http://%1:%2 "
                              "http_proxy=http://%1:%2 "
                              "all_proxy=socks5://%1:%2").arg("%1", proxyNumber);
    // 获取网络接口和IPv4地址
    QMap<QString, QString> interfaces = getNetworkInterfaces();

    // 为每个IP地址创建菜单项
    for (auto it = interfaces.constBegin(); it != interfaces.constEnd(); ++it) {
        QAction *interfaceAction = new QAction(it.key() + ": " + it.value(), &menu);
        menu.addAction(interfaceAction);

        // 连接菜单项的触发信号到槽函数，复制IP地址到剪切板
        QObject::connect(interfaceAction, &QAction::triggered, [proxyTemplate, it](bool) {
            QString formattedProxy = proxyTemplate.arg(it.value());
            QApplication::clipboard()->setText(formattedProxy);
        });
    }

    menu.addSeparator();
    menu.addAction(exitAction);

    // 连接退出菜单项的触发信号到应用程序的quit槽
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);

    trayIcon.setContextMenu(&menu);
    // QPixmap pixmap(":/clash.png");
    // QIcon icon(pixmap.scaled(10, 10, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // trayIcon.setIcon(icon);
    trayIcon.setIcon(QIcon(":/clash.png"));
    trayIcon.show();

    return app.exec();
}
