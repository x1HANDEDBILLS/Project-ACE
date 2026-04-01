// Location: include/dash/input_read/input_read_page1.h
#ifndef INPUT_READ_PAGE1_H
#define INPUT_READ_PAGE1_H
#include <QWidget>
class InputReadPage1 : public QWidget {
    Q_OBJECT
public:
    explicit InputReadPage1(QWidget *parent = nullptr);
};
// Add this so input_read_panel.cpp can link
void setup_input_read_page1_content(QWidget *p);
#endif
