//
// Created by ccharris on 5/12/20.
//

#ifndef IMAGISSRM_IMGSENDTHREAD_H
#define IMAGISSRM_IMGSENDTHREAD_H


#include <QtCore/QThread>
#include "dcmtk/dcmdata/dctk.h"

class imgSendThread: public QThread {
Q_OBJECT

    DcmDataset *dataset;
    QString in, pro, find, impr;

    void  run() override ;


public:
    void setIn(const QString &in);

    void setPro(const QString &pro);

    void setFind(const QString &find);

    void setImpr(const QString &impr);

public:
    imgSendThread(DcmDataset *dcmDataset){
        dataset = dcmDataset;
    }

    imgSendThread(DcmDataset *dcmDataset, QString in, QString pro, QString find, QString impr){
        this->in = in;
        this->pro = pro;
        this->find = find;
        this->impr = impr;
        dataset = dcmDataset;
    }

signals:
    void resultReady(bool);
    void progress(int);

public slots:
    void setProgress(int i);
};


#endif //IMAGISSRM_IMGSENDTHREAD_H
