#include "requesteditor.h"
#include "./ui_requesteditor.h"
#include "jsonConverter.h"

requestEditor::requestEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::requestEditor) {
    ui->setupUi(this);
    auto requests = jsonConverter::getRequests();
    for (auto &i : requests) {
        ui->listWidget->addItem(i.c_str());
    }
}

requestEditor::~requestEditor() {
    delete ui;
}
