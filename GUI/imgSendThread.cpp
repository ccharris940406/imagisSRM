//
// Created by ccharris on 5/12/20.
//

#include "imgSendThread.h"
#include "imgXMLMaker.h"

void imgSendThread::run() {
    auto state = true;
    imgXMLMaker *xmlMaker = new imgXMLMaker(dataset);
    connect(xmlMaker, SIGNAL(progress(int)), this, SLOT(setProgress(int)));
    state = xmlMaker->makeXML(in,
                                   pro,
                                   find,
                                   impr);
    emit resultReady(state);
}

void imgSendThread::setIn(const QString &in) {
    imgSendThread::in = in;
}

void imgSendThread::setPro(const QString &pro) {
    imgSendThread::pro = pro;
}

void imgSendThread::setFind(const QString &find) {
    imgSendThread::find = find;
}

void imgSendThread::setImpr(const QString &impr) {
    imgSendThread::impr = impr;
}

void imgSendThread::setProgress(int i) {
    emit progress(i);
}
