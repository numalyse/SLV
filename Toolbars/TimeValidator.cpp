#include "Toolbars/TimeValidator.h"
#include "TimeValidator.h"

TimeValidator::TimeValidator(QObject *parent) : QValidator(parent)
{
}

QValidator::State TimeValidator::validate(QString &input, int &pos) const {
        Q_UNUSED(pos);

        if (input.contains(' ')) {
            return Intermediate; 
        }
        
        return Acceptable; 
    }

void TimeValidator::fixup(QString &input) const{
    input.replace(' ', '0');
}
