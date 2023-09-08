#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "jsonConverter.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <vector>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    try {
        jsonConverter::initCache();
    }
    catch(const std::exception& e) {
        QMessageBox msg(
              QMessageBox::Critical
            , "Error"
            , e.what()
            , QMessageBox::Ok
            , this
        );
        msg.exec();
        std::exit(EXIT_FAILURE);
    }

    this->ui->maxResponsesBox->setValue(jsonConverter::getResponsesLimit());
    this->setWindowTitle(jsonConverter::getTitle().c_str());

    tp = new threadPool(std::thread::hardware_concurrency() - 1);
    idx = new invertedIndex;
    engine = new searchServer(*idx);
}

MainWindow::~MainWindow() {
    delete ui;
    delete engine;
    delete idx;
    delete tp;
}

#ifdef WIN32
constexpr auto HOMEDIR = "USERPROFILE";
#else
constexpr auto HOMEDIR = "HOME";
#endif //WIN32

void MainWindow::addItems() {
    ui->listWidget->addItems(QFileDialog::getOpenFileNames(
          this
        , "Add text files"
        , std::getenv(HOMEDIR)
        , "Text files (*.txt)"
        )
    );
}

void MainWindow::removeSelectedItems() {
    auto items = ui->listWidget->selectedItems();
    for (auto &i : items) {
        delete ui->listWidget->takeItem(ui->listWidget->row(i));
    }
}

void MainWindow::search() {
    std::vector<std::string> docs;

    if (ui->radioButton->isChecked()) {
        docs = jsonConverter::getTextDocuments();
    } else {
        int docsCount = ui->listWidget->count();
        docs.reserve(docsCount);


        for (int i = 0; i < docsCount; ++i) {
            QFile file(ui->listWidget->item(i)->text());
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream ts(&file);
                docs.emplace_back(ts.readAll().toStdString());
                file.close();
            }
        }
    }

    idx->updateDocumentBase(docs, tp);
    searchServer engine(*idx);

    jsonConverter::putAnswers(engine.search(jsonConverter::getRequests(), tp));

    QMessageBox msg(
          QMessageBox::Information
        , "Completed"
        , "Results are saved to answers.json"
        , QMessageBox::Ok
        , this
    );
    msg.exec();
}

void MainWindow::setMaxResponses(int val) {
    jsonConverter::setResponsesLimit(val);
}

void MainWindow::openRequestEditor() {
    if (!editor) {
        editor = new requestEditor(this);
    }

    editor->show();
}
