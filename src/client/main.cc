// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: main.cc; Created: Tue Nov 25 2025 01:35:18
// Description: Main entry point for client application

#include "client/precompiled.hh"

#include "core/exception.hh"

#include "client/clipboard.hh"
#include "client/identity.hh"
#include "client/session.hh"
#include "client/version.hh"

int main(int argc, char** argv)
{
    try {
        auto enet_check = enet_initialize();

        if(enet_check) {
            throw core::runtime_error("enet_initialize failed");
        }

        QApplication app(argc, argv);

        auto roboto_regular_id = QFontDatabase::addApplicationFont(":/res/fonts/Roboto-Medium.ttf");
        auto roboto_mono_id = QFontDatabase::addApplicationFont(":/res/fonts/RobotoMono-Medium.ttf");

        if(roboto_regular_id < 0 || roboto_mono_id < 0) {
            throw core::runtime_error("font loading failed");
        }

        QQuickStyle::setStyle("Fusion");

        QFont roboto_regular_font(QFontDatabase::applicationFontFamilies(roboto_regular_id).constFirst());
        QFont roboto_mono_font(QFontDatabase::applicationFontFamilies(roboto_mono_id).constFirst());

        roboto_regular_font.setPointSize(11);
        roboto_mono_font.setPointSize(11);

        app.setFont(roboto_regular_font);

        auto qml = new QQmlApplicationEngine();

        Clipboard::instance = new Clipboard(&app);
        Identity::instance = new Identity(&app);
        Session::instance = new Session(&app);
        Version::instance = new Version(&app);

        auto context = qml->rootContext();
        context->setContextProperty("g_clipboard", Clipboard::instance);
        context->setContextProperty("g_identity", Identity::instance);
        context->setContextProperty("g_monospace", roboto_mono_font);
        context->setContextProperty("g_session", Session::instance);
        context->setContextProperty("g_version", Version::instance);

        qml->load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

        auto result = app.exec();

        delete qml;

        delete Clipboard::instance;
        delete Identity::instance;
        delete Session::instance;
        delete Version::instance;

        enet_deinitialize();

        return result;
    }
    catch(const std::exception& ex) {
        QMessageBox::critical(nullptr, QObject::tr("Fatal Error"), QObject::tr("An unhandled exception occurred: %1").arg(ex.what()));
        return EXIT_FAILURE;
    }
    catch(...) {
        QMessageBox::critical(nullptr, QObject::tr("Fatal Error"), QObject::tr("An unhandled unknown exception occurred."));
        return EXIT_FAILURE;
    }
}
