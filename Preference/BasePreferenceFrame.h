#ifndef BASEPREFERENCEFRAME_H
#define BASEPREFERENCEFRAME_H

#include <QFrame>
#include <QString>
#include <QFormLayout>
#include <QLabel>

class BasePreferenceFrame : public QFrame
{
    Q_OBJECT

public:
    BasePreferenceFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent = nullptr);
    
    QString getSubCategory() const { return m_subCategory; }
    QString getKey() const { return m_key; }

    virtual void setUIValue(const QString& value) = 0;

signals:
    void updateJsonObjRequested(const QString& subCategory, const QString& key, const QString& newValue);
    void tabChanges();

protected:
    QString m_subCategory;
    QString m_key;
    QString m_prevValue;
    QFormLayout* m_layout;

    void setRightWidget(QWidget* rightWidget);
    void setRightLayout(QLayout* rightLayout); 
};

#endif // BASEPREFERENCEFRAME_H