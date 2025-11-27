// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: ipc.cc; Created: Thu Nov 27 2025 23:23:51
// Description: Inter-process communication

#include "client/precompiled.hh"

#include "client/ipc.hh"

#include "core/exception.hh"

IPC* IPC::instance = nullptr;

bool IPC::ensure_single_instance(void)
{
    // Create a client, try to connect; if it connects, assume
    // an another instance is running and send a "WAKEUP" command
    // to it, then return true. If it fails to connect, return false

    QLocalSocket socket;
    socket.connectToServer(IPC::SERVER_NAME);

    if(socket.waitForConnected(1000)) {
        QTextStream stream(&socket);
        stream << "wakeup" << Qt::endl;

        socket.flush();
        socket.waitForBytesWritten(1000);
        socket.disconnectFromServer();

        return true;
    }

    return false;
}

IPC::IPC(QObject* parent) : QObject(parent)
{
    m_server = new QLocalServer(this);

    if(!m_server->listen(IPC::SERVER_NAME)) {
        throw core::runtime_error("unable to start IPC server");
    }

    connect(m_server, &QLocalServer::newConnection, this, &IPC::add_connection);
}

IPC::~IPC(void)
{
    m_server->close();
}

void IPC::add_connection(void)
{
    auto socket = m_server->nextPendingConnection();
    connect(socket, &QLocalSocket::readyRead, this, &IPC::parse_command);
}

void IPC::parse_command(void)
{
    if(auto socket = qobject_cast<QLocalSocket*>(sender())) {
        QTextStream stream(socket);
        auto parameters = stream.readAll().trimmed().split(':');

        if(parameters.isEmpty()) {
            qWarning() << "IPC: empty command received";
            return;
        }

        if(0 == parameters[0].compare("wakeup", Qt::CaseInsensitive)) {
            emit wakeup_requested();
            return;
        }

        qWarning() << "IPC: unknown command received:" << parameters[0];
    }
}
