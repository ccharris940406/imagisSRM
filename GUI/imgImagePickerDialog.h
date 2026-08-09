//
// Dialog to pick one image instance (thumbnail) from a set of already-retrieved
// DICOM files belonging to the same study, to attach as a "slide" to the report.
//

#ifndef IMAGISSRM_IMGIMAGEPICKERDIALOG_H
#define IMAGISSRM_IMGIMAGEPICKERDIALOG_H

#include <QDialog>
#include <QStringList>

class QListWidget;
class QPushButton;

class imgImagePickerDialog : public QDialog {
Q_OBJECT

public:
    explicit imgImagePickerDialog(const QStringList &filePaths, QWidget *parent = nullptr);

    QString selectedFilePath() const;

private:
    QListWidget *list;
    QStringList filePaths;
};

#endif //IMAGISSRM_IMGIMAGEPICKERDIALOG_H
