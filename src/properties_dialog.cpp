#include "properties_dialog.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QTextEdit>
#include <QDialogButtonBox>

TransitionPropertiesDialog::TransitionPropertiesDialog(int currentDelay, const QString &currentGuard, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Edit Transition");

    QFormLayout *layout = new QFormLayout(this);

    // Setup Delay Input
    delaySpinBox = new QSpinBox(this);
    delaySpinBox->setRange(0, 999999);
    delaySpinBox->setValue(currentDelay);
    layout->addRow("Delay (ms):", delaySpinBox);

    // Setup Guard Input
    guardTextEdit = new QTextEdit(this);
    guardTextEdit->setPlainText(currentGuard);
    guardTextEdit->setAcceptRichText(false);
    guardTextEdit->setMaximumHeight(100);
    layout->addRow("Guard expression:", guardTextEdit);

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
