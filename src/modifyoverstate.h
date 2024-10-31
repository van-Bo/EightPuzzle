#ifndef MODIFYOVERSTATE_H
#define MODIFYOVERSTATE_H

#include <QDialog>

namespace Ui {
class modifyOverState;
}

class modifyOverState : public QDialog
{
    Q_OBJECT

public:
    explicit modifyOverState(QWidget *parent = nullptr);
    ~modifyOverState();
    bool validateInput(const QString &input);

private slots:
    void onAccepted_clicked();

signals:
    void dataEntered(const QString &data);

private:
    Ui::modifyOverState *ui;
};

#endif // MODIFYOVERSTATE_H
