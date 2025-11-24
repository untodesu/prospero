import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Main Window")

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        Text {
            anchors.centerIn: parent
            text: qsTr("Hello, World!")
            font.pointSize: 24
            color: "#333333"
        }
    }

    Component.onCompleted: {
        console.log("semver", g_version.semver);
        console.log("branch", g_version.branch);
        console.log("commit", g_version.commit);
    }
}
