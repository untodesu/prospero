import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("Client")

    Action {
        text: qsTr("Disconnect")

        enabled: g_session.is_connected

        onTriggered: {
            g_session.disconnect_from_host();

            chat.clear();
        }

        Accessible.name: qsTr("Disconnect from Host")
        Accessible.description: qsTr("Disconnects the client from the currently connected host")    
    }

    MenuSeparator {}

    Action {
        text: qsTr("Quit")

        onTriggered: {
            g_session.disconnect_from_host();

            Qt.quit();
        }

        Accessible.name: qsTr("Quit Application")
        Accessible.description: qsTr("Quits the application")
    }
}

