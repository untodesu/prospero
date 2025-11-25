import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    readonly property int initial_width: 640
    readonly property int initial_height: 480

    id: main_window

    title: qsTr("Prospero")

    width: initial_width
    height: initial_height
    visible: true

    minimumWidth: initial_width
    minimumHeight: initial_height

    menuBar: MainWindow_Menu {
        id: menu
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4

        MainWindow_Chat {
            id: chat
        }

        MainWindow_Input {
            id: input
        }
    }

    Dialog_About {
        id: about_dialog
    }
    
    Dialog_ConnectDirectly {
        id: direct_connect_dialog
    }

    Dialog_Identity {
        id: identity_dialog
    }
}
