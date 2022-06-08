#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ZipTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void on_pushButton_clicked();

private:
    std::shared_ptr<Ui::MainWindow> ui;
    std::shared_ptr<ZipTableModel> model;
};
#endif // MAINWINDOW_H
