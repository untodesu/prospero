import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Menu {
    id: filter_menu

    title: qsTr("Filters")

    font.pointSize: 10

    enabled: g_session.is_connected && g_session.channels.length > 0

    Instantiator {
        id: filter_instantiator

        model: g_session.channels

        delegate: Action {
            readonly property int channel_id: index
            readonly property string channel_name: modelData

            text: channel_name

            checkable: true
            checked: g_session.is_filtered(channel_id)

            onTriggered: {
                if(checked) {
                    g_session.filter_channel(channel_id);
                }
                else {
                    g_session.unfilter_channel(channel_id);
                }
            }
        }

        onObjectAdded: function(index, object) {
            if(g_session.channels[index].length > 0) {
                filter_menu.insertAction(index, object);
            }
        }

        onObjectRemoved: function(index, object) {
            if(g_session.channels[index].length > 0) {
                filter_menu.removeAction(object);
            }
        }
    }

    Connections {
        target: g_session

        function onFilter_changed() {
            for(let i = 0; i < filter_instantiator.count; ++i) {
                let action = filter_instantiator.objectAt(i);

                if(action && action.channel_name.length > 0) {
                    action.checked = g_session.is_filtered(action.channel_id);
                }
            }
        }

        function onConnection_changed(is_connected) {
            while(filter_menu.actionAt(0)) {
                filter_menu.removeAction(filter_menu.actionAt(0));
            }
        }
    }
}
