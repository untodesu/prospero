import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    title: qsTr("Help")

    Action {
        text: qsTr("Documentation")

        onTriggered: {
            console.log("TODO");
        }

        Accessible.name: qsTr("Open Documentation")
        Accessible.description: qsTr("Opens the application documentation in the default web browser")
    }
    
    MenuSeparator {}

    Action {
        text: qsTr("About")

        onTriggered: {
            about_dialog.open();
        }

        Accessible.name: qsTr("About Application")
        Accessible.description: qsTr("Opens the about dialog displaying application information")
    }
}
