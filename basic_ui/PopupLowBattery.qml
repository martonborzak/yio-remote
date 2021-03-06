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
import QtQuick.Controls 2.5
import StandbyControl 1.0
import Style 1.0

import "qrc:/basic_ui" as BasicUI

Popup {
    id: lowBatteryNotification
    x: 40; y: 40
    width: parent.width-80; height: parent.height-80
    modal: true
    focus: true
    clip: true
    closePolicy: Popup.CloseOnPressOutside

    enter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutExpo; duration: 300 }
            NumberAnimation { property: "scale"; from: 0.0; to: 1.0; easing.type: Easing.OutBack; duration: 300 }
        }

    exit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InExpo; duration: 300 }
            NumberAnimation { property: "scale"; from: 1.0; to: 0.0; easing.type: Easing.InExpo; duration: 300 }
        }

    background: Rectangle {
        anchors.fill: parent
        color: Style.color.highlight1
        radius: 8
    }

    onOpened: {
        lowBatteryNotificationTimer.start()
    }

    BasicUI.ProgressCircle {
        id: lowBatteryNotificationProgressCircle
        size: 40
        colorCircle: Style.color.line
        colorCircleGrad: Style.color.line
        colorBackground: Style.color.dark
        showBackground: false
        arcBegin: 0; arcEnd: 0
        animationDuration: 300
        lineWidth: 4
        anchors { right: parent.right; rightMargin: 10; top: parent.top; topMargin: 10 }
    }

    Timer {
        id: lowBatteryNotificationTimer
        running: false
        repeat: true
        interval: 200

        onTriggered: {
            if (lowBatteryNotificationProgressCircle.arcEnd >= 360) {
                lowBatteryNotificationTimer.stop()
                lowBatteryNotification.close()
                lowBatteryNotificationProgressCircle.arcEnd = 0
            } else {
                lowBatteryNotificationProgressCircle.arcEnd += 3;
            }

        }
    }

    Text {
        id: lowbatteryIcon
        color: Style.color.text
        text: Style.icon.low_battery
        width: 170; height: 170
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 240 }
        anchors { horizontalCenter: parent.horizontalCenter; horizontalCenterOffset: 14; top: parent.top; topMargin: 175 }
    }

    Text {
        color: Style.color.text
        text: qsTr("Low battery") + translateHandler.emptyString
        width: 200
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors { top: lowbatteryIcon.bottom; topMargin: -20; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Bold;  pixelSize: 27 }
    }

    Text {
        color: Style.color.text
        text: qsTr("Please charge the remote soon") + translateHandler.emptyString
        width: 200
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors { bottom: parent.bottom; bottomMargin: 60; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 25 }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            lowBatteryNotificationTimer.stop()
            lowBatteryNotification.close()
            lowBatteryNotificationProgressCircle.arcEnd = 0
        }
    }
}
