//
// C-FIND / C-GET query-retrieve against a remote PACS.
//

#include "imgSRQuery.h"
#include "utils.h"
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmdata/dctk.h"

static OFList<OFString> defaultTransferSyntaxes() {
    OFList<OFString> xfer;
    xfer.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfer.push_back(UID_LittleEndianImplicitTransferSyntax);
    return xfer;
}

static std::string getStringOrEmpty(DcmDataset *dataset, const DcmTagKey &tag) {
    OFString value;
    dataset->findAndGetOFString(tag, value);
    return value.c_str();
}

std::vector<imgStudyResult> imgSRQuery::findStudies(const std::string &peer, unsigned short port,
                                                     const std::string &patientNameFilter,
                                                     const std::string &patientIdFilter) {
    std::vector<imgStudyResult> results;

    DcmSCU scu;
    scu.setAETitle(APPLICATIONTITLE);
    scu.setPeerHostName(peer.c_str());
    scu.setPeerPort(port);
    scu.setPeerAETitle(PEERAPPLICATIONTITLE);

    scu.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, defaultTransferSyntaxes());

    if (scu.initNetwork().bad())
        return results;

    if (scu.negotiateAssociation().bad())
        return results;

    T_ASC_PresentationContextID presId =
            scu.findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");

    if (presId == 0) {
        scu.closeAssociation(DCMSCU_ABORT_ASSOCIATION);
        return results;
    }

    DcmDataset queryKeys;
    queryKeys.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY");
    queryKeys.putAndInsertOFStringArray(DCM_PatientName, patientNameFilter.empty() ? "*" : patientNameFilter.c_str());
    queryKeys.putAndInsertOFStringArray(DCM_PatientID, patientIdFilter.c_str());
    queryKeys.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    queryKeys.putAndInsertOFStringArray(DCM_StudyDate, "");
    queryKeys.putAndInsertOFStringArray(DCM_StudyDescription, "");
    queryKeys.putAndInsertOFStringArray(DCM_ModalitiesInStudy, "");

    OFList<QRResponse *> responses;
    scu.sendFINDRequest(presId, &queryKeys, &responses);

    for (auto it = responses.begin(); it != responses.end(); ++it) {
        QRResponse *response = *it;
        if (response->m_dataset != nullptr) {
            imgStudyResult result;
            result.patientName = getStringOrEmpty(response->m_dataset, DCM_PatientName);
            result.patientId = getStringOrEmpty(response->m_dataset, DCM_PatientID);
            result.studyDate = getStringOrEmpty(response->m_dataset, DCM_StudyDate);
            result.studyDescription = getStringOrEmpty(response->m_dataset, DCM_StudyDescription);
            result.modality = getStringOrEmpty(response->m_dataset, DCM_ModalitiesInStudy);
            result.studyInstanceUID = getStringOrEmpty(response->m_dataset, DCM_StudyInstanceUID);
            results.push_back(result);
        }
        delete response;
    }

    scu.releaseAssociation();
    return results;
}

bool imgSRQuery::getStudy(const std::string &peer, unsigned short port,
                           const std::string &studyInstanceUID, const std::string &outputDir) {
    DcmSCU scu;
    scu.setAETitle(APPLICATIONTITLE);
    scu.setPeerHostName(peer.c_str());
    scu.setPeerPort(port);
    scu.setPeerAETitle(PEERAPPLICATIONTITLE);
    scu.setStorageMode(DCMSCU_STORAGE_DISK);
    scu.setStorageDir(outputDir.c_str());

    OFList<OFString> xfer = defaultTransferSyntaxes();
    scu.addPresentationContext(UID_GETStudyRootQueryRetrieveInformationModel, xfer);

    for (int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; ++i)
        scu.addPresentationContext(dcmShortSCUStorageSOPClassUIDs[i], xfer, ASC_SC_ROLE_SCP);

    if (scu.initNetwork().bad())
        return false;

    if (scu.negotiateAssociation().bad())
        return false;

    T_ASC_PresentationContextID presId =
            scu.findPresentationContextID(UID_GETStudyRootQueryRetrieveInformationModel, "");

    if (presId == 0) {
        scu.closeAssociation(DCMSCU_ABORT_ASSOCIATION);
        return false;
    }

    DcmDataset queryKeys;
    queryKeys.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "STUDY");
    queryKeys.putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID.c_str());

    OFList<RetrieveResponse *> responses;
    OFCondition status = scu.sendCGETRequest(presId, &queryKeys, &responses);

    for (auto it = responses.begin(); it != responses.end(); ++it)
        delete *it;

    scu.releaseAssociation();
    return status.good();
}
