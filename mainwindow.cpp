#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

#include <stdexcept>

#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

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

}


ZipTableModel::ZipTableModel(const std::string &pathToZip)
{
    void* zipReader = nullptr;
    void *file_stream = nullptr;
    const char *path = pathToZip.c_str();
    int32_t* err = new int(0);
    uint64_t* numEntries = new uint64_t(0);
    mz_zip_reader_create(&zipReader);
    mz_stream_os_create(&file_stream);
    printf("before file opening\n");
    if (mz_stream_os_open(file_stream, path, MZ_OPEN_MODE_READ) == MZ_OK)
    {
    /*auto err = mz_stream_os_open(file_stream, path, MZ_OPEN_MODE_READ);/*/
        printf("after file opening\n");
        if (mz_zip_open(zipReader, file_stream, MZ_OPEN_MODE_READ) == MZ_OK)
        {                
            printf("Zip reader was opened %s\n", path);
                if (mz_zip_goto_first_entry(zipReader) == MZ_OK)
                {
                    mz_zip_get_number_entry(zipReader, numEntries);
                    *err = mz_zip_goto_first_entry(zipReader);

                    while (*err == MZ_OK)
                    {
                        mz_zip_entry_read_open(zipReader, 0, "");
                        if  (mz_zip_entry_is_dir(zipReader) != MZ_OK)
                        {
                            mz_zip_file* zipFileInfo = nullptr;
                            mz_zip_entry_get_info(zipReader, &zipFileInfo);
                            _data.append({QString(zipFileInfo->filename), zipFileInfo->uncompressed_size});
                        }
                        *err = mz_zip_goto_next_entry(zipReader);
                    }
                }
                //printf("%d\n", (int)entryCount);

                mz_zip_reader_close(zipReader);
         }
            else
            {
                printf("Zip reader was not opened %s\n", path);
            }
        }

        mz_stream_os_delete(&file_stream);
        mz_zip_reader_delete(&zipReader);
}

int ZipTableModel::rowCount(const QModelIndex &parent) const
{
    return _data.size();
}

int ZipTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant ZipTableModel::data(const QModelIndex &index, int role) const
{
    if (index.column() < 0 || index.column() >= 2)
        throw std::invalid_argument("index.column");

    if (index.row() < 0 || index.column() >= _data.size())
        throw std::invalid_argument("index.column");

    if (index.column() == 0)
        return _data[index.row()].first;

    return _data[index.row()].second;
}
