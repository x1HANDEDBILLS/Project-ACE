#ifndef INPUT_TUNE_PLANE_PAGE1_H
#define INPUT_TUNE_PLANE_PAGE1_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

class InputTunePlanePanel;

// Forward declaration of the bar used in rows
class NeonBar : public QWidget {
    Q_OBJECT
public:
    int current_val = -999999;
    QColor clr;
    NeonBar(QColor c, QWidget *p);
    void setValue(int val);
protected:
    void paintEvent(class QPaintEvent *event) override;
};

class TuneRow : public QFrame {
    Q_OBJECT
public:
    int slot_id;
    NeonBar *in_bar;
    NeonBar *out_bar;
    QLabel *val_label;
    QPushButton *dz_btn, *axial_btn, *expo_btn;
    InputTunePlanePanel *parent_panel;

    TuneRow(int id, QColor page_clr, QWidget *p, InputTunePlanePanel *panel);
    Q_INVOKABLE void refresh();
};

void setup_input_tune_page_content(QWidget *p, int offset, InputTunePlanePanel *panel);

#endif
