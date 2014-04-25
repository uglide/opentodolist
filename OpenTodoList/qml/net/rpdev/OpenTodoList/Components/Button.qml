/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014  Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import net.rpdev.OpenTodoList.Theme 1.0

Rectangle {
    id: button

    property alias text: label.text

    signal clicked()

    gradient: Gradient {
        GradientStop {
            color: Colors.button
            position: 0.0
        }
        GradientStop {
            color: Qt.darker( Colors.button )
            position: 1.0
        }
    }
    width: label.width * 1.5
    height: label.height * 2
    border { color: Colors.border; width: Measures.midBorderWidth }
    radius: Measures.extraTinySpace

    Label {
        id: label
        text: qsTr( "Button" )
        x: width * 0.25
        y: height * 0.5
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: button.clicked()
    }
}
