import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: main_window

    title: qsTr("Prospero")

    minimumWidth: 640
    minimumHeight: 480

    width: minimumWidth
    height: minimumHeight

    visible: true

    menuBar: MainWindow_Menu {
        id: main_window_menu
    }

    MainWindow_Chat {
        id: main_window_chat

        anchors.fill: parent
        anchors.margins: 4
    }

    MainWindow_Dialog_About {
        id: about_dialog
    }

    MainWindow_Dialog_ConnectByAddress {
        id: connect_by_address_dialog
    }

    MainWindow_Dialog_Settings {
        id: settings_dialog
    }
}
