// Location: include/vehicles/plane/map/rc_map_plane_page1.h
#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>
class MapperRow : public QWidget {
    Q_OBJECT
public:
    explicit MapperRow(int ch_idx, QWidget *parent = nullptr);
    void updateButton();
    void pickSource(); // Explicitly declared
private:
    int ch;
    QPushButton *id_btn;
};
void setup_rc_map_plane_page1_content(QWidget *p);
