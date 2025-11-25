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

    menuBar: MenuBar {

    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4

        TabBar {
            id: tab_bar

            Layout.fillWidth: false
            Layout.fillHeight: true

            contentItem: ListView {
                model: tab_bar.contentModel
                currentIndex: tab_bar.currentIndex
                spacing: tab_bar.spacing
                orientation: ListView.Vertical
            }

            TabButton {
                text: qsTr("Channel 1")
            }

            TabButton {
                text: qsTr("Channel 2")
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: "lightgray"

            Text {
                anchors.centerIn: parent
                text: qsTr("Page %1").arg(tab_bar.currentIndex)
            }
        }
    }
}
