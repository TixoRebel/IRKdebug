#include <kddockwidgets/MainWindow.h>
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/Config.h>

// #include <oclero/qlementine.hpp>

#include <QStyleFactory>
#include <QApplication>
#include <QDirIterator>
#include <QMenuBar>

#include "connectionmanager.h"
// #include "irkviewfactory.h"
#include "terminalwidget.h"

int main(int argc, char **argv)
{
    QApplication::setStyle("fusion");

    QColor darkGray(53, 53, 53);
    QColor gray(128, 128, 128);
    QColor black(25, 25, 25);
    QColor blue(42, 130, 218);

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, darkGray);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, black);
    darkPalette.setColor(QPalette::AlternateBase, darkGray);
    darkPalette.setColor(QPalette::ToolTipBase, blue);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, darkGray);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Link, blue);
    darkPalette.setColor(QPalette::Highlight, blue);
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    darkPalette.setColor(QPalette::Active, QPalette::Button, gray.darker());
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

    QApplication::setPalette(darkPalette);

    QApplication app(argc, argv);

    // QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    // QCoreApplication::setApplicationName(QStringLiteral("Test app"));

    // auto* style = new oclero::qlementine::QlementineStyle(&app);
    // style->setAnimationsEnabled(true);
    // style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
    // QApplication::setStyle(style);

    // auto* themeManager = new oclero::qlementine::ThemeManager(style);
    // themeManager->loadDirectory(":/resources/themes");
    // themeManager->setCurrentTheme("Dark");

    KDDockWidgets::initFrontend(KDDockWidgets::FrontendType::QtWidgets);
    KDDockWidgets::Config::self().setFlags(KDDockWidgets::Config::Flag_NativeTitleBar | KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible | KDDockWidgets::Config::Flag_ShowButtonsOnTabBarIfTitleBarHidden | KDDockWidgets::Config::Flag_AlwaysShowTabs | KDDockWidgets::Config::Flag_AllowReorderTabs | KDDockWidgets::Config::Flag_CloseOnlyCurrentTab);
    // KDDockWidgets::Config::self().setViewFactory(new IRKViewFactory());

    auto conMngr = new ConnectionManager();

    // # 1. Create our main window
    KDDockWidgets::QtWidgets::MainWindow mainWindow(QStringLiteral("MyMainWindow"));
    mainWindow.setWindowTitle("Main Window");
    mainWindow.resize(800, 400);
    mainWindow.menuBar()->addMenu("File")->addAction("Connect", [=] {
        auto connection = conMngr->addConnection(TcpConnection::create(QHostAddress::LocalHost, 38405));
        connection->start();
    });
    mainWindow.show();

    // # 2. Create a dock widget, it needs a unique name
    auto dock1 = new KDDockWidgets::QtWidgets::DockWidget(QStringLiteral("Terminal"));
    auto widget1 = new TerminalWidget();
    QObject::connect(conMngr, &ConnectionManager::packetReceived, widget1, &TerminalWidget::packetReceived, Qt::ConnectionType::BlockingQueuedConnection);
    QObject::connect(widget1, &TerminalWidget::sendPacket, conMngr, &ConnectionManager::sendPacket);
    dock1->setWidget(widget1);

    // 3. Add them to the main window
    mainWindow.addDockWidget(dock1, KDDockWidgets::Location_OnBottom);

    return app.exec();
}
