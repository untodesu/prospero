import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("View")

    Action {
        text: qsTr("Clear Chat")

        onTriggered: {
            chat.clear();
        }

        Accessible.name: qsTr("Clear Chat")
        Accessible.description: qsTr("Clears all messages from the chat window")
    }
}
