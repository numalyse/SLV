#ifndef BASEPREFERENCEFRAME_H
#define BASEPREFERENCEFRAME_H

#include <QFrame>
#include <QString>
#include <QFormLayout>
#include <QLabel>

/// @brief Base for preferences frame widgets, has a label for the row and stores the subcategory key and value to revert changes if needed
class BasePreferenceFrame : public QFrame
{
    Q_OBJECT

public:

    BasePreferenceFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, QWidget *parent = nullptr);
    
    QString getSubCategory() const { return m_subCategory; }
    QString getKey() const { return m_key; }

    /// @brief Each child class needs to define how to update the value 
    /// @param value 
    virtual void setUIValue(const QString& value) = 0;

signals:
    /// @brief signal emitted to the tab to update the jsonObject 
    void updateJsonObjRequested(const QString& subCategory, const QString& key, const QString& newValue);
    void tabChanges();

protected:
    QString m_subCategory;
    QString m_key;
    QString m_prevValue;
    QFormLayout* m_layout;

    /// @brief Use this if needing to set the right side of the row to a widget 
    void setRightWidget(QWidget* rightWidget);

    /// @brief  Use this if needing to set the right side of to a layout with multiple widgets 
    void setRightLayout(QLayout* rightLayout); 
};

#endif // BASEPREFERENCEFRAME_H