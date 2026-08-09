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
#include <CORE/imgSRQuery.h>
#include "imgQueryDialog.h"
#include "imgImagePickerDialog.h"
#include "imgReportPickerDialog.h"
#include <QtXml/QDomDocument>
#include <QInputDialog>
#include <QTextStream>
#include <QMessageBox>

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
    connect(ui->cbTemplates, SIGNAL(currentIndexChanged(int)), this, SLOT(applyTemplate(int)));
    connect(ui->Guardar, SIGNAL(clicked()), this, SLOT(saveTemplate()));

    btSearchPacs = new QPushButton("Buscar en PACS", this);
    ui->horizontalLayout_2->insertWidget(1, btSearchPacs);
    connect(btSearchPacs, SIGNAL(clicked()), this, SLOT(searchPacs()));

    btReports = new QPushButton("Reportes", this);
    ui->horizontalLayout_2->insertWidget(2, btReports);
    connect(btReports, SIGNAL(clicked()), this, SLOT(openReportPicker()));

    connect(ui->btAdd, SIGNAL(clicked()), this, SLOT(addOtherImage()));
    connect(ui->btDel, SIGNAL(clicked()), this, SLOT(removeOtherImage()));

    ui->tePreview->setCursorWidth(0);


    //é
    DcmFileFormat *fileFormat = new  DcmFileFormat();
//    fileFormat->loadFile("/home/ccharris/Downloads/srdoc103/image10.dcm");
//    fileFormat->loadFile("../AuxFolders/2.16.840.1.113662.2.1.53544936282433.12345.336.16650.dcm");
    fileFormat->loadFile("../AuxFolders/demo_report.dcm");
    //fileFormat->loadFile("../AuxFolders/Pending/2.16.840.1.113662.4.8796818069641.798806497.93296077602350.10.dst");

    DcmDataset * dst = new DcmDataset(*(fileFormat->getDataset()));
    setDataset(dst);

    setPreviewText();
    updatePending();
    loadTemplatesList();
    delete fileFormat;
}

imgSRWidget::~imgSRWidget()
{
    delete ui;
    delete btBallom;
    qDeleteAll(otherImages);
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
    QDir dir("../AuxFolders/Pending");
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

    element->mark();
    setDataset(element->getDataset());

    ui->pteIndication->setPlainText(element->getIndication());
    ui->pteProcedure->setPlainText(element->getProcedure());
    ui->pteFindings->setPlainText(element->getFindinds());
    ui->pteImpresionDiag->setPlainText(element->getImpression());
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

    if(!state)
        return;

    QFile::remove("file.xml");

    if (imgSRCstore::dsr2xml("file.xml", dataSetHandler.getDataSet()) != 0) {
        QMessageBox::warning(this, "Cargar reporte",
                              "No se pudo leer el contenido de este reporte SR.");
        return;
    }

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

static QString extractTemplateSection(const QString &content, const QString &startMarker, const QString &endMarker) {
    int start = content.indexOf(startMarker);
    if (start == -1)
        return QString();
    start += startMarker.length();
    int end = endMarker.isEmpty() ? content.length() : content.indexOf(endMarker, start);
    if (end == -1)
        end = content.length();
    return content.mid(start, end - start).trimmed();
}

void imgSRWidget::loadTemplatesList() {
    ui->cbTemplates->blockSignals(true);
    ui->cbTemplates->clear();
    ui->cbTemplates->addItem("-- Seleccionar plantilla --");

    QDir dir("../AuxFolders/Templates");
    dir.setFilter(QDir::Files);
    QStringList fileList = dir.entryList();
    fileList.removeAll("template.xml");
    ui->cbTemplates->addItems(fileList);

    ui->cbTemplates->blockSignals(false);
}

void imgSRWidget::applyTemplate(int index) {
    if (index <= 0)
        return;

    QFile file("../AuxFolders/Templates/" + ui->cbTemplates->itemText(index));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    ui->pteIndication->setPlainText(extractTemplateSection(content, "[INDICATION]\n", "[PROCEDURE]\n"));
    ui->pteProcedure->setPlainText(extractTemplateSection(content, "[PROCEDURE]\n", "[FINDINGS]\n"));
    ui->pteFindings->setPlainText(extractTemplateSection(content, "[FINDINGS]\n", "[IMPRESSION]\n"));
    ui->pteImpresionDiag->setPlainText(extractTemplateSection(content, "[IMPRESSION]\n", ""));
}

void imgSRWidget::saveTemplate() {
    bool ok;
    QString name = QInputDialog::getText(this, "Guardar plantilla", "Nombre de la plantilla:",
                                          QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || name.isEmpty())
        return;

    QFile file("../AuxFolders/Templates/" + name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << "[INDICATION]\n" << ui->pteIndication->toPlainText() << "\n"
           << "[PROCEDURE]\n" << ui->pteProcedure->toPlainText() << "\n"
           << "[FINDINGS]\n" << ui->pteFindings->toPlainText() << "\n"
           << "[IMPRESSION]\n" << ui->pteImpresionDiag->toPlainText() << "\n";
    file.close();

    loadTemplatesList();
    ui->cbTemplates->setCurrentIndex(ui->cbTemplates->findText(name));
}

void imgSRWidget::searchPacs() {
    imgQueryDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    std::string studyUID = dialog.selectedStudyInstanceUID();
    if (studyUID.empty())
        return;

    QString tmpDir = "../AuxFolders/PACS_tmp/" + QString::fromStdString(studyUID);
    QDir().mkpath(tmpDir);

    if (!imgSRQuery::getStudy(PEERHOST, PEERPORT, studyUID, tmpDir.toStdString())) {
        QMessageBox::warning(this, "Buscar en PACS", "No se pudo recuperar el estudio desde el PACS.");
        return;
    }

    QDir dir(tmpDir);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();

    if (files.isEmpty()) {
        QMessageBox::warning(this, "Buscar en PACS", "El estudio no contiene instancias recuperables.");
        return;
    }

    // Prefer the most recent SR report already in this study (so an existing
    // report reloads with its text intact); fall back to a plain image if
    // the study has no report yet.
    QString chosen;
    QString newestSRContentDateTime;
    QString firstImage;

    for (const QString &fileName: files) {
        DcmFileFormat probe;
        if (probe.loadFile((tmpDir + "/" + fileName).toStdString().c_str()).bad())
            continue;

        OFString modality;
        probe.getDataset()->findAndGetOFString(DCM_Modality, modality);

        if (modality != "SR") {
            if (firstImage.isEmpty())
                firstImage = tmpDir + "/" + fileName;
            continue;
        }

        OFString contentDate, contentTime;
        probe.getDataset()->findAndGetOFString(DCM_ContentDate, contentDate);
        probe.getDataset()->findAndGetOFString(DCM_ContentTime, contentTime);
        QString contentDateTime = QString(contentDate.c_str()) + QString(contentTime.c_str());

        if (chosen.isEmpty() || contentDateTime > newestSRContentDateTime) {
            chosen = tmpDir + "/" + fileName;
            newestSRContentDateTime = contentDateTime;
        }
    }

    if (chosen.isEmpty())
        chosen = firstImage.isEmpty() ? (tmpDir + "/" + files.first()) : firstImage;

    DcmFileFormat fileFormat;
    if (fileFormat.loadFile(chosen.toStdString().c_str()).bad()) {
        QMessageBox::warning(this, "Buscar en PACS", "No se pudo leer el archivo recuperado.");
        return;
    }

    DcmDataset *dst = new DcmDataset(*(fileFormat.getDataset()));
    setDataset(dst);
    setPreviewText();

    currentStudyDir = tmpDir;
    currentMainFilePath = chosen;

    qDeleteAll(otherImages);
    otherImages.clear();
    otherImagePaths.clear();
    ui->comboBox->clear();

    foreach(auto x, sideWEList){
        x->unmark();
    }
    auto *element = new imgSideWidgetElement(dst);
    connect(element, SIGNAL(clicked(imgSideWidgetElement*)), this, SLOT(setCurrentSWElement(imgSideWidgetElement*)));
    sideWEList.push_back(element);
    ui->scrollAreaWidgetContents->layout()->addWidget(element);
    element->mark();

    delete dst;
}

void imgSRWidget::addOtherImage() {
    if (currentStudyDir.isEmpty()) {
        QMessageBox::warning(this, "Otras imagenes",
                              "Primero carga un estudio con \"Buscar en PACS\".");
        return;
    }

    if (otherImages.size() >= 4) {
        QMessageBox::warning(this, "Otras imagenes", "Ya agregaste el maximo de 4 imagenes.");
        return;
    }

    std::string studyUID = dataSetHandler.GetStudyData();
    if (!studyUID.empty())
        imgSRQuery::getStudy(PEERHOST, PEERPORT, studyUID, currentStudyDir.toStdString());

    QDir dir(currentStudyDir);
    dir.setFilter(QDir::Files);
    QStringList candidates;
    for (const QString &fileName: dir.entryList()) {
        QString path = currentStudyDir + "/" + fileName;
        if (path == currentMainFilePath || otherImagePaths.contains(path))
            continue;
        candidates.push_back(path);
    }

    if (candidates.isEmpty()) {
        QMessageBox::information(this, "Otras imagenes",
                                  "No hay mas cortes de imagen disponibles en este estudio.");
        return;
    }

    imgImagePickerDialog dialog(candidates, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString chosenPath = dialog.selectedFilePath();
    if (chosenPath.isEmpty())
        return;

    DcmFileFormat fileFormat;
    if (fileFormat.loadFile(chosenPath.toStdString().c_str()).bad()) {
        QMessageBox::warning(this, "Otras imagenes", "No se pudo leer la imagen elegida.");
        return;
    }

    imgSRDataSetHandler probeHandler;
    DcmDataset *dst = new DcmDataset(*(fileFormat.getDataset()));
    probeHandler.setDataSet(dst);

    OFString instanceNumber;
    fileFormat.getDataset()->findAndGetOFString(DCM_InstanceNumber, instanceNumber);
    QString label = QString("Slide %1: %2 - Corte %3")
            .arg(otherImages.size() + 1)
            .arg(probeHandler.getModality().c_str())
            .arg(instanceNumber.empty() ? "?" : instanceNumber.c_str());

    ui->comboBox->addItem(label);
    otherImages.push_back(dst);
    otherImagePaths.push_back(chosenPath);
}

void imgSRWidget::removeOtherImage() {
    int index = ui->comboBox->currentIndex();
    if (index < 0 || index >= otherImages.size())
        return;

    delete otherImages[index];
    otherImages.removeAt(index);
    otherImagePaths.removeAt(index);
    ui->comboBox->removeItem(index);
}

void imgSRWidget::openReportPicker() {
    if (currentStudyDir.isEmpty()) {
        QMessageBox::warning(this, "Reportes", "Primero carga un estudio con \"Buscar en PACS\".");
        return;
    }

    std::string studyUID = dataSetHandler.GetStudyData();
    if (!studyUID.empty())
        imgSRQuery::getStudy(PEERHOST, PEERPORT, studyUID, currentStudyDir.toStdString());

    QDir dir(currentStudyDir);
    dir.setFilter(QDir::Files);
    QStringList allFiles;
    for (const QString &fileName: dir.entryList())
        allFiles.push_back(currentStudyDir + "/" + fileName);

    QString imageFallback;
    for (const QString &path: allFiles) {
        DcmFileFormat probe;
        if (probe.loadFile(path.toStdString().c_str()).good()) {
            OFString modality;
            probe.getDataset()->findAndGetOFString(DCM_Modality, modality);
            if (modality != "SR") {
                imageFallback = path;
                break;
            }
        }
    }

    imgReportPickerDialog dialog(allFiles, imageFallback, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString chosenPath = dialog.selectedPath();
    if (chosenPath.isEmpty()) {
        QMessageBox::warning(this, "Reportes",
                              "Este estudio no tiene una imagen fuente disponible para crear un reporte nuevo.");
        return;
    }

    DcmFileFormat fileFormat;
    if (fileFormat.loadFile(chosenPath.toStdString().c_str()).bad()) {
        QMessageBox::warning(this, "Reportes", "No se pudo leer el archivo elegido.");
        return;
    }

    bool isNewReport = (chosenPath == imageFallback);

    DcmDataset *dst = new DcmDataset(*(fileFormat.getDataset()));
    setDataset(dst);
    setPreviewText();
    currentMainFilePath = chosenPath;

    if (isNewReport) {
        ui->pteIndication->clear();
        ui->pteProcedure->clear();
        ui->pteFindings->clear();
        ui->pteImpresionDiag->clear();
    }

    foreach(auto x, sideWEList){
        x->unmark();
    }
    auto *element = new imgSideWidgetElement(dst);
    connect(element, SIGNAL(clicked(imgSideWidgetElement*)), this, SLOT(setCurrentSWElement(imgSideWidgetElement*)));
    sideWEList.push_back(element);
    ui->scrollAreaWidgetContents->layout()->addWidget(element);
    element->mark();

    delete dst;
}
