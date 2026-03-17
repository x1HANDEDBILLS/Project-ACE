// Location: include/dash/settings_page2.h
#ifndef SETTINGS_PAGE2_H
#define SETTINGS_PAGE2_H
#include "widgets/button.h"
#include <QListWidget>
#include <QProcess>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
class SettingsPage2 : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage2(QWidget *parent = nullptr);
    ~SettingsPage2();
public slots:
    void refresh_theme();
private slots:
    void start_scan();
    void stop_scan();
    void read_bt_output();
    void handle_connect();
    void handle_disconnect();
    void handle_forget();
    void handle_error(QProcess::ProcessError error);
    void silence_touchpads();

private:
    QListWidget *device_list;
    QTextEdit *debug_console;
    Button *scan_btn;
    Button *connect_btn;
    Button *disconnect_btn;
    Button *forget_btn;
    QProcess *bt_process;
    QTimer *scan_timer;
    QTimer *mute_timer;
    QList<int> grabbed_fds;
};
#endif
