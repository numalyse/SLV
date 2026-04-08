#include "Preference/BasePreferenceFrame.h"

class QLineEdit;
class QPushButton;

class FormPathEditFrame : public BasePreferenceFrame 
{
    Q_OBJECT

public:
    FormPathEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, bool isFolder = false, QWidget *parent = nullptr);

    void clearPathUI();
    void setUIValue(const QString &path);

private:
    QLineEdit* m_pathLineEdit = nullptr;
    QPushButton* m_browseBtn = nullptr;
};