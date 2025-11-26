import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("Chat")

    Action {
        text: qsTr("Clear")

        onTriggered: {
            main_window_chat.clear();
        }

        Accessible.name: qsTr("Clear Chat")
        Accessible.description: qsTr("Clears all messages from the chat window")
    }
}
