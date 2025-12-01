import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQml.Models

ListView {
    readonly property int message_type_notification: 0
    readonly property int message_type_text: 1
    readonly property int message_type_image: 2

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
        role: "message_type"

        DelegateChoice {
            roleValue: chat_messages.message_type_notification

            Chat_Delegate_Notification {
                timestamp: model.timestamp
                message: model.message
            }
        }

        DelegateChoice {
            roleValue: chat_messages.message_type_text

            Chat_Delegate_TextMessage {
                timestamp: model.timestamp
                username: model.username
                message: model.message
            }
        }

        DelegateChoice {
            roleValue: chat_messages.message_type_image

            Chat_Delegate_Image {
                timestamp: model.timestamp
                username: model.username
                data_url: model.message
            }
        }
    }

    Connections {
        target: g_session

        function onNotification_received(timestamp, message) {
            chat_model.append({
                message_type: chat_messages.message_type_notification,
                username: qsTr("System Message"),
                timestamp: timestamp,
                message: message
            });

            chat_messages.positionViewAtEnd();
        }

        function onText_message_received(timestamp, username, message) {
            chat_model.append({
                message_type: chat_messages.message_type_text,
                username: username,
                timestamp: timestamp,
                message: message
            });

            chat_messages.positionViewAtEnd();
        }

        function onImage_message_received(timestamp, username, data_url) {
            chat_model.append({
                message_type: chat_messages.message_type_image,
                username: username,
                timestamp: timestamp,
                message: data_url
            });

            chat_messages.positionViewAtEnd();
        }
    }
}
