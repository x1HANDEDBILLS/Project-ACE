// Location: src/GUI/vehicles/plane/map/rc_map_plane_page2.cpp

#include "Core/app_state.h"
#include "profiles/profile_manager.h"
#include "rc_map_plane_page2.h"
#include "theme/theme.h"
#include "widgets/source_selector.h"
#include <QCheckBox>
#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
RCMapPlanePage2::RCMapPlanePage2(QWidget *parent) : QWidget(parent) {
    auto *main = new QVBoxLayout(this);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background: transparent; border: none;");
    auto *container = new QWidget();
    auto *list = new QVBoxLayout(container);
    list->setSpacing(15);
    QString accent = ThemeManager::instance().active().hex;
    for (int i = 0; i < 6; i++) {
        QFrame *row = new QFrame();
        row->setStyleSheet("background: #0f0f15; border: 1px solid #333; border-radius: 10px;");
        auto *rl = new QVBoxLayout(row);
        auto *header = new QHBoxLayout();
        QLabel *title = new QLabel(QString("MIXER SLOT %1").arg(i + 1));
        title->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px; border:none;").arg(accent));
        mixers[i].target_btn = new QPushButton();
        mixers[i].target_btn->setFixedSize(220, 40);
        connect(mixers[i].target_btn, &QPushButton::clicked, [this, i]() {
            QDialog dlg(this);
            // STABILITY FIX: Use Dialog instead of Popup for better Pi touch registration
            dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
            dlg.setModal(true);
            // SIZE FIX: Explicit size prevents touch-offset glitches
            dlg.setFixedSize(420, 380);
            dlg.setStyleSheet("background: #15151b; border: 2px solid #555; border-radius: 12px;");
            auto *dl = new QVBoxLayout(&dlg);
            dl->setContentsMargins(15, 15, 15, 15);
            dl->setSpacing(10);
            auto *gl = new QGridLayout();
            gl->setSpacing(8);
            for (int j = 0; j < 16; j++) {
                auto *b = new QPushButton(QString("CH %1").arg(j + 1));
                b->setFixedSize(90, 55);
                b->setStyleSheet("background: #2a2a35; color: #eee; border: 1px solid #444; "
                                 "font-weight: bold; font-size: 16px; border-radius: 5px;");
                connect(b, &QPushButton::clicked, [this, i, j, &dlg]() {
                    AppState::instance().split_mixers[i].target_ch = j;
                    refresh_ui();
                    dlg.accept();
                });
                gl->addWidget(b, j / 4, j % 4);
            }
            dl->addLayout(gl);
            QPushButton *off = new QPushButton("DISABLE MIXER");
            off->setFixedHeight(55);
            off->setStyleSheet("background: #442222; color: white; font-weight: bold; "
                               "border: 1px solid #ff4444; border-radius: 5px; margin-top: 5px;");
            connect(off, &QPushButton::clicked, [this, i, &dlg]() {
                AppState::instance().split_mixers[i].target_ch = -1;
                refresh_ui();
                dlg.accept();
            });
            dl->addWidget(off);
            // Center the dialog relative to the main window
            dlg.exec();
        });
        header->addWidget(title);
        header->addStretch();
        header->addWidget(mixers[i].target_btn);
        rl->addLayout(header);
        auto *body = new QHBoxLayout();
        // Negative Side
        auto *v_neg = new QVBoxLayout();
        mixers[i].neg_id_btn = new QPushButton();
        mixers[i].neg_id_btn->setFixedHeight(55);
        connect(mixers[i].neg_id_btn, &QPushButton::clicked, [this, i]() {
            SourceSelector sel(AppState::instance().split_mixers[i].neg_id, this);
            if (sel.exec() == QDialog::Accepted) {
                AppState::instance().split_mixers[i].neg_id = sel.result_id;
                refresh_ui();
            }
        });
        auto *n_checks = new QHBoxLayout();
        QString nStyle = "QCheckBox { color: #bbb; font-weight: bold; } "
                         "QCheckBox::indicator { width: 45px; height: 45px; border: 1px solid #555; "
                         "background: #050505; border-radius: 5px; } "
                         "QCheckBox::indicator:checked { background: #ff4444; border: 1px solid white; }";
        mixers[i].neg_ctr = new QCheckBox("CTR");
        mixers[i].neg_rev = new QCheckBox("REV");
        mixers[i].neg_ctr->setStyleSheet(nStyle);
        mixers[i].neg_rev->setStyleSheet(nStyle);
        connect(mixers[i].neg_ctr, &QCheckBox::toggled,
                [this, i](bool val) { AppState::instance().split_mixers[i].neg_center = val; });
        connect(mixers[i].neg_rev, &QCheckBox::toggled,
                [this, i](bool val) { AppState::instance().split_mixers[i].neg_reverse = val; });
        n_checks->addWidget(mixers[i].neg_ctr);
        n_checks->addWidget(mixers[i].neg_rev);
        v_neg->addWidget(mixers[i].neg_id_btn);
        v_neg->addLayout(n_checks);
        // Positive Side
        auto *v_pos = new QVBoxLayout();
        mixers[i].pos_id_btn = new QPushButton();
        mixers[i].pos_id_btn->setFixedHeight(55);
        connect(mixers[i].pos_id_btn, &QPushButton::clicked, [this, i]() {
            SourceSelector sel(AppState::instance().split_mixers[i].pos_id, this);
            if (sel.exec() == QDialog::Accepted) {
                AppState::instance().split_mixers[i].pos_id = sel.result_id;
                refresh_ui();
            }
        });
        auto *p_checks = new QHBoxLayout();
        QString pStyle = "QCheckBox { color: #bbb; font-weight: bold; } "
                         "QCheckBox::indicator { width: 45px; height: 45px; border: 1px solid #555; "
                         "background: #050505; border-radius: 5px; } "
                         "QCheckBox::indicator:checked { background: #00ff88; border: 1px solid white; }";
        mixers[i].pos_ctr = new QCheckBox("CTR");
        mixers[i].pos_rev = new QCheckBox("REV");
        mixers[i].pos_ctr->setStyleSheet(pStyle);
        mixers[i].pos_rev->setStyleSheet(pStyle);
        connect(mixers[i].pos_ctr, &QCheckBox::toggled,
                [this, i](bool val) { AppState::instance().split_mixers[i].pos_center = val; });
        connect(mixers[i].pos_rev, &QCheckBox::toggled,
                [this, i](bool val) { AppState::instance().split_mixers[i].pos_reverse = val; });
        p_checks->addWidget(mixers[i].pos_ctr);
        p_checks->addWidget(mixers[i].pos_rev);
        v_pos->addWidget(mixers[i].pos_id_btn);
        v_pos->addLayout(p_checks);
        body->addLayout(v_neg);
        body->addSpacing(15);
        body->addLayout(v_pos);
        rl->addLayout(body);
        list->addWidget(row);
    }
    scroll->setWidget(container);
    main->addWidget(scroll);
    connect(&ProfileManager::instance(), &ProfileManager::profileLoaded, this, &RCMapPlanePage2::refresh_ui);
    refresh_ui();
}
void RCMapPlanePage2::refresh_ui() {
    auto &state = AppState::instance();
    QString accent = ThemeManager::instance().active().hex;
    for (int i = 0; i < 6; i++) {
        auto &m = state.split_mixers[i];
        mixers[i].target_btn->setText(m.target_ch == -1 ? "OFF / SELECT TARGET"
                                                        : QString("TO OUTPUT CH %1").arg(m.target_ch + 1));
        mixers[i].target_btn->setStyleSheet(
            m.target_ch == -1
                ? "background: #222; color: #777; border: 1px solid #444; font-weight: bold;"
                : QString("background: #000; color: %1; border: 1px solid %1; font-weight: bold;").arg(accent));
        mixers[i].neg_id_btn->setText(QString("NEG (-): ID %1").arg(m.neg_id));
        mixers[i].neg_id_btn->setStyleSheet(
            "background: #000; color: #ff4444; border: 1px solid #444; font-weight: bold; font-size: 16px;");
        mixers[i].pos_id_btn->setText(QString("POS (+): ID %1").arg(m.pos_id));
        mixers[i].pos_id_btn->setStyleSheet(
            "background: #000; color: #00ff88; border: 1px solid #444; font-weight: bold; font-size: 16px;");
        mixers[i].neg_ctr->blockSignals(true);
        mixers[i].neg_ctr->setChecked(m.neg_center);
        mixers[i].neg_ctr->blockSignals(false);
        mixers[i].neg_rev->blockSignals(true);
        mixers[i].neg_rev->setChecked(m.neg_reverse);
        mixers[i].neg_rev->blockSignals(false);
        mixers[i].pos_ctr->blockSignals(true);
        mixers[i].pos_ctr->setChecked(m.pos_center);
        mixers[i].pos_ctr->blockSignals(false);
        mixers[i].pos_rev->blockSignals(true);
        mixers[i].pos_rev->setChecked(m.pos_reverse);
        mixers[i].pos_rev->blockSignals(false);
    }
}
