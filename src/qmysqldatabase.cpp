/*
   Copyright (c) 2000, 2021, Oracle and/or its affiliates.
   Copyright (c) 2022, Carlos Enrique Duarte Ortiz (carlosduarte.1@hotmail.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   Without limiting anything contained in the foregoing, this file,
   which is part of C Driver for MySQL (Connector/C), is also subject to the
   Universal FOSS Exception, version 1.0, a copy of which can be found at
   http://oss.oracle.com/licenses/universal-foss-exception.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "qmysqlutil.h"
#include "qmysqldatabase.h"
#include "qmysqlblobhelper.h"

QMySqlDatabase *QMySqlDatabase::s_globalInstance = nullptr;

void QMySqlDatabase::initGlobalInstance()
{
    s_globalInstance = new QMySqlDatabase;
}

void QMySqlDatabase::releaseGlobalInstance()
{
    closeGlobalInstance();
    deleteGlobalInstance();
}

bool QMySqlDatabase::connectGlobalInstance(const QString &host, unsigned port, const QString &username, const QString &password)
{
    closeGlobalInstance();
    s_globalInstance->setHostAddress(host);
    s_globalInstance->setPort(port);
    s_globalInstance->setUsername(username);
    s_globalInstance->setPassword(password);
    return s_globalInstance->open();
}

bool QMySqlDatabase::connectGlobalInstance(const QString &host, unsigned port, const QString &username, const QString &password,
                                           const QString &database)
{
    closeGlobalInstance();
    s_globalInstance->setHostAddress(host);
    s_globalInstance->setPort(port);
    s_globalInstance->setUsername(username);
    s_globalInstance->setPassword(password);
    s_globalInstance->setDatabaseName(database);
    return s_globalInstance->open();
}

void QMySqlDatabase::closeGlobalInstance()
{
    s_globalInstance->close();
}

std::vector<QString> QMySqlDatabase::databases()
{
    std::vector<QString> dbnames;
    MYSQL_RES *res = mysql_list_dbs(s_globalInstance->driver(), nullptr);
    MYSQL_ROW row = mysql_fetch_row(res);
    while (row)
    {
        dbnames.push_back(QString(row[0]));
        row = mysql_fetch_row(res);
    }
    mysql_free_result(res);
    return dbnames;
}

std::vector<QString> QMySqlDatabase::tables()
{
    std::vector<QString> tbnames;
    MYSQL_RES *res = mysql_list_tables(s_globalInstance->driver(), nullptr);
    MYSQL_ROW row = mysql_fetch_row(res);
    while (row)
    {
        tbnames.push_back(QString(row[0]));
        row = mysql_fetch_row(res);
    }
    mysql_free_result(res);
    return tbnames;
}

std::vector<QMySqlField> QMySqlDatabase::fields(const QString &table)
{
    std::vector<QMySqlField> fieldList;
    QString qry = QString("show columns from %1").arg(table);
    mysql_real_query(s_globalInstance->driver(), qry.toStdString().c_str(), qry.size());
    MYSQL_RES *res = mysql_store_result(s_globalInstance->driver());
    MYSQL_ROW row = mysql_fetch_row(res);
    while (row)
    {
        QMySqlField field;
        field.setName(row[0]);
        field.setType(row[1]);
        fieldList.push_back(field);
        row = mysql_fetch_row(res);
    }
    mysql_free_result(res);
    return fieldList;
}

QString QMySqlDatabase::backupTable(const QString &table, const QMySqlExportationData &expData)
{
    QString qry;
    if (expData.dropTableIncluded())
    {
        qry += QString("drop table if exists %1;\n").arg(table);
    }
    qry += QString("create table if not exists %1(").arg(table);
    std::vector<QMySqlField> fieldList = fields(table);
    qry += QString("%1 %2").arg(fieldList.at(0).name()).arg(fieldList.at(0).type());
    for (int i = 1; i < int(fieldList.size()); ++i)
    {
        qry += QString(", %1 %2").arg(fieldList.at(i).name()).arg(fieldList.at(i).type());
    }
    qry += ");";
    s_globalInstance->prepare(QString("select * from %1").arg(table));
    s_globalInstance->exec();
    while (s_globalInstance->next())
    {
        // columns
        QMySqlField field = fieldList.at(0);
        qry += "\ninsert into " + table + "(";
        qry += field.name();
        for (int i = 1; i < int(fieldList.size()); ++i)
        {
            field = fieldList.at(i);
            qry += ", " + field.name();
        }
        qry += ") values(";

        // values
        field = fieldList.at(0);
        qry += getValue(0, field.type());
        for (int i = 1; i < int(fieldList.size()); ++i)
        {
            field = fieldList.at(i);
            qry += ", " + getValue(i, field.type());
        }
        qry += ");";
    }
    s_globalInstance->endNext();
    return qry;
}

QString QMySqlDatabase::backupTables(const std::vector<QString> &tableList, const QMySqlExportationData &expData)
{
    QString qry;
    expData.addData(qry);
    qry += backupTable(tableList.at(0), expData);
    for (int i = 1; i < int(tableList.size()); ++i)
    {
        qry += "\n\n" + backupTable(tableList.at(i), expData);
    }
    return qry;
}

QMySqlDatabase *QMySqlDatabase::db()
{
    return s_globalInstance;
}

QMySqlDatabase::QMySqlDatabase()
{
    m_driver = nullptr;
    m_result = nullptr;
    m_currentRow = nullptr;
    m_port = 3306;
    m_numFields = 0;
    m_hostAddress = "127.0.0.1";
    m_username = "root";
    m_blobHelper = new QMySqlBlobHelper(this);
}

QMySqlDatabase::~QMySqlDatabase()
{
    delete m_blobHelper;
}

bool QMySqlDatabase::open()
{
    m_driver = mysql_init(nullptr);
    return connectToServer();
}

void QMySqlDatabase::close() const
{
    mysql_close(m_driver);
}

void QMySqlDatabase::prepare(const QString &queryString)
{
    m_preparedQuery = queryString;
}

bool QMySqlDatabase::exec()
{
    if (mysql_real_query(m_driver, m_preparedQuery.toStdString().c_str(), m_preparedQuery.size()) == 0)
    {
        return handleSelectStatement();
    }
    else
    {
        return false;
    }
}

bool QMySqlDatabase::next()
{
    if (m_result)
    {
        m_currentRow = mysql_fetch_row(m_result);
        return (m_currentRow != nullptr);
    }
    else
    {
        return false;
    }
}

void QMySqlDatabase::endNext()
{
    mysql_free_result(m_result);
    m_result = nullptr;
}

QString QMySqlDatabase::preparedQuery() const
{
    return m_preparedQuery;
}

int QMySqlDatabase::getInteger(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return 0;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toInt();
        }
        else
        {
            oktrue(ok, false);
            return 0;
        }
    }
}

double QMySqlDatabase::getDouble(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return 0.0;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toDouble();
        }
        else
        {
            oktrue(ok, false);
            return 0.0;
        }
    }
}

bool QMySqlDatabase::getBoolean(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return false;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toInt() == 0 ? false : true;
        }
        else
        {
            oktrue(ok, false);
            return false;
        }
    }
}

QString QMySqlDatabase::getString(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return "";
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]);
        }
        else
        {
            oktrue(ok, false);
            return "";
        }
    }
}

QDateTime QMySqlDatabase::getDateTime(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return QDateTime();
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QDateTime::fromString(m_currentRow[columnIndex], "yyyy-MM-dd hh:mm:ss");
        }
        else
        {
            oktrue(ok, false);
            return QDateTime();
        }
    }
}

void QMySqlDatabase::addInteger(int value)
{
    addBindValue(QString::number(value));
}

void QMySqlDatabase::addDouble(double value)
{
    addBindValue(QString::number(value));
}

void QMySqlDatabase::addBoolean(bool value)
{
    addBindValue(value ? "1" : "0");
}

void QMySqlDatabase::addString(const QString &value)
{
    addBindValue(QString("'%1'").arg(value));
}

void QMySqlDatabase::addDateTime(const QDateTime &value)
{
    addBindValue(QString("'%1'").arg(value.toString("yyyy-MM-dd hh:mm:ss")));
}

bool QMySqlDatabase::insertBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName) const
{
    return m_blobHelper->insertBlob(binaryFilepath, tableName, columnName);
}

bool QMySqlDatabase::setBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName,
                             const QString &referenceColumnName, const QString &referenceFieldValue) const
{
    return m_blobHelper->setBlob(binaryFilepath, tableName, columnName, referenceColumnName, referenceFieldValue);
}

bool QMySqlDatabase::getBlob(const QString &tableName, const QString &columnName, const QString &referenceColumnName,
                             const QString &referenceFieldValue, const QString &outputFilepath) const
{
    return m_blobHelper->getBlob(tableName, columnName, referenceColumnName, referenceFieldValue, outputFilepath);
}

QString QMySqlDatabase::apiVersion() const
{
    return mysql_get_client_info();
}

void QMySqlDatabase::setHostAddress(const QString &hostAddress)
{
    m_hostAddress = hostAddress;
}

void QMySqlDatabase::setUsername(const QString &username)
{
    m_username = username;
}

void QMySqlDatabase::setPassword(const QString &password)
{
    m_password = password;
}

void QMySqlDatabase::setDatabaseName(const QString &databaseName)
{
    m_databaseName = databaseName;
}

void QMySqlDatabase::setPort(unsigned port)
{
    m_port = port;
}

QString QMySqlDatabase::hostAddress() const
{
    return m_hostAddress;
}

QString QMySqlDatabase::username() const
{
    return m_username;
}

QString QMySqlDatabase::password() const
{
    return m_password;
}

QString QMySqlDatabase::databaseName() const
{
    return m_databaseName;
}

unsigned QMySqlDatabase::port() const
{
    return m_port;
}

MYSQL *QMySqlDatabase::driver() const
{
    return m_driver;
}

void QMySqlDatabase::addBindValue(const QString &value)
{
    const int index = m_preparedQuery.indexOf("?");
    m_preparedQuery = m_preparedQuery.remove(index, 1);
    m_preparedQuery = m_preparedQuery.insert(index, value);
}

bool QMySqlDatabase::handleSelectStatement()
{
    if (m_preparedQuery.startsWith("select", Qt::CaseInsensitive))
    {
        m_result = mysql_store_result(m_driver);
        if (m_result)
        {
            m_numFields = mysql_num_fields(m_result);
        }
        else
        {
            m_numFields = 0;
        }
        return true;
    }
    else
    {
        m_numFields = 0;
        return true;
    }
}

bool QMySqlDatabase::connectToServer()
{
    if (m_driver)
    {
        if (mysql_real_connect(m_driver,
                               m_hostAddress.toStdString().c_str(),
                               m_username.toStdString().c_str(),
                               m_password.toStdString().c_str(),
                               m_databaseName.toStdString().c_str(),
                               m_port,
                               nullptr,
                               0))
        {
            return true;
        }
        else
        {
            QMySqlUtil::qdebug(m_driver, __FILE__, __LINE__);
            close();
            return false;
        }
    }
    else
    {
        QMySqlUtil::qdebug(m_driver, __FILE__, __LINE__);
        return false;
    }
}

bool QMySqlDatabase::wrongColumnIndex(int columnIndex) const
{
    return (columnIndex < 0 || columnIndex >= m_numFields);
}

void QMySqlDatabase::oktrue(bool *ok, bool value)
{
    if (ok)
    {
        *ok = value;
    }
}

void QMySqlDatabase::deleteGlobalInstance()
{
    delete s_globalInstance;
    s_globalInstance = nullptr;
}

QString QMySqlDatabase::getValue(int index, const QString &fieldType)
{
    if (fieldType.startsWith("char", Qt::CaseInsensitive)
            || fieldType.startsWith("varchar", Qt::CaseInsensitive)
            || fieldType.startsWith("binary", Qt::CaseInsensitive)
            || fieldType.startsWith("varbinary", Qt::CaseInsensitive)
            || fieldType.startsWith("tinyblob", Qt::CaseInsensitive)
            || fieldType.startsWith("tinytext", Qt::CaseInsensitive)
            || fieldType.startsWith("text", Qt::CaseInsensitive)
            || fieldType.startsWith("blob", Qt::CaseInsensitive)
            || fieldType.startsWith("mediumtext", Qt::CaseInsensitive)
            || fieldType.startsWith("mediumblob", Qt::CaseInsensitive)
            || fieldType.startsWith("longtext", Qt::CaseInsensitive)
            || fieldType.startsWith("longblob", Qt::CaseInsensitive)
            || fieldType.startsWith("enum", Qt::CaseInsensitive)
            || fieldType.startsWith("set", Qt::CaseInsensitive))
    {
        return QString("'%1'").arg(s_globalInstance->getString(index));
    }
    else if (fieldType.startsWith("bool", Qt::CaseInsensitive)
             || fieldType.startsWith("boolean", Qt::CaseInsensitive))
    {
        return QString::number(s_globalInstance->getBoolean(index));
    }
    else if (fieldType.startsWith("bit", Qt::CaseInsensitive)
             || fieldType.startsWith("tinyint", Qt::CaseInsensitive)
             || fieldType.startsWith("smallint", Qt::CaseInsensitive)
             || fieldType.startsWith("mediumint", Qt::CaseInsensitive)
             || fieldType.startsWith("int", Qt::CaseInsensitive)
             || fieldType.startsWith("integer", Qt::CaseInsensitive)
             || fieldType.startsWith("bigint", Qt::CaseInsensitive))
    {
        return QString::number(s_globalInstance->getInteger(index));
    }
    else if (fieldType.startsWith("float", Qt::CaseInsensitive)
             || fieldType.startsWith("double", Qt::CaseInsensitive)
             || fieldType.startsWith("decimal", Qt::CaseInsensitive)
             || fieldType.startsWith("dec", Qt::CaseInsensitive))
    {
        return QString::number(s_globalInstance->getDouble(index), 'f', 6);
    }
    else if (fieldType.startsWith("date", Qt::CaseInsensitive)
             || fieldType.startsWith("datetime", Qt::CaseInsensitive)
             || fieldType.startsWith("timestamp", Qt::CaseInsensitive)
             || fieldType.startsWith("time", Qt::CaseInsensitive)
             || fieldType.startsWith("year", Qt::CaseInsensitive))
    {
        return QString("'%1'").arg(s_globalInstance->getString(index));
    }
    else
    {
        return "";
    }
}
