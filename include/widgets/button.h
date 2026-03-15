// LOCATION: /home/user/ACE/include/widgets/button.h
#ifndef BUTTON_H
#define BUTTON_H

#include <QPixmap>
#include <QPushButton>
#include <QString>

class Button : public QPushButton
{
    Q_OBJECT
   public:
    explicit Button(const QString &text, QWidget *parent = nullptr);
    explicit Button(const QString &text, QWidget *parent, bool small);

    // Overrides for Python-style logic
    int font_size = -1;  // -1 means use default logic
    float edge_size = -1.0f;
    int y_nudge = 0;

   private:
    void regenerateCache();
    bool m_small = false;
    int m_defaultFontSize = 11;
    float m_defaultEdgeSize = 10.0f;
    QPixmap m_bgCache;
    QString m_lastThemeHex;

   protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif
