//
// Created by ccharris on 4/28/20.
//

#ifndef IMAGISSRM_IMGSRWIDGET_H
#define IMAGISSRM_IMGSRWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QtCore/QThread>
#include "ui_imgSRWidget.h"
#include "imgSideWidgetElement.h"

class imgSRWidget : public QWidget
{
Q_OBJECT

public:
    explicit imgSRWidget(QWidget *parent = nullptr);
    ~imgSRWidget();

private:
    QList<imgSideWidgetElement*> sideWEList;
    imgSRDataSetHandler dataSetHandler;
    Ui::imgSRWidget *ui;
    QPushButton *btBallom;
    bool isVisibleSideWidget;
    void mouseMoveEvent(QMouseEvent *event) override;
    QPixmap *success, *fail;
public slots:
    void setSideWidgetState(bool state, int pos);
    void setSideWidgetVisible();
    void deleteSWElement(imgSideWidgetElement* element);
    void setCurrentSWElement(imgSideWidgetElement *element);
    void setPreviewText();
    void sendPending();
    void sendToServer();
    void sendPrint();
    void updatePending();
    void setDataset(DcmDataset *dataset);
    void getSendState(bool state);
    void setSRState(bool state);
signals:
    void showSideWidgetButon(bool, int);
    void isDSR(bool);

};


#endif //IMAGISSRM_IMGSRWIDGET_H
