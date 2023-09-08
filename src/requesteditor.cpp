#include "requesteditor.h"
#include "./ui_requesteditor.h"
#include "jsonConverter.h"

#include <QInputDialog>
#include <QString>

#include <vector>
#include <string>

requestEditor::requestEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::requestEditor) {
    ui->setupUi(this);
    auto requests = jsonConverter::getRequests();
    for (auto &i : requests) {
        ui->listWidget->addItem(i.c_str());
    }

    connect(ui->listWidget, &QListWidget::doubleClicked, [this](){
        QString currentText = ui->listWidget->item(ui->listWidget->currentRow())->text();
        QString changedText = QInputDialog::getText(
              this
            , "Edit request"
            , "Text:"
            , QLineEdit::Normal
            , currentText
        );

        if (!changedText.isEmpty() && changedText != currentText) {
            ui->listWidget->item(ui->listWidget->currentRow())->setText(changedText);
        }
    });
}

requestEditor::~requestEditor() {
    delete ui;
}

void requestEditor::addRequest() {
    QString tmpText = QInputDialog::getText(
          this
        , "Add new request"
        , "Text:"
    );

    if (!tmpText.isEmpty()) {
        ui->listWidget->addItem(tmpText);
    }
}

void requestEditor::removeSelectedRequest() {
    auto items = ui->listWidget->selectedItems();
    for (auto &i : items) {
        delete ui->listWidget->takeItem(ui->listWidget->row(i));
    }
}

void requestEditor::acceptRequests() {
    std::vector<std::string> requests(ui->listWidget->count());

    for (size_t i = 0; i < requests.size(); ++i) {
        requests[i] = ui->listWidget->item(i)->text().toStdString();
    }

    jsonConverter::setRequests(requests);
    this->close();
}
