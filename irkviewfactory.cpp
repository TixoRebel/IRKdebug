#include "irkviewfactory.h"

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/core/ViewFactory.h>
#include <qtoolbutton.h>

QAbstractButton *IRKViewFactory::createTitleBarButton(QWidget *parent, KDDockWidgets::TitleBarButtonType type) const
{
    if (!parent) {
        qWarning() << Q_FUNC_INFO << "Parent not provided";
        return nullptr;
    }

    auto button = new TitleBarButton(parent);
    button->setIcon(iconForButtonType(type, parent->devicePixelRatioF()));

    return button;
}
