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

#ifndef QMYSQLDATABASE_H
#define QMYSQLDATABASE_H

#include "qmysqlfield.h"
#include "QMySqlModule_global.h"
#include "qmysqlexportationdata.h"

#include <vector>
#include <mysql.h>
#include <QString>
#include <QDateTime>

class QMySqlBlobHelper;

class QMYSQLMODULE_EXPORT QMySqlDatabase
{
public:
    static void initGlobalInstance();
    static void releaseGlobalInstance();
    static bool connectGlobalInstance(const QString &host, unsigned port, const QString &username, const QString &password);
    static bool connectGlobalInstance(const QString &host, unsigned port, const QString &username, const QString &password,
                        const QString &database);
    static void closeGlobalInstance();
    static std::vector<QString> databases();
    static std::vector<QString> tables();
    static std::vector<QMySqlField> fields(const QString &table);
    static QString backupTable(const QString &table, const QMySqlExportationData &expData);
    static QString backupTables(const std::vector<QString> &tableList, const QMySqlExportationData &expData);
    static QMySqlDatabase *db();

    explicit QMySqlDatabase();
    virtual ~QMySqlDatabase();
    bool open();
    void close() const;
    void prepare(const QString &queryString);
    bool exec();
    bool next();
    void endNext();
    QString preparedQuery() const;
    int getInteger(int columnIndex, bool *ok = nullptr) const;
    double getDouble(int columnIndex, bool *ok = nullptr) const;
    bool getBoolean(int columnIndex, bool *ok = nullptr) const;
    QString getString(int columnIndex, bool *ok = nullptr) const;
    QDateTime getDateTime(int columnIndex, bool *ok = nullptr) const;
    void addInteger(int value);
    void addDouble(double value);
    void addBoolean(bool value);
    void addString(const QString &value);
    void addDateTime(const QDateTime &value);
    bool insertBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName) const;
    bool setBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName,
                 const QString &referenceColumnName, const QString &referenceFieldValue) const;
    bool getBlob(const QString &tableName, const QString &columnName, const QString &referenceColumnName,
                 const QString &referenceFieldValue, const QString &outputFilepath) const;
    QString apiVersion() const;
    void setHostAddress(const QString &hostAddress = "127.0.0.1");
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setDatabaseName(const QString &databaseName);
    void setPort(unsigned port = 3306);
    QString hostAddress() const;
    QString username() const;
    QString password() const;
    QString databaseName() const;
    unsigned port() const;
    MYSQL *driver() const;
private:
    void addBindValue(const QString &value);
    bool handleSelectStatement();
    bool connectToServer();
    bool wrongColumnIndex(int columnIndex) const;
    static void oktrue(bool *ok, bool value = true);
    static QString getValue(int index, const QString &fieldType);
    static void deleteGlobalInstance();

    MYSQL *m_driver;
    MYSQL_RES *m_result;
    MYSQL_ROW m_currentRow;
    QString m_preparedQuery;
    QString m_hostAddress;
    QString m_username;
    QString m_password;
    QString m_databaseName;
    unsigned m_port;
    int m_numFields;
    QMySqlBlobHelper *m_blobHelper;
    static QMySqlDatabase *s_globalInstance;
};

#endif // QMYSQLDATABASE_H
