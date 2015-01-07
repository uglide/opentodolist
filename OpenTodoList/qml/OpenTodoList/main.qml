/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015  Martin Höher <martin@rpdev.net>
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

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

import net.rpdev.OpenTodoList.Core 1.0

/*import net.rpdev.OpenTodoList.Core 1.0
import net.rpdev.OpenTodoList.DataModel 1.0
import net.rpdev.OpenTodoList.Models 1.0
import net.rpdev.OpenTodoList.SystemIntegration 1.0
import net.rpdev.OpenTodoList.Components 1.0
import net.rpdev.OpenTodoList.Views 1.0
import net.rpdev.OpenTodoList.Theme 1.0
*/

import "style" as Style
import "components" as Components
import "pages" as Pages
import "app" as App

ApplicationWindow {
    id: root

    width: 800
    height: 600

    Component.onCompleted: {
        application.handler.requestShow.connect( function() { show(); raise(); } );
        application.handler.requestHide.connect( function() { hide(); } );
        application.handler.requestToggleWindow.connect( function() {
            if ( visible ) {
                hide();
            } else {
                show();
                raise();
            }
        } );

        width = settings.getValue( "OpenTodoList/Window/width", width );
        height = settings.getValue( "OpenTodoList/Window/height", height );
    }

    onWidthChanged: settings.setValue( "OpenTodoList/Window/width", width )
    onHeightChanged: settings.setValue( "OpenTodoList/Window/height", height )

    Settings {
        id: settings
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr( "Todos" )
            MenuItem {
                text: qsTr( "Quit" )
                shortcut: StandardKey.Quit
                action: App.Actions.quit
            }
        }
    }

    toolBar: ToolBar {
        style: ToolBarStyle {
            background: Rectangle {
                color: Style.Colors.primary
            }
        }

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: toggleNavButton
                text: Style.Symbols.bars
                onClicked: {
                    navBar.state = ( navBar.state === "shown" ) ?
                                "" : "shown";
                }
                style: ButtonStyle {
                    label: Style.H1 {
                        text: toggleNavButton.text
                        color: Style.Colors.lightText
                    }
                    background: Item{}
                }
            }

            Style.H1 {
                text: todoListsPage.title
                Layout.fillWidth: true
                color: Style.Colors.lightText
            }
        }
    }

    Pages.TodoListsPage {
        id: todoListsPage
        anchors.fill: parent
    }

    Components.NavigationBar {
        id: navBar
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        width: Math.min( Style.Measures.mWidth * 30, parent.width )
        x: -width

        states: State {
            name: "shown"
            PropertyChanges {
                target: navBar
                x: 0
            }
        }

        Behavior on x { SmoothedAnimation { duration: 300 } }
    }
}

/*Window {
    id: root

    width: 800
    height: 600
    color: Colors.window

    function saveViewSettings() {
        console.debug( "SaveViewSettings" );
        settings.setValue( "OpenTodoList/ViewSettings/todoSortMode", ViewSettings.todoSortMode );
        settings.setValue( "OpenTodoList/ViewSettings/showDoneTodos", ViewSettings.showDoneTodos );
    }


    onActiveFocusItemChanged: {
        if ( activeFocusItem === null ) {
            pageStack.focus = true;
        }
    }

    Component.onCompleted: {
        application.handler.requestShow.connect( function() { show(); raise(); } );
        application.handler.requestHide.connect( function() { hide(); } );
        application.handler.requestToggleWindow.connect( function() {
            if ( visible ) {
                hide();
            } else {
                show();
                raise();
            }
        } );

        width = settings.getValue( "OpenTodoList/Window/width", width );
        height = settings.getValue( "OpenTodoList/Window/height", height );
        ViewSettings.todoSortMode = settings.getValue( "OpenTodoList/ViewSettings/todoSortMode", ViewSettings.todoSortMode );
        ViewSettings.showDoneTodos = settings.getValue( "OpenTodoList/ViewSettings/showDoneTodos", ViewSettings.showDoneTodos ) === "true";

        ViewSettings.onSettingsChanged.connect( saveViewSettings );
    }

    onWidthChanged: settings.setValue( "OpenTodoList/Window/width", width )
    onHeightChanged: settings.setValue( "OpenTodoList/Window/height", height )

    Settings {
        id: settings
    }

    PageStack {
        id: pageStack

        anchors.fill: parent
        focus: true

        onLastPageClosing: {
            if ( Qt.platform.os === "android" ) {
                // TODO: Stop main activity here
                application.handler.hideWindow()
            }
        }

        TodoListsPage {
            onTodoSelected: {
                var component = Qt.createComponent( "TodoPage.qml" );
                if ( component.status === Component.Ready ) {
                    var page = component.createObject( pageStack );
                    page.todo.fromVariant( todo.toVariant() );
                } else {
                    console.error( component.errorString() );
                }
            }
        }
    }

    Shortcut {
        keySequence: fromStandardKey( StandardKey.Quit )
        onTriggered: {
            application.handler.terminateApplication()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Style.Style.colors.
    }
}*/

