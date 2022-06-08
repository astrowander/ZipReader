#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ziptablemodel.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr)
{
    ui->setupUi(this);    
}

MainWindow::~MainWindow()
{
    if (model)
        delete model;

    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    auto path = QFileDialog::getOpenFileName(this, "Open zip file");
    model = new ZipTableModel(path.toStdString());
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0, 500);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
