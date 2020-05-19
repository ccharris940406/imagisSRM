//
// Created by ccharris on 5/12/20.
//

#ifndef IMAGISSRM_IMGSRCSTORE_H
#define IMAGISSRM_IMGSRCSTORE_H


#include <string>
#include "dcmtk/dcmdata/dctk.h"

class imgSRCstore {
public:

    static int makeThubImage(const char *ofname, DcmFileFormat *fileFormat);

    static int xml2dsr(const std::string& ifile, const std::string& ofile);

    static int dcmSend( const std::string& peer, unsigned short port,
             const std::string& filename);

    static int dsr2xml(const std::string &filename, DcmDataset *dts);

    static int dsr2html(const std::string & filename, DcmDataset *dcmDataset);
};


#endif //IMAGISSRM_IMGSRCSTORE_H
