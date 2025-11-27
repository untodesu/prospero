// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: ipc.hh; Created: Thu Nov 27 2025 23:19:51
// Description: Inter-process communication

#ifndef CLIENT_IPC_HH
#define CLIENT_IPC_HH 1
#pragma once

class IPC final : public QObject {
    Q_OBJECT

public:
    constexpr static const char* SERVER_NAME = "Prospero_IPC";

    static bool ensure_single_instance(void);

    static IPC* instance;

    explicit IPC(QObject* parent = nullptr);
    virtual ~IPC(void) override;

signals:
    void wakeup_requested(void);

private slots:
    void add_connection(void);
    void parse_command(void);

private:
    QLocalServer* m_server;
};

#endif
