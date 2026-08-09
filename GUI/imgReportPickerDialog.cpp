//
// Dialog to browse the SR report(s) already sent for a study (if any) and
// pick one to continue editing, or start a brand-new report from scratch.
//

#include "imgReportPickerDialog.h"
#include "dcmtk/dcmdata/dctk.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QAbstractItemView>

imgReportPickerDialog::imgReportPickerDialog(const QStringList &filePaths, const QString &imageFallbackPath,
                                              QWidget *parent)
        : QDialog(parent), imageFallbackPath(imageFallbackPath) {
    setWindowTitle("Reportes de este estudio");
    resize(450, 350);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Elige un reporte existente para editarlo, o crea uno nuevo:", this));

    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(list);

    auto *newItem = new QListWidgetItem("-- Crear reporte nuevo --");
    newItem->setData(Qt::UserRole, "");
    list->addItem(newItem);

    for (const QString &path: filePaths) {
        DcmFileFormat fileFormat;
        if (fileFormat.loadFile(path.toStdString().c_str()).bad())
            continue;

        OFString modality;
        fileFormat.getDataset()->findAndGetOFString(DCM_Modality, modality);
        if (modality != "SR")
            continue;

        OFString contentDate, contentTime;
        fileFormat.getDataset()->findAndGetOFString(DCM_ContentDate, contentDate);
        fileFormat.getDataset()->findAndGetOFString(DCM_ContentTime, contentTime);

        QString label = QString("Reporte del %1 %2").arg(contentDate.c_str(), contentTime.c_str());
        auto *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, path);
        list->addItem(item);
    }

    list->setCurrentRow(0);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto *btOpen = new QPushButton("Abrir", this);
    auto *btCancel = new QPushButton("Cancelar", this);
    buttonLayout->addWidget(btOpen);
    buttonLayout->addWidget(btCancel);
    mainLayout->addLayout(buttonLayout);

    connect(btOpen, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem * )), this, SLOT(accept()));
}

QString imgReportPickerDialog::selectedPath() const {
    QListWidgetItem *item = list->currentItem();
    if (item == nullptr)
        return imageFallbackPath;
    QString path = item->data(Qt::UserRole).toString();
    return path.isEmpty() ? imageFallbackPath : path;
}
