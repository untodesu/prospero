import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    readonly property color hover_color: palette.midlight
    readonly property color normal_color: "transparent"

    property var timestamp
    property string message

    width: parent ? parent.width : implicitWidth
    height: 4 + message_layout.implicitHeight

    color: normal_color

    RowLayout {
        id: message_layout

        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        TextEdit {
            id: timestamp_text

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
            id: message_text

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: message

            font.family: g_monospace.family
            font.pointSize: 11

            opacity: 0.5

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

            color: palette.text
        }
    }

    HoverHandler {
        onHoveredChanged: {
            parent.color = hovered ? parent.hover_color : parent.normal_color;
        }
    }
}
