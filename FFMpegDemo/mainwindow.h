#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <audiothread.h>
#include "playthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
//    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    AudioThread  *_audioThread = nullptr;
    PlayThread *_playThread = nullptr;

};
#endif // MAINWINDOW_H
