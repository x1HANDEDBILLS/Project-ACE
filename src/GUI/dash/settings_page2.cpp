// Location: src/GUI/dash/settings_page2.cpp
#include "dash/settings_page2.h"
#include "theme.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QThread>
#include <QVBoxLayout>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <unistd.h>
SettingsPage2::SettingsPage2(QWidget *parent) : QWidget(parent) {
    QProcess::execute("rfkill", QStringList() << "unblock" << "bluetooth");
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(30, 10, 30, 30);
    main_layout->setSpacing(10);
    QHBoxLayout *top_bar = new QHBoxLayout();
    QLabel *info = new QLabel("CONNECTIVITY MANAGER", this);
    info->setStyleSheet("font-family: 'Consolas'; font-size: 18px; color: #c0c0c0;");
    scan_btn = new Button("SCAN", this);
    scan_btn->setFixedSize(120, 40);
    top_bar->addWidget(info);
    top_bar->addStretch();
    top_bar->addWidget(scan_btn);
    main_layout->addLayout(top_bar);
    QHBoxLayout *mid_layout = new QHBoxLayout();
    device_list = new QListWidget(this);
    device_list->setStyleSheet("QListWidget { background: rgba(0,0,0,80); color: white; font-family: 'Consolas'; "
                               "font-size: 14px; border: 1px solid #444; }"
                               "QListWidget::item { padding: 10px; border-bottom: 1px solid #222; }");
    QVBoxLayout *actions = new QVBoxLayout();
    connect_btn = new Button("CONNECT", this);
    disconnect_btn = new Button("DISCONNECT", this);
    forget_btn = new Button("FORGET", this);
    connect_btn->setFixedSize(120, 50);
    disconnect_btn->setFixedSize(120, 50);
    forget_btn->setFixedSize(120, 50);
    actions->addWidget(connect_btn);
    actions->addWidget(disconnect_btn);
    actions->addWidget(forget_btn);
    actions->addStretch();
    mid_layout->addWidget(device_list, 1);
    mid_layout->addLayout(actions);
    main_layout->addLayout(mid_layout, 3);
    debug_console = new QTextEdit(this);
    debug_console->setReadOnly(true);
    debug_console->setStyleSheet(
        "background: #050505; color: #00ff00; font-family: 'Consolas'; font-size: 10px; border: "
        "1px solid #333;");
    debug_console->setFixedHeight(100);
    main_layout->addWidget(debug_console, 1);
    scan_timer = new QTimer(this);
    scan_timer->setSingleShot(true);
    connect(scan_timer, &QTimer::timeout, this, &SettingsPage2::stop_scan);
    mute_timer = new QTimer(this);
    connect(mute_timer, &QTimer::timeout, this, &SettingsPage2::silence_touchpads);
    mute_timer->start(3000);
    bt_process = new QProcess(this);
    bt_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(bt_process, &QProcess::readyReadStandardOutput, this, &SettingsPage2::read_bt_output);
    bt_process->start("bluetoothctl");
    bt_process->waitForStarted();
    bt_process->write("power on\nagent on\ndefault-agent\n");
    connect(scan_btn, &QPushButton::clicked, this, &SettingsPage2::start_scan);
    connect(connect_btn, &QPushButton::clicked, this, &SettingsPage2::handle_connect);
    connect(disconnect_btn, &QPushButton::clicked, this, &SettingsPage2::handle_disconnect);
    connect(forget_btn, &QPushButton::clicked, this, &SettingsPage2::handle_forget);
    silence_touchpads();
}
SettingsPage2::~SettingsPage2() {
    stop_scan();
    for (int fd : grabbed_fds) {
        ioctl(fd, EVIOCGRAB, 0);
        ::close(fd);
    }
    if (bt_process && bt_process->state() == QProcess::Running) {
        bt_process->write("quit\n");
        bt_process->waitForFinished(500);
    }
}
void SettingsPage2::silence_touchpads() {
    DIR *dir = opendir("/dev/input/");
    if (!dir) return;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) == 0) {
            char path[256];
            snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
            int fd = open(path, O_RDONLY | O_NONBLOCK);
            if (fd >= 0) {
                char name[256] = "Unknown";
                if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0) {
                    if (strstr(name, "Touchpad")) {
                        if (ioctl(fd, EVIOCGRAB, 1) == 0) {
                            grabbed_fds.append(fd);
                            // Removed console message for a clean SSH terminal
                        } else {
                            ::close(fd);
                        }
                    } else {
                        ::close(fd);
                    }
                } else {
                    ::close(fd);
                }
            }
        }
    }
    closedir(dir);
}
void SettingsPage2::start_scan() {
    if (scan_btn->text() == "STOP") {
        stop_scan();
    } else {
        device_list->clear();
        bt_process->write("devices\n");
        bt_process->write("scan on\n");
        scan_btn->setText("STOP");
        scan_timer->start(30000);
    }
}
void SettingsPage2::stop_scan() {
    if (bt_process && bt_process->state() == QProcess::Running) {
        bt_process->write("scan off\n");
        scan_btn->setText("SCAN");
    }
    scan_timer->stop();
}
void SettingsPage2::read_bt_output() {
    QString raw = bt_process->readAllStandardOutput();
    raw.remove(QRegularExpression("\\x1B\\[[0-9;]*[mK]"));
    raw.remove(QRegularExpression("\\[.*\\][#>] "));
    debug_console->append(raw.trimmed());
    QStringList lines = raw.split("\n");
    static QString last_seen_mac = "";
    QColor accent = QColor("#00ffff");
    bool is_scanning = scan_timer->isActive();
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith("[DEL]")) continue;
        QRegularExpression devRe("(?:Device|\\[NEW\\] Device|\\[CHG\\] Device) ([0-9A-Fa-f:]{17})(.*)");
        QRegularExpressionMatch devMatch = devRe.match(line);
        if (devMatch.hasMatch()) {
            last_seen_mac = devMatch.captured(1);
            QString name = devMatch.captured(2).trimmed();
            bool is_trash =
                (name.length() < 3 || name == "TY" || name.contains("PROV_") || name.contains(last_seen_mac));
            if (is_scanning && !name.isEmpty() && !is_trash) {
                bool exists = false;
                for (int i = 0; i < device_list->count(); ++i) {
                    if (device_list->item(i)->text().contains(last_seen_mac)) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) { device_list->addItem(name + " [" + last_seen_mac + "]"); }
                bt_process->write(QString("info %1\n").arg(last_seen_mac).toUtf8());
            }
        }
        if (line.contains("Connected: yes", Qt::CaseInsensitive) && !last_seen_mac.isEmpty()) {
            for (int i = 0; i < device_list->count(); ++i) {
                if (device_list->item(i)->text().contains(last_seen_mac)) {
                    device_list->item(i)->setForeground(accent);
                    if (!device_list->item(i)->text().contains("[CONNECTED]")) {
                        device_list->item(i)->setText(device_list->item(i)->text() + " [CONNECTED]");
                    }
                }
            }
            silence_touchpads();
        }
        if (line.contains("Connected: no", Qt::CaseInsensitive) && !last_seen_mac.isEmpty()) {
            for (int i = 0; i < device_list->count(); ++i) {
                if (device_list->item(i)->text().contains(last_seen_mac)) {
                    device_list->item(i)->setForeground(QColor(192, 192, 192));
                    device_list->item(i)->setText(device_list->item(i)->text().replace(" [CONNECTED]", ""));
                }
            }
        }
    }
}
void SettingsPage2::handle_connect() {
    QListWidgetItem *item = device_list->currentItem();
    if (!item) return;
    QRegularExpression re("\\[([0-9A-Fa-f:]{17})\\]");
    QRegularExpressionMatch m = re.match(item->text());
    if (m.hasMatch()) {
        QString mac = m.captured(1);
        bt_process->write(QString("pair %1\n").arg(mac).toUtf8());
        QThread::msleep(250);
        bt_process->write(QString("trust %1\n").arg(mac).toUtf8());
        QThread::msleep(250);
        bt_process->write(QString("connect %1\n").arg(mac).toUtf8());
    }
}
void SettingsPage2::handle_disconnect() {
    QListWidgetItem *item = device_list->currentItem();
    if (!item) return;
    QRegularExpression re("\\[([0-9A-Fa-f:]{17})\\]");
    QRegularExpressionMatch m = re.match(item->text());
    if (m.hasMatch()) {
        QString mac = m.captured(1);
        bt_process->write(QString("disconnect %1\n").arg(mac).toUtf8());
    }
}
void SettingsPage2::handle_forget() {
    QListWidgetItem *item = device_list->currentItem();
    if (!item) return;
    QRegularExpression re("\\[([0-9A-Fa-f:]{17})\\]");
    QRegularExpressionMatch m = re.match(item->text());
    if (m.hasMatch()) {
        QString mac = m.captured(1);
        bt_process->write(QString("disconnect %1\n").arg(mac).toUtf8());
        QThread::msleep(150);
        bt_process->write(QString("remove %1\n").arg(mac).toUtf8());
        delete item;
    }
}
void SettingsPage2::handle_error(QProcess::ProcessError error) {
    if (error != QProcess::Crashed) std::cerr << "[BT ERROR] " << error << std::endl;
}
void SettingsPage2::refresh_theme() {
    scan_btn->update();
    connect_btn->update();
    disconnect_btn->update();
    forget_btn->update();
}
