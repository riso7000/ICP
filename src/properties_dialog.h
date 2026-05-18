// ------------------------------
// properties_dialog.h
//
// Author:
// Richard Henček (xhencer00)
//
// Defines shared interface for properties windows.
// ------------------------------

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QFormLayout>



// Forward declarations to keep the header lean
class QSpinBox;
class QTextEdit;
class QLineEdit;

class TransitionPropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit TransitionPropertiesDialog(const QString &currentName, int currentDelay, const QString &currentEventName,
        const QString &currentGuard, const QString &currentAction, bool readOnly, QWidget *parent = nullptr
    );

    int getDelay() const;
    QString getGuard() const;

    QLineEdit *nameLineEdit;
    QSpinBox *delaySpinBox;
    QLineEdit *eventLineEdit;
    QTextEdit *guardTextEdit;
    QTextEdit *actionTextEdit;
};

class PlacePropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PlacePropertiesDialog(const QString& name, int tokens, bool readOnly, QWidget* parent = nullptr);

    QSpinBox* tokensSpinBox;
    QLineEdit* nameLineEdit;
};

#endif // PROPERTIESDIALOG_H
