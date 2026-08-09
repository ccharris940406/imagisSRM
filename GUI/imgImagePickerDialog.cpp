//
// Dialog to pick one image instance (thumbnail) from a set of already-retrieved
// DICOM files belonging to the same study, to attach as a "slide" to the report.
//

#include "imgImagePickerDialog.h"
#include "CORE/imgSRCstore.h"
#include "dcmtk/dcmdata/dctk.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>

imgImagePickerDialog::imgImagePickerDialog(const QStringList &candidatePaths, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Elegir imagen del estudio");
    resize(500, 420);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Cortes disponibles en este estudio:", this));

    list = new QListWidget(this);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(120, 120));
    list->setResizeMode(QListView::Adjust);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(list);

    int counter = 0;
    for (const QString &path: candidatePaths) {
        DcmFileFormat fileFormat;
        if (fileFormat.loadFile(path.toStdString().c_str()).bad())
            continue;

        OFString modality;
        fileFormat.getDataset()->findAndGetOFString(DCM_Modality, modality);
        if (modality == "SR")
            continue;

        QString thumbPath = QDir::tempPath() + QString("/imgisSRM_pick_%1.png").arg(counter++);
        QPixmap pixmap;
        if (imgSRCstore::makeThubImage(thumbPath.toStdString().c_str(), &fileFormat) == 0) {
            pixmap.load(thumbPath);
            QFile::remove(thumbPath);
        }

        OFString instanceNumber;
        fileFormat.getDataset()->findAndGetOFString(DCM_InstanceNumber, instanceNumber);
        QString label = instanceNumber.empty() ? QFileInfo(path).fileName()
                                                 : "Corte " + QString(instanceNumber.c_str());

        auto *item = new QListWidgetItem(QIcon(pixmap), label);
        item->setData(Qt::UserRole, path);
        list->addItem(item);
        filePaths.push_back(path);
    }

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto *btAdd = new QPushButton("Agregar", this);
    auto *btCancel = new QPushButton("Cancelar", this);
    buttonLayout->addWidget(btAdd);
    buttonLayout->addWidget(btCancel);
    mainLayout->addLayout(buttonLayout);

    connect(btAdd, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem * )), this, SLOT(accept()));
}

QString imgImagePickerDialog::selectedFilePath() const {
    QListWidgetItem *item = list->currentItem();
    if (item == nullptr)
        return "";
    return item->data(Qt::UserRole).toString();
}
