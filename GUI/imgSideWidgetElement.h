//
// Created by ccharris on 4/29/20.
//

#ifndef IMAGISSRM_IMGSIDEWIDGETELEMENT_H
#define IMAGISSRM_IMGSIDEWIDGETELEMENT_H


#include <QWidget>
#include <ui_imgSideWidgetElement.h>
#include <CORE/imgSRDataSetHandler.h>
#include "dcmtk/dcmdata/dctk.h"


class imgSideWidgetElement : public QWidget
{
Q_OBJECT

public:
    explicit imgSideWidgetElement(DcmDataset *dataSet = nullptr, QList<DcmDataset*> asosiatedImages = {}, QWidget *parent = nullptr);
    imgSideWidgetElement(const imgSideWidgetElement&);
    QString getStudyId();
    void updateView();
    ~imgSideWidgetElement() override;
private:
    QString Indication, Procedure, Findinds, Impression;
    void paintImage();
    DcmDataset *dataset;
public:
    void renewPending(const QString &ind, const QString &pro, const QString &fin, const QString &imp);

    const QString &getIndication() const;

    void setIndication(const QString &indication);

    const QString &getProcedure() const;

    void setProcedure(const QString &procedure);

    const QString &getFindinds() const;

    void setFindinds(const QString &findinds);

    const QString &getImpression() const;

    void setImpression(const QString &impression);

    void unmark();

    void mark();

    DcmDataset * getDataset();

    QString getSWEModality();

private:
    imgSRDataSetHandler dataSetHandler;

private:
    QString StudyId;
    QList<DcmDataset*> asosiatedImages;
    Ui::imgSideWidgetElement *ui;
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void clicked(imgSideWidgetElement *);
};


#endif //IMAGISSRM_IMGSIDEWIDGETELEMENT_H
