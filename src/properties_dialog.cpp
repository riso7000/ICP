// ------------------------------
// properties_dialog.cpp
//
// Author:
// Richard Henček (xhencer00)
//
// Implements properties windows for places and transitions.
// ------------------------------

#include <QFormLayout>
#include <QSpinBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDialogButtonBox>

// Module headers
#include "properties_dialog.h"



TransitionPropertiesDialog::TransitionPropertiesDialog(
    const QString &currentName, int currentDelay, const QString &currentEventName,
    const QString &currentGuard, const QString &currentAction, bool readOnly, QWidget *parent
) : QDialog(parent)
{
    setWindowTitle(readOnly ? "Edit Transition (Read Only)" : "Edit Transition");

    QFormLayout *layout = new QFormLayout(this);

    // Setup Name Change
    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setText(currentName);
    nameLineEdit->setReadOnly(readOnly);
    layout->addRow("Name of Transition:", nameLineEdit);

    // Setup Delay Input
    delaySpinBox = new QSpinBox(this);
    delaySpinBox->setRange(0, 9999);
    delaySpinBox->setValue(currentDelay);
    delaySpinBox->setButtonSymbols( QAbstractSpinBox::NoButtons);
    delaySpinBox->setReadOnly(readOnly);
    if (!readOnly) delaySpinBox->setFocus();
    layout->addRow("Delay (ms):", delaySpinBox);

    // Setup Event Input
    eventLineEdit = new QLineEdit(this);
    eventLineEdit->setText(currentEventName);
    eventLineEdit->setReadOnly(readOnly);
    layout->addRow("Event input variable:", eventLineEdit);

    // Setup Guard Input
    guardTextEdit = new QTextEdit(this);
    guardTextEdit->setPlainText(currentGuard);
    guardTextEdit->setAcceptRichText(false);
    guardTextEdit->setMaximumHeight(45);
    guardTextEdit->setReadOnly(readOnly);
    layout->addRow("Guard expression:", guardTextEdit);

    // Setup Action Input
    actionTextEdit = new QTextEdit(this);
    actionTextEdit->setPlainText(currentAction);
    actionTextEdit->setAcceptRichText(false);
    actionTextEdit->setMaximumHeight(75);
    actionTextEdit->setReadOnly(readOnly);
    layout->addRow("Action expression:", actionTextEdit);

    if (!readOnly) {
        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addRow(btns);
    }
    else {
        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Close);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addRow(btns);
    }
}

int TransitionPropertiesDialog::getDelay() const {
    return delaySpinBox->value();
}

QString TransitionPropertiesDialog::getGuard() const {
    return guardTextEdit->toPlainText();
}


PlacePropertiesDialog::PlacePropertiesDialog(const QString& name, int tokens, bool readOnly, QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(readOnly ? "Edit Place (Read Only)" : "Edit Place");
    QFormLayout* f = new QFormLayout(this);

    nameLineEdit = new QLineEdit(name);
    nameLineEdit->setReadOnly(readOnly);

    tokensSpinBox = new QSpinBox();
    tokensSpinBox->setRange(0, 999);
    tokensSpinBox->setValue(tokens);
    tokensSpinBox->setReadOnly(readOnly);
    tokensSpinBox->setButtonSymbols( QAbstractSpinBox::NoButtons);

    f->addRow("Name:", nameLineEdit);
    f->addRow("Tokens:", tokensSpinBox);

    if (!readOnly) {
        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        f->addRow(btns);
    }
    else {
        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Close);
        connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
        f->addRow(btns);
    }

    if (!readOnly) {
        tokensSpinBox->setFocus();
    }
}
