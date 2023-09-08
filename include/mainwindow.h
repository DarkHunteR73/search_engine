#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "threadPool.h"
#include "invertedIndex.h"
#include "searchServer.h"
#include "requesteditor.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void addItems();
    void removeSelectedItems();
    void search();
    void setMaxResponses(int);
    void openRequestEditor();

private:
    Ui::MainWindow *ui;

    threadPool *tp;
    invertedIndex *idx;
    searchServer *engine;
    requestEditor *editor = nullptr;
};
#endif // MAINWINDOW_H
