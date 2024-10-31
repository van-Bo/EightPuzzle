#ifndef SEARCHINGSHOW_H
#define SEARCHINGSHOW_H

#include <QWidget>
#include <queue>
#include <QSet>
#include <QPair>

typedef QPair<int, int> PII;

namespace Ui {
class SearchingShow;
}

class SearchingShow : public QWidget
{
    Q_OBJECT

public:
    explicit SearchingShow(QString over, QWidget *parent = nullptr);
    ~SearchingShow();

    int bfs(QString start, QString over);
    void initPosMap();
    int F_Manhattan(const QString &s);
    int F_PlaceHolder(const QString &s);
    void clear_DistMap_CloseSet();
    int Astar_Manhattan(QString start, QString over);
    int Astar_PlaceHolder(QString start, QString over);
    void dfs_init(QString &start);
    bool dfs(QString &over, QString p, QString t, int depth);

    // right-up-left-down
    int dx[4] = {0, -1, 0, 1};
    int dy[4] = {1, 0, -1, 0};

    // dist[v] = d 针对 close 表中已经处理的节点状态 v 和其对应的搜索步数 d 建立映射关系
    std::unordered_map<QString, int> dist;

    // 工作队列 open 用于存储当前所有待处理的节点（还未被完全扩展的节点），节点被处理后即被弹出队列
    std::queue<QString> open;

    // close 记录已处理的节点（记录工作队列中弹出的元素）
    QSet<QString> close;

    std::unordered_map<QString, QString> f;

    // Astar 优先队列 heap
    std::priority_queue<QPair<int, QString>, QVector<QPair<int, QString>>, std::greater<QPair<int, QString>>> heap;

    std::unordered_map<QChar, PII> pos;

    QSet<QString> openDFS;
    const int MAX_DEPTH = 50; // 设定一个合理的最大递归深度

private:
    Ui::SearchingShow *ui;
    QString start;
};

#endif // SEARCHINGSHOW_H
