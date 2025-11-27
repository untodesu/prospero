import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: greeter_dialog
    
    width: 480

    x: 0.5 * (parent.width - width)
    y: 0.5 * (parent.height - height)

    modal: true

    visible: !g_session.is_connected

    onRejected: Qt.callLater(greeter_dialog.open)

    Connections {
        target: g_session

        function onConnection_changed() {
            greeter_dialog.visible = !g_session.is_connected;
        }
    }

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 8

        Label {
            text: qsTr("Prospero Client")

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.bold: true

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Label {
            text: g_version.full

            Layout.fillWidth: true
            Layout.fillHeight: false

            font.family: g_monospace.family
            font.pointSize: 8

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: false
            color: palette.button
            opacity: 0.5
            height: 1
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            columns: 2

            Label {
                text: qsTr("Username:")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }

            TextField {
                id: username_field
                
                anchors.margins: 4

                Layout.fillWidth: true
                Layout.fillHeight: true

                text: g_settings.username

                font.family: g_monospace.family

                onTextChanged: {
                    g_settings.username = text;
                }

                validator: RegularExpressionValidator {
                    regularExpression: /^[a-zA-Z0-9_\-\.]+$/
                }

                Accessible.name: qsTr("Username Input Field")
                Accessible.description: qsTr("Field to type the desired username for the session")
            }

            Label {
                text: qsTr("Identity:")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                TextField {
                    Layout.fillWidth: true
                    
                    anchors.margins: 4

                    text: g_settings.public_key

                    font.family: g_monospace.family
                    font.pointSize: 10

                    readOnly: true
                    selectByMouse: true

                    Accessible.name: qsTr("Identity Display Field")
                    Accessible.description: qsTr("Field displaying the public key identity")
                }

                Button {
                    Layout.fillWidth: false
                    Layout.fillHeight: true

                    text: qsTr("Copy")

                    onClicked: {
                        g_clipboard.set_text(g_settings.public_key);
                    }

                    Accessible.name: qsTr("Copy Identity Button")
                    Accessible.description: qsTr("Button to copy the public key identity to the clipboard")
                }
            }
        }
        
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: false
            color: palette.button
            opacity: 0.5
            height: 1
        }

        ListView {
            id: recent_servers

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 160

            spacing: 0

            clip: true

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            model: []

            Component.onCompleted: {
                recent_servers.model = g_recents.list;
            }

            Connections {
                target: g_recents

                function onUpdated() {
                    recent_servers.model = g_recents.list;
                }
            }

            delegate: Rectangle {
                readonly property color click_color: palette.mid
                readonly property color hover_color: palette.midlight
                readonly property color normal_color: "transparent"

                width: parent ? parent.width : implicitWidth
                height: 8 + font.pixelSize

                color: normal_color

                Label {
                    id: server_address

                    text: qsTr("[%1] %2").arg(index + 1).arg(modelData)

                    anchors.fill: parent
                    anchors.margins: 4

                    font.family: g_monospace.family
                    font.pointSize: 11

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft

                    opacity: 1.0 - 0.5 * (index / g_recents.list.length)
                }

                MouseArea {
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    anchors.fill: parent

                    hoverEnabled: true

                    onClicked: function(mouse) {
                        if(mouse.button === Qt.RightButton) {
                            g_recents.remove(modelData);
                        }
                        else {
                            address_field.text = modelData;
                        }
                    }

                    onDoubleClicked: function(mouse) {
                        if(mouse.button === Qt.LeftButton) {
                            address_field.text = modelData;
                            connect_button.clicked();
                        }
                    }

                    onEntered: {
                        parent.color = hover_color;
                    }

                    onExited: {
                        parent.color = normal_color;
                    }

                    onPressed: {
                        parent.color = click_color;
                    }

                    onReleased: {
                        if(containsMouse) {
                            parent.color = hover_color;
                        }
                        else {
                            parent.color = normal_color;
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: false

            TextField {
                id: address_field

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.minimumHeight: font.pixelSize + 8

                placeholderText: qsTr("Host address...")

                font.family: g_monospace.family

                focus: true

                validator: RegularExpressionValidator {
                    regularExpression: /^[a-zA-Z0-9\.\-_:]+$/
                }

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
                Layout.alignment: Qt.AlignTop

                Layout.minimumHeight: address_field.height
                Layout.maximumHeight: address_field.height

                text: qsTr("Connect")

                onClicked: {
                    if(address_field.text.length > 0) {
                        g_recents.add(address_field.text);
                        g_session.connect_to_host(address_field.text);
                    }
                }

                Accessible.name: qsTr("Connect by Address Button")
                Accessible.description: qsTr("Button to connect to the host specified in the input field")
            }

            Connections {
                target: g_session

                function onConnection_changed() {
                    if(g_session.is_connected) {
                        recent_servers.enabled = false;
                        address_field.enabled = false;
                        connect_button.enabled = false;
                    }
                    else {
                        recent_servers.enabled = true;
                        address_field.enabled = true;
                        connect_button.enabled = true;
                    }
                }

                function onConnection_started() {
                    recent_servers.enabled = false;
                    address_field.enabled = false;
                    connect_button.enabled = false;
                }
            }
        }
    }
}
