// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: main.cc; Created: Tue Nov 25 2025 01:35:18
// Description: Main entry point for client application

#include "qclient/precompiled.hh"

#include "core/exception.hh"

#include "qclient/clipboard_wrap.hh"
#include "qclient/version_wrap.hh"

int main(int argc, char** argv)
{
    try {
        auto enet_check = enet_initialize();

        if(enet_check) {
            throw core::runtime_error("enet_initialize failed");
        }

        QGuiApplication app(argc, argv);

        QQuickStyle::setStyle("Fusion");

        auto qml = new QQmlApplicationEngine();

        auto clipboard_wrap = new ClipboardWrap(&app);
        auto version_wrap = new VersionWrap(&app);

        auto context = qml->rootContext();
        context->setContextProperty("g_clipboard", clipboard_wrap);
        context->setContextProperty("g_version", version_wrap);

        qml->load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

        auto result = app.exec();

        delete qml;

        delete version_wrap;
        delete clipboard_wrap;

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
