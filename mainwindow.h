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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFileBrowser();
    void changePath(QString);
    void currentSelectionChanged();
    void backupSelectionChanged();
    void backupSavegame();
    void restoreSavegame();
    void deleteSavegame();

    void aboutQt();
    void aboutFSM();

private:
    Ui::MainWindow *ui;
    QFileSystemModel mCurrentModel;
    QFileSystemModel mBackupModel;

    static const QString SAVEGAME_FILENAME;
};

#endif // MAINWINDOW_H
