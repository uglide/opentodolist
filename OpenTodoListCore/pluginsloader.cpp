/*
 *  OpenTodoList - A todo and task manager
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

#include "pluginsloader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QSet>

PluginsLoader::PluginsLoader(QObject *parent) :
    QObject(parent),
    m_backendInterfaces()
{
    // load static instanced
    foreach ( QObject* instance, QPluginLoader::staticInstances() ) {
        //instance->setParent( this ); // static instances already have parent!
        addBackendInterface( instance );
    }

    foreach ( QString libraryPath, QCoreApplication::libraryPaths() ) {
        // Load backends
        QDir dir( libraryPath + "/opentodobackends" );
        foreach ( QString entry, dir.entryList( QDir::Files ) ) {
            QString pluginPath = dir.absolutePath() + "/" + entry;
            QPluginLoader* loader = new QPluginLoader( pluginPath, this );
            if ( loader->load() ) {
                loader->instance()->setParent( this );
                addBackendInterface( loader->instance() );
            } else {
                qDebug() << "Failed to load" << loader->fileName() << "because of:" << loader->errorString();
            }
        }
    }
}

PluginsLoader::BackendInterfaces PluginsLoader::backendInterfaces() const
{
    return m_backendInterfaces;
}

void PluginsLoader::addBackendInterface(QObject *plugin)
{
    BackendInterface *backend = qobject_cast< BackendInterface* >( plugin );
    if ( backend ) {
        m_backendInterfaces.append( backend );
        qDebug() << "Added backend" << backend->name() << "(" << backend->id() << ")";
    }
}
