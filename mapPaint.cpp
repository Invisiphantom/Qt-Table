#include "header/mainwindow.h"

#include <QPainter>

// 绘制地图中的道路
void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制道路
    for (auto way : ways) {
        for (int i = 0; i < way.second.nodes.size() - 1; i++) {
            Node node1 = way.second.nodes[i];
            Node node2 = way.second.nodes[i + 1];
            double x1 = (node1.lon - minLon) / (maxLon - minLon) * this->width();
            double y1 = (node1.lat - minLat) / (maxLat - minLat) * this->height();
            double x2 = (node2.lon - minLon) / (maxLon - minLon) * this->width();
            double y2 = (node2.lat - minLat) / (maxLat - minLat) * this->height();
            // 因为高纬度在屏幕上方，低纬度在屏幕下方，所以需要上下翻转
            y1 = this->height() - y1;
            y2 = this->height() - y2;
            // 计算中心缩放后的坐标
            x1 = x1 * scaleFactor + this->width() / 2 * (1 - scaleFactor);
            y1 = y1 * scaleFactor + this->height() / 2 * (1 - scaleFactor);
            x2 = x2 * scaleFactor + this->width() / 2 * (1 - scaleFactor);
            y2 = y2 * scaleFactor + this->height() / 2 * (1 - scaleFactor);
            painter.setPen(QPen(QColor(180, 180, 180), 6, Qt::SolidLine, Qt::FlatCap));
            painter.drawLine(x1, y1, x2, y2);

            // 如果两边的结点都在最短路径中，则为红色
            if (visitedVertex.count(node1.id) && visitedVertex.count(node2.id))
                painter.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::FlatCap));
            // 如果该道路没有被选择，则为白色
            else
                painter.setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::FlatCap));
            painter.drawLine(x1, y1, x2, y2);
        }
    }
}