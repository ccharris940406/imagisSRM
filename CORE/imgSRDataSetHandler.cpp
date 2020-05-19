//
// Created by ccharris on 4/30/20.
//

#include "imgSRDataSetHandler.h"
#include "utils.h"
#include <ctime>

void convertDate(const std::string &date, int &year) {

    int multiply = 100;
    year = 0;
    if (date[3] != 'Y')
        return;
    for (int i = 0; i < 3; ++i, multiply/=10) {
        year += (date[i]-48)*multiply;
    }

}

void replaceAux(std::string &aux) {

    for(char & i : aux){
        if(i == '^')
            i = ' ';
    }

}

void convertDate(const std::string &date, int &year, int &month) {

    int i;
    int multiply = 1000;
    year = 0;month=0;
    std::string yearS, monthS;
    for(i = 0; i < 4; i++, multiply/=10)
        year+=(date[i]-48)*multiply;
    multiply = 10;
    for(i = 4; i < 6; i++, multiply/=10)
        month +=(date[i]-48)*multiply;

}

imgSRDataSetHandler::imgSRDataSetHandler()= default;

imgSRDataSetHandler::~imgSRDataSetHandler()= default;

bool imgSRDataSetHandler::setDataSet(const std::string &str) {
    OFCondition status;
    auto *fileFormat = new DcmFileFormat();
    status = fileFormat->loadFile(str.c_str());
    dataSet = fileFormat->getDataset();
    delete fileFormat;
    return status.good() != 0;
}

bool imgSRDataSetHandler::setDataSet(DcmDataset *dcmDataset) {
    DcmDataset *dcmDataset1 = new DcmDataset(*dcmDataset);
    dataSet = dcmDataset1;
}

std::string imgSRDataSetHandler::getModality() {
    const char *buffer =  nullptr;
    dataSet->findAndGetString(DCM_Modality, buffer);
    return std::__cxx11::string(buffer);
}

std::string imgSRDataSetHandler::getHospitalName() {
    const char *buffer = nullptr;
    dataSet->findAndGetString(DCM_InstitutionName, buffer);
    return std::__cxx11::string(buffer);
}

std::string imgSRDataSetHandler::getSopclassUID() {
    const char *buffer = nullptr;
    dataSet->findAndGetString(DCM_SOPClassUID, buffer);
    return std::__cxx11::string(buffer);
}

std::string imgSRDataSetHandler::getInstanceUID() {
    const char *buffer = nullptr;
    dataSet->findAndGetString(DCM_SOPInstanceUID, buffer);
    return std::__cxx11::string(buffer);
}

std::string imgSRDataSetHandler::GetPatientData(int type) {
    std::__cxx11::string aux;

    const char *buffer = nullptr;
    switch (type){
        case PatientAge:
            dataSet->findAndGetString(DCM_PatientBirthDate, buffer);
            if(buffer != nullptr && buffer != "")
            {
                std::time_t now = time(0);
                struct tm now_t = *localtime(&now);
                int year, month;
                convertDate(buffer, year, month);
                int age = now_t.tm_mon >= month? now_t.tm_year - year + 1900 : now_t.tm_year - year + 1899;
                return std::__cxx11::string(std::to_string(age));
            } else{
                int year;
                dataSet->findAndGetString(DCM_PatientAge, buffer);
                if(buffer == nullptr || buffer == "")
                    return "";
                convertDate(buffer, year);
                return std::__cxx11::string(std::to_string(year));
            }
        case PatientID:
            dataSet->findAndGetString(DCM_PatientID, buffer);
            return std::__cxx11::string(buffer);
        case PatientName:{
            dataSet->findAndGetString(DCM_PatientName, buffer);
            aux = buffer;
            replaceAux(aux);
            return std::__cxx11::string(aux);}
        case PatientSex:
            dataSet->findAndGetString(DCM_PatientSex, buffer);
            return std::__cxx11::string(buffer);
        default:
            return std::__cxx11::string("");
    }

}

std::string imgSRDataSetHandler::GetStudyData(int type) {
    const char *buffer = nullptr;
    dataSet->findAndGetString(DCM_StudyInstanceUID, buffer);
    return std::__cxx11::string(buffer);
}

std::string imgSRDataSetHandler::GetSeriesData(int type) {
    const char *buffer = nullptr;
    dataSet->findAndGetString(DCM_SeriesInstanceUID, buffer);
    return std::__cxx11::string(buffer);
}

DcmDataset *imgSRDataSetHandler::getDataSet() {
    return dataSet;
}
