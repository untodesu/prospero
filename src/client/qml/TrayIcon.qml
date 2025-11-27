import QtQuick 2.15
import QtQuick.Controls 2.15

import Qt.labs.platform 1.1

SystemTrayIcon {
    id: tray_icon

    icon.source: "qrc:/res/icons/prospero.ico"

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
            text: qsTr("Show Main Window")

            enabled: !main_window.visible

            font.bold: true

            onTriggered: {
                main_window.visible = true;
                main_window.raise();
                main_window.requestActivate();
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
