/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
import Haptic 1.0

Item {
    id: container
    width: 480; height: 800

    Text {
        id: titleText
        color: Style.color.text
        text: qsTr("Hello") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: parent.top; topMargin: 340; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 60 }
        lineHeight: 1
    }

    Text {
        id: smalltext
        color: Style.color.text
        opacity: 0.5
        text: qsTr("Tap the screen to begin") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: titleText.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Haptic.playEffect(Haptic.Click);
            container.parent.parent.incrementCurrentIndex();
        }
    }
}
