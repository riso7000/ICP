#include "properties_dialog.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDialogButtonBox>

TransitionPropertiesDialog::TransitionPropertiesDialog(const QString &currentName, int currentDelay, const QString &currentEventName,
                                                       const QString &currentGuard, const QString &currentAction, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Edit Transition");

    QFormLayout *layout = new QFormLayout(this);

    // Setup Name Change
    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setText(currentName);
    layout->addRow("Name of Transition:", nameLineEdit);

    // Setup Delay Input
    delaySpinBox = new QSpinBox(this);
    delaySpinBox->setRange(0, 9999);
    delaySpinBox->setValue(currentDelay);
    delaySpinBox->setFocus();
    layout->addRow("Delay (ms):", delaySpinBox);

    // Setup Event Input
    eventLineEdit = new QLineEdit(this);
    eventLineEdit->setText(currentEventName);
    layout->addRow("Event input variable:", eventLineEdit);

    // Setup Guard Input
    guardTextEdit = new QTextEdit(this);
    guardTextEdit->setPlainText(currentGuard);
    guardTextEdit->setAcceptRichText(false);
    guardTextEdit->setMaximumHeight(45);
    layout->addRow("Guard expression:", guardTextEdit);

    // Setup Action Input
    actionTextEdit = new QTextEdit(this);
    actionTextEdit->setPlainText(currentAction);
    actionTextEdit->setAcceptRichText(false);
    actionTextEdit->setMaximumHeight(75);
    layout->addRow("Action expression:", actionTextEdit);

    // Standard OK/Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int TransitionPropertiesDialog::getDelay() const {
    return delaySpinBox->value();
}

QString TransitionPropertiesDialog::getGuard() const {
    return guardTextEdit->toPlainText();
}


PlacePropertiesDialog::PlacePropertiesDialog(const QString& name, int tokens, QWidget* parent)
    : QDialog(parent)
{

    setWindowTitle("Edit Place");
    QFormLayout* f = new QFormLayout(this);

    nameLineEdit  = new QLineEdit(name);

    tokensSpinBox = new QSpinBox();
    tokensSpinBox->setRange(0, 9999);


    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);

    f->addRow("Name:",   nameLineEdit);
    f->addRow("Tokens:", tokensSpinBox);
    f->addRow(btns);

    tokensSpinBox->setValue(tokens);
    tokensSpinBox->setFocus();

}
