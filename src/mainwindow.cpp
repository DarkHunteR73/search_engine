#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

extern json configCache;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    tp = new threadPool(std::thread::hardware_concurrency() - 1);
    idx = new invertedIndex;
}

MainWindow::~MainWindow() {
    delete ui;
    delete idx;
    delete tp;
}

#ifdef WIN32
#define HOMEDIR "USERPROFILE"
#else
#define HOMEDIR "HOME"
#endif //WIN32

void MainWindow::addItems()
{
    ui->listWidget->addItems(QFileDialog::getOpenFileNames(
          nullptr
        , "Open text file"
        , std::getenv(HOMEDIR)
        , "Text files (*.txt)"
        )
    );
}

void MainWindow::removeSelectedItems()
{
    auto items = ui->listWidget->selectedItems();
    for (auto &i : items)
    {
        delete ui->listWidget->takeItem(ui->listWidget->row(i));
    }
}

void MainWindow::search()
{

}
