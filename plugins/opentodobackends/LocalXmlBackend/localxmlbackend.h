/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013 - 2015  Martin Höher <martin@rpdev.net>
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

#ifndef LOCALXMLBACKEND_H
#define LOCALXMLBACKEND_H

#include "opentodolistinterfaces.h"

#include <QDomDocument>

using namespace OpenTodoList;

class LocalXmlBackend : public QObject, public OpenTodoList::IBackend
{
    Q_OBJECT
    Q_INTERFACES(OpenTodoList::IBackend)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "net.rpdev.OpenTodoList.Backend/1.0" FILE "LocalXmlBackend.json")
#endif // QT_VERSION >= 0x050000
    
public:
    explicit LocalXmlBackend(QObject *parent = 0);
    virtual ~LocalXmlBackend();

    // BackendInterface interface
    void setDatabase(OpenTodoList::IDatabase *database) override;
    void setLocalStorageDirectory(const QString &directory) override;
    QString name() const override;
    QString title() const override;
    QString description() const override;
    QSet<Capabilities> capabilities() const override;
    bool start() override;
    bool stop() override;
    void sync() override;

private:

    OpenTodoList::IDatabase         *m_database;
    QString                          m_localStorageDirectory;

    OpenTodoList::IAccount          *m_account;

    QStringList locateTodoLists() const;
    QStringList locateTodos( const QString &todoList ) const;
    QStringList locateTasks( const QString &todo ) const;

    static bool todoListToFile( const OpenTodoList::ITodoList *todoList );
    static bool todoToFile( const OpenTodoList::ITodo *todo );
    static bool taskToFile( const OpenTodoList::ITask *task );

    static bool todoListToDom( const OpenTodoList::ITodoList *list, QDomDocument &doc );
    static bool domToTodoList( const QDomDocument &doc, OpenTodoList::ITodoList *list );
    static bool todoToDom( const OpenTodoList::ITodo *todo, QDomDocument &doc );
    static bool domToTodo(const QDomDocument &doc, OpenTodoList::ITodo *todo );
    static bool taskToDom( const OpenTodoList::ITask *task, QDomDocument &doc );
    static bool domToTask( const QDomDocument &doc, OpenTodoList::ITask *task );

    void deleteTodoLists();
    void deleteTodos();
    void deleteTasks();

    void saveTodoLists();
    void saveTodos();
    void saveTasks();

    void saveTodoList(OpenTodoList::ITodoList *todoList);
    void saveTodo(OpenTodoList::ITodo *todo);
    void saveTask(OpenTodoList::ITask *task);

    void fixTodoList( const QString &todoList );
    void fixTodo( const QString &todo );

    QDomDocument documentForFile( const QString &fileName ) const;
    void documentToFile( const QDomDocument &doc, const QString &fileName ) const;
    QByteArray hashForFile( const QString &fileName ) const;

    bool todoListNeedsUpdate( OpenTodoList::ITodoList *todoList, const QString &fileName, QByteArray &hash ) const;
    bool todoNeedsUpdate( OpenTodoList::ITodo *todo, const QString &fileName, QByteArray &hash ) const;
    bool taskNeedsUpdate( OpenTodoList::ITask *task, const QString &fileName, QByteArray &hash ) const;


    static const QString TodoListConfigFileName;
    static const QString TodoDirectoryName;

    static const QString TodoListMetaFileName;
    static const QString TodoListMetaHash;
    static const QString TodoMetaFileName;
    static const QString TodoMetaHash;
    static const QString TaskMetaFileName;
    static const QString TaskMetaHash;

};


#endif // LOCALXMLBACKEND_H
