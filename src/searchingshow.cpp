#include "searchingshow.h"
#include "ui_searchingshow.h"

#include <unordered_map>
#include <set>
#include <queue>
#include <QPair>
#include <QStringList>
#include <QListWidget>

#define x first
#define y second

SearchingShow::SearchingShow(QString start, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchingShow), start(start)
{
    ui->setupUi(this);
    this->setStyleSheet(
        "QLineEdit {"
        "    font-size: 18px;"  // 设置字体大小
        "    padding: 4px;"     // 设置内边距，使文本框更宽
        "    border: 2px solid #8f8f91;"  // 设置边框颜色和宽度
        "    border-radius: 10px;"  // 设置圆角
        "    background: #f0f0f0;"  // 设置背景颜色
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #00aaff;"  // 设置聚焦时的边框颜色
        "}"
        "QLineEdit:placeholder {"
        "    color: #a9a9a9;"  // 设置 placeholder 文本颜色
        "    font-style: italic;"  // 设置 placeholder 文本样式
        "}"
    );

    QString over = "12345678x";

    // bfs block
    int steps = bfs(start, over);
    ui->steps1->setText(QString::number(steps));
    ui->close1->setText(QString::number(close.size()));
    ui->open1->setText(QString::number(close.size() + open.size()));
    QString point = over;
    QStringList list;
    list.clear();
    while (f[point] != "none")
    {
        list.append(point);
        point = f[point];
    }
    list.append(start);
    std::reverse(list.begin(), list.end());

    for (int i = 0; i < list.size(); i ++)
    {
        QString item = list[i];
        QListWidgetItem *listItem = new QListWidgetItem(QString::number(i + 1) + " >>  " + item);
        listItem->setTextAlignment(Qt::AlignCenter); // 设置文本居中
        ui->trace1->addItem(listItem);
    }


    // Astar_Manhattan block
    clear_DistMap_CloseSet();
    initPosMap();
    f.clear();

    steps = Astar_Manhattan(start, over);
    ui->steps2->setText(QString::number(steps));
    ui->close2->setText(QString::number(close.size()));
    ui->open2->setText(QString::number(close.size() + heap.size()));

    point = over;
    list.clear();
    while (f[point] != "none")
    {
        list.append(point);
        point = f[point];
    }
    list.append(start);
    std::reverse(list.begin(), list.end());

    for (int i = 0; i < list.size(); i ++)
    {
        QString item = list[i];
        QListWidgetItem *listItem = new QListWidgetItem(QString::number(i + 1) + " >>  " + item);
        listItem->setTextAlignment(Qt::AlignCenter); // 设置文本居中
        ui->trace2->addItem(listItem);
    }


    // Astar_PlaceHolder block
    clear_DistMap_CloseSet();
    initPosMap();
    f.clear();
    // 清空优先队列
    while (!heap.empty()) {
        heap.pop();
    }

    steps = Astar_PlaceHolder(start, over);
    ui->steps3->setText(QString::number(steps));
    ui->close3->setText(QString::number(close.size()));
    ui->open3->setText(QString::number(close.size() + heap.size()));

    point = over;
    list.clear();
    while (f[point] != "none")
    {
        list.append(point);
        point = f[point];
    }
    list.append(start);
    std::reverse(list.begin(), list.end());

    for (int i = 0; i < list.size(); i ++)
    {
        QString item = list[i];
        QListWidgetItem *listItem = new QListWidgetItem(QString::number(i + 1) + " >>  " + item);
        listItem->setTextAlignment(Qt::AlignCenter); // 设置文本居中
        ui->trace3->addItem(listItem);
    }


    // dfs block
    clear_DistMap_CloseSet();
    dfs_init(start);
    bool res = dfs(over, "none", start, 0);
    if (res == true)
    {
        ui->steps4->setText(QString::number(dist[over]));
        ui->close4->setText(QString::number(close.size()));
        ui->open4->setText(QString::number(openDFS.size()));

        point = over;
        list.clear();
        while (f[point] != "none")
        {
            list.append(point);
            point = f[point];
        }
        list.append(start);
        std::reverse(list.begin(), list.end());

        for (int i = 0; i < list.size(); i ++)
        {
            QString item = list[i];
            QListWidgetItem *listItem = new QListWidgetItem(QString::number(i + 1) + " >>  " + item);
            listItem->setTextAlignment(Qt::AlignCenter); // 设置文本居中
            ui->trace4->addItem(listItem);
        }
    }
    else
    {
        ui->steps4->setText("none...");
        ui->close4->setText("none...");
        ui->open4->setText("none...");
        QString item = "Exceeded MAX(recursion depth)";
        QListWidgetItem *listItem = new QListWidgetItem(" Hint ");
        listItem->setTextAlignment(Qt::AlignCenter); // 设置文本居中
        ui->trace4->addItem(listItem);
        QListWidgetItem *listItem2 = new QListWidgetItem(item);
        ui->trace4->addItem(listItem2);
    }
}

// bfs 求解八数码问题
int SearchingShow::bfs(QString start, QString over)
{
    open.push(start);
    dist[start] = 0;

    f[start] = "none";

    while (open.size())
    {
        auto t = open.front();
        open.pop();
        int preDist = dist[t];

        if (t == over) return preDist;

        int pos = t.indexOf('x');
        int x = pos / 3, y = pos % 3;
        for (int i = 0; i < 4; i ++)
        {
            int a = x + dx[i], b = y + dy[i];
            if (a >= 0 && a <= 2 && b >= 0 && b <= 2)
            {
                std::swap(t[pos], t[a * 3 + b]);
                if (dist.find(t) == dist.end())
                {
                    dist[t] = preDist + 1;
                    open.push(t);

                    QString temp = t;
                    std::swap(temp[pos], temp[a * 3 + b]);
                    f[t] = temp;
                }
                std::swap(t[pos], t[a * 3 + b]); // 恢复现场
            }
        }

        close.insert(t);
    }
    return -1;
}

void SearchingShow::initPosMap()
{
    QString standard = "12345678x";
    int len = standard.size();
    for (int i = 0; i < len; i ++)
    {
        int x = i / 3, y = i % 3;
        pos[standard[i]] = {x, y};
    }
}

void SearchingShow::clear_DistMap_CloseSet()
{
    dist.clear();
    close.clear();
}

int SearchingShow::F_Manhattan(const QString &s)
{
    int res = 0;

    int len = s.size();
    for (int i = 0; i < len; i ++)
    {
        auto c = s[i];
        int x = i / 3, y = i % 3;
        res += abs(pos[c].x - x) + abs(pos[c].y - y);
    }

    return res;
}

int SearchingShow::F_PlaceHolder(const QString &s)
{
    int res = 0;

    int len = s.size();
    for (int i = 0; i < len; i ++)
    {
        auto c = s[i];
        int x = c.unicode() - '1';
        if (x == i) res ++;
    }

    return res;
}

// A* 求解八数码问题，其估价函数为 曼哈顿距离
int SearchingShow::Astar_Manhattan(QString start, QString over)
{
    heap.push({F_Manhattan(start), start});
    dist[start] = 0;
    f[start] = "none";

    while (heap.size())
    {
        auto t = heap.top();
        heap.pop();

        QString state = t.y, source = state;
        int preDist = dist[state];

        if (state == over) return preDist;
        close.insert(state);

        int pos = state.indexOf('x');
        int x = pos / 3, y = pos % 3;

        for (int i = 0; i < 4; i ++)
        {
            int a = x + dx[i], b = y + dy[i];
            if (a >= 0 && a <= 2 && b >= 0 && b <= 2)
            {
                std::swap(state[pos], state[a * 3 + b]);
                if (dist.find(state) == dist.end() || dist[state] > preDist + 1)
                {
                    dist[state] = preDist + 1;
                    heap.push({dist[state] + F_Manhattan(state), state});

                    f[state] = source;
                }
                std::swap(state[pos], state[a * 3 + b]); // 恢复现场
            }
        }
    }
    return -1;
}

// A* 求解八数码问题，其估价函数为 不在位数
int SearchingShow::Astar_PlaceHolder(QString start, QString over)
{
    heap.push({F_PlaceHolder(start), start});
    dist[start] = 0;
    f[start] = "none";

    while (heap.size())
    {
        auto t = heap.top();
        heap.pop();

        QString state = t.y, source = state;
        int preDist = dist[state];

        if (state == over) return preDist;
        close.insert(state);

        int pos = state.indexOf('x');
        int x = pos / 3, y = pos % 3;

        for (int i = 0; i < 4; i ++)
        {
            int a = x + dx[i], b = y + dy[i];
            if (a >= 0 && a <= 2 && b >= 0 && b <= 2)
            {
                std::swap(state[pos], state[a * 3 + b]);
                if (dist.find(state) == dist.end() || dist[state] > preDist + 1)
                {
                    dist[state] = preDist + 1;
                    heap.push({dist[state] + F_PlaceHolder(state), state});

                    f[state] = source;
                }
                std::swap(state[pos], state[a * 3 + b]); // 恢复现场
            }
        }
    }
    return -1;
}


// dfs 求解八数码问题
void SearchingShow::dfs_init(QString &start)
{
    dist[start] = 0;
}

bool SearchingShow::dfs(QString &over, QString p, QString t, int depth)
{
    f[t] = p;

    if (depth > MAX_DEPTH) return false;
    if (t == over) return true;
    close.insert(t);

    int pos = t.indexOf('x');
    int preDist = dist[t];
    QString preState = t;
    int x = pos / 3, y = pos % 3;

    for (int i = 0; i < 4; i ++)
    {
        int a = x + dx[i], b = y + dy[i];
        if (a >= 0 && a < 3 && b >= 0 && b < 3)
        {
            std::swap(t[pos], t[a * 3 + b]);
            if (t == p) // 剪枝，避免回溯
            {
                std::swap(t[pos], t[a * 3 + b]);
                continue;
            }

//            if (dist.find(t) == dist.end() || dist[t] > preDist + 1)
            if (close.find(t) == close.end() || dist[t] > preDist + 1)
//            if (close.find(t) == close.end())
            {
                dist[t] = preDist + 1;

                openDFS.insert(t);
                if (dfs(over, preState, t, depth + 1))
                    return true;
            }

            std::swap(t[pos], t[a * 3 + b]);
        }
    }
    return false;
}

SearchingShow::~SearchingShow()
{
    delete ui;
}
