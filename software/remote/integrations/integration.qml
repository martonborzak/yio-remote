////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTEGRATION TEMPLATE FILE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

import QtQuick 2.0
import Integration 1.0

Integration {
    id: integration

    // PROPERTIES OF THE INTEGRATION
    // bool connected - tells if the integration is connected. Set connected to true on succesfull connection. Set connected to false when disconnected.
    // int integrationId - the id of the integration
    // string type - type of the integration, for example: homeassistant
    // string friendlyName - friendly name of the integration

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT AND DISCONNECT FUNCTIONS
    // Must be the same function name for every integration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function connect()
    {
        // write connect function here
    }

    function disconnect()
    {
        // write disconnect function here
    }

    onConnectedChanged: {
        // when the connection state changes this signal triggered
        if (connected) {
            // remove notifications that say couldn't connec to Home Assistant
            var tmp = notifications;
            tmp.forEach(function(entry, index, object) {
                if (entry.text === "Failed to connect to " + friendlyName + ".") {
                    tmp.splice(index, 1);
                }
            });
            notifications = tmp;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // rest of the code starts here
}
