//
// Dialog to search (C-FIND) studies on a remote PACS and pick one to retrieve.
//

#include "imgQueryDialog.h"
#include "CORE/utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QAbstractItemView>

imgQueryDialog::imgQueryDialog(QWidget *parent, const QString &lockedPatientId)
        : QDialog(parent), lockedPatientId(lockedPatientId) {
    setWindowTitle(lockedPatientId.isEmpty() ? "Buscar estudio en PACS" : "Estudios del paciente en PACS");
    resize(650, 400);

    auto *mainLayout = new QVBoxLayout(this);

    auto *searchLayout = new QHBoxLayout();
    if (lockedPatientId.isEmpty()) {
        searchLayout->addWidget(new QLabel("Paciente:"));
        leFilter = new QLineEdit("*", this);
        searchLayout->addWidget(leFilter);
    } else {
        searchLayout->addWidget(new QLabel("PatientID: " + lockedPatientId));
        leFilter = nullptr;
    }
    btSearch = new QPushButton("Buscar", this);
    searchLayout->addWidget(btSearch);
    mainLayout->addLayout(searchLayout);

    table = new QTableWidget(0, 5, this);
    table->setHorizontalHeaderLabels({"Paciente", "ID", "Fecha", "Descripcion", "Modalidad"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(table);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    btLoad = new QPushButton("Cargar", this);
    btLoad->setEnabled(false);
    btCancel = new QPushButton("Cancelar", this);
    buttonLayout->addWidget(btLoad);
    buttonLayout->addWidget(btCancel);
    mainLayout->addLayout(buttonLayout);

    connect(btSearch, SIGNAL(clicked()), this, SLOT(doSearch()));
    connect(btLoad, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(table, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));

    doSearch();
}

void imgQueryDialog::doSearch() {
    table->setRowCount(0);
    btLoad->setEnabled(false);

    if (lockedPatientId.isEmpty()) {
        results = imgSRQuery::findStudies(PEERHOST, PEERPORT, leFilter->text().toStdString());
    } else {
        results = imgSRQuery::findStudies(PEERHOST, PEERPORT, "*", lockedPatientId.toStdString());
    }

    if (results.empty()) {
        QMessageBox::information(this, "Buscar estudio",
                                  "No se encontraron estudios (o no se pudo conectar al PACS).");
        return;
    }

    table->setRowCount(static_cast<int>(results.size()));
    for (size_t i = 0; i < results.size(); ++i) {
        table->setItem((int) i, 0, new QTableWidgetItem(QString::fromStdString(results[i].patientName)));
        table->setItem((int) i, 1, new QTableWidgetItem(QString::fromStdString(results[i].patientId)));
        table->setItem((int) i, 2, new QTableWidgetItem(QString::fromStdString(results[i].studyDate)));
        table->setItem((int) i, 3, new QTableWidgetItem(QString::fromStdString(results[i].studyDescription)));
        table->setItem((int) i, 4, new QTableWidgetItem(QString::fromStdString(results[i].modality)));
    }
}

void imgQueryDialog::onSelectionChanged() {
    btLoad->setEnabled(!table->selectionModel()->selectedRows().isEmpty());
}

std::string imgQueryDialog::selectedStudyInstanceUID() const {
    QModelIndexList rows = table->selectionModel()->selectedRows();
    if (rows.isEmpty())
        return "";
    int row = rows.first().row();
    if (row < 0 || row >= static_cast<int>(results.size()))
        return "";
    return results[row].studyInstanceUID;
}
