#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include "todolistlibrary.h"
#include "opentodolistinterfaces.h"
#include "todolist.h"

#include "todoliststoragequery.h"

#include <QAbstractListModel>
#include <QSet>
#include <QVariantMap>

class TodoListLibrary;

class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( TodoListLibrary* library READ library WRITE setLibrary NOTIFY libraryChanged )
public:
    explicit TodoListModel(QObject *parent = 0);
    virtual ~TodoListModel();

    TodoListLibrary *library() const;
    void setLibrary( TodoListLibrary* library );

    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

signals:

    void libraryChanged();
    void beginUpdate();
    void endUpdate();

public slots:

    void update();

private:

    QVector< TodoList* >      m_todoLists;
    TodoListLibrary*          m_library;
    QSet< QString >           m_newTodoLists;
    QSet< QString >           m_loadedTodoLists;

    static QString todoListId(const QString &backend, const TodoListStruct &list );
    static QString todoListId( const TodoList *list );


private slots:

    void addTodoList( const QString &backend, const TodoListStruct &list );
    void removeExtraLists();

};

/**
   @brief Query all todo lists

   This query is used to get all todo lists from the database.
 */
class TodoListQuery : public TodoListStorageQuery
{
    Q_OBJECT

public:

    explicit TodoListQuery();
    virtual ~TodoListQuery();

    // TodoListStorageQuery interface
    virtual void beginRun();
    virtual bool query(QString &query, QVariantMap &args);
    virtual void recordAvailable(const QVariantMap &record);
    virtual void endRun();

signals:

    void clearList();
    void addTodoList( const QString &backend, const TodoListStruct &list );
    void finished();

};

#endif // TODOLISTMODEL_H
