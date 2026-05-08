#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H


#include <QDialog>

// Forward declarations to keep the header lean
class QSpinBox;
class QTextEdit;
class QLineEdit;

class TransitionPropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit TransitionPropertiesDialog(int currentDelay, const QString &currentEventName, const QString &currentGuard, QWidget *parent = nullptr);

    // Getters to retrieve the data after the dialog closes
    int getDelay() const;
    QString getGuard() const;


    QSpinBox *delaySpinBox;
    QLineEdit *eventLineEdit;
    QTextEdit *guardTextEdit;
};

#endif // PROPERTIESDIALOG_H
