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
import Style 1.0

import SoftwareUpdate 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: container
    width: parent.width; height: childrenRect.height
    radius: Style.cornerRadius
    color: Style.color.dark

    Flow {
        id: flow
        width: parent.width
        spacing: 0

        // AUTO UPDATE
        Item {
            width: parent.width; height: childrenRect.height + 40
            enabled: false
            opacity: 0.5

            Text {
                id: softwareUpdateText
                color: Style.color.text
                text: qsTr("Auto update") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
            }

            BasicUI.CustomSwitch {
                id: softwareUpdateButton

                anchors { right: parent.right; rightMargin: 20; verticalCenter: softwareUpdateText.verticalCenter }

                checked: config.settings.softwareupdate.autoUpdate
                mouseArea.onClicked: {
                    var tmp = config.config
                    tmp.settings.softwareupdate.autoUpdate = !tmp.settings.softwareupdate.autoUpdate
                    config.config = tmp
                    SoftwareUpdate.autoUpdate = !SoftwareUpdate.autoUpdate
                }
            }

            Text {
                id: smallText
                color: Style.color.text
                opacity: 0.5
                text: qsTr("Automatically look for updates and update when a new software version is available.\nUpdates are installed between 03.00 am and 05.00 am.") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                width: parent.width - 40 - softwareUpdateButton.width
                anchors { left: parent.left; leftMargin: 20; top: softwareUpdateButton.bottom; topMargin: 10 }
                font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
                lineHeight: 1
            }
        }


        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: SoftwareUpdate.updateAvailable ? (childrenRect.height + 40) : (childrenRect.height + 20 - updateButton.height)
            Text {
                id: uptodateText
                color: Style.color.text
                text: SoftwareUpdate.updateAvailable ? qsTr("New software is available.\nYIO remote %1").arg(SoftwareUpdate.newVersion) + translateHandler.emptyString : qsTr("Your software is up to date.") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                width: parent.width-40
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
            }

            Text {
                id: uptodateTextsmall
                color: Style.color.text
                opacity: 0.5
                text: SoftwareUpdate.updateAvailable ? qsTr("Installed version: YIO Remote ") + SoftwareUpdate.currentVersion + translateHandler.emptyString : qsTr("YIO Remote ") + SoftwareUpdate.currentVersion
                wrapMode: Text.WordWrap
                width: parent.width - 40
                anchors { left: parent.left; leftMargin: 20; top: uptodateText.bottom; topMargin: 10 }
                font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
                lineHeight: 1
            }

            BasicUI.CustomButton {
                id: updateButton
                buttonText: qsTr("Update") + translateHandler.emptyString
                anchors { top: uptodateTextsmall.bottom; topMargin: 30; left: parent.left; leftMargin: 20 }
                visible: SoftwareUpdate.updateAvailable
                enabled: SoftwareUpdate.updateAvailable

                mouseArea.onClicked: {
                    if (SoftwareUpdate.updateAvailable) {
                        loader_second.setSource("qrc:/basic_ui/settings/SoftwareUpdateDownloading.qml")
                        loader_second.active = true;
                        SoftwareUpdate.startDownload();
                    }
                }
            }

            BasicUI.CustomButton {
                id: downloadButton
                buttonText: qsTr("Download") + translateHandler.emptyString
                anchors { top: uptodateTextsmall.bottom; topMargin: 30; left: parent.left; leftMargin: 20 }
                visible: SoftwareUpdate.installAvailable
                enabled: SoftwareUpdate.installAvailable

                mouseArea.onClicked: {
                    if (SoftwareUpdate.installAvailable) {
                        SoftwareUpdate.performAppUpdate();
                    }
                }
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: checkforUpdateButton.height + 40

            BasicUI.CustomButton {
                id: checkforUpdateButton
                buttonText: qsTr("Check for update") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }

                mouseArea.onClicked: {
                    SoftwareUpdate.checkForUpdate();
                }
            }
        }
    }
}
