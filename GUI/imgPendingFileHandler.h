//
// Created by ccharris on 5/6/20.
//

#ifndef IMAGISSRM_IMGPENDINGFILEHANDLER_H
#define IMAGISSRM_IMGPENDINGFILEHANDLER_H


#include <QtCore/QString>
#include <QDataStream>
#include "dcmtk/dcmdata/dctk.h"

class imgPendingFileHandler;

QDataStream &operator<<(QDataStream &out, const imgPendingFileHandler &);
QDataStream &operator>>(QDataStream &in, imgPendingFileHandler &);

class imgPendingFileHandler{

private:
    DcmDataset *dcmDataset;
public:
    DcmDataset *getDcmDataset() const;

    void setDcmDataset(DcmDataset *dcmDataset);

    const QString &getIndication() const;

    void setIndication(const QString &indication);

    const QString &getProcedure() const;

    void setProcedure(const QString &procedure);

    const QString &getFindings() const;

    void setFindings(const QString &findings);

    const QString &getImpression() const;

    void setImpression(const QString &impresion);

private:
    QString Indication, Procedure, Findings, Impression, dataSetFileName;
public:
    virtual ~imgPendingFileHandler();

    imgPendingFileHandler();

    imgPendingFileHandler(const QString &dataSetFileName);

    const QString &getDataSetFileName() const;

    void setDataSetFileName(const QString &dataSetFileName);

    bool loadDataSetFile();

    bool saveDataSetFile() const;

    bool sendToPending();

    bool loadFromPending(const QString &path);
};


#endif //IMAGISSRM_IMGPENDINGFILEHANDLER_H
