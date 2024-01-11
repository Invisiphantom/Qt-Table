#include "./ui_mainwindow.h"
#include "header/mainwindow.h"

void MainWindow::on_scaleSlider_valueChanged(int value) {
    scaleFactor = pow(1.2, value);
    updateVertexPos();
    this->update();
}

void MainWindow::updateDistLabel(double minDist) {
    // 更新textBrowser的面板文字：
    // 起点为：selectedVertex.front()
    // 终点为：selectedVertex.back()
    // 最短距离为：visitedVertex
    QString text;
    if (selectedVertex.size() == 0) {
        text += "起点为：\n";
        text += "终点为：\n";
        text += "最短距离为：\n";
    } else if (selectedVertex.size() == 1) {
        text += "起点为：" + QString::number(selectedVertex.front()) + "\n";
        text += "终点为：\n";
        text += "最短距离为：\n";
    } else if (selectedVertex.size() == 2) {
        text += "起点为：" + QString::number(selectedVertex.front()) + "\n";
        text += "终点为：" + QString::number(selectedVertex.back()) + "\n";
        if (minDist == 1e9)
            text += "最短距离为：NaN\n";
        else {
            if (minDist < 10)  // m
                text += "最短距离为：" + QString::number(minDist * 1000, 'f', 2) + "m\n";
            else  // km
                text += "最短距离为：" + QString::number(minDist, 'f', 2) + "km\n";
        }
    }
    distanceLabel->setText(text);
}

// 至多选中顶点组中的两个顶点
void MainWindow::onVertexToggled(bool checked, ll id) {
    QRadioButton *button = qobject_cast<QRadioButton *>(sender());

    if (checked) {
        if (selectedVertex.size() == 2) {
            // 取消选中第一个顶点
            ll first = selectedVertex.front();
            selectedVertex.pop_front();
            vertex[first]->setChecked(false);
        }
        // 选中当前顶点
        selectedVertex.push_back(id);
    } else {
        // 取消选中当前顶点
        selectedVertex.remove(id);
    }

    // 更新最短路径
    double minDist = 0;
    visitedVertex.clear();
    if (selectedVertex.size() == 2)
        if (algo_tag == Dijkstra)
            minDist = updateShortestPath(dijkstra_tag{});
        else if (algo_tag == AStar)
            minDist = updateShortestPath(AStar_tag{});

    this->updateDistLabel(minDist);
    this->update();
}

void MainWindow::on_algoButton_released() {
    if (algo_tag == Dijkstra) {
        algo_tag = AStar;
        ui->algoButton->setText("AStar");
    } else if (algo_tag == AStar) {
        algo_tag = Dijkstra;
        ui->algoButton->setText("Dijkstra");
    }

    // 更新最短路径
    double minDist = 0;
    if (selectedVertex.size() == 2)
        if (algo_tag == Dijkstra)
            minDist = updateShortestPath(dijkstra_tag{});
        else if (algo_tag == AStar)
            minDist = updateShortestPath(AStar_tag{});
        else
            visitedVertex.clear();

    this->updateDistLabel(minDist);
    this->update();
}

void MainWindow::on_flushButton_released() {
    this->loadOSM();
    this->updateVertexPos();
    this->updateLocationLabel();
    this->updateDistLabel(0);
    this->update();
}
