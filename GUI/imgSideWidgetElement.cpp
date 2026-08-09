//
// Created by ccharris on 4/29/20.
//

#include "imgSideWidgetElement.h"

#include <QMouseEvent>
#include <QDebug>
#include <QDir>
#include <utility>
#include "CORE/utils.h"
#include "CORE/imgSRCstore.h"
#include "imgPendingFileHandler.h"


imgSideWidgetElement::imgSideWidgetElement(DcmDataset *dataSet, QList<DcmDataset*> asosiatedImages, QWidget *parent) :
        QWidget(parent)
{
    this->asosiatedImages = std::move(asosiatedImages);
    ui = new Ui::imgSideWidgetElement;
    ui->setupUi(this);

    this -> dataset = new DcmDataset(*dataSet);
    if(dataSet != nullptr){
        dataSetHandler.setDataSet(this->dataset);
        ui->lbPatientName->setText(dataSetHandler.GetPatientData(PatientName).c_str());
        this->StudyId = dataSetHandler.GetStudyData().c_str();
        ui->verticalFrame->setToolTip(StudyId);
        ui->lbStudyType->setText(dataSetHandler.getModality().c_str());
        paintImage();
    }
}

imgSideWidgetElement::~imgSideWidgetElement()
{
    delete ui;
    delete dataset;
}

void imgSideWidgetElement::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::RightButton){
        return;
    }

    if(event->type() == QEvent::MouseButtonPress){
        ui->verticalFrame->setFrameShadow(QFrame::Sunken);
        emit clicked(this);
    }

}

QString imgSideWidgetElement::getStudyId() {
    return this->StudyId;
}

const QString &imgSideWidgetElement::getIndication() const {
    return Indication;
}

void imgSideWidgetElement::setIndication(const QString &indication) {
    Indication = indication;
}

const QString &imgSideWidgetElement::getProcedure() const {
    return Procedure;
}

void imgSideWidgetElement::setProcedure(const QString &procedure) {
    Procedure = procedure;
}

const QString &imgSideWidgetElement::getFindinds() const {
    return Findinds;
}

void imgSideWidgetElement::setFindinds(const QString &findinds) {
    Findinds = findinds;
}

const QString &imgSideWidgetElement::getImpression() const {
    return Impression;
}

void imgSideWidgetElement::setImpression(const QString &impression) {
    Impression = impression;
}

DcmDataset *imgSideWidgetElement::getDataset() {
    return dataSetHandler.getDataSet();
}

void imgSideWidgetElement::unmark() {
    ui->verticalFrame->setFrameShadow(QFrame::Raised);
}

void imgSideWidgetElement::mark() {
    ui->verticalFrame->setFrameShadow(QFrame::Sunken);
}

void imgSideWidgetElement::paintImage() {
    DcmFileFormat fileFormat(dataSetHandler.getDataSet());
    if(dataSetHandler.getModality() != "SR"){
        imgSRCstore::makeThubImage("current.png", &fileFormat);
        ui->lbMiniatura->setPixmap(QPixmap("current.png"));
        QFile::remove("current.png");
        return;
    }

    // SR reports have no pixel data of their own: look for the actual image
    // this report is about, in the local cache of the same study (populated
    // by "Buscar en PACS"), and thumbnail that instead of a generic icon.
    QString studyDir = "../AuxFolders/PACS_tmp/" + QString(dataSetHandler.GetStudyData().c_str());
    QDir dir(studyDir);
    if (dir.exists()) {
        dir.setFilter(QDir::Files);
        for (const QString &fileName: dir.entryList()) {
            DcmFileFormat imageFileFormat;
            if (imageFileFormat.loadFile((studyDir + "/" + fileName).toStdString().c_str()).bad())
                continue;

            OFString modality;
            imageFileFormat.getDataset()->findAndGetOFString(DCM_Modality, modality);
            if (modality == "SR")
                continue;

            imgSRCstore::makeThubImage("current.png", &imageFileFormat);
            ui->lbMiniatura->setPixmap(QPixmap("current.png"));
            QFile::remove("current.png");
            return;
        }
    }

    ui->lbMiniatura->setPixmap(QPixmap(":/Resources/reportIcon.png"));
}

void
imgSideWidgetElement::renewPending(const QString &ind, const QString &pro, const QString &fin, const QString &imp) {
    imgPendingFileHandler pendingFileHandler(StudyId);
    Indication = ind;
    Procedure  = pro;
    Findinds   = fin;
    Impression = imp;
    pendingFileHandler.setIndication(Indication);
    pendingFileHandler.setProcedure(Procedure);
    pendingFileHandler.setFindings(Findinds);
    pendingFileHandler.setImpression(Impression);
    pendingFileHandler.setDcmDataset(dataset);
    pendingFileHandler.sendToPending();
    QFile::rename(StudyId + ".dat", "../AuxFolders/Pending/" + StudyId + ".dat");
    QFile::rename(StudyId + ".dat", "../AuxFolders/Pending/" + StudyId + ".dat");
}

QString imgSideWidgetElement::getSWEModality() {
    return QString(ui->lbStudyType->text());
}
