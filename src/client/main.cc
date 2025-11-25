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

        QQuickStyle::setStyle("Fusion");

        auto qml = new QQmlApplicationEngine();

        Clipboard::instance = new Clipboard(&app);
        Identity::instance = new Identity(&app);
        Session::instance = new Session(&app);
        Version::instance = new Version(&app);

        auto context = qml->rootContext();
        context->setContextProperty("g_clipboard", Clipboard::instance);
        context->setContextProperty("g_identity", Identity::instance);
        context->setContextProperty("g_session", Session::instance);
        context->setContextProperty("g_version", Version::instance);

        qml->load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

        // Debugging purposes
        Session::instance->connect_to_host("127.0.0.1");

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
