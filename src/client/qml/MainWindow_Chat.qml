import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ListView {
    id: chat_list_view

    Layout.fillWidth: true
    Layout.fillHeight: true

    spacing: 8

    clip: true

    ScrollBar.vertical: ScrollBar {
        id: vertical_scrollbar
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
                    font.pointSize: 10

                    text: model.is_system ? qsTr("System Message") : model.sender

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

                TextEdit {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    font.family: g_monospace.family
                    font.pointSize: 8

                    opacity: 0.5

                    text: g_session.channels[model.channel]

                    visible: !model.is_system

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    readOnly: true
                    selectByMouse: true

                    color: palette.windowText
                }
            }

            TextEdit {
                anchors.margins: 2

                Layout.fillWidth: true
                Layout.fillHeight: true

                font.pointSize: 11

                textFormat: TextEdit.MarkdownText
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
                "is_system": true,
                "timestamp": timestamp,
                "message": message,
                "channel": 0,
                "sender": ""
            });

            chat_list_view.positionViewAtEnd();
        }

        function onText_message_received(channel, timestamp, sender, message) {
            if(g_session.is_filtered(channel)) {
                chat_model.append({
                    "is_system": false,
                    "timestamp": timestamp,
                    "message": message,
                    "channel": channel,
                    "sender": sender
                });

                chat_list_view.positionViewAtEnd();
            }
        }
    }

    function clear() {
        chat_model.clear();
    }
}
