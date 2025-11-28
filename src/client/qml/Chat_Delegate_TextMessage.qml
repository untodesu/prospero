import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import ProsperoChat 1.0

Rectangle {
    readonly property color hover_color: palette.mid
    readonly property color normal_color: "transparent"

    property var timestamp
    property string username
    property string message

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

            font.family: g_roboto_mono_font.family
            font.italic: true
            font.pointSize: 8

            opacity: 0.25

            text: Qt.formatDateTime(timestamp, "hh:mm:ss")

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            color: palette.text
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: false

            color: palette.text
            opacity: 0.125

            width: 1
        }

        TextEdit {
            Layout.fillWidth: false
            Layout.fillHeight: true

            font.family: g_roboto_mono_font.family
            font.pointSize: 11

            text: username

            opacity: 0.5

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            color: palette.text
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: false

            color: palette.text
            opacity: 0.125

            width: 1
        }

        TextEdit {
            id: content_text

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: actions_button.height

            font.family: g_roboto_mono_font.family
            font.pointSize: 11

            text: message

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

            color: palette.windowText
        }

        Button {
            id: actions_button

            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.preferredWidth: implicitHeight
            Layout.alignment: Qt.AlignTop

            text: "\u21A9" // Unicode Leftwards Arrow with Hook

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
                        chat.focus_input();
                    }
                }

                Action {
                    text: qsTr("Quote && Reply")

                    onTriggered: {
                        const quoted_message = message.split('\n').map(line => (`> ${line}`)).join('\n');

                        chat.clear_input_text();
                        chat.append_input_text(quoted_message + '\n');
                        chat.append_input_text(`@${username} `);
                        chat.focus_input();
                    }
                }
            }
        }

        Highlighter {
            target: content_text.textDocument

            quote_color: {
                if(palette.window.hslLightness < 0.5) {
                    return "#80FF80";
                }
                else {
                    return "#336600";
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
