import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: chat
    
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

    function clear() {
        chat_messages.model.clear();
    }

    Keys.forwardTo: chat_input
}
