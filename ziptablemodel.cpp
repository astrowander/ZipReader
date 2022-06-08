#include "ziptablemodel.h"

#include <QMessageBox>
#include <stdexcept>

#include <mz.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

static const QString columnNames[2] = {"Path to file", "Size"};

void ShowErrorMessage(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

ZipTableModel::ZipTableModel(const std::string &pathToZip)
{
    void* zipReader = nullptr;
    void *file_stream = nullptr;
    int32_t* err = new int(0);

    mz_zip_reader_create(&zipReader);
    mz_stream_os_create(&file_stream);

    if (mz_stream_os_open(file_stream, pathToZip.c_str(), MZ_OPEN_MODE_READ) != MZ_OK)
    {
        ShowErrorMessage("Unable to open the file");
        return;
    }

    if (mz_zip_open(zipReader, file_stream, MZ_OPEN_MODE_READ) != MZ_OK)
    {
        ShowErrorMessage("Unable to read the archive");
        return;
    }

    *err = mz_zip_goto_first_entry(zipReader);

    if (*err != MZ_OK)
    {
        ShowErrorMessage("The archive is empty");
        return;
    }

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

     mz_zip_reader_close(zipReader);
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

    if (index.row() < 0 || index.row() >= _data.size())
        throw std::invalid_argument("index.column");

    switch (role)
    {
    case Qt::DisplayRole:
        return (index.column() == 0) ? QVariant(_data[index.row()].first) : QVariant(_data[index.row()].second);
    case Qt::TextAlignmentRole:
         return (index.column() == 0) ? QVariant(Qt::AlignLeft | Qt::AlignVCenter) : QVariant(Qt::AlignRight | Qt::AlignVCenter);
    default:
         return QVariant();
    }
}

QVariant ZipTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return orientation == Qt::Orientation::Horizontal ? QVariant(columnNames[section]) : QVariant(section + 1);
    default:
        return QVariant();
    }

}
