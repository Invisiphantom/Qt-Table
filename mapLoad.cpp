#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "header/mainwindow.h"
#include "tinyxml/tinyxml.h"

// 从资源管理器中选择导入文件，选择要求是.osm后缀的文件，替换掉当前目录下的map.osm
void MainWindow::importFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open OSM File", "./", "OSM Files (*.osm)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        TiXmlDocument tinyXmlDoc;
        tinyXmlDoc.Parse(content.toStdString().c_str());
        tinyXmlDoc.SaveFile("map.osm");
        this->loadOSM();
        this->updateVertexPos();
        this->updateLocationLabel();
        this->updateDistLabel(0);
    }
}

// 将当前目录下的map.osm文件导出到资源管理器中，文件名为map.osm
void MainWindow::exportFile() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save OSM File", "./", "OSM Files (*.osm)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 直接复制过去
        QFile mapFile("map.osm");
        mapFile.open(QIODevice::ReadOnly | QIODevice::Text);
        out << mapFile.readAll();
        mapFile.close();
        file.close();
    }
}

// 通过菜单栏的按钮在Qt中打开帮助界面
// 帮助界面的内容为:/res/Help.md中的内容
void MainWindow::helpShow() {
    QFile file(":/res/Help.md");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = file.readAll();
    file.close();
    QMessageBox::information(this, "Help", content);
}


void MainWindow::loadOSM() {
    TiXmlDocument tinyXmlDoc("map.osm");
    tinyXmlDoc.LoadFile();

    TiXmlElement *root = tinyXmlDoc.RootElement();

    TiXmlElement *boundsElement = root->FirstChildElement("bounds");
    minLon = stold(boundsElement->Attribute("minlon"));
    maxLon = stold(boundsElement->Attribute("maxlon"));
    minLat = stold(boundsElement->Attribute("minlat"));
    maxLat = stold(boundsElement->Attribute("maxlat"));

    nodes.clear();
    vertex.clear();
    selectedVertex.clear();
    visitedVertex.clear();
    for (auto button : vertexGroup.buttons()) {
        vertexGroup.removeButton(button);
        delete button;
    }

    vertexGroup.setExclusive(false);
    TiXmlElement *nodeElement = root->FirstChildElement("node");
    for (; nodeElement; nodeElement = nodeElement->NextSiblingElement("node")) {
        Node node;
        node.id = stoll(nodeElement->Attribute("id"));
        node.lon = stold(nodeElement->Attribute("lon"));
        node.lat = stold(nodeElement->Attribute("lat"));
        nodes[node.id] = node;
        // 创建顶点组件
        QRadioButton *button = new QRadioButton(this);
        button->show();
        connect(button, &QRadioButton::toggled, this, [this, node](bool checked) { this->onVertexToggled(checked, node.id); });
        vertexGroup.addButton(button, node.id);
        vertex[node.id] = button;
    }

    adj.clear();
    ways.clear();
    TiXmlElement *wayElement = root->FirstChildElement("way");
    for (; wayElement; wayElement = wayElement->NextSiblingElement("way")) {
        Way way;
        way.id = stoll(wayElement->Attribute("id"));
        // 获取标签
        TiXmlElement *childTag = wayElement->FirstChildElement("tag");
        for (; childTag; childTag = childTag->NextSiblingElement("tag")) {
            string name = childTag->Attribute("k");
            string value = childTag->Attribute("v");
            way.tags[name] = value;
        }
        TiXmlElement *childNode = wayElement->FirstChildElement("nd");
        // 如果不是道路，则删除这些顶点
        if (way.tags.find("highway") == way.tags.end()) {
            for (; childNode; childNode = childNode->NextSiblingElement("nd")) {
                ll ref = stoll(childNode->Attribute("ref"));
                // 删除这些顶点
                if (vertex.find(ref) != vertex.end()) {
                    vertexGroup.removeButton(vertex[ref]);
                    delete vertex[ref];
                    vertex.erase(ref);
                    nodes.erase(ref);
                }
            }
            continue;
        } 
        // 如果是道路，则添加这些顶点
        for (; childNode; childNode = childNode->NextSiblingElement("nd")) {
            ll ref = stoll(childNode->Attribute("ref"));
            way.nodes.push_back(nodes[ref]);
            // 构建顶点之间的邻接表
            if (childNode->NextSiblingElement("nd")) {
                ll nextRef = stoll(childNode->NextSiblingElement("nd")->Attribute("ref"));
                adj[ref].push_back(nextRef);
                adj[nextRef].push_back(ref);
            }
        }

        ways[way.id] = way;
    }
    this->updateVertexPos();
}