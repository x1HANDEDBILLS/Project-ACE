// Location: src/GUI/vehicles/plane/input_tune_plane_page1.cpp
#include "Core/app_state.h"
#include "input/input_manage.h"
#include "profiles/profile_manager.h"
#include "theme/theme.h"
#include "input_tune_plane_page1.h"
#include "widgets/keypad.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <vector>
class ResponseCurve : public QWidget {
public:
    int16_t current_raw = 0;
    ResponseCurve(QWidget *p) : QWidget(p) { setFixedSize(220, 220); }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        auto &state = AppState::instance();
        QColor theme = ThemeManager::instance().active().primary;
        p.setBrush(QColor(10, 10, 10));
        p.setPen(QPen(QColor(40, 40, 40), 2));
        p.drawRect(rect());
        p.setPen(QColor(25, 25, 25));
        p.drawLine(110, 0, 110, 220);
        p.drawLine(0, 110, 220, 110);
        p.setPen(QPen(theme, 2));
        QPainterPath path;
        for (int i = 0; i <= 220; i++) {
            double x_norm = (i - 110) / 110.0;
            double abs_x = std::abs(x_norm);
            double threshold = state.deadzones[0] / 100.0;
            double y_out = 0;
            if (abs_x > threshold) {
                double val = (abs_x - threshold) / (1.0 - threshold);
                double e = state.current_expo / 100.0;
                val = e * (val * val * val) + (1.0 - e) * val;
                y_out = (x_norm > 0) ? val : -val;
            }
            int screen_y = 110 - (y_out * 110);
            if (i == 0) path.moveTo(i, screen_y);
            else
                path.lineTo(i, screen_y);
        }
        p.drawPath(path);
        double dot_x = (current_raw / 32768.0) * 110 + 110;
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(dot_x, 110), 5, 5);
    }
};
class AxisRow : public QWidget {
    Q_OBJECT
public:
    AxisRow(QString name, int axis_index, QWidget *parent) : QWidget(parent), idx(axis_index) {
        auto *l = new QHBoxLayout(this);
        QString h = ThemeManager::instance().active().primary.name();
        title = new QLabel(name);
        title->setFixedWidth(60);
        title->setStyleSheet("font-weight: bold; color: white;");
        raw_lbl = new QLabel("R:0");
        raw_lbl->setFixedWidth(80);
        raw_lbl->setStyleSheet("font-family: 'Consolas'; color: #888;");
        val_box = new QPushButton();
        val_box->setFixedSize(75, 35);
        val_box->setStyleSheet(QString("background:#000; color:%1; border:1px solid %1; "
                                       "font-weight:bold; border-radius:4px;")
                                   .arg(h));
        tuned_lbl = new QLabel("O:0");
        tuned_lbl->setFixedWidth(80);
        tuned_lbl->setStyleSheet("font-family: 'Consolas'; color: #00ff00; font-weight: bold;");
        l->addWidget(title);
        l->addWidget(raw_lbl);
        l->addWidget(val_box);
        l->addWidget(tuned_lbl);
        QObject::connect(val_box, &QPushButton::clicked, this, &AxisRow::openKeypad);
    }
    void updateValues(int16_t raw) {
        auto &state = AppState::instance();
        float dz_pct = state.deadzones[idx];
        int32_t threshold = static_cast<int32_t>(32768.0f * (dz_pct / 100.0f));
        int32_t abs_raw = std::abs(raw);
        int32_t tuned = 0;
        if (abs_raw > threshold) {
            double x = (abs_raw - threshold) / (32768.0 - threshold);
            x = std::clamp(x, 0.0, 1.0);
            if (state.expo_enabled[idx] && state.current_expo > 0) {
                double e = state.current_expo / 100.0;
                x = e * (x * x * x) + (1.0 - e) * x;
            }
            tuned = static_cast<int32_t>(std::round(x * 32767.0));
            if (raw < 0) tuned = -tuned;
        }
        raw_lbl->setText(QString("R:%1").arg(raw));
        tuned_lbl->setText(QString("O:%1").arg(tuned));
        val_box->setText(QString("%1%").arg(dz_pct, 0, 'f', 1));
    }
private slots:
    void openKeypad() {
        auto *kp = new Keypad(AppState::instance().deadzones[idx]);
        kp->show();
        QObject::connect(kp, &Keypad::submitted, [this, kp](double v) {
            AppState::instance().deadzones[idx] = static_cast<float>(v);
            kp->deleteLater();
        });
    }

private:
    int idx;
    QLabel *title, *raw_lbl, *tuned_lbl;
    QPushButton *val_box;
};
class TuningPageController : public QObject {
    Q_OBJECT
public:
    QSlider *expo_slider;
    QLabel *expo_title;
    std::vector<QCheckBox *> checkboxes;
    QList<AxisRow *> axis_rows;
    TuningPageController(QObject *p) : QObject(p) {
        connect(&ProfileManager::instance(), &ProfileManager::profileLoaded, this, &TuningPageController::refreshUI);
    }
public slots:
    void refreshUI() {
        auto &state = AppState::instance();
        if (expo_slider) {
            QSignalBlocker blocker(expo_slider);
            expo_slider->setValue(static_cast<int>(state.current_expo));
        }
        if (expo_title) expo_title->setText(QString("GLOBAL EXPO CURVE: %1%").arg(state.current_expo));
        for (int i = 0; i < (int)checkboxes.size(); i++) {
            if (i < 6) {
                QSignalBlocker blocker(checkboxes[i]);
                checkboxes[i]->setChecked(state.expo_enabled[i]);
            }
        }
    }
};
void setup_input_tune_plane_page1_content(QWidget *p) {
    auto &state = AppState::instance();
    auto *ctrl = new TuningPageController(p);
    auto *main_layout = new QVBoxLayout(p);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(
        QString("QScrollArea { border: none; background: transparent; }"
                "QScrollBar:vertical { width: 40px; background: #111; }"
                "QScrollBar::handle:vertical { background: %1; min-height: 40px; border-radius: 5px; }")
            .arg(ThemeManager::instance().active().primary.name()));
    auto *container = new QWidget();
    auto *v_box = new QVBoxLayout(container);
    v_box->setContentsMargins(10, 10, 35, 10);
    v_box->setSpacing(15);
    QString headStyle = "color: white; font-weight: bold; font-size: 16px; border-bottom: 2px solid #333; "
                        "padding-bottom: 5px;";
    auto *top_row = new QHBoxLayout();
    QLabel *header = new QLabel("INPUT PIPELINE TUNING");
    header->setStyleSheet(headStyle);
    QPushButton *save_btn = new QPushButton("SAVE CHANGES");
    save_btn->setFixedSize(140, 35);
    save_btn->setStyleSheet("background: #00aa00; color: white; font-weight: bold; border-radius: 5px;");
    QObject::connect(save_btn, &QPushButton::clicked, []() { ProfileManager::instance().save(); });
    top_row->addWidget(header);
    top_row->addStretch();
    top_row->addWidget(save_btn);
    v_box->addLayout(top_row);
    QStringList names = {"ROLL", "PITCH", "YAW", "THR", "L2", "R2"};
    for (int i = 0; i < 6; ++i) {
        auto *row = new AxisRow(names[i], i, container);
        ctrl->axis_rows.append(row);
        v_box->addWidget(row);
    }
    ctrl->expo_title = new QLabel(QString("GLOBAL EXPO CURVE: %1%").arg(state.current_expo));
    ctrl->expo_title->setStyleSheet(headStyle);
    v_box->addWidget(ctrl->expo_title);
    ctrl->expo_slider = new QSlider(Qt::Horizontal);
    ctrl->expo_slider->setRange(0, 100);
    ctrl->expo_slider->setMinimumHeight(80); // Increased height for the larger handle
    ctrl->expo_slider->setStyleSheet(
        QString("QSlider::groove:horizontal { height: 16px; background: #222; border-radius: 8px; }"
                "QSlider::handle:horizontal { background: #ffffff; border: 2px solid #888; width: "
                "60px; height: 60px; margin: -22px 0; border-radius: 30px; }"));
    ctrl->expo_slider->setValue(static_cast<int>(state.current_expo));
    v_box->addWidget(ctrl->expo_slider);
    QLabel *graph_header = new QLabel("RESPONSE CURVE PREVIEW");
    graph_header->setStyleSheet(headStyle);
    v_box->addWidget(graph_header);
    auto *mid_area = new QHBoxLayout();
    ResponseCurve *graph = new ResponseCurve(container);
    auto *grid_container = new QWidget();
    auto *grid = new QGridLayout(grid_container);
    QString cbStyle = QString("QCheckBox { color: white; font-size: 12px; font-weight: bold; }"
                              "QCheckBox::indicator { width: 38px; height: 38px; border: 2px solid #444; background: "
                              "#111; border-radius: 6px; }"
                              "QCheckBox::indicator:checked { background: %1; border: 2px solid white; }")
                          .arg(ThemeManager::instance().active().primary.name());
    for (int i = 0; i < 6; i++) {
        auto *cb = new QCheckBox(names[i]);
        cb->setChecked(state.expo_enabled[i]);
        cb->setStyleSheet(cbStyle);
        ctrl->checkboxes.push_back(cb);
        QObject::connect(cb, &QCheckBox::toggled,
                         [=](bool checked) { AppState::instance().expo_enabled[i] = checked; });
        grid->addWidget(cb, i / 3, i % 3);
    }
    mid_area->addWidget(graph);
    mid_area->addWidget(grid_container);
    v_box->addLayout(mid_area);
    QObject::connect(ctrl->expo_slider, &QSlider::valueChanged, [=](int v) {
        AppState::instance().current_expo = static_cast<float>(v);
        ctrl->expo_title->setText(QString("GLOBAL EXPO CURVE: %1%").arg(v));
        graph->update();
    });
    scroll->setWidget(container);
    main_layout->addWidget(scroll);
    QTimer *timer = new QTimer(p);
    QObject::connect(timer, &QTimer::timeout, [ctrl, graph]() {
        DeviceState dev = InputManager::instance().getSlotState(0);
        for (int i = 0; i < ctrl->axis_rows.size(); i++) { ctrl->axis_rows[i]->updateValues(dev.axes[i]); }
        graph->current_raw = dev.axes[0];
        graph->update();
    });
    timer->start(16);
}
#include "input_tune_plane_page1.moc"
