//
// Created by ccharris on 4/30/20.
//

#ifndef IMAGISSRM_IMGSRDATASETHANDLER_H
#define IMAGISSRM_IMGSRDATASETHANDLER_H

#include "dcmtk/dcmdata/dctk.h"

class imgSRDataSetHandler {
private:
    DcmDataset *dataSet{dataSet = nullptr};
public:
    DcmDataset *getDataSet();

public:
    imgSRDataSetHandler();
    ~imgSRDataSetHandler();
    bool setDataSet(const std::string &str);
    bool setDataSet(DcmDataset *dcmDataset = nullptr);
    std::string getModality();
    std::string getHospitalName();
    std::string getSopclassUID();
    std::string getInstanceUID();
    std::string GetPatientData(int type);
    virtual std::string GetStudyData(int type = 0);
    virtual std::string GetSeriesData(int type = 0);

};


#endif //IMAGISSRM_IMGSRDATASETHANDLER_H
