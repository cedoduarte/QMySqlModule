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

#include <cstdio>
#include <QDebug>
#include <mysql.h>

QMySqlBlobHelper::QMySqlBlobHelper(QMySqlDatabase *parent)
{
    m_parent = parent;
    m_data = nullptr;
    m_chunk = nullptr;
    m_query = nullptr;
    m_size = 0;
}

QMySqlBlobHelper::~QMySqlBlobHelper()
{
    // nothing to do here
}

bool QMySqlBlobHelper::insertBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName)
{
    m_tableName = tableName;
    m_columnName = columnName;
    m_binaryFilepath = binaryFilepath;
    return readBinaryFileAndExecuteStatement(QString("insert into %1(%2) values('%s')")
                                             .arg(m_tableName)
                                             .arg(m_columnName));
}

bool QMySqlBlobHelper::setBlob(const QString &binaryFilepath, const QString &tableName, const QString &columnName,
                               const QString &referenceColumnName, const QString &referenceFieldValue)
{
    m_tableName = tableName;
    m_columnName = columnName;
    m_binaryFilepath = binaryFilepath;
    return readBinaryFileAndExecuteStatement(QString("update %1 set %2 = '%s' where %3 like %4")
                                             .arg(m_tableName)
                                             .arg(m_columnName)
                                             .arg(referenceColumnName)
                                             .arg(referenceFieldValue));
}

bool QMySqlBlobHelper::getBlob(const QString &tableName, const QString &columnName, const QString &referenceColumnName,
                               const QString &referenceFieldValue, const QString &outputFilepath)
{
    m_tableName = tableName;
    m_columnName = columnName;
    FILE *fp = fopen(outputFilepath.toStdString().c_str(), "wb");
    if (fp == nullptr)
    {
        qDebug() << "cannot open binary file";
    }
    else
    {
        QString queryString = QString("select %1 from %2 where %3 like %4").arg(m_columnName).arg(m_tableName)
                .arg(referenceColumnName).arg(referenceFieldValue);
        if (mysql_query(m_parent->driver(), queryString.toStdString().c_str()) == 0)
        {
            MYSQL_RES *result = mysql_store_result(m_parent->driver());
            if (result)
            {
                MYSQL_ROW row = mysql_fetch_row(result);
                unsigned long *lengths = mysql_fetch_lengths(result);
                if (lengths)
                {
                    fwrite(row[0], lengths[0], 1, fp);
                    if (ferror(fp))
                    {
                        qDebug() << "fwrite() failed";
                    }
                    else
                    {
                        if (fclose(fp) == -1)
                        {
                            qDebug() << "cannot close file handler";
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool QMySqlBlobHelper::executeQuery(const QString &queryString)
{
    bool ok;
    m_chunk = (char*)calloc(2*m_size+1, sizeof(char));
    mysql_real_escape_string(m_parent->driver(), m_chunk, m_data, m_size);
    const char *queryStr = queryString.toStdString().c_str();
    const unsigned st_len = strlen(queryStr);
    m_query = (char*)calloc(st_len + 2*m_size+1, sizeof(char));
    const int len = snprintf(m_query, st_len + 2*m_size+1, queryStr, m_chunk);
    if (mysql_real_query(m_parent->driver(), m_query, len) == 0)
    {
        qDebug() << "blob updated/inserted...";
        ok = true;
    }
    else
    {
        QMySqlUtil::qdebug(m_parent->driver(), __FILE__, __LINE__);
        ok = false;
    }
    free(m_data);
    free(m_chunk);
    free(m_query);
    m_data = nullptr;
    m_chunk = nullptr;
    m_query = nullptr;
    return ok;
}

bool QMySqlBlobHelper::readBinaryFileAndExecuteStatement(const QString &queryString)
{
    FILE *fp = fopen(m_binaryFilepath.toStdString().c_str(), "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        if (ferror(fp))
        {
            qDebug() << "fseek() failed";
            if (fclose(fp) == -1)
            {
                qDebug() << "cannot close file handler";
            }
        }
        else
        {
            const int flen = ftell(fp);
            if (flen == -1)
            {
                perror("error occurred");
                if (fclose(fp) == -1)
                {
                    qDebug() << "cannot close file handler";
                }
            }
            else
            {
                fseek(fp, 0, SEEK_SET);
                if (ferror(fp))
                {
                    qDebug() << "fseek() failed";
                    if (fclose(fp) == -1)
                    {
                        qDebug() << "cannot close file handler";
                    }
                }
                else
                {
                    m_data = (char*)calloc(flen+1, sizeof(char));
                    m_size = fread(m_data, 1, flen, fp);
                    if (ferror(fp))
                    {
                        qDebug() << "fread() failed";
                        if (fclose(fp) == -1)
                        {
                            qDebug() << "cannot close file handler";
                        }
                        free(m_data);
                        m_data = nullptr;
                    }
                    else
                    {
                        if (fclose(fp) == -1)
                        {
                            qDebug() << "cannot close file handler";
                            free(m_data);
                            m_data = nullptr;
                        }
                        else
                        {
                            return executeQuery(queryString);
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "cannot open binary file";
    }
    return false;
}
