import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    readonly property color normal_color: "transparent"
    
    property color hover_color

    property var timestamp
    property string message

    hover_color: {
        if(palette.window.hslLightness < 0.5) {
            return palette.mid;
        }
        else {
            return palette.midlight;
        }
    }

    width: parent ? parent.width : implicitWidth
    height: 10 + message_layout.implicitHeight

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
            Layout.minimumWidth: g_roboto_mono_font.pixelSize * 4

            font.family: g_roboto_mono_font.family
            font.italic: true
            font.pointSize: 8

            text: Qt.formatDateTime(timestamp, "hh:mm:ss")

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: message.split('\n').length > 1 ? Text.AlignTop : Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.25)
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: false
            
            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.125)
            
            width: 1
        }

        TextEdit {
            id: message_text

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: message

            font.family: g_roboto_mono_font.family
            font.pointSize: 11

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: message.split('\n').length > 1 ? Text.AlignTop : Text.AlignVCenter

            readOnly: true
            selectByMouse: true

            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

            color: Qt.rgba(palette.text.r, palette.text.g, palette.text.b, 0.5)
        }
    }

    HoverHandler {
        enabled: g_session.is_connected

        onHoveredChanged: {
            parent.color = hovered ? parent.hover_color : parent.normal_color;
        }
    }
}
