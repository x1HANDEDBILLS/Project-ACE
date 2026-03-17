// Location: src/GUI/widgets/source_selector.h
#pragma once
#include "Core/app_state.h"
#include "theme/theme.h"
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <vector>
class SourceSelector : public QDialog {
public:
    int result_id = -1;
    SourceSelector(int current, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
        setFixedSize(720, 540);
        setStyleSheet("background: #050508; border: 2px solid #444; border-radius: 10px; color: white;");
        auto *main_layout = new QVBoxLayout(this);
        auto *port_container = new QHBoxLayout();
        for (int i = 0; i < 4; i++) {
            auto *pBtn = new QPushButton(QString("PORT %1").arg(i + 1));
            pBtn->setCheckable(true);
            pBtn->setFixedHeight(45);
            if (i == (current / 50)) {
                currentPort = i;
                pBtn->setChecked(true);
            }
            QObject::connect(pBtn, &QPushButton::clicked, [this, i]() {
                currentPort = i;
                for (auto *b : pBtns) b->setChecked(false);
                pBtns[i]->setChecked(true);
            });
            port_container->addWidget(pBtn);
            pBtns.push_back(pBtn);
        }
        main_layout->addLayout(port_container);
        auto *scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("background:transparent; border:none;");
        auto *container = new QWidget();
        auto *grid = new QGridLayout(container);
        for (int i = 0; i < 50; i++) {
            auto *b = new QPushButton();
            b->setFixedSize(120, 55);
            QObject::connect(b, &QPushButton::clicked, [this, i]() {
                result_id = (currentPort * 50) + i;
                accept();
            });
            grid->addWidget(b, i / 5, i % 5);
            gBtns.push_back(b);
        }
        scroll->setWidget(container);
        main_layout->addWidget(scroll);
        QTimer *t = new QTimer(this);
        QObject::connect(t, &QTimer::timeout, [this]() {
            auto &state = AppState::instance();
            QString theme_color = ThemeManager::instance().active().hex;
            for (int i = 0; i < 4; i++)
                pBtns[i]->setStyleSheet(i == currentPort
                                            ? QString("background:%1; color:black; font-weight:bold;").arg(theme_color)
                                            : "background:#222;");
            for (int i = 0; i < 50; i++) {
                int id = (currentPort * 50) + i;
                int16_t val = state.tuned_slots[id];
                gBtns[i]->setText(QString("ID %1\n%2").arg(id).arg(val));
                gBtns[i]->setStyleSheet(
                    std::abs(val) > 100
                        ? QString("border: 2px solid %1; background: #1a1a25; color:white;").arg(theme_color)
                        : "background: #111; color:#777;");
            }
        });
        t->start(30);
    }

private:
    int currentPort = 0;
    std::vector<QPushButton *> pBtns, gBtns;
};
