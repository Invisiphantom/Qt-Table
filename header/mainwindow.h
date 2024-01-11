#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QPaintEvent>
#include <QRadioButton>
#include <QSlider>
#include <QTextBrowser>
#include <QWheelEvent>
#include <list>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

using namespace std;
using ll = long long;
using pdll = pair<double, ll>;

class MainWindow : public QMainWindow {
    Q_OBJECT

    struct Node {
        ll id;
        double lon, lat;
    };
    struct Way {
        ll id;
        vector<Node> nodes;
        map<string, string> tags;
    };

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   protected:
    // mapPaint.cpp
    void paintEvent(QPaintEvent *event) override;
    // mapEvent.cpp
    void updateVertexPos();
    void updateLocationLabel();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

   private slots:
    // mapSlot.cpp
    void updateDistLabel(double minDist);
    void onVertexToggled(bool checked, ll id);
    void on_scaleSlider_valueChanged(int value);
    void on_algoButton_released();
    void on_flushButton_released();

   public:
    double minLon, maxLon;     // 经度范围
    double minLat, maxLat;     // 纬度范围
    double scaleFactor = 1.0;  // 缩放因子

   private:
    Ui::MainWindow *ui;        // ui界面
    QButtonGroup vertexGroup;  // 顶点组
    QSlider *scaleSlider;      // 缩放滚动条
    QLabel *distanceLabel;     // 文本显示器
    QLabel *locationLabel;     // 地图区域坐标

    map<ll, list<ll>> adj;            // 顶点邻接表
    list<ll> selectedVertex;          // 起点终点队列
    map<ll, QRadioButton *> vertex;   // 顶点按钮组件
    unordered_set<ll> visitedVertex;  // 最短路径途经的顶点

    QPoint lastMousePos;  // 鼠标位置

    map<ll, Node> nodes;  // 顶点
    map<ll, Way> ways;    // 道路

    // mapLoad.cpp
    void loadOSM();     // 加载OSM文件
    void importFile();  // 导入文件
    void exportFile();  // 导出文件
    void helpShow();    // 显示帮助窗口

    // mapSPath.cpp
    struct SPath_tag {};
    struct dijkstra_tag : public SPath_tag {};
    struct AStar_tag : public SPath_tag {};
    enum { Dijkstra, AStar } algo_tag = Dijkstra;
    
    double toRadians(double degrees);
    double calcuDist(double lat1, double lon1, double lat2, double lon2);
    double updateShortestPath(dijkstra_tag);
    double updateShortestPath(AStar_tag);
};
#endif  // MAINWINDOW_H
