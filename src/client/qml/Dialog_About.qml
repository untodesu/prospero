import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    width: 360
    height: 240

    x: 0.5 * (parent.width - width)
    y: 0.5 * (parent.height - height)

    modal: true

    standardButtons: Dialog.Close

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: false

            AnimatedImage {
                readonly property real preferred_size: 32

                source: "qrc:/res/fish-spinning.gif"

                fillMode: Image.PreserveAspectFit

                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter

                Layout.preferredWidth: preferred_size * (sourceSize.width / sourceSize.height)
                Layout.preferredHeight: preferred_size
            }

            Label {
                text: qsTr("Prospero Client")

                Layout.fillWidth: true
                Layout.fillHeight: false

                font.pointSize: 12
                font.bold: true

                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 4
            color: "transparent"
        }

        TextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: g_version.full

            font.family: g_monospace.family
            font.pointSize: 10

            wrapMode: TextArea.WrapAnywhere

            readOnly: true
        }
    }
}
