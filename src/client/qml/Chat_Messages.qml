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

    contentWidth: parent === null ? -1 : parent.width

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
                message: model.payload
            }
        }

        DelegateChoice {
            roleValue: chat_messages.message_type_text

            Chat_Delegate_TextMessage {
                timestamp: model.timestamp
                username: model.username
                message: model.payload
            }
        }

        DelegateChoice {
            roleValue: chat_messages.message_type_image

            Chat_Delegate_ImageMessage {
                timestamp: model.timestamp
                username: model.username
                source_url: model.payload

                onFinished_loading: {
                    chat_messages.positionViewAtEnd();
                }
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
                payload: message
            });

            chat_messages.positionViewAtEnd();
        }

        function onText_message_received(timestamp, username, message) {
            chat_model.append({
                message_type: chat_messages.message_type_text,
                username: username,
                timestamp: timestamp,
                payload: message
            });

            chat_messages.positionViewAtEnd();
        }

        function onImage_message_received(timestamp, username, source) {
            chat_model.append({
                message_type: chat_messages.message_type_image,
                username: username,
                timestamp: timestamp,
                payload: source
            });

            chat_messages.positionViewAtEnd();
        }
    }
}
