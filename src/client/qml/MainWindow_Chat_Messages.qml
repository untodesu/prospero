import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models

ListView {
    id: main_window_chat_messages

    Layout.fillWidth: true
    Layout.fillHeight: true

    spacing: 0

    clip: true

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    model: ListModel {
        id: chat_model
    }

    delegate: DelegateChooser {
        role: "is_system_message"

        DelegateChoice {
            roleValue: true

            MainWindow_Chat_NotificationDelegate {
                timestamp: model.timestamp
                message: model.message
            }
        }

        DelegateChoice {
            roleValue: false

            MainWindow_Chat_TextMessageDelegate {
                timestamp: model.timestamp
                username: model.username
                message: model.message
            }
        }
    }

    Connections {
        target: g_session

        function onSystem_message_received(timestamp, message) {
            chat_model.append({
                is_system_message: true,
                username: qsTr("System Message"),
                timestamp: timestamp,
                message: message
            });

            main_window_chat_messages.positionViewAtEnd();
        }

        function onText_message_received(timestamp, username, message) {
            chat_model.append({
                is_system_message: false,
                username: username,
                timestamp: timestamp,
                message: message
            });

            main_window_chat_messages.positionViewAtEnd();
        }
    }

    function clear() {
        chat_model.clear();
    }
}
