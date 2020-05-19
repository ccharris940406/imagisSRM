//
// Created by ccharris on 5/6/20.
//

#include "imgPendingFileHandler.h"
#include <QDebug>
#include <QFile>

imgPendingFileHandler::imgPendingFileHandler(const QString &dataSetFileName)
: dataSetFileName(dataSetFileName) {}

imgPendingFileHandler::imgPendingFileHandler() {}

const QString &imgPendingFileHandler::getDataSetFileName() const {
    return dataSetFileName;
}

void imgPendingFileHandler::setDataSetFileName(const QString &dataSetFileName) {
    imgPendingFileHandler::dataSetFileName = dataSetFileName;
}

imgPendingFileHandler::~imgPendingFileHandler() {

}

DcmDataset *imgPendingFileHandler::getDcmDataset() const {
    return dcmDataset;
}

void imgPendingFileHandler::setDcmDataset(DcmDataset *dcmDataset) {
    imgPendingFileHandler::dcmDataset = new DcmDataset(*dcmDataset);
}

const QString &imgPendingFileHandler::getIndication() const {
    return Indication;
}

void imgPendingFileHandler::setIndication(const QString &indication) {
    Indication = indication;
}

const QString &imgPendingFileHandler::getProcedure() const {
    return Procedure;
}

void imgPendingFileHandler::setProcedure(const QString &procedure) {
    Procedure = procedure;
}

const QString &imgPendingFileHandler::getFindings() const {
    return Findings;
}

void imgPendingFileHandler::setFindings(const QString &findings) {
    Findings = findings;
}

const QString &imgPendingFileHandler::getImpression() const {
    return Impression;
}

void imgPendingFileHandler::setImpression(const QString &impresion) {
    Impression = impresion;
}

bool imgPendingFileHandler::loadDataSetFile() {
    DcmFileFormat fileFormat;
    OFCondition cond = fileFormat.loadFile(QString(this->dataSetFileName + ".dst").toStdString().c_str());
    fileFormat.loadAllDataIntoMemory();
    this->dcmDataset = new DcmDataset(*fileFormat.getDataset());
    return cond.good();

}

bool imgPendingFileHandler::saveDataSetFile() const {
    DcmDataset *dcmDataset1 = new DcmDataset(*getDcmDataset());
    DcmFileFormat fileFormat(dcmDataset1);
    return fileFormat.saveFile(QString(this->dataSetFileName + ".dst").toStdString().c_str()).good();
}

bool imgPendingFileHandler::sendToPending() {

    QFile file(this->getDataSetFileName()+".dat");

    if (!file.open(QIODevice::WriteOnly)){
        return false;
    }
    QDataStream stream(&file);
    stream << *this;
    file.close();
    return true;
}

bool imgPendingFileHandler::loadFromPending(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){
        return false;
    }
    QDataStream stream(&file);
    stream >> *this;
    file.close();

    return true;
}

QDataStream &operator<<(QDataStream &out, const imgPendingFileHandler &fileHandler) {
    out << fileHandler.getIndication() << fileHandler.getProcedure()
        << fileHandler.getFindings() << fileHandler.getImpression() << fileHandler.getDataSetFileName();
    fileHandler.saveDataSetFile();
    return out;
}

QDataStream &operator>>(QDataStream &in, imgPendingFileHandler &fileHandler) {
    QString dataSetFileName, Indication, Procedure, Findings, Impression;
    in >> Indication >> Procedure >> Findings >> Impression >> dataSetFileName;
    fileHandler.setImpression(Impression);
    fileHandler.setFindings(Findings);
    fileHandler.setIndication(Indication);
    fileHandler.setProcedure(Procedure);
    fileHandler.setDataSetFileName(dataSetFileName);
    fileHandler.loadDataSetFile();
    return in;
}
