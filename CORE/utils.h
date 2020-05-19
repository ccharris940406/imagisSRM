//
// Created by ccharris on 4/30/20.
//

#ifndef IMAGISSRM_UTILS_H
#define IMAGISSRM_UTILS_H

//Patient data
#define PatientID                               0b0000
#define PatientName                             0b0001
#define PatientSex                              0b0010
#define PatientAge                              0b0011

//dcmSend codes
#define APPLICATIONTITLE                        "DCMSEND"
#define PEERAPPLICATIONTITLE                    "ANY-SCP"
//exit codes
//File
#define EXIT_FAIL_FILE_DAMAGED                  0b1101
//Network
#define EXIT_FAIL_NO_INITIALIZED_NETWORK        0b1001
#define EXIT_FAIL_NO_SEND_REQUEST               0b1010
#define EXIT_SEND_SUCCESS                       0b0000

#endif //IMAGISSRM_UTILS_H
