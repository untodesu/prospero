import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

MenuBar {
    id: main_window_menu

    MainWindow_Menu_Client {
        id: main_window_menu_client
    }

    MainWindow_Menu_Chat {
        id: main_window_menu_chat
    }

    MainWindow_Menu_Help {
        id: main_window_menu_help
    }
}
