// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2025 Kirill Dmitrievich
// File: host.cc; Created: Tue Nov 25 2025 00:44:38
// Description: Network management

#include "server/precompiled.hh"

#include "server/host.hh"

#include "core/buffer.hh"
#include "core/exception.hh"
#include "core/protocol.hh"

#include "server/sessions.hh"
#include "server/settings.hh"

static ENetHost* enet_host = nullptr;

void host::init(void)
{
    settings::host::max_peers = std::clamp<unsigned int>(settings::host::max_peers, 4U, ENET_PROTOCOL_MAXIMUM_PEER_ID + 1U);
    settings::host::udp_port = std::clamp<unsigned short>(settings::host::udp_port, 1024U, 65535U);

    ENetAddress address = {};
    address.host = ENET_HOST_ANY;
    address.port = settings::host::udp_port;

    enet_host = enet_host_create(&address, settings::host::max_peers, 2U, 0U, 0U);

    if(enet_host == nullptr) {
        throw core::runtime_error("failed to create a server host");
    }

    LOG_INFO("server listening on ENET_HOST_ANY:{}", settings::host::udp_port);
}

void host::shutdown(void)
{
    enet_host_service(enet_host, nullptr, 500);
    enet_host_destroy(enet_host);
}

void host::update(void)
{
    thread_local ENetEvent event;

    while(0 < enet_host_service(enet_host, &event, 10)) {
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                sessions::create(event.peer);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                sessions::remove(event.peer);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                sessions::update(event.peer, event.packet);
                enet_packet_destroy(event.packet);
                break;
        }
    }
}
