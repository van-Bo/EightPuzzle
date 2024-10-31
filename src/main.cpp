#include "widget.h"

#include <QApplication>
#include <QString>
#include <unordered_map>
#include <set>
#include <queue>
#include <QPair>
#include <QFile>

#define x first
#define y second

typedef QPair<int, int> PII;

int dx[] = {0, -1, 1, 0};
int dy[] = {1, 0, 0, -1};

// dist[v] = d 针对 close 表中已经处理的节点状态 v 和其对应的搜索步数 d 建立映射关系
std::unordered_map<QString, int> dist;

// 工作队列 open 用于存储当前所有待处理的节点（还未被完全扩展的节点），节点被处理后即被弹出队列
std::queue<QString> open;

// close 记录已处理的节点（记录工作队列中弹出的元素）
QSet<QString> close;

std::unordered_map<QString, QString> f;

void clear_DistMap_CloseSet()
{
    dist.clear();
    close.clear();
}

// bfs 求解八数码问题
int bfs(QString start, QString over)
{
    open.push(start);
    dist[start] = 0;

    f[start] = "-1";

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


QSet<QString> openDFS;
const int MAX_DEPTH = 5625; // 设定一个合理的最大递归深度

void dfs_init(QString &start)
{
    dist[start] = 0;
}

bool dfs(QString &over, QString p, QString t, int depth)
{
    qDebug() << "dfs(" << over << ", " << p << ", " << t << ")";
    if (depth > MAX_DEPTH)
    {
        throw std::overflow_error("Stack overflow detected");
    }
    if (p != "none") close.insert(p);
    if (t == over) return true;

    int pos = t.indexOf('x');
    int preDist = dist[t];
    QString preState = t;
    int x = pos / 3, y = pos % 3;
    for (int i = 0; i < 4; i ++)
    {
        int a = x + dx[i], b = y + dy[i];
        if (a >= 0 && a < 3 && b >= 0 && b < 3)
        {

            openDFS.insert(t);
            std::swap(t[pos], t[a * 3 + b]);
            if (t == p) // 剪枝，避免回溯
            {
                std::swap(t[pos], t[a * 3 + b]);
                continue;
            }

            if (dist.find(t) == dist.end() || dist[t] > preDist + 1)
            {
                dist[t] = preDist + 1;

                openDFS.insert(t);
                if (dfs(over, preState, t, depth + 1))
                    return true;
                openDFS.remove(t);
                qDebug() << p << " " << preState << " " << t;
            }

            std::swap(t[pos], t[a * 3 + b]);
        }
    }
    return false;
}

std::unordered_map<QChar, PII> pos;
void initPosMap()
{
    QString standard = "12345678x";
    int len = standard.size();
    for (int i = 0; i < len; i ++)
    {
        int x = i / 3, y = i % 3;
        pos[standard[i]] = {x, y};
    }
}

int F_Manhattan(const QString &s)
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

int F_PlaceHolder(const QString &s)
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

// Astar 优先队列 heap
std::priority_queue<QPair<int, QString>, QVector<QPair<int, QString>>, std::greater<QPair<int, QString>>> heap;

// A* 求解八数码问题，其估价函数为 曼哈顿距离
int Astar_Manhattan(QString start, QString over)
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

int Astar_PlaceHolder(QString start, QString over)
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


// 加载样式表文件
void loadStyleSheet(const QString &sheetName) {
    QFile file(sheetName);
    if (file.exists()) {
       qDebug() << "File exists.";
    } else {
       qDebug() << "File does not exist.";
    }

    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
        qDebug() << "Style sheet loaded successfully!!!";
    }
    else
        qDebug() << "Fail to load style sheet >_<";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    loadStyleSheet(":/style.qss");
    w.show();

    QString start = "23415x768";
    QString over = "12345678x";


    int steps = bfs(start, over);
    qDebug() << steps;
    qDebug() << "CLOSE table(扩展节点数) size: " << close.size();
    qDebug() << "Generated Nodes' number: " << close.size() + open.size();

//    QString point = over;
//    while (f[point] != "-1")
//    {
//        qDebug() << f[point];
//        point = f[point];
//    }
    qDebug() << "=================";
    clear_DistMap_CloseSet();
    initPosMap();
    f.clear();

    steps = Astar_Manhattan(start, over);
    qDebug() << steps;
    qDebug() << "CLOSE table(扩展节点数) size: " << close.size();
    qDebug() << "Generated Nodes' number: " << close.size() + heap.size();

//    point = over;
//    while (f[point] != "none")
//    {
//        qDebug() << f[point];
//        point = f[point];
//    }

    qDebug() << "=================";
    clear_DistMap_CloseSet();
    initPosMap();
    f.clear();
    // 清空优先队列
    while (!heap.empty()) {
        heap.pop();
    }

    steps = Astar_PlaceHolder(start, over);
    qDebug() << steps;
    qDebug() << "CLOSE table(扩展节点数) size: " << close.size();
    qDebug() << "Generated Nodes' number: " << close.size() + heap.size();


    qDebug() << "=================";
    clear_DistMap_CloseSet();

    start = "1234567x8";
    dfs_init(start);
    try {
        dfs(over, "none", start, 0);
        qDebug() << dist[over];
        qDebug() << "CLOSE table(扩展节点数) size: " << close.size();
        qDebug() << "Generated Nodes' number: " << openDFS.size();

        qDebug() << "close";
        for (auto it = close.begin(); it != close.end(); ++it) {
            qDebug() << *it;
        }

        qDebug() << "openDFS";
        for (auto it = openDFS.begin(); it != openDFS.end(); ++it) {
            qDebug() << *it;
        }

    } catch (const std::overflow_error &e) {
        qDebug() << "Error:" << e.what();
    }

    return a.exec();
}
