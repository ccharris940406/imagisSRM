//
// Created by ccharris on 5/9/20.
//

//#define xml2dsr "/home/ccharris/Projects/CBM/libs/DCMTK/3.6.5/install/bin/xml2dsr "

#include <QtCore/QFile>
#include <QDate>
#include <QDebug>
#include <QProgressBar>
#include <utility>
#include "imgXMLMaker.h"
#include "./CORE/utils.h"
#include "./CORE/imgSRCstore.h"
#include "dcmtk/dcmdata/dcuid.h"

#define peer "127.0.0.1"
#define port 5020

imgXMLMaker::imgXMLMaker(DcmDataset *dataset) {
    dataSetHandler.setDataSet(dataset);
    doc = new QDomDocument();
    QFile file("../AuxFolders/Templates/template.xml");
    file.open(QIODevice::ReadOnly);
    doc->setContent(&file);
    file.close();
}

bool imgXMLMaker::makeXML(QString indication, QString procedure, QString findings, QString impression) {

    this->indication = std::move(indication);
    this->procedure  = std::move(procedure);
    this->findings   = std::move(findings);
    this->impression = std::move(impression);

    QDomElement referring, patient, study, series, instance, evidence,document;

    QDomElement first = doc->firstChildElement();

    referring = first.firstChildElement("referringphysician");
    patient   = first.firstChildElement("patient");
    study     = first.firstChildElement("study");
    series    = first.firstChildElement("series");
    instance  = first.firstChildElement("instance");
    evidence  = first.firstChildElement("evidence");
    document  = first.firstChildElement("document");

    int i = 0;
    setReferringphysician(referring);
    emit progress(++i);
    setPatient(patient);
    emit progress(++i);
    setStudy(study);
    emit progress(++i);
    setSeries(series);
    emit progress(++i);
    setInstance(instance);
    emit progress(++i);
    setEvidence(evidence);
    emit progress(++i);
    setDocument(document);
    emit progress(++i);

    QFile file("../AuxFolders/" + QString(dataSetHandler.GetStudyData().c_str()) + ".xml");
    file.open(QIODevice::WriteOnly);
    emit progress(++i);
    QTextStream outXML(&file);
    outXML << doc->toString();
    emit progress(++i);
    file.close();

    std::string flnstr = "../AuxFolders/" + dataSetHandler.GetStudyData() +".dcm";
    imgSRCstore::xml2dsr(file.fileName().toStdString(), flnstr);
    file.remove();
    emit progress(++i);

    std::string fln =  "../AuxFolders/" + dataSetHandler.GetStudyData() + ".dcm";
    std::string host = peer;
    unsigned short myPort = port;
    int ret = imgSRCstore::dcmSend(host, myPort, fln);



//    QFile::remove("../AuxFolders/" + QString(dataSetHandler.GetStudyData().c_str()) + ".dcm");
    if(!ret){
    QFile::remove("../AuxFolders/Pending/" + QString(dataSetHandler.GetStudyData().c_str()) + ".dst");
    QFile::remove("../AuxFolders/Pending/" + QString(dataSetHandler.GetStudyData().c_str()) + ".dat");
    }
    emit progress(++i);
    return !ret;
}

bool imgXMLMaker::sendToServer() {
    return false;
}

void imgXMLMaker::setReferringphysician(QDomElement &element) {

}

void imgXMLMaker::setPatient(QDomElement &element) {

    QString first = "", last = "", full;

    full = dataSetHandler.GetPatientData(PatientName).c_str();

    bool blast = 0;

    foreach(auto x, full){
        if(x == ' ' && !blast){
            blast = true;
        }
        else if(!blast){
            last += x;
        } else{
            first += x;
        }
    }

    element.firstChildElement("id")
    .appendChild(doc->createTextNode(
            dataSetHandler.GetPatientData(PatientID).c_str()));
    element.firstChildElement("name").firstChild().appendChild(
            doc->createTextNode(first));
    element.firstChildElement("name").firstChild().nextSibling().appendChild(
            doc->createTextNode(last));

    const char * buffer = nullptr;
    dataSetHandler.getDataSet()->findAndGetString(DCM_PatientBirthDate, buffer);
    if(buffer != "" && buffer != nullptr){
        QString date = "";
        date += buffer[0];date += buffer[1];date += buffer[2];date += buffer[3];date += '-';
        date += buffer[4];date += buffer[5];date += '-';date += buffer[6];date += buffer[7];
        element.firstChildElement("birthday").firstChild().appendChild(doc->createTextNode(date));
    }

    element.firstChildElement("sex").appendChild(
            doc->createTextNode(dataSetHandler.GetPatientData(PatientSex).c_str()));
}

void imgXMLMaker::setStudy(QDomElement &element) {
    element.setAttribute("uid", dataSetHandler.GetStudyData().c_str());
}

void imgXMLMaker::setSeries(QDomElement &element) {
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    element.setAttribute("uid", uid);
}

void imgXMLMaker::setInstance(QDomElement &element) {
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
    element.setAttribute("uid", uid);
    element.firstChildElement("creation").firstChild().appendChild(
            doc->createTextNode(QDate::currentDate().toString("yyyy-MM-dd")));
    element.firstChildElement("creation").firstChild().nextSibling().appendChild(
            doc->createTextNode(QTime::currentTime().toString("hh:mm:ss")));

}

void imgXMLMaker::setEvidence(QDomElement &element) {
    element.firstChildElement().setAttribute("uid", dataSetHandler.GetStudyData().c_str());
    element.firstChildElement().firstChildElement().setAttribute(
            "uid", dataSetHandler.GetSeriesData().c_str());
    element.firstChildElement().firstChildElement().firstChildElement()
        .firstChildElement().setAttribute("uid", dataSetHandler.getSopclassUID().c_str());
    element.firstChildElement().firstChildElement().firstChildElement()
            .firstChildElement().appendChild(doc->createTextNode(
                    dcmFindNameOfUID(dataSetHandler.getSopclassUID().c_str(),"")
                    ));

    element.firstChildElement().firstChildElement().firstChildElement()
        .firstChildElement().nextSiblingElement().setAttribute("uid", dataSetHandler.getInstanceUID().c_str());
}

void imgXMLMaker::setDocument(QDomElement &element) {

    element.firstChildElement().nextSiblingElement().nextSiblingElement()
        .firstChildElement().appendChild(doc->createTextNode(QDate::currentDate().toString(
                "yyyy-MM-dd"
                )));

    element.firstChildElement().nextSiblingElement().nextSiblingElement()
            .firstChildElement().nextSiblingElement()
            .appendChild(doc->createTextNode(QTime::currentTime().toString(
                    "hh:mm:ss"
                    )));

    QDomElement eleIndication, eleProcedure, eleFindings, eleImpression;

    element.firstChildElement().nextSiblingElement().nextSiblingElement()
            .firstChildElement().nextSiblingElement().nextSiblingElement().firstChildElement()
            .firstChildElement().nextSiblingElement().nextSiblingElement()
            .appendChild(doc->createTextNode(
                    QString((dataSetHandler.getModality() +
                             " Structured Report").c_str())
                    ));

    eleIndication = element.firstChildElement().nextSiblingElement().nextSiblingElement()
            .firstChildElement().nextSiblingElement().nextSiblingElement()
            .firstChildElement().nextSiblingElement().nextSiblingElement().nextSiblingElement();

    eleProcedure = eleIndication.nextSiblingElement();

    eleFindings = eleProcedure.nextSiblingElement();

    eleImpression = eleFindings.nextSiblingElement();

    setTextValue(eleIndication, indication);
    setTextValue(eleProcedure, procedure);
    setTextValue(eleFindings, findings);
    setTextValue(eleImpression, impression);

}

void imgXMLMaker::setTextValue(QDomElement &element, const QString &text) {
    element.firstChildElement().nextSiblingElement()
        .nextSiblingElement().appendChild(doc->createTextNode(text));
}
