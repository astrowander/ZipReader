#include "ziptablemodel.h"

#include <QMessageBox>

#include <stdexcept>
#include <memory>

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

void* CreateZipReader()
{
    void* pZipReader = nullptr;
    mz_zip_reader_create(&pZipReader);
    return pZipReader;
}

void* CreateFileStream()
{
    void* pFileStream = nullptr;
    mz_stream_os_create(&pFileStream);
    return pFileStream;
}

ZipTableModel::ZipTableModel(const std::string &pathToZip)
{
    std::shared_ptr<void> pZipReader(CreateZipReader(), [](void* p){ mz_zip_reader_delete(&p); });
    std::shared_ptr<void> pFileStream(CreateFileStream(), [](void* p){ mz_stream_os_delete(&p); });

    if (mz_stream_os_open(pFileStream.get(), pathToZip.c_str(), MZ_OPEN_MODE_READ) != MZ_OK)
    {
        ShowErrorMessage("Unable to open the file");
        return;
    }

    if (mz_zip_open(pZipReader.get(), pFileStream.get(), MZ_OPEN_MODE_READ) != MZ_OK)
    {
        ShowErrorMessage("Unable to read the archive");
        return;
    }

    if (mz_zip_goto_first_entry(pZipReader.get()) != MZ_OK)
    {
        ShowErrorMessage("The archive is empty");
        return;
    }

    do
    {
        if  (mz_zip_entry_is_dir(pZipReader.get()) == MZ_OK)
            continue;

        mz_zip_file* zipFileInfo = nullptr;
        mz_zip_entry_get_info(pZipReader.get(), &zipFileInfo);
        _data.append({QString(zipFileInfo->filename), zipFileInfo->uncompressed_size});
     } while (mz_zip_goto_next_entry(pZipReader.get()) == MZ_OK);
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
        throw std::invalid_argument("index.row");

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
