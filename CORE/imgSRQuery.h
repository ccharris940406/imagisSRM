//
// C-FIND / C-GET query-retrieve against a remote PACS.
//

#ifndef IMAGISSRM_IMGSRQUERY_H
#define IMAGISSRM_IMGSRQUERY_H

#include <string>
#include <vector>

struct imgStudyResult {
    std::string patientName;
    std::string patientId;
    std::string studyDate;
    std::string studyDescription;
    std::string modality;
    std::string studyInstanceUID;
};

class imgSRQuery {
public:
    static std::vector<imgStudyResult> findStudies(const std::string &peer, unsigned short port,
                                                     const std::string &patientNameFilter,
                                                     const std::string &patientIdFilter = "");

    static bool getStudy(const std::string &peer, unsigned short port,
                          const std::string &studyInstanceUID, const std::string &outputDir);
};

#endif //IMAGISSRM_IMGSRQUERY_H
