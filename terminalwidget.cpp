#include "terminalwidget.h"
#include "ui_terminalwidget.h"

#include <QScrollBar>
#include <QClipboard>

TerminalWidget::TerminalWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::TerminalWidget), events(*this)
{
    ui->setupUi(this);
    ui->terminalText->viewport()->setCursor(QCursor{});

    ui->terminalText->installEventFilter(&events);
}

TerminalWidget::~TerminalWidget()
{
    delete ui;
}

void TerminalWidget::packetReceived(const uint8_t *data, size_t length)
{
    QString str = QString::fromUtf8(reinterpret_cast<const char *>(data), static_cast<qsizetype>(length));

    QMetaObject::invokeMethod(this, [=] {
        QTextCursor prev_cursor = ui->terminalText->textCursor();
        int scroll_y = ui->terminalText->verticalScrollBar()->sliderPosition();
        bool is_at_max = scroll_y == ui->terminalText->verticalScrollBar()->maximum();
        ui->terminalText->moveCursor(QTextCursor::MoveOperation::End);
        ui->terminalText->insertPlainText(str);
        ui->terminalText->setTextCursor(prev_cursor);
        if (is_at_max)
            ui->terminalText->verticalScrollBar()->setValue(ui->terminalText->verticalScrollBar()->maximum());
        else
            ui->terminalText->verticalScrollBar()->setValue(scroll_y);
    });
}

TerminalWidgetEvents::TerminalWidgetEvents(TerminalWidget &widget) : QObject(&widget), widget(widget) { }

bool TerminalWidgetEvents::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (key_event->matches(QKeySequence::Paste))
        {
            QString text = QApplication::clipboard()->text();
            if (text.length() > 0)
            {
                auto raw = text.toUtf8();
                if (raw.length() > 0)
                    emit widget.sendPacket(reinterpret_cast<const uint8_t *>(raw.constData()), static_cast<size_t>(raw.length()));
            }
        }
        else if (!(key_event->modifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::AltModifier | Qt::KeyboardModifier::MetaModifier)))
        {
            QString str = key_event->text();
            if (str.length() > 0)
            {
                if ("\r" == str)
                    str = '\n';
                auto raw = str.toUtf8();
                if (raw.length() > 0)
                    emit widget.sendPacket(reinterpret_cast<const uint8_t *>(raw.constData()), static_cast<size_t>(raw.length()));
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
