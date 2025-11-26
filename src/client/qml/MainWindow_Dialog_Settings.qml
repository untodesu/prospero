import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: settings_dialog

    width: 480

    x: 0.5 * (parent.width - width)
    y: 0.5 * (parent.height - height)

    modal: true

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }

    ColumnLayout {
        anchors.margins: 2
        spacing: 4

        width: parent.width

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
            Layout.minimumHeight: 240

            contentWidth: parent.width

            GridLayout {
                width: parent.width

                columns: 2

                Label {
                    text: qsTr("Desired username")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                }

                TextField {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: g_settings.username

                    font.family: g_monospace.family

                    enabled: !g_session.is_connected

                    onTextChanged: {
                        g_settings.username = text;
                    }

                    validator: RegularExpressionValidator {
                        regularExpression: /^[a-zA-Z0-9_\-\.]+$/
                    }

                    Accessible.name: qsTr("Desired username input field")
                    Accessible.description: qsTr("Input field for the desired username")
                }

                Label {
                    text: qsTr("Identity")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: g_settings.public_key

                        font.family: g_monospace.family

                        readOnly: true
                        selectByMouse: true

                        Accessible.name: qsTr("Public key display field")
                        Accessible.description: qsTr("Display field for the public key")
                    }

                    Button {
                        id: identity_button

                        Layout.fillWidth: false
                        Layout.fillHeight: true

                        text: qsTr("Copy")

                        onClicked: {
                            g_clipboard.set_text(g_settings.public_key);
                        }

                        Accessible.name: qsTr("Copy public key menu item")
                        Accessible.description: qsTr("Menu item to copy the public key to the clipboard")
                    }
                }
            }
        }
    }
}
