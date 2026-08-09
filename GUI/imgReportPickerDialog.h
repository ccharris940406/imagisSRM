//
// Dialog to browse the SR report(s) already sent for a study (if any) and
// pick one to continue editing, or start a brand-new report from scratch.
//

#ifndef IMAGISSRM_IMGREPORTPICKERDIALOG_H
#define IMAGISSRM_IMGREPORTPICKERDIALOG_H

#include <QDialog>
#include <QStringList>

class QListWidget;

class imgReportPickerDialog : public QDialog {
Q_OBJECT

public:
    // filePaths: every file retrieved for the study (images + SR reports mixed).
    // imageFallbackPath: file to load if the user picks "new report" (a plain image).
    explicit imgReportPickerDialog(const QStringList &filePaths, const QString &imageFallbackPath,
                                    QWidget *parent = nullptr);

    // Path to load as the main dataset: an existing SR, or imageFallbackPath for "new".
    QString selectedPath() const;

private:
    QListWidget *list;
    QString imageFallbackPath;
};

#endif //IMAGISSRM_IMGREPORTPICKERDIALOG_H
