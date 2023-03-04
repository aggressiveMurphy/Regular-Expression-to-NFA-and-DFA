#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{//点击“开始”按钮，关闭主窗口，切换到子窗口
    this->close();
    SubWidget *s=new SubWidget();
    s->show();
}
