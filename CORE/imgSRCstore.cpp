//
// Created by ccharris on 5/12/20.
//
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diciefn.h"
#include "dcmtk/dcmimage/dipipng.h"
#include "imgSRCstore.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/dstorscu.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "utils.h"

int imgSRCstore::dcmSend( const std::string& peer, unsigned short port,  const std::string& filename) {

    DcmStorageSCU storageScu;
    OFCondition status = storageScu.addDicomFile(filename.c_str());

    if(status.bad()){
        return EXIT_FAIL_FILE_DAMAGED;
    }

    storageScu.setPeerHostName(peer.c_str());
    storageScu.setPeerPort(port);
    storageScu.setPeerAETitle(PEERAPPLICATIONTITLE);
    storageScu.setAETitle(APPLICATIONTITLE);

    status = storageScu.addPresentationContexts();

    if(status.bad()){
        return EXIT_FAIL_NO_INITIALIZED_NETWORK;
    }

    status = storageScu.initNetwork();

    if(status.bad()){
        return EXIT_FAIL_NO_INITIALIZED_NETWORK;
    }

    status = storageScu.negotiateAssociation();

    if(status.bad()){
        return EXIT_FAIL_NO_INITIALIZED_NETWORK;
    }

    status = storageScu.sendSOPInstances();

    if(status.bad()){
        if (status == DUL_PEERREQUESTEDRELEASE)
        {
            // peer requested release (aborting)
            storageScu.closeAssociation(DCMSCU_PEER_REQUESTED_RELEASE);
        }
        else if (status == DUL_PEERABORTEDASSOCIATION)
        {
            // peer aborted the association
            storageScu.closeAssociation(DCMSCU_PEER_ABORTED_ASSOCIATION);
        }
        return EXIT_FAIL_NO_SEND_REQUEST;
    }

    storageScu.releaseAssociation();

    return EXIT_SEND_SUCCESS;
}

int imgSRCstore::xml2dsr(const std::string& ifile, const std::string& ofile) {
    OFCondition status;

    DcmFileFormat fileFormat;
    auto * dsrDocument = new DSRDocument();
    DcmDataset * dataset = fileFormat.getDataset();

    status = dsrDocument->readXML(ifile.c_str());

    if(status.bad()){
        return EXIT_FAIL_FILE_DAMAGED;
    }

    status = dsrDocument->write(*dataset);

    if(status.bad()){
        return EXIT_FAILURE;
    }

//    status = fileFormat.saveFile(ofile.c_str(), EXS_LittleEndianExplicit,
//                               EET_ExplicitLength, EGL_recalcGL, EPD_withPadding);
    fileFormat.convertToUTF8();
    status = fileFormat.saveFile(ofile.c_str(), EXS_LittleEndianExplicit);


    if(status.bad())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int imgSRCstore::makeThubImage(const char *ofname, DcmFileFormat *fileFormat) {
    unsigned long width = 140, heigth = 0;
    auto *dicomImage = new DicomImage(fileFormat, fileFormat->getDataset()->getOriginalXfer());
    DicomImage *newDicomImage;
    dicomImage->setWindow(dicomImage->getWindowCount()-1);
    newDicomImage = dicomImage->createScaledImage(width, heigth);
    DiPNGPlugin pngPlugin;
    newDicomImage->writePluginFormat(&pngPlugin, ofname);
    return 0;
}

int imgSRCstore::dsr2xml(const std::string &filename, DcmDataset *dts) {

    std::ofstream stream(filename.c_str(), std::ios::out);

    DSRDocument *dsrDocument = new DSRDocument();
    OFCondition cond = dsrDocument->read(*dts);

    if(cond.bad()){
        return 10;
    }

    cond = dsrDocument->writeXML(stream);
    stream.close();

    if(cond.bad()){
        return 10;
    }

    return 0;
}

int imgSRCstore::dsr2html(const std::string &filename, DcmDataset *dcmDataset) {
    std::ofstream stream(filename.c_str(), std::ios::out);
    DSRDocument *dsrDocument = new DSRDocument();
    OFCondition cond = dsrDocument->read(*dcmDataset);

    if(cond.bad()){
        return 10;
    }

    cond = dsrDocument->renderHTML(stream);
    stream.close();

    if(cond.bad()){
        return 10;
    }
    return 0;
}
