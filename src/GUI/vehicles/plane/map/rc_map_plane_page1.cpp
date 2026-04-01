// Location: src/GUI/vehicles/plane/map/rc_map_plane_page1.cpp

#include "Core/app_state.h"
#include "rc_map_plane_page1.h"
#include "theme/theme.h"
#include "widgets/source_selector.h"
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
class Page1Manager : public QObject {
public:
    QWidget *parentWidget;
    QList<QPushButton *> buttons;
    QString accent;
    Page1Manager(QWidget *p) : QObject(p), parentWidget(p) {
        accent = ThemeManager::instance().active().hex;
        buildUI();
        refresh();
        // Sync with theme changes
        connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this]() {
            accent = ThemeManager::instance().active().hex;
            refresh();
        });
        parentWidget->installEventFilter(this);
    }
    void buildUI() {
        auto *main = new QVBoxLayout(parentWidget);
        auto *grid = new QGridLayout();
        grid->setSpacing(10);
        auto &state = AppState::instance();
        for (int i = 0; i < 16; i++) {
            QFrame *f = new QFrame();
            f->setStyleSheet("background: #0f0f15; border: 1px solid #222; border-radius: 6px;");
            auto *l = new QVBoxLayout(f);
            QLabel *name = new QLabel(QString("CH %1").arg(i + 1));
            name->setStyleSheet("color: #555; font-size: 11px; font-weight: bold; text-align: center;");
            name->setAlignment(Qt::AlignCenter);
            QPushButton *btn = new QPushButton();
            btn->setFixedHeight(50);
            buttons.append(btn);
            connect(btn, &QPushButton::clicked, this, [this, i, &state]() {
                SourceSelector sel(state.channel_map[i], parentWidget);
                if (sel.exec() == QDialog::Accepted) {
                    state.channel_map[i] = sel.result_id;
                    refresh();
                }
            });
            l->addWidget(name);
            l->addWidget(btn);
            grid->addWidget(f, i / 4, i % 4);
        }
        main->addStretch();
        main->addLayout(grid);
        main->addStretch();
    }
    void refresh() {
        auto &state = AppState::instance();
        for (int i = 0; i < buttons.size(); i++) {
            bool is_overridden = false;
            // Check if any of the 6 split mixers are targeting this channel
            for (int m = 0; m < 6; m++) {
                if (state.split_mixers[m].target_ch == i) {
                    is_overridden = true;
                    break;
                }
            }
            QPushButton *b = buttons[i];
            if (is_overridden) {
                b->setText("SPLIT ACTIVE");
                b->setEnabled(false);
                b->setStyleSheet(
                    "background:#080808; color:#444; border:1px solid #222; font-size:10px; font-weight:bold;");
            } else {
                int current_id = state.channel_map[i];
                b->setText(current_id == 22 ? "NONE" : QString("ID %1").arg(current_id));
                b->setEnabled(true);
                b->setStyleSheet(
                    QString("background:#111; color:%1; border:1px solid #333; font-weight:bold; font-size:18px;")
                        .arg(accent));
            }
        }
    }

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override {
        if (obj == parentWidget && ev->type() == QEvent::Show) { refresh(); }
        return QObject::eventFilter(obj, ev);
    }
};
void setup_rc_map_plane_page1_content(QWidget *p) { new Page1Manager(p); }
