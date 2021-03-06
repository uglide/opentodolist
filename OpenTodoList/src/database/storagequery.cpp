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

#include "database/storagequery.h"

#include "database/database.h"
#include "database/databaseworker.h"

#include <QJsonDocument>

namespace OpenTodoList {

namespace DataBase {

/**
   @brief Constructor
 */
StorageQuery::StorageQuery(QObject *parent) :
  QObject(parent)
{
}

/**
   @brief Destructor
 */
StorageQuery::~StorageQuery()
{

}

/**
   @brief The query is to be executed

   This method is called when the query is to be executed.
 */
void StorageQuery::beginRun()
{
  // nothing to be done here
}

/**
   @brief The query to be run

   This method is called to get the query that is to be run. Sub-classes
   should write the SQL query into @p query. If the query contains any
   user specified values, they can be put into the @p args map. When executing
   the query, the args will be inserted into the query string in a safe way.

   Certain queries might have additional prerequisites for their execution. These can be expressed
   by setting flags in the @p queryOptions which are specified in the QueryOptions enum. These
   flags influence the execution of the query.

   @return True in case everything is okay. In that case, the query will next
     be run on the database. If this returns false, nothing happens and
     execution will stop (but endRun() will still be called.
 */
bool StorageQuery::query(QString &query, QVariantMap &args, int &queryOptions)
{
  Q_UNUSED( query );
  Q_UNUSED( args );
  Q_UNUSED( queryOptions );
  return false;
}

/**
   @brief A new record is available

   This is called for each record that is read from the database when
   running the query.

   The values read are passed in via the @p record map.
 */
void StorageQuery::recordAvailable(const QVariantMap &record)
{
  Q_UNUSED( record );
}

/**
   @brief A new insert ID is available

   This method is called after queries that insert rows in a database. If supported, the
   @p id of the last record inserted is passed to this method.

   @note This method is only called, of a new ID has been generated.
 */
void StorageQuery::newIdAvailable(const QVariant &id)
{
  Q_UNUSED( id );
}

/**
   @brief Running the query has finished

   This method is called when running the query is done.
 */
void StorageQuery::endRun()
{

}

/**
   @brief Is the query finished?

   This method can be used to implement a multi-step query. The default implementation
   returns false, that means the sequence of method calls is:

   1. beginRun()
   2. query()
   3. recordAvailable() (for every record that might be returned)
   4. endRun()

   Sometimes it might be required to continue with another query after a
   first successful one. For this, you can re-implement this method and return true
   as long as you have another query to be made. The sequence of calls would then change
   to:

   1. beginRun()
   2. query()
   3. recordAvailable() (for every record that might be returned)
   4. endRun()
   5. if hasNext() goto 1

 */
bool StorageQuery::hasNext() const
{
  return false;
}



/**
   @brief The worker that is processing the query.
 */
DatabaseWorker *StorageQuery::worker() const
{
  return m_worker;
}

} /* DataBase */

} /* OpenTodoList */
