import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: main_window

    title: qsTr("Prospero %1").arg(g_version.full)

    minimumWidth: 640
    minimumHeight: 480

    width: minimumWidth
    height: minimumHeight

    visible: true

    menuBar: MainMenu {
        id: main_menu
    }

    onClosing: function(close) {
        if(g_settings.hide_on_close) {
            main_window.visible = false;
            close.accepted = false;
            tray_icon.show();
        }
    }

    Greeter {
        id: greeter
        z: 2
    }

    Chat {
        id: chat
        z: 0
    }

    TrayIcon {
        id: tray_icon
    }

    Connections {
        target: g_session

        function onText_message_received(timestamp, username, message) {
            if(username === g_session.assigned_username) {
                return; // Don't show notifications for our own messages
            }

            let is_in_focus = true;
            is_in_focus = is_in_focus && main_window.visibility !== Window.Hidden;
            is_in_focus = is_in_focus && main_window.visibility !== Window.Minimized;
            is_in_focus = is_in_focus && main_window.active === true;

            if(is_in_focus) {
                return; // Window is in focus
            }

            if(g_settings.mute_mentions === true && g_settings.mute_messages === true) {
                return; // All notifications are muted
            }

            let should_send_notification = false;

            if(g_settings.mute_mentions === false) {
                const mentions = message.match(/\@[a-zA-Z0-9_\-\.]+/g);

                if(mentions && mentions.includes(`@${g_session.assigned_username}`)) {
                    should_send_notification = true;
                }
            }

            if(g_settings.mute_messages === false) {
                should_send_notification = true;
            }

            if(should_send_notification === true) {
                tray_icon.showMessage(username, message);
            }
        }
    }

    Connections {
        target: g_ipc

        function onWakeup_requested() {
            main_window.visible = true;
            main_window.raise();
            main_window.requestActivate();
        }
    }
}
