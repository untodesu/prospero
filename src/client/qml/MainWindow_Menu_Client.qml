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
        }

        Action {
            text: qsTr("By bookmark")

            onTriggered: {
                // TODO
            }
        }
    }

    Action {
        text: qsTr("Disconnect")

        enabled: g_session.is_connected

        onTriggered: {
            g_session.disconnect_from_host();
        }
    }

    MenuSeparator {}

    Action {
        text: qsTr("Settings")

        onTriggered: {
            settings_dialog.open();
        }
    }
    
    MenuSeparator {}

    Action {
        text: qsTr("Quit")

        onTriggered: {
            g_session.disconnect_from_host();

            Qt.quit();
        }
    }
}

