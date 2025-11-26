import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("Client")

    Menu {
        title: qsTr("Connect")

        cascade: true

        enabled: !g_session.is_connected

        Action {
            text: qsTr("By addess")

            onTriggered: {
                connect_by_address_dialog.open();
            }

            Accessible.name: qsTr("Connect by Address")
            Accessible.description: qsTr("Open dialog to connect to a host by specifying its address")
        }

        Action {
            text: qsTr("By bookmark")

            onTriggered: {
                // TODO
            }

            Accessible.name: qsTr("Connect by Bookmark")
            Accessible.description: qsTr("Open dialog to connect to a host by selecting a stored server address")
        }
    }

    Action {
        text: qsTr("Disconnect")

        enabled: g_session.is_connected

        onTriggered: {
            g_session.disconnect_from_host();
        }

        Accessible.name: qsTr("Disconnect from Host")
        Accessible.description: qsTr("Disconnects the client from the currently connected host")    
    }

    MenuSeparator {}

    Action {
        text: qsTr("Settings")

        onTriggered: {
            settings_dialog.open();
        }

        Accessible.name: qsTr("Open Settings")
        Accessible.description: qsTr("Open the settings dialog to configure application preferences")
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

