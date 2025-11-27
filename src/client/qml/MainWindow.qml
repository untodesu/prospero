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

    menuBar: MainMenu {
        id: main_menu
    }

    Chat {
        id: chat

        anchors.fill: parent
        anchors.margins: 4
    }

    Greeter {
        id: greeter
    }
}
