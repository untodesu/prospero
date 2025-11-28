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

            font.family: g_monospace.family
            font.pointSize: 10

            opacity: 0.5

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
            opacity: 0.25
            width: 2
        }

        TextEdit {
            Layout.fillWidth: false
            Layout.fillHeight: true

            font.family: g_monospace.family
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
            opacity: 0.25
            width: 2
        }

        TextEdit {
            id: content_text

            Layout.fillWidth: true
            Layout.fillHeight: true

            font.family: g_monospace.family
            font.pointSize: 11

            text: message

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

            color: palette.windowText

            MouseArea {
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton

                cursorShape: Qt.IBeamCursor
                
                onClicked: function(mouse) {
                    if(mouse.modifiers & Qt.ShiftModifier) {
                        let reply_lines = [];
    
                        const lines = message.split("\n");
    
                        for(let i = 0; i < lines.length; i++) {
                            reply_lines.push("> " + lines[i]);
                        }
    
                        reply_lines.push("");

                        reply_lines.push("@" + username + " ");
    
                        chat.set_input_text(reply_lines.join("\n"));
                    }
                }
            }
        }

        Highlighter {
            target: content_text.textDocument

            mention_color: {
                if(palette.window.hslLightness < 0.5) {
                    return "#00CCFF";
                }
                else {
                    return "#333399";
                }
            }

            quotation_color: {
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
        enabled: g_session.is_connected

        onHoveredChanged: {
            parent.color = hovered ? parent.hover_color : parent.normal_color;
        }
    }
}
