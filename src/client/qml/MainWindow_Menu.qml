import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

MenuBar {
    readonly property int fontSize: 10

    id: menu_bar

    font.pointSize: fontSize

    Menu {
        title: qsTr("Client")

        font.pointSize: menu_bar.fontSize

        Menu {
            title: qsTr("Connect")

            font.pointSize: menu_bar.fontSize

            enabled: !g_session.is_connected

            cascade: true

            Action {
                text: qsTr("By address")

                enabled: !g_session.is_connected

                onTriggered: {
                    direct_connect_dialog.open();
                }
            }

            Action {
                text: qsTr("By bookmark")

                enabled: !g_session.is_connected

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
            text: qsTr("Identity")

            onTriggered: {
                identity_dialog.open()
            }
        }

        Action {
            text: qsTr("Settings")

            onTriggered: {
                // TODO
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

    MainWindow_MenuFilters {
        id: filter_menu
    }

    Menu {
        title: qsTr("View")

        font.pointSize: menu_bar.fontSize

        Action {
            text: qsTr("Clear Chat")

            onTriggered: {
                chat.clear();
            }
        }
    }

    Menu {
        title: qsTr("Help")

        font.pointSize: menu_bar.fontSize

        Action {
            text: qsTr("Documentation")

            onTriggered: {
                // TODO
            }
        }

        MenuSeparator {}

        Action {
            text: qsTr("About")

            onTriggered: about_dialog.open()
        }
    }
}
