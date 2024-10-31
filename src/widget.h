#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

typedef QPair<int, int> PII;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
    void adjustTableWidgetFont();
    QString generateRandomEightPuzzle();
    bool isSolvable(QString s);
    void setStartState(QString s);
    QString getStartState();
    void handleDataEntered(const QString &data);
    ~Widget();

private slots:
    void onRandomGenerate_clicked();
    void onIsSolvalbe_clicked();
    void onHandleInput_clicked();
    void onSearchingShow_clicked();

private:
    Ui::Widget *ui;
    QString start;
    bool isValidToSearch;
};
#endif // WIDGET_H
