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
    explicit TransitionPropertiesDialog(const QString &currentName, int currentDelay, const QString &currentEventName, const QString &currentGuard, QWidget *parent = nullptr);

    // Getters to retrieve the data after the dialog closes
    int getDelay() const;
    QString getGuard() const;

    QLineEdit *nameLineEdit;
    QSpinBox *delaySpinBox;
    QLineEdit *eventLineEdit;
    QTextEdit *guardTextEdit;
};

class PlacePropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PlacePropertiesDialog(const QString& name, int tokens, QWidget* parent = nullptr);

    QLineEdit* nameLineEdit;
    QSpinBox* tokensSpinBox;
};

#endif // PROPERTIESDIALOG_H
