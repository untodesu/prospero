import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: connect_by_address_dialog
    
    width: 320

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
            text: qsTr("Connect by Address")

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 4
            color: "transparent"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextField {
                id: address_field

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.minimumHeight: font.pixelSize + 8

                placeholderText: qsTr("address or address:port")

                font.family: g_monospace.family

                focus: true

                Keys.onPressed: function (event) {
                    if(event.key === Qt.Key_Enter || event.key == Qt.Key_Return) {
                        connect_button.clicked();

                        event.accepted = true;
                    }
                }

                Accessible.name: qsTr("Connect by Address Input Field")
                Accessible.description: qsTr("Field to type the address or address and port of the host to connect to")
            }

            Button {
                id: connect_button
                
                Layout.fillWidth: false
                Layout.fillHeight: false
                Layout.preferredWidth: height
                Layout.alignment: Qt.AlignTop

                Layout.minimumHeight: address_field.height
                Layout.maximumHeight: address_field.height

                text: "\u25B6" // Unicode black right-pointing triangle

                onClicked: {
                    if(address_field.text.length > 0) {
                        address_field.enabled = false;
                        connect_button.enabled = false;

                        g_session.connect_to_host(address_field.text);

                        connect_by_address_dialog.close();

                        address_field.enabled = true;
                        connect_button.enabled = true;
                    }
                }

                Accessible.name: qsTr("Connect by Address Button")
                Accessible.description: qsTr("Button to connect to the host specified in the input field")
            }
        }
    }
}
