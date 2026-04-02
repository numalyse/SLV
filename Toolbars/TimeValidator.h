#ifndef TIMEVALIDATOR_H
#define TIMEVALIDATOR_H

#include <QValidator>

class TimeValidator : public QValidator
{
Q_OBJECT

public:
    explicit TimeValidator(QObject* parent = nullptr);
    QValidator::State validate(QString &input, int &pos) const override;
    void fixup(QString &input) const override;

};

#endif