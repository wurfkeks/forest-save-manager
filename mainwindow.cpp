/*
 * (C) Copyright 2015 Stanley Förster
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     Stanley F. <stanley.foerster@gmail.com>
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcessEnvironment>
#include <QInputDialog>
#include <QMessageBox>

const QString MainWindow::SAVEGAME_FILENAME = "PlayerPrefsFile___RESUME__.dat";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnbox_current->setEnabled(false);
    ui->btnbox_backups->setEnabled(false);
    ui->btnbox_backups->button(QDialogButtonBox::Ok)->setText("Restore");
    ui->btnbox_backups->button(QDialogButtonBox::Cancel)->setText("Delete");
    ui->table_current->setModel(&mCurrentModel);
    ui->table_current->hideColumn(1);
    ui->table_current->hideColumn(2);
    ui->table_backups->setModel(&mBackupModel);
    ui->table_backups->hideColumn(1);
    ui->table_backups->hideColumn(2);

    connect(ui->btn_browse, &QPushButton::clicked, this, &MainWindow::openFileBrowser);
    connect(ui->edit_path, &QLineEdit::textChanged, this, &MainWindow::changePath);
    connect(ui->table_current->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::currentSelectionChanged);
    connect(ui->table_backups->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::backupSelectionChanged);
    connect(ui->btnbox_current, &QDialogButtonBox::accepted, this, &MainWindow::backupSavegame);
    connect(ui->btnbox_backups, &QDialogButtonBox::accepted, this, &MainWindow::restoreSavegame);
    connect(ui->btnbox_backups, &QDialogButtonBox::rejected, this, &MainWindow::deleteSavegame);

    mCurrentModel.setFilter(QDir::Files);
    mBackupModel.setFilter(QDir::Files);
    QString path = QProcessEnvironment::systemEnvironment().value("USERPROFILE");
    if (!path.isEmpty()) {
        path += "/AppData/LocalLow/SKS/TheForest";
    }
    ui->edit_path->setText(path);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileBrowser() {
    QString path = QFileDialog::getExistingDirectory(this);
    if (!path.isEmpty() && ui->edit_path->text() != path) {
        ui->edit_path->setText(path);
    }
}

void MainWindow::changePath(QString path) {
    QFile mainFolder(path);
    if (mainFolder.exists()) {
        mCurrentModel.setRootPath(path);
        QModelIndex curRootIndex = mCurrentModel.index(path);
        ui->table_current->setRootIndex(curRootIndex);

        QFile backupFolder(path+"/backup");
        if (!backupFolder.exists()) {
            QModelIndex rootIndex = mCurrentModel.index(path);
            mCurrentModel.mkdir(rootIndex, "backup");
        }

        QString bakPath = path + "/backup";
        mBackupModel.setRootPath(bakPath);
        QModelIndex bakRootIndex = mBackupModel.index(bakPath);
        ui->table_backups->setRootIndex(bakRootIndex);
    }
}

void MainWindow::currentSelectionChanged() {
    QModelIndex index = ui->table_current->currentIndex();
    if (index.isValid()) {
        ui->btnbox_current->setEnabled(true);
    } else {
        ui->btnbox_current->setEnabled(false);
    }
}

void MainWindow::backupSelectionChanged() {
    QModelIndex index = ui->table_backups->currentIndex();
    if (index.isValid()) {
        ui->btnbox_backups->setEnabled(true);
    } else {
        ui->btnbox_backups->setEnabled(false);
    }
}

void MainWindow::backupSavegame() {
    QModelIndex index = ui->table_current->currentIndex();
    if (index.isValid()) {
        QString newFileName;
        bool ok = true;

        do {
            newFileName = QInputDialog::getText(this, "Backup Savegame", "Backup name", QLineEdit::Normal, newFileName, &ok);
        } while (ok &&
                 QFile(mBackupModel.rootPath()+"/"+newFileName).exists() &&
                 (QMessageBox::Retry == QMessageBox::warning(this, "Backup Savegame", "A Backup with this name already exists. Do you want to override it?", QMessageBox::Ok, QMessageBox::Retry)));

        if (!ok) {
            return;
        }

        QString fileName = mCurrentModel.data(index, QFileSystemModel::FileNameRole).toString();
        QString sourcePath = mCurrentModel.rootPath()+"/"+fileName;
        QString destPath = mBackupModel.rootPath()+"/"+newFileName;

        QFile::copy(sourcePath, destPath);
    }
}

void MainWindow::restoreSavegame() {
    QModelIndex index = ui->table_backups->currentIndex();
    if (index.isValid()) {
        QString backupFileName = mBackupModel.data(index, QFileSystemModel::FileNameRole).toString();
        QString sourcePath = mBackupModel.rootPath()+"/"+backupFileName;
        QString destPath = mCurrentModel.rootPath()+"/"+SAVEGAME_FILENAME;
        QFile saveFile(destPath);
        if (!saveFile.exists() || (QMessageBox::Ignore == QMessageBox::warning(this, "Restore Savegame", "You are about to override the curent savegame. This cannot be undone. You may want to do a backup first!", QMessageBox::Ignore, QMessageBox::Abort))) {
            saveFile.remove();
            QFile::copy(sourcePath, destPath);
        }
    }
}

void MainWindow::deleteSavegame() {
    QModelIndex index = ui->table_backups->currentIndex();
    if (index.isValid()) {
        QString fileName = mBackupModel.data(index, QFileSystemModel::FileNameRole).toString();
        if (QMessageBox::Ok == QMessageBox::warning(this, "Delete Savegame", "Do you really want to delete the savegame "+fileName+"?\nThis cannot be undone!", QMessageBox::Ok, QMessageBox::Cancel)) {
            QString sourcePath = mBackupModel.rootPath()+"/"+fileName;
            QFile(sourcePath).remove();
        }
    }
}

void MainWindow::aboutFSM() {
    QMessageBox::about(this, "About ForestSaveManager",
                       "Forest Save Manager\n"
                       "Version: 1.0\n"
                       "Creator: Stanley F. <stanley.foerster@gmail.com>\n"
                       "Code: https://github.com/wurfkeks/forest-save-manager\n"
                       "\u00A9 2015");
}

void MainWindow::aboutQt() {
    QMessageBox::aboutQt(this, "About Qt");
}
