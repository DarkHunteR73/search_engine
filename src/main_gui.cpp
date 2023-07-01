#include "threadPool.h"
#include "invertedIndex.h"
#include "searchServer.h"
#include "jsonConverter.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    //
    
    //
    window.show();
    return app.exec();
}