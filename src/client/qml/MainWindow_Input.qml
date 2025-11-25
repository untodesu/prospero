import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: input_row

    Layout.fillWidth: true
    Layout.fillHeight: false

    ComboBox {
        id: channel_selector

        Layout.fillWidth: false
        Layout.fillHeight: true
        Layout.minimumWidth: 160
        Layout.alignment: Qt.AlignTop

        Layout.minimumHeight: g_monospace.pixelSize
        Layout.maximumHeight: g_monospace.pixelSize * 2
        Layout.preferredHeight: message_scroll_view.height

        font.family: g_monospace.family
        font.pointSize: 10

        Accessible.name: qsTr("Channel Selector")
        Accessible.description: qsTr("Selects the channel to send the message to")

        textRole: "channel_name"

        popup.y: -popup.height
    }

    ScrollView {
        id: message_scroll_view

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: g_monospace.pixelSize
        Layout.maximumHeight: g_monospace.pixelSize * 5

        TextArea {
            id: message_input

            width: parent.width

            clip: true

            focus: true

            font.family: g_monospace.family
            font.pointSize: 10

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

        Layout.minimumHeight: g_monospace.pixelSize
        Layout.maximumHeight: g_monospace.pixelSize * 2
        Layout.preferredHeight: message_scroll_view.height

        text: "\u25B6" // Unicode black right-pointing triangle

        Accessible.name: qsTr("Send Message")
        Accessible.description: qsTr("Sends the message typed in the input field")

        onClicked: {
            if(channel_selector.currentIndex >= 0) {
                let message = message_input.text.trim();

                if(message.length > 0) {
                    g_session.send_text_message(channel_selector.model[channel_selector.currentIndex].channel_id, message);
                    message_input.focus = true;
                    message_input.clear();
                }
            }
        }
    }

    Connections {
        target: g_session

        function onChannels_changed() {
            input_row.rebuild_channel_selector();
        }

        function onFilter_changed() {
            input_row.rebuild_channel_selector();
        }
    }

    function rebuild_channel_selector() {
        channel_selector.model = [];

        for(let i = 0; i < g_session.channels.length; ++i) {
            if(g_session.channels[i].length > 0 && g_session.is_filtered(i)) {
                channel_selector.model.push({
                    channel_name: g_session.channels[i],
                    channel_id: i
                });
            }
        }

        if(channel_selector.model.length > 0) {
            channel_selector.currentIndex = 0;
        }
    }
}
