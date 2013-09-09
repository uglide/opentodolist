#include "abstracttodo.h"

#include "abstracttodolist.h"
#include "todosortfiltermodel.h"

/**
 * @class AbstractTodo
 * @brief Base class for all todos
 *
 * This is the base class for all todos. Depending on the platform, this
 * class get's subclassed to provide e.g. platform specific storage.
 */

/**
 * @brief Constructor
 * @param parent The parent object to use for the todo
 */
AbstractTodo::AbstractTodo(QUuid id, AbstractTodoList *parent) :
    QObject(parent),
    m_id( id ),
    m_title( QString() ),
    m_description( QString() ),
    m_progress( 0 ),
    m_priority( -1 ),
    m_parentTodo( 0 ),
    m_deleted( false ),
    m_dueDate( QDateTime() ),
    m_subTodosModel( new TodoSortFilterModel( this ) )
{
    m_subTodosModel->setParentTodo( this );
    m_subTodosModel->setSourceModel( parent->todos() );
    m_subTodosModel->setFilterMode( TodoSortFilterModel::SubTodos | TodoSortFilterModel::HideDeleted );
    m_subTodosModel->sort( 0 );
    
    connect( m_subTodosModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
             this, SLOT(childDataChanged()) );
    
    connect( this, SIGNAL(idChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(titleChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(descriptionChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(progressChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(priorityChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(parentTodoChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(deletedChanged()), this, SIGNAL(changed()) );
    connect( this, SIGNAL(dueDateChanged()), this, SIGNAL(changed()));
}

/**
   @brief The globally unique ID of the todo

   A globally unique ID that can be used to identify a todo even across
   networks.
 */
QUuid AbstractTodo::id() const
{
    return m_id;
}

int AbstractTodo::priority() const
{
    return m_priority;
}

void AbstractTodo::setPriority(int priority)
{
    m_priority = qBound( -1, priority, 10 );
    emit priorityChanged();
}

int AbstractTodo::progress() const
{
    if ( m_subTodosModel->rowCount() > 0 ) {
        int result = 0;
        for ( int i = 0; i < m_subTodosModel->rowCount(); ++i ) {
            AbstractTodo* subTodo = qobject_cast< AbstractTodo* >(
                        m_subTodosModel->index( i, 0 ).data(
                            TodoSortFilterModel::TodoModel::ObjectRole ).value< QObject* >() );
            result += subTodo->progress();
        }
        return result / m_subTodosModel->rowCount();
    } else {
        return m_progress;
    }
}

void AbstractTodo::setProgress(int progress)
{
    m_progress = qBound( 0, progress, 100 );
    emit progressChanged();
}

QString AbstractTodo::description() const
{
    return m_description;
}

void AbstractTodo::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QString AbstractTodo::title() const
{
    return m_title;
}

void AbstractTodo::setTitle(const QString &title)
{
    m_title = title;
    emit titleChanged();
}

AbstractTodo* AbstractTodo::parentTodo() const
{
    return m_parentTodo;
}

void AbstractTodo::setParentTodo(QObject* parentTodo)
{
    if ( !parentTodo || parentTodo->parent() == parent() ) {
        AbstractTodo* p = static_cast< AbstractTodo* >( parentTodo );
        while ( p && p != this ) {
            p = p->parentTodo();
        }
        if ( p != this ) {
            m_parentTodo = static_cast< AbstractTodo* >( parentTodo );
            emit parentTodoChanged();
        }
    }
}

bool AbstractTodo::isDeleted() const
{
    return m_deleted;
}

void AbstractTodo::setDeleted(bool deleted)
{
    m_deleted = deleted;
    emit deletedChanged();
}

QDateTime AbstractTodo::dueDate() const
{
    return m_dueDate;
}

void AbstractTodo::setDueDate(const QDateTime &dateTime)
{
    m_dueDate = dateTime;
    emit dueDateChanged();
}

AbstractTodoList* AbstractTodo::parent() const
{
    return qobject_cast< AbstractTodoList* >( QObject::parent() );
}

void AbstractTodo::childDataChanged()
{
    // emulate a change of progress to propagate any changes up the tree
    if ( m_subTodosModel->rowCount() > 0 ) {
        emit progressChanged();
    }
}

/**
   @brief Overrides the ID of the todo

   This overrides the @p id of the todo. Note that you do not usually want to use
   this method as it internally changes the identity of the todo. In fact, the
   only real use case is when the todo is restored when restarting the
   application and there is no "sane" way for the todo list to
   know the ID upfront the todo is created and can read the ID back on it's own
   (e.g. from external memory or the network).
 */
void AbstractTodo::setId(QUuid id)
{
    m_id = id;
    emit idChanged();
}
