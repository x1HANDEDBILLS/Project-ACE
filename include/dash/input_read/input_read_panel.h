// Location: include/dash/input_read_panel.h
#ifndef INPUT_READ_PANEL_H
#define INPUT_READ_PANEL_H
#include "widgets/button.h"
#include "widgets/frame.h"
#include <QFrame>
#include <QLabel>
#include <QStackedWidget>
class InputReadPanel : public QFrame {
    Q_OBJECT
public:
    explicit InputReadPanel(QWidget *parent = nullptr);
    virtual ~InputReadPanel(); // Declared!
    void refresh_theme();
private slots:
    void next_page();
    void prev_page();

private:
    void update_page_ui();
    Frame *content_frame;
    QLabel *status_label;
    QLabel *header_title;
    QLabel *page_label;
    QStackedWidget *pages;
    Button *prev_btn;
    Button *next_btn;
    QStringList page_titles;
};
#endif
