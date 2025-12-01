import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Rectangle {
    readonly property color normal_color: "transparent"

    property color hover_color

    property var timestamp
    property string username
    property string data_url

    hover_color: {
        if(palette.window.hslLightness < 0.5) {
            return palette.mid;
        }
        else {
            return palette.midlight;
        }
    }

    width: parent ? parent.width : implicitWidth
    height: 6 + message_layout.implicitHeight

    color: normal_color

    RowLayout {
        id: message_layout

        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        TextEdit {
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.minimumWidth: g_roboto_mono_font.pixelSize * 4

            font.family: g_roboto_mono_font.family
            font.italic: true
            font.pointSize: 8

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.25)

            text: Qt.formatDateTime(timestamp, "hh:mm:ss")

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignTop

            readOnly: true
            selectByMouse: true
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: false

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.125)

            width: 1
        }

        TextEdit {
            Layout.fillWidth: false
            Layout.fillHeight: true

            font.family: g_roboto_mono_font.family
            font.pointSize: 11

            text: username

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignTop

            readOnly: true
            selectByMouse: true

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.5)
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: false

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.125)

            width: 1
        }

        AnimatedImage {
            id: content_data

            Layout.fillWidth: true
            Layout.fillHeight: false

            fillMode: Image.PreserveAspectFit

            source: {
                console.log(data_url);
                return data_url;
            }
        }

        Button {
            id: actions_button

            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.preferredWidth: implicitHeight
            Layout.alignment: Qt.AlignTop

            text: "\u25BE" // Unicode Black Down-Pointing Small Triangle

            opacity: hover_handler.hovered && g_session.is_connected ? 1.0 : 0.0

            onClicked: {
                actions_menu.open();
                actions_menu.x = -actions_menu.implicitWidth - 4;
            }

            Menu {
                id: actions_menu

                Action {
                    text: qsTr("Reply")

                    onTriggered: {
                        chat.clear_input_text();
                        chat.append_input_text(`@${username} `);
                    }
                }

                MenuSeparator {}

                Action {
                    text: qsTr("Copy Image")

                    onTriggered: {
                        g_clipboard.set_text(data_url);
                    }
                }
            }
        }
    }

    HoverHandler {
        id: hover_handler

        enabled: g_session.is_connected

        onHoveredChanged: {
            if(hovered || actions_menu.visible) {
                parent.color = parent.hover_color;
            }
            else {
                parent.color = parent.normal_color;
            }
        }
    }

    Connections {
        target: actions_menu

        function onClosed() {
            hover_handler.hoveredChanged();
        }
    }
}
