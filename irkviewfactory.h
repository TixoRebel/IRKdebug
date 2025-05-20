#ifndef IRKVIEWFACTORY_H
#define IRKVIEWFACTORY_H

#include <kddockwidgets/ViewFactory.h>
#include <qtoolbutton.h>

class IRKViewFactory : public KDDockWidgets::QtWidgets::ViewFactory
{
    Q_OBJECT
public:
    IRKViewFactory() = default;

    QAbstractButton *createTitleBarButton(QWidget *parent, KDDockWidgets::TitleBarButtonType) const override;

private:
    Q_DISABLE_COPY(IRKViewFactory)
};

class TitleBarButton : public QToolButton
{
    Q_OBJECT
public:
    explicit TitleBarButton(QWidget *parent): QToolButton(parent)
    {
        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    }
};

#endif // IRKVIEWFACTORY_H
