#include "Preference/BasePreferenceTab.h"
#include "PrefManager.h"
#include <QFormLayout>
#include <QDebug>

BasePreferenceTab::BasePreferenceTab(const QString& categoryName, QWidget* parent)
    : QScrollArea(parent), m_categoryName(categoryName)
{
    setWidgetResizable(true); 
    setFrameShape(QFrame::NoFrame);

    m_container = new QWidget(this);
    m_layout = new QFormLayout(m_container);
    setWidget(m_container);

    auto& prefManager = PrefManager::instance();
    m_baseJson = prefManager.getCategory(m_categoryName);
    m_updatedJson = m_baseJson;
}

void BasePreferenceTab::addPreferenceFrame(BasePreferenceFrame* frame)
{
    m_frames.append(frame);
    m_layout->addRow(frame); 
    
    connect(frame, &BasePreferenceFrame::updateJsonObjRequested, this, &BasePreferenceTab::updateJsonObj);
}

bool BasePreferenceTab::needSave() const
{
    return m_baseJson != m_updatedJson;
}

void BasePreferenceTab::save()
{
    if(!needSave()) return;

    auto& prefManager = PrefManager::instance();
    if(prefManager.setCategory(m_categoryName, m_updatedJson)) {
        m_baseJson = m_updatedJson;
    } else {
        qWarning() << "Echec de la sauvegarde pour l'onglet" << m_categoryName;
    }
}

void BasePreferenceTab::discard()
{
    if(!needSave()) return;
    
    m_updatedJson = m_baseJson;

    for (BasePreferenceFrame* frame : std::as_const(m_frames)) {
        QString subCat = frame->getSubCategory();
        QString key = frame->getKey();
        QString originalValue;

        if (m_baseJson.contains(subCat) && m_baseJson[subCat].isObject()) {
            originalValue = m_baseJson[subCat].toObject().value(key).toString();
        } else {
            originalValue = m_baseJson.value(key).toString();
        }

        frame->setUIValue(originalValue);
    }
}

void BasePreferenceTab::updateJsonObj(const QString& subCategory, const QString& key, const QString& newValue)
{
    if (m_updatedJson.contains(subCategory) && m_updatedJson[subCategory].isObject()) {
        QJsonObject subObject = m_updatedJson[subCategory].toObject();
        subObject[key] = newValue;
        m_updatedJson[subCategory] = subObject;
    } else {
        m_updatedJson[key] = newValue;
    }
    
    emit tabChanges();
}