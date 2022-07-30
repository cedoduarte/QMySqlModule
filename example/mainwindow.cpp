/****************************************************************************
**
** Copyright (C) 2022 Carlos Enrique Duarte Ortiz (carlosduarte.1@hotmail.com)
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "database.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Database::open();
    Database::createTable();
    Database::insertValues();

    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels({
        "id",
        "integervalue",
        "doublevalue",
        "booleanvalue",
        "stringvalue",
        "datetimevalue",
        "blobvalueFilepath"
    });

    std::vector<MyTable> records = Database::selectAll();
    for (int i = 0; i < int(records.size()); ++i)
    {
        const int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(records.at(i).id)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(records.at(i).integervalue)));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(records.at(i).doublevalue)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(records.at(i).booleanvalue ? "true" : "false"));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(records.at(i).stringvalue));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(records.at(i).datetimevalue.toString("yyyy-MM-dd hh:mm:ss")));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(records.at(i).blobvalueFilePath));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
