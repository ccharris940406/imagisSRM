//
// Dialog to search (C-FIND) studies on a remote PACS and pick one to retrieve.
//

#ifndef IMAGISSRM_IMGQUERYDIALOG_H
#define IMAGISSRM_IMGQUERYDIALOG_H

#include <QDialog>
#include <string>
#include <vector>
#include "CORE/imgSRQuery.h"

class QLineEdit;
class QPushButton;
class QTableWidget;

class imgQueryDialog : public QDialog {
Q_OBJECT

public:
    explicit imgQueryDialog(QWidget *parent = nullptr, const QString &lockedPatientId = QString());

    std::string selectedStudyInstanceUID() const;

private slots:
    void doSearch();
    void onSelectionChanged();

private:
    QLineEdit *leFilter;
    QPushButton *btSearch;
    QTableWidget *table;
    QPushButton *btLoad;
    QPushButton *btCancel;
    std::vector<imgStudyResult> results;
    QString lockedPatientId;
};

#endif //IMAGISSRM_IMGQUERYDIALOG_H
