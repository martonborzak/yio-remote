/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11
import WifiControl 1.0
import Style 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: section
    width: parent.width
    height: childrenRect.height + 40 //remoteConfigText.height + smallText.height + 60
    radius: Style.cornerRadius
    color: Style.color.dark

    Text {
        id: remoteConfigText
        color: Style.color.text
        text: qsTr("Remote configuration") + translateHandler.emptyString
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    BasicUI.CustomSwitch {
        id: remoteConfigButton

        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: remoteConfigText.verticalCenter

        checked: remoteConfigEnabled
        mouseArea.onClicked: {
            if (!remoteConfigEnabled) {
                webserver.startService();
                remoteConfigEnabled = true;
            } else {
                webserver.stopService();
                remoteConfigEnabled = false;
            }
        }
    }


    Text {
        id: smallText
        color: Style.color.text
        opacity: 0.5
        text: qsTr("Use your browser to configure your YIO remote or download and upload backups.\nNavigate your internet browser to: http://") + wifi.wifiStatus.ipAddress + translateHandler.emptyString
        wrapMode: Text.WordWrap
        width: parent.width - 40 - remoteConfigButton.width
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: remoteConfigButton.bottom
        anchors.topMargin: 10
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }
}
