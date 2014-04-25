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
import QtQuick.Layouts 1.1
import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Theme 1.0

Rectangle {
    id: root

    width: 360
    height: 360
    focus: true
    color: Colors.window

    Keys.onEscapePressed: Qt.quit()
    Keys.onBackPressed: Qt.quit()

    TodoListLibrary {
        id: todoListLibrary
    }

    TabView {
        id: tabView
        anchors.fill: parent
        tabs: [
            Tab {
                id: todoListTab

                name: qsTr( "Todo Lists" )

                TodoModel {
                    id: todoModel
                    library: todoListLibrary
                    queryType: TodoModel.QueryTopLevelTodosInTodoList
                }

                TodoListView {
                    id: todoListView

                    library: todoListLibrary
                    showTodosInline: todoListTab.width < Measures.minimumPageWidth * 2
                    anchors {
                        left: parent.left
                        right: todoView.left
                        top: parent.top
                        bottom: parent.bottom
                    }

                    onTodoListSelected: {
                        todoModel.todoList = todoList;
                        todoModel.update();
                    }
                }
                TodoView {
                    id: todoView

                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: todoModel.count > 0 && parent.width > 2 * Measures.minimumPageWidth ?
                               parent.width - Measures.minimumPageWidth : 0
                    model: width > 0 ? todoModel : null

                    Behavior on width { SmoothedAnimation { velocity: 1500 } }
                }
            },
            Tab {
                name: qsTr( "DevUtils" )
                Column {
                    anchors.fill: parent
                    Button {
                        text: qsTr( "Reload Todo Lists" )
                        onClicked: todoListView.update()
                    }
                }
            }
        ]
    }
}
