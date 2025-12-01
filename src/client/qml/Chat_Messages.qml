import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models

ListView {
    id: chat_messages

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
        role: "is_notification"

        DelegateChoice {
            roleValue: true

            Chat_Delegate_Notification {
                timestamp: model.timestamp
                message: model.message
            }
        }

        DelegateChoice {
            roleValue: false

            Chat_Delegate_TextMessage {
                timestamp: model.timestamp
                username: model.username
                message: model.message
            }
        }
    }

    Connections {
        target: g_session

        function onNotification_received(timestamp, message) {
            chat_model.append({
                is_notification: true,
                username: qsTr("System Message"),
                timestamp: timestamp,
                message: message
            });

            chat_messages.positionViewAtEnd();
        }

        function onText_message_received(timestamp, username, message) {
            chat_model.append({
                is_notification: false,
                username: username,
                timestamp: timestamp,
                message: message
            });

            chat_messages.positionViewAtEnd();
        }
    }
}
