#include "FormPathEditFrame.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

FormPathEditFrame::FormPathEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, bool isFolder, QWidget *parent)
: BasePreferenceFrame(name, subCategory, key, value, parent)
{

    m_pathLabel = new QLabel(value, this);
    m_browseBtn = new QPushButton("...", this);
    m_browseBtn->setCursor(Qt::PointingHandCursor);
    m_browseBtn->setFixedWidth(30);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->setContentsMargins(0, 0, 0, 0);
    pathLayout->setSpacing(5);
    pathLayout->addWidget(m_pathLabel);
    pathLayout->addWidget(m_browseBtn);

    setRightLayout(pathLayout);

    connect(m_browseBtn, &QPushButton::clicked, this, [this, isFolder]() {
        QString selectedPath = isFolder ? 
            QFileDialog::getExistingDirectory(this, "Sélectionner un dossier", m_pathLabel->text()) : 
            QFileDialog::getOpenFileName(this, "Sélectionner un fichier", m_pathLabel->text(), "Tous les fichiers (*.*)");

        if (!selectedPath.isEmpty() && selectedPath != m_prevValue) {
            m_pathLabel->setText(selectedPath);
            m_prevValue = selectedPath;
            emit updateJsonObjRequested(m_subCategory, m_key, selectedPath); 
        }
    });
}

void FormPathEditFrame::clearPathUI(){
    if (m_pathLabel) {
        m_pathLabel->blockSignals(true); 
        m_prevValue = "";
        m_pathLabel->clear();     
        m_pathLabel->blockSignals(false);
    }
}

void FormPathEditFrame::setUIValue(const QString& path){
    if (m_pathLabel) {
        m_pathLabel->blockSignals(true); 
        m_prevValue = path;
        m_pathLabel->setText(path);
        m_pathLabel->blockSignals(false);
    }
}