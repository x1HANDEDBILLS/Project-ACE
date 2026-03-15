// LOCATION: /home/user/ACE/src/GUI/dash/panel_buttons.cpp
#include "dash/panel_buttons.h"

#include <QGridLayout>

PanelButtons::PanelButtons(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(14, 10, 10, 10);
    layout->setSpacing(14);

    for (int i = 0; i < 14; ++i) {
        int row = i / 2;
        int col = i % 2;
        Button *btn = new Button("", this);
        btn->setFixedSize(74, 74);

        btn->font_size = 42;
        btn->edge_size = 12.0;

        if (i == 0) {
            btn->setText("RC\nTYPE");
            btn->font_size = 14;
            rc_type_btn = btn;
            connect(btn, &QPushButton::clicked, this, &PanelButtons::rcTypeClicked);
        } else if (i == 1) {
            btn->setText("→");
            back_btn = btn;
            // Note: back_btn connection is usually handled in dashboard_panel.cpp,
            // but we can trigger it here if desired.
        } else if (i == 2) {
            btn->setText("RC\nTUNE");
            btn->font_size = 14;
            rc_tune_btn = btn;
            connect(btn, &QPushButton::clicked, this, &PanelButtons::rcTuneClicked);
        } else if (i == 4) {
            btn->setText("RC\nMAP");
            btn->font_size = 13;
            rc_map_btn = btn;
            connect(btn, &QPushButton::clicked, this, &PanelButtons::rcMapClicked);
        } else if (i == 10) {
            btn->setText("INPUT\nTUNE");
            btn->font_size = 12;
            tune_btn = btn;
            connect(btn, &QPushButton::clicked, this, &PanelButtons::inputTuneClicked);
        } else if (i == 11) {
            btn->setText("PROFILE");
            btn->font_size = 12;
            profile_btn = btn;
            connect(btn, &QPushButton::clicked, this, &PanelButtons::profileClicked);
        } else if (i == 12) {
            btn->setText("INPUT");
            btn->font_size = 18;
            input_btn = btn;
            // Dashboard handles this directly via button_grid->input_btn
        } else if (i == 13) {
            btn->setText("⚙");
            inner_settings_btn = btn;
            // Dashboard handles this directly via button_grid->inner_settings_btn
        }

        layout->addWidget(btn, row, col);
        buttons.append(btn);
    }
}
PanelButtons::~PanelButtons() {}