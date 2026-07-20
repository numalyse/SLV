#ifndef BASEPREFERENCEFRAME_H
#define BASEPREFERENCEFRAME_H

#include "ToolbarButtons/ToolbarButton.h"

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

    void addResetButton(const QString& defaultValue);

signals:
    /// @brief signal emitted to the tab to update the jsonObject 
    void updateJsonObjRequested(const QString& subCategory, const QString& key, const QString& newValue);
    void tabChanges();

protected:
    QString backgroundFillColor = "palette(base)";
    QString m_subCategory;
    QString m_key;
    QString m_prevValue;
    QHBoxLayout* m_layoutHB;

    /// @brief Container holding every widget on the right side, used to control the spacing between them
    QWidget* m_rightContainer = nullptr;
    QHBoxLayout* m_rightLayout = nullptr;

    ToolbarButton* m_resetBtn = nullptr;
    QString m_defaultValue;

    /// @brief Appends a widget to the right side of the frame (added to m_rightContainer, so call order determines the layout order)
    void addRightWidget(QWidget* rightWidget);

    /// @brief Applies the frame style to the child
    void applyFrameStyleToChild(QWidget* child);
};

#endif // BASEPREFERENCEFRAME_H
