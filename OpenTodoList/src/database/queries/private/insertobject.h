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

#ifndef OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H
#define OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H

#include "database/storagequery.h"

#include "datamodel/objectinfo.h"

#include <QString>
#include <QTextStream>
#include <QVariantMap>

namespace OpenTodoList {
namespace DataBase {
namespace Queries {
namespace Private {

using namespace OpenTodoList::DataModel;

/**
  @brief Inserts a complex object into the database

  This class provides a generic way to insert objects into the database. Complex objects
  are supposed to fulfill the following requirements:

  - They have properties which can be accessed via the Meta Object System. These properties,
    together with the properties for implementing the database sync protocol, are saved in a
    table which has the same name as the class that is saved without any namespace and the first
    character in lower case (called the base table).
  - They have meta attributes which are exposed via a metaAttributes() method. These are saved in
    tables starting with the base table name and having "MetaAttribute" and "MetaAttributeName"
    appended.
  - They belong to a parent object.

  This template class implements a basic protocol that will insert such objects
  into the database. It supports two modes of insertion: The first is insertion from backend
  (i.e. the dirty and disposed flags both will be reset after the operation) as well as an
  update operation where the object will be marked as modified by incrementing the
  dirty flag by one. The latter mode can be enabled by passing update=true in the constructor.


 */
template<typename T>
class InsertObject : public StorageQuery
{
public:
  explicit InsertObject( T* object,
                         const QStringList &attributes,
                         bool update );
  ~InsertObject();

  // StorageQuery interface
  bool query(QString &query, QVariantMap &args, int &options) override;
  void newIdAvailable(const QVariant &id) override;
  bool hasNext() const override;

  bool weightAtEnd() const;
  void setWeightAtEnd(bool weightAtEnd);

private:

  enum State {
    InsertObjectState,
    InsertObjectMetaNameState,
    InsertObjectMetaValueState,
    RemoveExtraMetaValuesState,
    FinishedState
  };

  State       m_state;
  T*          m_object;
  bool        m_waitingForId;
  QString     m_baseTable;
  QString     m_attributeNameTable;
  QString     m_attributeValueTable;
  QStringList m_attributes;
  bool        m_update;
  QString     m_parentAttribute;
  QString     m_parentIdAttribute;
  bool        m_weightAtEnd;

  void queryInsertObject(QTextStream &stream, QVariantMap &args );
  void queryInsertMetaName(QTextStream &stream, QVariantMap &args );
  void queryInsertMetaValue(QTextStream &stream, QVariantMap &args );
  void queryRemoveExtraMeta(QTextStream &stream, QVariantMap &args );

  void insertObjectInfo( QTextStream &stream, QVariantMap &args );

};

/**
  @brief Constructor

  Creates a new query to insert objects of type T into the database. The
  constructed query will insert the @p object into the database.
  The objects core @p attributes will be written into the @p baseTable.
  Furthermore, dynamic attributes (which can be get via the
  metaAttributes() method of the object) will be inserted into the
  @p attributeNameTable as well as @p attributeValueTable.

  If @p update is true, then the query will increase the dirty
  flag of the object and preserve the current state of the
  disposed flag. Otherwise, both flags will be reset.

  When the insertion is done, the @p finishedCallback will be invoked.

  The object's parent object is identified via the @p parentAttribute.
  It is supposed that there is a table in the database with the same name and
  which contains a value @p parentIdAttribute which is referred to by
  the objects parentAttribute property.
 */
template<typename T>
InsertObject<T>::InsertObject(T* object,
                              const QStringList &attributes,
                              bool update ) :
  StorageQuery(),
  m_state( InsertObjectState ),
  m_object( object ),
  m_waitingForId( false ),
  m_baseTable( ObjectInfo<T>::classNameLowerFirst() ),
  m_attributeNameTable( ObjectInfo<T>::classNameLowerFirst() + "MetaAttributeName" ),
  m_attributeValueTable( ObjectInfo<T>::classNameLowerFirst() + "MetaAttribute" ),
  m_attributes( attributes ),
  m_update( update ),
  m_parentAttribute( ObjectInfo< typename T::ContainerType >::classNameLowerFirst() ),
  m_parentIdAttribute( ObjectInfo< typename T::ContainerType >::classUuidProperty() ),
  m_weightAtEnd( false )
{
  Q_ASSERT( m_object != nullptr );
  Q_ASSERT( !m_baseTable.isEmpty() );
  Q_ASSERT( !m_attributeNameTable.isEmpty() );
  Q_ASSERT( !m_attributeValueTable.isEmpty() );
  Q_ASSERT( !m_parentAttribute.isEmpty() );
  Q_ASSERT( !m_parentIdAttribute.isEmpty() );
  Q_ASSERT( !m_attributes.isEmpty() );
}

template<typename T>
InsertObject<T>::~InsertObject()
{
}

template<typename T>
bool InsertObject<T>::query(QString &query, QVariantMap &args, int &options )
{
  Q_ASSERT( m_state <= RemoveExtraMetaValuesState );

  m_waitingForId = false;

  QTextStream stream( &query );

  switch ( m_state ) {

  case InsertObjectState:
  {
    options = QueryIsUpdateQuery;
    queryInsertObject( stream, args );
    return true;
  }

  case InsertObjectMetaNameState:
  {
    queryInsertMetaName( stream, args );
    return true;
  }

  case InsertObjectMetaValueState:
  {
    queryInsertMetaValue( stream, args );
    return true;
  }

  case RemoveExtraMetaValuesState:
  {
    queryRemoveExtraMeta( stream, args );
    return true;
  }

  case FinishedState: return false;

  }
  return false;
}

template<typename T>
void InsertObject<T>::newIdAvailable(const QVariant &id)
{
  if ( m_waitingForId && id.isValid() ) {
    m_object->setId( id.toInt() );
  }
}

template<typename T>
bool InsertObject<T>::hasNext() const
{
  return m_state != FinishedState;
}

/**
  @brief Put inserted objects at end of list by its weight

  If this flag is set to true, then the insertion will select a weight for the
  object such that the weight is greater than any other object in the list.
  This has only an effect, if a property called weight is part of the attribute
  list. The default is false.

  @sa setWeightAtEnd()
 */
template<typename T>
bool InsertObject<T>::weightAtEnd() const
{
  return m_weightAtEnd;
}

/**
  @brief Sets whether the object shall be weighted to be at the end of the list

  @sa weightAtEnd()
 */
template<typename T>
void InsertObject<T>::setWeightAtEnd(bool weightAtEnd)
{
  m_weightAtEnd = weightAtEnd;
}


template<typename T>
void InsertObject<T>::queryInsertObject(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  if ( !m_object->hasId() ) {
    m_waitingForId = true;
  }
  stream << ", protoInfo ( dirty, disposed ) AS ( ";
  if ( m_update ) {
    stream << "VALUES ( "
           << " COALESCE( ( SELECT dirty FROM " << m_baseTable << " WHERE ";
    if ( m_object->hasId() ) {
      stream << " id = :protoInfoSearchId1 ";
      args.insert( "protoInfoSearchId1", m_object->id() );
    } else {
      stream << " uuid = :protoInfoSearchUuid1 ";
      args.insert( "protoInfoSearchUuid1", m_object->uuid() );
    }
    stream << "), 0 ) + 1, COALESCE( ( SELECT disposed FROM " << m_baseTable << " WHERE ";
    if ( m_object->hasId() ) {
      stream << " id = :protoInfoSearchId2 ";
      args.insert( "protoInfoSearchId2", m_object->id() );
    } else {
      stream << " uuid = :protoInfoSearchUuid2 ";
      args.insert( "protoInfoSearchUuid2", m_object->uuid() );
    }
    stream << " ), 0 ) ) ";
  } else {
    stream << " VALUES ( 0, 0 ) ";
  }
  stream << " ) ";
  stream << "INSERT OR REPLACE INTO " << m_baseTable << " "
         << " ( id, dirty, disposed, " << m_parentAttribute << ", "
         << m_attributes.join( ", " ) << " ) "
         << "VALUES ( "
         << "(SELECT id FROM objectInfo), "
         << "(SELECT dirty FROM protoInfo), "
         << "(SELECT disposed FROM protoInfo), "
         << "(SELECT id FROM " << m_parentAttribute << " WHERE "
         << " " << m_parentIdAttribute  << " = :parentRef ) ";
  args.insert( "parentRef", m_object->property( m_parentAttribute.toUtf8().constData() ) );
  for ( const QString &attribute : m_attributes ) {
    if ( attribute == "weight" && m_weightAtEnd ) {
      stream << ", COALESCE( ( SELECT MAX(weight) FROM " << m_baseTable
             << " WHERE " << m_parentAttribute << " = "
             << "(SELECT id FROM " << m_parentAttribute << " WHERE "
             << " " << m_parentIdAttribute  << " = :parentRefWeight ) "
             << " ), 0 ) + 10 ";
      args.insert( "parentRefWeight", args.value( "parentRef" ) );
    } else {
      stream << ", :" << attribute;
    }
  }
  stream << " );";
  for ( const QString &attribute : m_attributes ) {
    if ( !m_weightAtEnd || ( attribute != "weight" ) ) {
      args.insert( attribute, m_object->property( attribute.toUtf8().constData() ) );
    }
  }
  m_state = m_object->metaAttributes().isEmpty() ?
        RemoveExtraMetaValuesState : InsertObjectMetaNameState;
}

template<typename T>
void InsertObject<T>::queryInsertMetaName(QTextStream &stream, QVariantMap &args)
{
  stream << "INSERT OR IGNORE INTO " << m_attributeNameTable << "( name ) VALUES ";
  QStringList metaAttributeNames = m_object->metaAttributes().keys();
  for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
    if ( i > 0 ) {
      stream << ", ";
    }
    QString placeholder = QString( "name%1" ).arg( i );
    stream << "( :" << placeholder << ")";
    args.insert( placeholder, metaAttributeNames.at( i ) );
  }
  stream << ";";
  m_state = InsertObjectMetaValueState;
}

template<typename T>
void InsertObject<T>::queryInsertMetaValue(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  stream << "INSERT OR REPLACE INTO " << m_attributeValueTable
         << "( " << m_baseTable << ", attributeName, value ) "
         << "VALUES ";
  QVariantMap metaAttributes = m_object->metaAttributes();
  QStringList metaAttributeNames = metaAttributes.keys();
  for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
    QString attrName = QString( "name%1" ).arg( i );
    QString attrValue = QString( "value%1" ).arg( i );
    const QString &attribute = metaAttributeNames.at( i );
    if ( i > 0 ) {
      stream << ", ";
    }
    stream << "("
           << " ( SELECT id FROM objectInfo ), "
           << " ( SELECT id FROM " << m_attributeNameTable << " WHERE name = :" << attrName << " ), "
           << " :" << attrValue
           << ")";
    args.insert( attrName, attribute );
    args.insert( attrValue, metaAttributes.value( attribute ) );
  }
  stream << ";";
  m_state = RemoveExtraMetaValuesState;
}

template<typename T>
void InsertObject<T>::queryRemoveExtraMeta(QTextStream &stream, QVariantMap &args)
{
  insertObjectInfo( stream, args );
  QStringList metaAttributeNames = m_object->metaAttributes().keys();
  if ( metaAttributeNames.isEmpty() ) {
    stream << "DELETE FROM " << m_attributeValueTable
           << " WHERE " << m_baseTable << " = "
           << " ( SELECT id FROM  objectInfo );";
  } else {
    stream << "DELETE FROM " << m_attributeValueTable
           << " WHERE " << m_baseTable << " = ( SELECT id FROM objectInfo ) AND "
           << " attributeName NOT IN "
           << " ( SELECT id FROM " << m_attributeNameTable << " WHERE name IN (";
    for ( int i = 0; i < metaAttributeNames.size(); ++i ) {
      QString name = QString( "name%1" ).arg( i );
      if ( i > 0 ) {
        stream << ", ";
      }
      stream << ":" << name;
      args.insert( name, metaAttributeNames.at( i ) );
    }
    stream << ") );";
  }
  m_state = FinishedState;
}

template<typename T>
void InsertObject<T>::insertObjectInfo(QTextStream &stream, QVariantMap &args)
{
  if ( m_object->hasId() ) {
    stream << "WITH objectInfo ( id ) AS ( VALUES ( :insertObjectInfoObjectId ) ) ";
    args.insert( "insertObjectInfoObjectId", m_object->id() );
  } else {
    stream << "WITH objectInfo ( id ) AS ( SELECT id FROM " << m_baseTable
           << " WHERE uuid = :insertObjectInfoObjectUuid ) ";
    args.insert( "insertObjectInfoObjectUuid", m_object->uuid() );
  }
}


} // namespace Private
} // namespace Queries
} // namespace DataBase
} // namespace OpenTodoList

#endif // OPENTODOLIST_DATABASE_QUERIES_PRIVATE_INSERTOBJECT_H
