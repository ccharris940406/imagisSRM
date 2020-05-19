//
// Created by ccharris on 5/9/20.
//

#ifndef IMAGISSRM_IMGXMLMAKER_H
#define IMAGISSRM_IMGXMLMAKER_H

#include <QObject>
#include <QDomDocument>
#include <CORE/imgSRDataSetHandler.h>

class imgXMLMaker: public QObject {
Q_OBJECT

    QDomDocument*doc;
    imgSRDataSetHandler dataSetHandler;

    QString indication, procedure, findings, impression;

    void setReferringphysician(QDomElement &element);
    void setPatient(QDomElement &element);
    void setStudy(QDomElement &element);
    void setSeries(QDomElement &element);
    void setInstance(QDomElement &element);
    void setEvidence(QDomElement &element);
    void setDocument(QDomElement &element);

    void setTextValue(QDomElement &element, const QString &text);


public:
    explicit imgXMLMaker(DcmDataset *dataset);
    bool makeXML(QString indication, QString procedure, QString findings, QString impression);
    bool sendToServer();
signals:
    void progress(int);

};


#endif //IMAGISSRM_IMGXMLMAKER_H
