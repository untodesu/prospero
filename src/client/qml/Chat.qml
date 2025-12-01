import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: chat
    
    anchors.fill: parent
    anchors.margins: 4

    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: false

        height: 1

        color: palette.mid
        opacity: 0.5
    }

    Chat_Messages {
        id: chat_messages
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: false

        height: 1

        color: palette.mid
        opacity: 0.5
    }

    Chat_Input {
        id: chat_input
    }

    Keys.forwardTo: [chat_input]

    function clear_input_text() {
        chat_input.clear_text();
    }

    function append_input_text(text) {
        chat_input.append_text(text);
    }

    function focus_input() {
        chat_input.focus_input();
    }
}
