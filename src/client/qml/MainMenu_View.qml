import QtQuick 2.15
import QtQuick.Controls 2.15

Menu {
    title: qsTr("View")

    Action {
        text: qsTr("Clear Chat")

        onTriggered: {
            chat.clear();
        }
    }

    Action {
        text: qsTr("Clear on Disconnect")

        checkable: true

        checked: g_settings.clear_on_disconnect

        onTriggered: {
            g_settings.clear_on_disconnect = checked;
        }
    }

    MenuSeparator {}

    Action {
        text: qsTr("Hide on Close")

        checkable: true

        checked: g_settings.hide_on_close

        onTriggered: {
            g_settings.hide_on_close = checked;
        }
    }

    MenuSeparator {}

    Action {
        text: qsTr("Mute Messages")

        checkable: true

        checked: g_settings.mute_messages

        onTriggered: {
            g_settings.mute_messages = checked;
        }
    }

    Action {
        text: qsTr("Mute Mentions")

        checkable: true

        checked: g_settings.mute_mentions

        onTriggered: {
            g_settings.mute_mentions = checked;
        }
    }
}
