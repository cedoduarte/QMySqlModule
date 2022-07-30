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

#include "qmysqldatabase.h"
#include "qmysqlexportationdata.h"

#include <QDateTime>

QMySqlExportationData::QMySqlExportationData(const QString &outputFilepath,
                                             bool includeUseDatabase,
                                             bool includeCreateDatabase,
                                             bool includeDropTable,
                                             bool includeVersionInformation,
                                             bool convertBlobToHex)
{
    m_includeUseDatabase = includeUseDatabase;
    m_includeCreateDatabase = includeCreateDatabase;
    m_includeDropTable = includeDropTable;
    m_includeVersionInformation = includeVersionInformation;
    m_convertBlobToHex = convertBlobToHex;
    m_outputFilepath = outputFilepath;
}

QMySqlExportationData::QMySqlExportationData(const QMySqlExportationData &other)
    : QMySqlExportationData(other.m_outputFilepath,
                            other.m_includeUseDatabase,
                            other.m_includeCreateDatabase,
                            other.m_includeDropTable,
                            other.m_includeVersionInformation,
                            other.m_convertBlobToHex)
{
    // nothing to do here
}

void QMySqlExportationData::operator=(const QMySqlExportationData &other)
{
    *this = other;
}

QMySqlExportationData::~QMySqlExportationData()
{
    // nothing to do here
}

void QMySqlExportationData::addData(QString &qry) const
{
    if (m_includeVersionInformation)
    {
        qry += QString("# MySQL Server %1\n").arg(QMySqlDatabase::db()->apiVersion());
        qry += QString("# %1\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        qry += "\n";
    }
    if (m_includeCreateDatabase)
    {
        qry += QString("drop database if exists %1;\n").arg(QMySqlDatabase::db()->databaseName());
        qry += QString("create database if not exists %1;\n").arg(QMySqlDatabase::db()->databaseName());
        qry += "\n";
    }
    if (m_includeUseDatabase)
    {
        qry += QString("use %1;\n").arg(QMySqlDatabase::db()->databaseName());
        qry += "\n";
    }
}

void QMySqlExportationData::setIncludeUseDatabase(bool yes)
{
    m_includeUseDatabase = yes;
}

void QMySqlExportationData::setIncludeCreateDatabase(bool yes)
{
    m_includeCreateDatabase = yes;
}

void QMySqlExportationData::setIncludeDropTable(bool yes)
{
    m_includeDropTable = yes;
}

void QMySqlExportationData::setIncludeVersionInformation(bool yes)
{
    m_includeVersionInformation = yes;
}

void QMySqlExportationData::setConvertBlobToHex(bool yes)
{
    m_convertBlobToHex = yes;
}

void QMySqlExportationData::setOutputFilepath(const QString &filepath)
{
    m_outputFilepath = filepath;
}

bool QMySqlExportationData::useDatabaseIncluded() const
{
    return m_includeUseDatabase;
}

bool QMySqlExportationData::createDatabaseIncluded() const
{
    return m_includeCreateDatabase;
}

bool QMySqlExportationData::dropTableIncluded() const
{
    return m_includeDropTable;
}

bool QMySqlExportationData::versionInformationIncluded() const
{
    return m_includeVersionInformation;
}

bool QMySqlExportationData::convertBlobToHexEnabled() const
{
    return m_convertBlobToHex;
}

QString QMySqlExportationData::outputFilepath() const
{
    return m_outputFilepath;
}
