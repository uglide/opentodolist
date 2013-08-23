/*
 *  OpenTodoListDesktopQml - Desktop QML frontend for OpenTodoList
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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
import net.rpdev.OpenTodoList 1.0
import "Utils.js" as Utils

View {
    id: todoListView

    property QtObject currentList : null

    property string filterText

    property TodoSortFilterModel allTopLevelTodos : TodoSortFilterModel {
        sourceModel: library.todos
        filterMode: TodoSortFilterModel.TodoListEntries | TodoSortFilterModel.HideDeleted
    }

    property TodoSortFilterModel dueTodayModel : TodoSortFilterModel {
        sourceModel: library.todos
        maxDueDate: new Date()
    }

    property TodoSortFilterModel dueThisWeekModel : TodoSortFilterModel {
        sourceModel: library.todos
        maxDueDate: Utils.getLastDateOfWeek()
    }

    property TodoSortFilterModel model : TodoSortFilterModel {
        sourceModel: layout.useCompactLayout ? null : allTopLevelTodos
        searchString: filterText
        sortMode: TodoSortFilterModel.PrioritySort
    }

    signal resetFilter()

    onCurrentListChanged: {
        filterText = "";
        resetFilter();
    }

    onModelChanged: {
        filterText = "";
        resetFilter();
    }

    signal todoSelected(QtObject todo)
    signal showTrashForList( QtObject list )
    signal showSearch()
    signal todoListClicked(QtObject list)

    onResetFilter: filterText = ""

    toolButtons: [
        ToolButton {
            label: "New List"

            onClicked: newTodoListView.hidden = false
        },
        ToolButton {
            label: "\uf002"
            font.family: symbolFont.name

            onClicked: todoListView.showSearch()
        },
        ToolButton {
            label: "\uf014"
            font.family: symbolFont.name
            enabled: todoListView.currentList !== null

            onClicked: if ( todoListView.currentList ) {
                           todoListView.showTrashForList( todoListView.currentList );
                       }
        },
        ToolButton {
            font.family: symbolFont.name
            label: "\uf011"

            onClicked: Qt.quit()
        }

    ]
    
    Rectangle {
        id: sideBar

        width: layout.useCompactLayout ? parent.width : 300
        height: todoListView.clientHeight
        color: colors.primary
        
        Behavior on width { SmoothedAnimation { velocity: 1200 } }

        Image {
            id: linkToWebPage
            source: "rpdevlogo_webheader.png"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            anchors.fill: linkToWebPage
            onClicked: Qt.openUrlExternally( "http://www.rpdev.net/home/project/opentodolist" )
            cursorShape: Qt.PointingHandCursor
        }
        
        Flickable {
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: linkToWebPage.top
            contentWidth: width
            contentHeight: todoListColumn.height
            clip: true

            Column {
                id: todoListColumn
                width: parent.width

                Button {
                    id: showAllTodosButton
                    label: "All Todos"
                    down: todoListView.model.sourceModel === todoListView.allTopLevelTodos
                    width: parent.width

                    onClicked: {
                        todoListView.currentList = null;
                        todoListView.model.sourceModel = todoListView.allTopLevelTodos
                        todoListView.todoListClicked( null )
                    }
                }

                Button {
                    id: showTodosDueToday
                    label: "Due Today"
                    down: todoListView.model.sourceModel === todoListView.dueTodayModel
                    width: parent.width

                    onClicked: {
                        todoListView.currentList = null;
                        todoListView.model.sourceModel = todoListView.dueTodayModel
                        todoListView.todoListClicked( null )
                    }
                }

                Button {
                    id: showTodosDueThisWeek
                    label: "Due This Week"
                    down: todoListView.model.sourceModel === todoListView.dueThisWeekModel
                    width: parent.width

                    onClicked: {
                        todoListView.currentList = null;
                        todoListView.model.sourceModel = todoListView.dueThisWeekModel
                        todoListView.todoListClicked( null )
                    }
                }

                Repeater {
                    model: library.todoLists
                    delegate: Button {
                        width: parent.width
                        label: object.name
                        down: todoListView.currentList == object

                        onClicked: {
                            todoListView.currentList = object
                            todoListView.model.sourceModel = object.entries
                            todoListView.todoListClicked( object )
                        }
                    }
                }
            }
        }
    }
    
    TodoListContents {
        anchors.left: sideBar.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        height: parent.height

        onTodoSelected: todoListView.todoSelected( todo )
    }

    Item {
        Timer {
            interval: 60000
            onTriggered: {
                dueTodayModel.maxDueDate = new Date()
                dueThisWeekModel.maxDueDate = Utils.getLastDateOfWeek()
            }
        }
    }

}
