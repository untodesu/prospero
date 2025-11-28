import QtQuick 2.15
import QtQuick.Controls 2.15

import Qt.labs.platform 1.1

SystemTrayIcon {
    id: tray_icon

    icon.source: "qrc:/res/icons/prospero.ico"

    tooltip: qsTr("Prospero")

    onActivated: function(reason) {
        if(reason == SystemTrayIcon.DoubleClick) {
            main_window.visible = true;
            main_window.raise();
            main_window.requestActivate();
        }
    }

    onMessageClicked: {
        main_window.visible = true;
        main_window.raise();
        main_window.requestActivate();
    }

    Component.onCompleted: {
        tray_icon.show();
    }

    menu: Menu {
        MenuItem {
            text: qsTr("Prospero")

            icon.source: "qrc:/res/icons/prospero.ico"

            font.bold: true

            enabled: false
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Mute Messages")

            checkable: true

            checked: g_settings.mute_messages

            onTriggered: {
                g_settings.mute_messages = checked;
            }
        }

        MenuItem {
            text: qsTr("Mute Mentions")

            checkable: true

            checked: g_settings.mute_mentions

            onTriggered: {
                g_settings.mute_mentions = checked;
            }
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Quit")

            onTriggered: {
                g_session.disconnect_from_host();

                Qt.quit();
            }
        }
    }
}
