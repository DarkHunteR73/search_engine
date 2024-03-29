#ifndef REQUESTEDITOR_H
#define REQUESTEDITOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class requestEditor; }
QT_END_NAMESPACE

class requestEditor : public QMainWindow {
    Q_OBJECT

public:
    requestEditor(QWidget *parent = nullptr);
    ~requestEditor();

public slots:
    void addRequest();
    void removeSelectedRequest();
    void acceptRequests();

private:
    Ui::requestEditor *ui;
};
#endif // REQUESTEDITOR_H
