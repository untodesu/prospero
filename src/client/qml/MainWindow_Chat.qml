import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: main_window_chat

    MainWindow_Chat_Messages {
        id: main_window_chat_list
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: false

        height: 1

        color: palette.button
        opacity: 0.5
    }

    MainWindow_Chat_Input {
        id: main_window_chat_input
    }

    function clear() {
        main_window_chat_list.model.clear();
    }
}
