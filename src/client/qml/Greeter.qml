import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    readonly property string icon_normal: "qrc:/res/icons/prospero-256.png"
    readonly property string icon_funny: "qrc:/res/icons/fish-spinning.gif"

    id: greeter_panel

    anchors.fill: parent
    anchors.margins: 0

    color: "transparent"

    Connections {
        target: g_session

        function onConnection_changed() {
            greeter_panel.visible = !g_session.is_connected;

            if(greeter_panel.visible) {
                address_field.focus = true;

                if(Math.random() < 0.125) {
                    logo_image.source = greeter_panel.icon_funny;
                    logo_image.playing = true;
                }
                else {
                    logo_image.source = greeter_panel.icon_normal;
                    logo_image.playing = false;
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.2; color: Qt.rgba(0, 0, 0, 0.50); }
            GradientStop { position: 0.8; color: Qt.rgba(0, 0, 0, 0.75); }
        }

        MouseArea {
            anchors.fill: parent
            enabled: true
            propagateComposedEvents: true
        }
    }

    Rectangle {
        width: 480
        height: 400

        x: 0.5 * (parent.width - width)
        y: 0.5 * (parent.height - height)

        color: palette.window

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            RowLayout {
                AnimatedImage {
                    id: logo_image

                    Layout.maximumWidth: title_section.height
                    Layout.maximumHeight: title_section.height

                    source: greeter_panel.icon_normal
                }

                ColumnLayout {
                    id: title_section

                    Layout.fillWidth: true
                    Layout.fillHeight: false

                    Label {
                        text: qsTr("Prospero Client")

                        Layout.fillWidth: true
                        Layout.fillHeight: false

                        font.pointSize: 12
                        font.bold: true

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    TextEdit {
                        text: g_version.full

                        Layout.fillWidth: true
                        Layout.fillHeight: false

                        font.family: g_roboto_mono_font.family
                        font.pointSize: 8

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        readOnly: true
                        selectByMouse: true

                        color: palette.text
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

            GridLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                columns: 2

                Label {
                    Layout.fillWidth: false
                    Layout.fillHeight: true

                    text: qsTr("Username:")

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                }

                TextField {
                    id: username_field
                    
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: g_settings.username

                    font.family: g_roboto_mono_font.family

                    onTextChanged: {
                        g_settings.username = text;
                    }

                    validator: RegularExpressionValidator {
                        regularExpression: /^[a-zA-Z0-9_\-\.]+$/
                    }

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft

                    ToolTip.text: qsTr("Desired username sent to Prospero servers upon connection")
                }

                Label {
                    Layout.fillWidth: false
                    Layout.fillHeight: true

                    text: qsTr("Identity:")

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    TextField {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: g_settings.public_key.substring(0, 24) + "..."

                        font.family: g_roboto_mono_font.family

                        readOnly: true
                        selectByMouse: true
                        enabled: false

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft

                        ToolTip.text: qsTr("An Ed25519 public key used for authentication and encryption with Prospero servers")
                    }

                    Button {
                        Layout.fillWidth: false
                        Layout.fillHeight: true

                        text: qsTr("Copy")

                        onClicked: {
                            g_clipboard.set_text(g_settings.public_key);
                        }
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
                    height: 16 + font.pixelSize

                    color: normal_color

                    Label {
                        id: server_address

                        text: qsTr("[%1] %2").arg(index + 1).arg(modelData)

                        anchors.fill: parent
                        anchors.margins: 4

                        font.family: g_roboto_mono_font.family
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

                    font.family: g_roboto_mono_font.family

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
                }

                Button {
                    id: connect_button

                    Layout.fillWidth: false
                    Layout.fillHeight: true
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
}
