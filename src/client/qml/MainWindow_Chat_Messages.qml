import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ListView {
    id: main_window_chat_messages

    Layout.fillWidth: true
    Layout.fillHeight: true

    spacing: 8

    clip: true

    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
    }

    model: ListModel {
        id: chat_model
    }

    delegate: Rectangle {
        readonly property color hover_color: palette.button
        readonly property color normal_color: "transparent"

        width: parent ? parent.width : implicitWidth
        height: message_layout.implicitHeight + 8

        color: normal_color

        ColumnLayout {
            id: message_layout

            anchors.fill: parent
            anchors.margins: 4
            spacing: 2

            RowLayout {
                TextEdit {
                    Layout.fillWidth: false
                    Layout.fillHeight: true

                    font.family: g_monospace.family
                    font.bold: true

                    text: model.username

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    readOnly: true
                    selectByMouse: true

                    color: palette.windowText
                }

                TextEdit {
                    Layout.fillWidth: false
                    Layout.fillHeight: true

                    font.family: g_monospace.family
                    font.pointSize: 8

                    opacity: 0.5

                    text: Qt.formatDateTime(model.timestamp, "yyyy-MM-dd hh:mm:ss")

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    readOnly: true
                    selectByMouse: true

                    color: palette.windowText
                }
            }

            TextEdit {
                anchors.margins: 64

                Layout.fillWidth: true
                Layout.fillHeight: true

                text: model.message

                wrapMode: Text.WordWrap

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop

                readOnly: true
                selectByMouse: true

                color: palette.windowText
            }
        }

        HoverHandler {
            onHoveredChanged: {
                parent.color = hovered ? parent.hover_color : parent.normal_color;
            }
        }
    }

    Connections {
        target: g_session

        function onSystem_message_received(timestamp, message) {
            chat_model.append({
                username: qsTr("System Message"),
                timestamp: timestamp,
                message: message
            });

            main_window_chat_messages.positionViewAtEnd();
        }

        function onText_message_received(timestamp, sender, message) {
            chat_model.append({
                username: sender,
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
