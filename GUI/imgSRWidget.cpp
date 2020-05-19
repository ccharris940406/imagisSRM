//
// Created by ccharris on 4/28/20.
//

#include "imgSRWidget.h"
#include "imgSideWidgetElement.h"
#include "imgPendingFileHandler.h"
#include "imgSendThread.h"
#include <QDir>
#include <QDate>
#include "../CORE/utils.h"
#include <QPainter>
#include <QtConcurrent>
#include <QSizePolicy>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <CORE/imgSRCstore.h>
#include <QtXml/QDomDocument>

imgSRWidget::imgSRWidget(QWidget *parent) :
        QWidget(parent)
{
    isVisibleSideWidget = false;
    ui = new Ui::imgSRWidget;
    ui->setupUi(this);
    ui->sideWidget->hide();
    this->setMouseTracking(true);
    btBallom = new QPushButton("", this);
    btBallom->hide();
    btBallom->setMaximumSize(30,30);
    btBallom->setIcon(QIcon(QPixmap(":/Resources/show.png")));
    success = new QPixmap(":Resources/success.png");
    fail    = new QPixmap(":Resources/fail.png");
    btBallom->setMouseTracking(true);
    ui->progressBar->setMaximum(11);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(11);
    ui->progressBar->hide();
    ui->hsProgreess->changeSize(0,0,QSizePolicy::Expanding);

    connect(this, SIGNAL(showSideWidgetButon(bool, int)), this, SLOT(setSideWidgetState(bool, int)));
    connect(btBallom, SIGNAL(clicked()), this, SLOT(setSideWidgetVisible()));
    connect(ui->btPending, SIGNAL(clicked()), this, SLOT(sendPending()));
    connect(ui->btSend, SIGNAL(clicked()), this, SLOT(sendToServer()));
    connect(ui->btPrint, SIGNAL(clicked()), this, SLOT(sendPrint()));
    connect(ui->pteIndication, SIGNAL(textChanged()), this, SLOT(setPreviewText()));
    connect(ui->pteProcedure, SIGNAL(textChanged()), this, SLOT(setPreviewText()));
    connect(ui->pteFindings, SIGNAL(textChanged()), this, SLOT(setPreviewText()));
    connect(ui->pteImpresionDiag, SIGNAL(textChanged()), this, SLOT(setPreviewText()));
    connect(this, SIGNAL(isDSR(bool)), this, SLOT(setSRState(bool)));

    ui->tePreview->setCursorWidth(0);


    //é
    DcmFileFormat *fileFormat = new  DcmFileFormat();
//    fileFormat->loadFile("/home/ccharris/Downloads/srdoc103/image10.dcm");
//    fileFormat->loadFile("../AuxFolders/2.16.840.1.113662.2.1.53544936282433.12345.336.16650.dcm");
    fileFormat->loadFile("../AuxFolders/2.16.840.1.113662.4.8796818069641.798806497.93296077602350.10.dcm");
    //fileFormat->loadFile("../AuxFolders/Pending/2.16.840.1.113662.4.8796818069641.798806497.93296077602350.10.dst");

    DcmDataset * dst = new DcmDataset(*(fileFormat->getDataset()));
    setDataset(dst);

    setPreviewText();
    updatePending();
    delete fileFormat;
}

imgSRWidget::~imgSRWidget()
{
    delete ui;
    delete btBallom;
}

void imgSRWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x() >= 0 && event->x() < 5 && btBallom->isHidden()){
        emit showSideWidgetButon(true, event->y());
    }
    else if((event->x() < 0 || event->x() >= btBallom->width()-2) && !btBallom->isHidden())
            emit showSideWidgetButon(false, event->y());
}

void imgSRWidget::setSideWidgetState(bool state, int pos){
    btBallom->setGeometry(0, pos, 30, 30);
    if (state){
        btBallom->show();
    }else{
        btBallom->hide();
    }
}

void imgSRWidget::setSideWidgetVisible()
{
    if(isVisibleSideWidget){
        btBallom->setIcon(QIcon(QPixmap(":/Resources/show.png")));
        ui->sideWidget->hide();
    }else{
        btBallom->setIcon(QIcon(QPixmap(":/Resources/hide.png")));
        ui->sideWidget->show();
    }
    isVisibleSideWidget = !isVisibleSideWidget;

}

void imgSRWidget::deleteSWElement(imgSideWidgetElement *element)
{
    int index = sideWEList.indexOf(element);
    QList<imgSideWidgetElement *>::iterator it = sideWEList.begin();
    if(index != -1){
        it+=index;
        sideWEList.erase(it);
    }
    delete element;
}

void imgSRWidget::setPreviewText() {
    QDate date = QDate::currentDate();
    QString Name = "Patient Name", Age = "Patient Age", Sex = "Patient Sex";
    QString Modality = "Modality";

    if(dataSetHandler.getDataSet() != nullptr)
        Modality = dataSetHandler.getModality().c_str();

    if(Modality == "SR"){

        imgSRCstore::dsr2html("./SR.html", dataSetHandler.getDataSet());
        QFile file("./SR.html");
        file.open(QIODevice::ReadOnly);
        QTextStream textStream(&file);
        QString allDocumment = textStream.readAll();
        ui->tePreview->setHtml(allDocumment);
        file.remove();
        return;
    }

    if (dataSetHandler.getDataSet() != nullptr){
        Name = dataSetHandler.GetPatientData(PatientName).c_str();
        Age  = dataSetHandler.GetPatientData(PatientAge).c_str();
        Sex  = dataSetHandler.GetPatientData(PatientSex).c_str();
        Modality = dataSetHandler.getModality().c_str();
    }

    QString previewString("Reporte estructurado\n"
                          "===========================================\n"
                          "*** \n"
                          "**Hospital de creado:** Hospital.\n"
                          "\n"
                          "**Fecha de creacion del reporte:** " + date.toString() + ".\n"
                          "### Paciente\n"
                          "**Nombre:** " + Name + " **Sexo:** " + Sex + " **Edad:** " + Age + "\n"
                          "### Estudio\n"
                          "**Fecha:** "+ date.toString() +". **Modalidad:** "+Modality+"\n"
                          "* * *\n"
                          "## Indicacion\n"
                          ">" +  ui->pteIndication->toPlainText() + "\n"
                          "## Procedimiento\n"
                          ">" + ui->pteProcedure->toPlainText()+"\n"
                          "## Hallazgos\n"
                          ">" + ui->pteFindings->toPlainText()+ "\n"
                          "## Impression Diagnostica\n"
                          ">"+ ui->pteImpresionDiag->toPlainText());

    ui->tePreview->setMarkdown(previewString);
}

void imgSRWidget::updatePending() {
    foreach(auto x, sideWEList){
            deleteSWElement(x);
    }
    sideWEList.clear();
    QDir dir("/home/ccharris/CLionProjects/imagisSRM/AuxFolders/Pending");
    dir.setFilter(QDir::Files);
    dir.setNameFilters({"*.dat"});
    QStringList fileList = dir.entryList();
    QString path = dir.absolutePath() + "/";

    foreach(auto x, fileList){
        imgPendingFileHandler *pendingFileHandler = new imgPendingFileHandler();
        qDebug() << path + x;
        pendingFileHandler->loadFromPending(path+x);
        auto *element = new imgSideWidgetElement(pendingFileHandler->getDcmDataset());
        element->setIndication(pendingFileHandler->getIndication());
        element->setProcedure(pendingFileHandler->getProcedure());
        element->setFindinds(pendingFileHandler->getFindings());
            element->setImpression(pendingFileHandler->getImpression());
        connect(element, SIGNAL(clicked(imgSideWidgetElement*)), this, SLOT(setCurrentSWElement(imgSideWidgetElement*)));
        sideWEList.push_back(element);
        pendingFileHandler->setDataSetFileName("../test");
        pendingFileHandler->sendToPending();
        delete pendingFileHandler;
    }

    foreach(auto x, sideWEList){
    ui->scrollAreaWidgetContents->layout()->addWidget(x);

        if(dataSetHandler.getDataSet() == nullptr){
            continue;
        }
        if (x->getStudyId().toStdString() == dataSetHandler.GetStudyData()
            && dataSetHandler.getModality().c_str() == x->getSWEModality()){
                x->mark();
        }
    }
}

void imgSRWidget::sendPending() {

    QString modal = dataSetHandler.getModality().c_str();
    QString studyId;
    if(modal == "SR"){
        studyId = dataSetHandler.getInstanceUID().c_str();
    } else{
        studyId = dataSetHandler.GetStudyData().c_str();
    }

    DcmDataset *dts = new DcmDataset(*dataSetHandler.getDataSet());
    imgPendingFileHandler *pendingFileHandler
    = new imgPendingFileHandler("../AuxFolders/Pending/" + studyId);
    pendingFileHandler->setIndication(ui->pteIndication->toPlainText());
    pendingFileHandler->setProcedure(ui->pteProcedure->toPlainText());
    pendingFileHandler->setFindings(ui->pteFindings->toPlainText());
    pendingFileHandler->setImpression(ui->pteImpresionDiag->toPlainText());
    pendingFileHandler->setDcmDataset(dts);
    pendingFileHandler->sendToPending();
    delete dts;
    updatePending();
    delete(pendingFileHandler);
    return;
}

void imgSRWidget::setCurrentSWElement(imgSideWidgetElement *element) {

    foreach(auto x, sideWEList){
        if(x != element){
            x->unmark();
        }
    }

    ui->pteIndication->setPlainText(element->getIndication());
    ui->pteProcedure->setPlainText(element->getProcedure());
    ui->pteFindings->setPlainText(element->getFindinds());
    ui->pteImpresionDiag->setPlainText(element->getImpression());
    element->mark();
    setDataset(element->getDataset());
    setPreviewText();

}

void imgSRWidget::sendToServer() {
    ui->hsProgreess->changeSize(0,0,QSizePolicy::Ignored);
    ui->progressBar->show();
    ui->progressBar->setValue(0);
    ui->btSend->setEnabled(false);
    qDebug() << "here";
    auto * sendThread = new imgSendThread(dataSetHandler.getDataSet(), ui->pteIndication->toPlainText(),
                                                   ui->pteProcedure->toPlainText(),
                                                   ui->pteFindings->toPlainText(),
                                                   ui->pteImpresionDiag->toPlainText());

    connect(sendThread, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(sendThread, SIGNAL(resultReady(bool)), this, SLOT(getSendState(bool)));

    sendThread->start();
}

void imgSRWidget::getSendState(bool state) {

    ui->progressBar->setValue(state? ui->progressBar->value() : 0);
    ui->lbSendStatus->setPixmap(state? *success: *fail);
    ui->btSend->setEnabled(true);
    ui->progressBar->hide();
    ui->hsProgreess->changeSize(0,0,QSizePolicy::Expanding);
    updatePending();

}

void imgSRWidget::setDataset(DcmDataset *dataset) {
    dataSetHandler.setDataSet(dataset);
    bool isSR = dataSetHandler.getModality() == "SR"? true: false;
    emit isDSR(isSR);
}

void imgSRWidget::sendPrint() {
    QPrinter printer;
    printer.setPrinterName("Imprimir reporte");
    printer.setOutputFileName("../AuxFolders/PDF/");
    QPrintDialog printDialog(&printer, this);
    if(printDialog.exec() == QDialog::Rejected)return;
    ui->tePreview->print(&printer);
}

void imgSRWidget::setSRState(bool state) {

    ui->gbEditZone->setVisible(!state);
    ui->btSend->setVisible(!state);

    if(!state)
        return;

    imgSRCstore::dsr2xml("file.xml", dataSetHandler.getDataSet());

    QDomDocument document;
    QFile file("file.xml");
    file.open(QIODevice::ReadOnly);
    document.setContent(&file);

    QPlainTextEdit * array [4] = {ui->pteIndication, ui->pteProcedure, ui->pteFindings, ui->pteImpresionDiag};
    QDomNodeList list = document.elementsByTagName("text");

    for (int i = 0; i < 4 ; ++i) {
        QDomElement element = list.item(i).firstChild().nextSibling().nextSibling().toElement();
        QString str(element.firstChild().toText().data());
        array[i]->setPlainText(str);
    }
    file.close();
    file.remove();
}
