import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: main_window_chat_input

    Layout.fillWidth: true
    Layout.fillHeight: false

    ScrollView {
        id: message_scroll_view

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: g_monospace.pixelSize * 2
        Layout.maximumHeight: g_monospace.pixelSize * 5

        TextArea {
            id: message_input

            width: parent.width

            clip: true

            focus: true

            font.family: g_monospace.family


            placeholderText: qsTr("Type a message...")

            Accessible.name: qsTr("Message Input Field")
            Accessible.description: qsTr("Field to type your message before sending")

            wrapMode: TextArea.Wrap

            Keys.onPressed: function (event) {
                if (event.key === Qt.Key_Enter || event.key == Qt.Key_Return) {
                    if (!(event.modifiers & Qt.ShiftModifier)) {
                        send_message.clicked();

                        event.accepted = true;
                    }
                }
            }

            horizontalAlignment: TextArea.AlignLeft
            verticalAlignment: TextArea.AlignVCenter
        }

        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        contentItem: Flickable {
            boundsBehavior: Flickable.StopAtBounds
            contentWidth: parent.width
            contentHeight: parent.height
        }
    }

    Button {
        id: send_message

        Layout.fillWidth: false
        Layout.fillHeight: false
        Layout.preferredWidth: height
        Layout.alignment: Qt.AlignTop

        Layout.minimumHeight: g_monospace.pixelSize * 2
        Layout.maximumHeight: g_monospace.pixelSize * 2
        Layout.preferredHeight: message_scroll_view.height

        text: "\u25B6" // Unicode black right-pointing triangle

        Accessible.name: qsTr("Send Message")
        Accessible.description: qsTr("Sends the message typed in the input field")

        onClicked: {
            let message = message_input.text.trim();

            if (message.length > 0 && g_session.is_authenticated) {
                g_session.send_text_message(message);
                message_input.focus = true;
                message_input.clear();
            }
        }
    }
}
