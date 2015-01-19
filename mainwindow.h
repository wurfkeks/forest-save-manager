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

private:
    Ui::MainWindow *ui;
    QFileSystemModel mCurrentModel;
    QFileSystemModel mBackupModel;

    static const QString SAVEGAME_FILENAME;
};

#endif // MAINWINDOW_H
