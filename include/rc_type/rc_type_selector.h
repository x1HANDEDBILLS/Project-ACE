// Location: include/rc_type/rc_type_selector.h
#ifndef RC_TYPE_SELECTOR_H
#define RC_TYPE_SELECTOR_H
#include <QButtonGroup>
#include <QWidget>
class RCTypeSelector : public QWidget {
    Q_OBJECT
public:
    explicit RCTypeSelector(QWidget *parent = nullptr);
    void refresh_selection(); // Matches UI to AppState
signals:
    void typeChanged(int id);

private:
    QButtonGroup *group;
};
#endif
