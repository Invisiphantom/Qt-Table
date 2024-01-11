#include "header/mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), vertexGroup(this) {
    ui->setupUi(this);

    this->scaleSlider = ui->scaleSlider;
    // 设置缩放滚动条的范围
    scaleSlider->setMinimum(-3);
    scaleSlider->setMaximum(4);
    // 加载滚动条的css样式
    QFile scaleSliderStyle(":/res/css/scaleSlider.css");
    scaleSliderStyle.open(QFile::ReadOnly);
    scaleSlider->setStyleSheet(scaleSliderStyle.readAll());
    scaleSliderStyle.close();

    this->distanceLabel = ui->distanceLabel;
    this->locationLabel = ui->locationLabel;

    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::importFile);
    connect(ui->actionSave_File, &QAction::triggered, this, &MainWindow::exportFile);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::helpShow);

    // 设置地图背景颜色
    this->setPalette(QPalette(QColor(242, 239, 233)));

    // 加载地图数据
    this->loadOSM();
    this->updateVertexPos();
    this->updateLocationLabel();
    this->updateDistLabel(0);
}

MainWindow::~MainWindow() { delete ui; }

