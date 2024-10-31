#include "widget.h"
#include "ui_widget.h"
#include "modifyoverstate.h"
#include "searchingshow.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QRandomGenerator>
#include <QVector>
#include <algorithm>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <unordered_map>
#include <QSet>
#include <set>
#include <queue>
#include <QPair>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->start->setRowCount(3);
    ui->start->setColumnCount(3);
    ui->over->setRowCount(3);
    ui->over->setColumnCount(3);


    // 填充数据
    QStringList puzzle = {"1", "2", "3", "4", "5", "6", "7", "8", " "};
    start = "12345678x";
    isValidToSearch = true;
    for (int i = 0; i < puzzle.size(); ++i) {
        QTableWidgetItem *item1 = new QTableWidgetItem(puzzle[i]);
        item1->setTextAlignment(Qt::AlignCenter);
        ui->start->setItem(i / 3, i % 3, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(puzzle[i]);
        item2->setTextAlignment(Qt::AlignCenter);
        ui->over->setItem(i / 3, i % 3, item2);
    }

    // 隐藏行列标号
    ui->start->horizontalHeader()->setVisible(false);
    ui->start->verticalHeader()->setVisible(false);
    ui->over->horizontalHeader()->setVisible(false);
    ui->over->verticalHeader()->setVisible(false);

    // 使单元格大小随tableWidget大小变化而变化
    ui->start->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->start->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->over->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->over->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    adjustTableWidgetFont();

    connect(ui->randomGenerate, &QPushButton::clicked, this, &Widget::onRandomGenerate_clicked);
    connect(ui->isSolvable, &QPushButton::clicked, this, &Widget::onIsSolvalbe_clicked);
    connect(ui->handleInput, &QPushButton::clicked, this, &Widget::onHandleInput_clicked);
    connect(ui->searching, &QPushButton::clicked, this, &Widget::onSearchingShow_clicked);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustTableWidgetFont();
}

void Widget::adjustTableWidgetFont()
{
    int widgetWidth = ui->start->width();
    int widgetHeight = ui->start->height();
    int size = qMin(widgetWidth, widgetHeight) / 3;

    // 设置字体大小
    int fontSize = size / 3; // 这里你可以根据需要调整字体大小的比例
    QFont font = ui->start->font();
    font.setPointSize(fontSize);

    for (int i = 0; i < ui->start->rowCount(); ++i) {
        for (int j = 0; j < ui->start->columnCount(); ++j) {
            QTableWidgetItem *item1 = ui->start->item(i, j);
            QTableWidgetItem *item2 = ui->over->item(i, j);
            if (item1) {
                item1->setFont(font);
            }
            if (item2) {
                item2->setFont(font);
            }
        }
    }
}

QString Widget::generateRandomEightPuzzle() {
    QVector<int> numbers;
    for (int i = 1; i <= 8; ++i) {
        numbers.append(i);
    }
    numbers.append(0); // 用0表示空格

    // 使用 std::random_device 和 std::mt19937 生成随机数
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);

    QString puzzle;
    for (int num : numbers) {
        if (num == 0) {
            puzzle += 'x';
        } else {
            puzzle += QString::number(num);
        }
    }

    return puzzle;
}

void Widget::onRandomGenerate_clicked()
{
    QString demo = generateRandomEightPuzzle();
    setStartState(demo);
    isValidToSearch = isSolvable(demo);
    qDebug() << "<module::RandomGenerate> startState is setted by " << start;
    qDebug() << "<module::RandomGenerate> isValidToSearch is modified by " << isValidToSearch;
    for (int i = 0; i < demo.size(); ++i)
    {
        if (demo[i] == 'x') demo[i] = ' ';
        QTableWidgetItem *item = new QTableWidgetItem(QString(1, demo[i]));
        item->setTextAlignment(Qt::AlignCenter);
        ui->over->setItem(i / 3, i % 3, item);
    }

    ui->over->horizontalHeader()->setVisible(false);
    ui->over->verticalHeader()->setVisible(false);

    ui->over->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->over->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    adjustTableWidgetFont();
}

bool Widget::isSolvable(QString s)
{
    int cnt = 0;

    int len = s.length();
    for (int i = 0; i < len; i ++)
    {
        if (s[i] == 'x') continue;
        for (int j = i + 1; j < len; j ++)
        {
            if (s[j] == 'x') continue;
            if (s[i] > s[j]) cnt ++;
        }
    }

    return cnt % 2 == 0;
}

void Widget::setStartState(QString s)
{
    start = s;
}

QString Widget::getStartState()
{
    return start;
}

void Widget::onIsSolvalbe_clicked()
{
    bool hasSolution = isSolvable(start);
    qDebug() << "<module::checkIsSolvable> result is " << hasSolution;
    if (hasSolution) {
        QMessageBox::information(this, "检测结果",
                                 "<h2 style='font-family:Arial; font-size:24px; color:green;'>问题有解</h2>"
                                 "<p style='font-family:Segoe UI; font-size:18px; color:green;'>恭喜！当前问题有解。</p>"
                                 "<p style='font-family:Segoe UI; font-size:16px;'>可以继续进行结果搜索。</p>");
    } else {
        QMessageBox::warning(this, "检测结果",
                             "<h2 style='font-family:Arial; font-size:24px; color:red;'>问题无解</h2>"
                             "<p style='font-family:Segoe UI; font-size:18px; color:red;'>很遗憾，目前的问题没有解。</p>"
                             "<p style='font-family:Segoe UI; font-size:16px;'>不要气馁，再试一次，请重新随机生成问题。</p>");
    }
}

void Widget::onHandleInput_clicked()
{
    modifyOverState *demo = new modifyOverState(this);

    // 连接子窗口的信号到父窗口的槽函数
    connect(demo, &modifyOverState::dataEntered, this, &Widget::handleDataEntered);

    int result = demo->exec();
    qDebug() << "modifyOverState Dialog closed with result:" << result;
}



void Widget::onSearchingShow_clicked()
{
    if (isValidToSearch)
    {
        SearchingShow *demo = new SearchingShow(this->getStartState(), nullptr); // 设置父窗口为 nullptr
    //    demo->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
        demo->show();
        qDebug() << "searchingShow displaying...";
    }
    else
    {
        QMessageBox::warning(this, "警告提示",
                             "<h2 style='font-family:Arial; font-size:24px; color:red;'>当前问题无解</h2>"
                             "<p style='font-family:Segoe UI; font-size:18px; color:red;'>很遗憾，当前的问题没有解，无法进行搜索。</p>"
                             "<p style='font-family:Segoe UI; font-size:16px;'>请重新随机生成或手动输入问题。</p>");
    }
}

void Widget::handleDataEntered(const QString &data)
{
    setStartState(data);
    isValidToSearch = isSolvable(data);
    QString temp = data;
    qDebug() << "module<Widget::handleDataEntered>:" << data;
    for (int i = 0; i < temp.size(); ++i)
    {
        if (temp[i] == 'x') temp[i] = ' ';
        QTableWidgetItem *item = new QTableWidgetItem(QString(1, temp[i]));
        item->setTextAlignment(Qt::AlignCenter);
        ui->over->setItem(i / 3, i % 3, item);
    }

    ui->over->horizontalHeader()->setVisible(false);
    ui->over->verticalHeader()->setVisible(false);

    ui->over->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->over->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    adjustTableWidgetFont();
}

Widget::~Widget()
{
    delete ui;
}

