import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    width: 480
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

        Label {
            text: qsTr("Client identity")

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.pointSize: 12
            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            color: "transparent"
        }

        TextArea {
            id: public_key_area

            text: g_identity.public_key

            font.family: g_monospace.family

            wrapMode: TextArea.WrapAnywhere

            readOnly: true

            Layout.fillWidth: true
            Layout.fillHeight: true

            Connections {
                target: g_identity

                function onKeypair_changed() {
                    public_key_area.text = g_identity.public_key;
                }
            }
        }

        RowLayout {
            Button {
                Layout.fillWidth: true

                text: qsTr("Copy to Clipboard")

                onClicked: {
                    g_clipboard.set_text(g_identity.public_key);
                }

                Accessible.name: qsTr("Copy to Clipboard")
                Accessible.description: qsTr("Copies the public key to the clipboard")
            }

            Button {
                id: menu_button

                Layout.preferredWidth: height

                text: "\u25BC"

                onClicked: {
                    dropdown_menu.popup(menu_button, 0, menu_button.height);
                }

                Accessible.name: qsTr("Additional operations")
                Accessible.description: qsTr("Opens a menu with additional identity operations")
            }
        }
    }

    Menu {
        id: dropdown_menu

        MenuItem {
            text: qsTr("Import")

            onTriggered: {
                g_identity.import_keypair()
            }

            enabled: !g_session.is_connected

            Accessible.name: qsTr("Import Keypair")
            Accessible.description: qsTr("Imports a keypair from a file")
        }

        MenuItem {
            text: qsTr("Export")

            onTriggered: {
                g_identity.export_keypair()
            }

            Accessible.name: qsTr("Export Keypair")
            Accessible.description: qsTr("Exports the current keypair to a file")
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Generate")

            onTriggered: {
                g_identity.generate_keypair()
            }

            enabled: !g_session.is_connected

            Accessible.name: qsTr("Generate Keypair")
            Accessible.description: qsTr("Generates a new keypair, replacing the current one")
        }
    }
}
