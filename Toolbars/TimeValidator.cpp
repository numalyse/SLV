#include "Toolbars/TimeValidator.h"
#include "TimeValidator.h"

#include <QRegularExpression>

TimeValidator::TimeValidator(QObject *parent) : QValidator(parent)
{
}

QValidator::State TimeValidator::validate(QString &input, int &pos) const {
        Q_UNUSED(pos);

        if (input.contains(' ')) {
            return Intermediate; 
        }

        if (!input.contains(':') && !input.contains('.')) {
            bool isNumeric = false;
            input.toInt(&isNumeric);
            return isNumeric && input.size() <= 2 ? Acceptable : Invalid;
        }

        const QStringList parts = input.split(QRegularExpression("[:.]"));
        if (parts.size() != 4) {
            return Invalid;
        }

        int values[4];
        for (int i = 0; i < 4; ++i) {
            bool isNumeric = false;
            values[i] = parts[i].toInt(&isNumeric);
            if (!isNumeric || parts[i].size() != 2) {
                return Invalid;
            }
        }

        return Acceptable; 
    }

void TimeValidator::fixup(QString &input) const{
    input.replace(' ', '0');
}
