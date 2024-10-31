## 平台环境
- Qt Creator 10.0.1
- Qt6 6.2.4

## 实验内容
八数码搜索比对

## 实验要求
1. 分别以各种搜索算法为例演示搜索过程，比较、分析使用不同算法处理相同问题的效率和优缺点。
2. 分析各种算法中的OPEN表、CLOSED表的生成过程。
3. 对比分析不同评价函数对搜索算法的影响。
4. 分析某种启发式搜索算法（A*）的特点。

![image1](/images/MainWidget.png)
![image2](/images/ModifyStartState.png)
![image3](/images/876x34512.png)
![image4](/images/74561283x.png)

## 要点分析
### 递归解决八数码问题
- 如下所提供的是修正之后的代码
- dfs 的目的并不是得到最优解, 而是通过暴力搜索获取可行解
- 原先错误点 1 : `if (depth > MAX_DEPTH) throw ..`, 致使一旦递归层数超过 `MAX_DEPTH`, 并且没有找寻到 over 状态, 由于递归的盲目性, 会有极高的概率符合前述的条件, 继而会自动抛出异常, 从而导致每次 dfs 的搜索结果均显示超过 `MAX_DEPTH` 
- 原先错误点 2 : 对于集合 `close`、`openDFS` 的操作与 `dfs` 函数的执行意义不匹配, `dfs(over, p, t, depth)` 的执行意义为当前递归处理到 depth 层, 处理的局面为 t, t 局面的上一局面为 p, 终局为 over, 在当前递归函数的函数体中，会以当前局面 t 为中继节点进行合法的拓展, 故针对集合的操作为: 在进入递归函数后, 结束当前递归的判定条件不符的前提下, 可以将当前局面纳入 `close` 集合; 由局面 t 拓展而来的其他合法局面则纳入 `openDFS` 集合
```C++
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
```

### 递归结果分析
- **现象阐述**: 使用 dfs 搜索某一局面的可行解(前提一定有解)时，同样的初始状态，递归的最大深度分别为50 和100，发现最终二者搜索到的可行解的步长有点特殊，递归最大深度为50的可行解搜索步数趋近于50，而另一个则趋近于100
- **解释**
    - **DFS特性**: 深度优先搜索会尽量深入到路径的尽头，然后再回溯。这意味着，如果设定较大的最大深度，它会探索更深的路径。
    - **解的路径**: 在递归最大深度设为 100 时，DFS 会在更深的层级找到一个解，而不一定是最优解。而在递归最大深度设为 50 时，它会在较浅的层级找到一个解。
    - **非最优解**: DFS 不保证找到的是最优解。因此，较大深度的 DFS 可能会找到一个较长路径的解。
