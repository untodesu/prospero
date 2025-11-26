import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    width: 480

    x: 0.5 * (parent.width - width)
    y: 0.5 * (parent.height - height)

    modal: true

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 4

        Label {
            text: qsTr("Settings")

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 4
            color: "transparent"
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.minimumHeight: 240
            Layout.maximumHeight: 240
        }
    }
}
