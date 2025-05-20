#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QWidget>
#include <QTextCursor>

namespace Ui {
class TerminalWidget;
}

class TerminalWidget;

class TerminalWidgetEvents : public QObject
{
    Q_OBJECT

    friend class TerminalWidget;

    TerminalWidget &widget;

    explicit TerminalWidgetEvents(TerminalWidget &widget);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

class TerminalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TerminalWidget(QWidget *parent = nullptr);
    ~TerminalWidget() override;

signals:
    void sendPacket(const uint8_t *data, size_t length);

public slots:
    void packetReceived(const uint8_t *data, size_t length);

private:
    Ui::TerminalWidget *ui;

    TerminalWidgetEvents events;
};

#endif // TERMINALWIDGET_H
