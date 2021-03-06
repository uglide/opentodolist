/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
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

#ifndef OPENTODOLISTCOREQMLPLUGIN_H
#define OPENTODOLISTCOREQMLPLUGIN_H

#include <QQmlExtensionPlugin>

namespace OpenTodoList {

/**
 * @brief Encapsulates core functionality of the application
 *
 * This namespace is used for generic functionality required within the application. This
 * functionality is either in the form of "helper functions" or bridging C++ with QML.
 */
namespace Core {

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = 0);

    virtual void registerTypes( const char *uri );

signals:

public slots:

};

} /* Core */

} /* OpenTodoList */

#endif // OPENTODOLISTCOREQMLPLUGIN_H
