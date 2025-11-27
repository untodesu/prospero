import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("View")

    Action {
        text: qsTr("Minimize on Close")

        checkable: true

        checked: g_settings.minimize_on_close

        onTriggered: {
            g_settings.minimize_on_close = checked;
        }

        Accessible.name: qsTr("Minimize on Close")
        Accessible.description: qsTr("Toggles whether the application minimizes to the system tray when the window is closed")
    }

    MenuSeparator {}

    Action {
        text: qsTr("Mute Messages")

        checkable: true

        checked: g_settings.mute_messages

        onTriggered: {
            g_settings.mute_messages = checked;
        }

        Accessible.name: qsTr("Mute Messages")
        Accessible.description: qsTr("Toggles chat message sounds")
    }

    Action {
        text: qsTr("Mute Mentions")

        checkable: true

        checked: g_settings.mute_mentions

        onTriggered: {
            g_settings.mute_mentions = checked;
        }

        Accessible.name: qsTr("Mute Mentions")
        Accessible.description: qsTr("Toggles mention notification sounds")
    }

    MenuSeparator {}
    
    Action {
        text: qsTr("Clear Chat")

        onTriggered: {
            chat.clear();
        }

        Accessible.name: qsTr("Clear Chat")
        Accessible.description: qsTr("Clears all messages from the chat window")
    }
}
