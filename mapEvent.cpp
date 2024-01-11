#include "header/mainwindow.h"

void MainWindow::updateVertexPos() {
    // 更新顶点组件的位置
    for (auto v : vertex) {
        double x = (nodes[v.first].lon - minLon) / (maxLon - minLon) * this->width();
        double y = (nodes[v.first].lat - minLat) / (maxLat - minLat) * this->height();
        y = this->height() - y;
        x = x * scaleFactor + this->width() / 2 * (1 - scaleFactor);
        y = y * scaleFactor + this->height() / 2 * (1 - scaleFactor);
        // 使其居中
        x = x - v.second->width() / 2;
        y = y - v.second->height() / 2;
        v.second->setGeometry(x, y, 15, 15);
    }
}

void MainWindow::updateLocationLabel() {
    // 更新地图区域坐标的内存
    // 当前经纬度：(midLon EW, midLat NS)
    double midLon = (minLon + maxLon) / 2;
    double midLat = (minLat + maxLat) / 2;

    QString text;
    text += "当前经纬度：(";
    text += QString::number(abs(midLon), 'f', 6) + (midLon > 0 ? "E" : "W") + ", ";
    text += QString::number(abs(midLat), 'f', 6) + (midLat > 0 ? "N" : "S") + ")";
    locationLabel->setText(text);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // 计算鼠标移动的距离(需要考虑缩放)
        double dx = (event->pos().x() - lastMousePos.x()) / scaleFactor;
        double dy = (event->pos().y() - lastMousePos.y()) / scaleFactor;
        // 更新地图的显示范围
        minLon -= dx / (double)this->width() * (maxLon - minLon);
        maxLon -= dx / (double)this->width() * (maxLon - minLon);
        minLat += dy / (double)this->height() * (maxLat - minLat);
        maxLat += dy / (double)this->height() * (maxLat - minLat);
        // 记录鼠标移动的位置
        lastMousePos = event->pos();
        this->update();
        this->updateVertexPos();
        this->updateLocationLabel();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0)
        scaleFactor *= 1.2;
    else if (log(scaleFactor) / log(1.2) > -2)
        scaleFactor /= 1.2;

    // 更新滚动条的值
    scaleSlider->setValue(round(log(scaleFactor) / log(1.2)));
    this->update();
    this->updateVertexPos();
    this->updateLocationLabel();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    this->updateVertexPos();
    this->updateLocationLabel();
}

