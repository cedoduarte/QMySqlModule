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

#ifndef QMYSQLEXPORTATIONDATA_H
#define QMYSQLEXPORTATIONDATA_H

#include "QMySqlModule_global.h"

#include <QString>

class QMYSQLMODULE_EXPORT QMySqlExportationData
{
public:
    QMySqlExportationData(const QString &outputFilepath = "",
                          bool includeUseDatabase = true,
                          bool includeCreateDatabase = true,
                          bool includeDropTable = true,
                          bool includeVersionInformation = true,
                          bool convertBlobToHex = true);
    QMySqlExportationData(const QMySqlExportationData &other);
    void operator=(const QMySqlExportationData &other);
    virtual ~QMySqlExportationData();
    void addData(QString &qry) const;
    void setIncludeUseDatabase(bool yes);
    void setIncludeCreateDatabase(bool yes);
    void setIncludeDropTable(bool yes);
    void setIncludeVersionInformation(bool yes);
    void setConvertBlobToHex(bool yes);
    void setOutputFilepath(const QString &filepath);
    bool useDatabaseIncluded() const;
    bool createDatabaseIncluded() const;
    bool dropTableIncluded() const;
    bool versionInformationIncluded() const;
    bool convertBlobToHexEnabled() const;
    QString outputFilepath() const;
private:
    bool m_includeUseDatabase;
    bool m_includeCreateDatabase;
    bool m_includeDropTable;
    bool m_includeVersionInformation;
    bool m_convertBlobToHex;
    QString m_outputFilepath;
};

#endif // QMYSQLEXPORTATIONDATA_H
