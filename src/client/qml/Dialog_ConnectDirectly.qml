import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    width: 240
    height: 130

    x: 0.5 * (parent.width - width)
    y: 0.5 * (parent.height - height)

    modal: true

    standardButtons: Dialog.Ok | Dialog.Cancel

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            text: qsTr("Enter the server address to connect to:")

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.pointSize: 11

            wrapMode: Text.WordWrap

            horizontalAlignment: Text.AlignHCenter
        }

        TextField {
            id: address_field

            Layout.fillWidth: true
            Layout.fillHeight: false

            placeholderText: qsTr("address[:port]")

            font.family: g_monospace.family
            font.pointSize: 10
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 8
            color: "transparent"
        }
    }

    onAccepted: {
        if(address_field.text.length > 0) {
            g_session.connect_to_host(address_field.text);
        }
    }
}
