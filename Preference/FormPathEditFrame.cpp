#include "FormPathEditFrame.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

FormPathEditFrame::FormPathEditFrame(const QString &name, const QString &subCategory, const QString &key, const QString &value, bool isFolder, QWidget *parent)
    : BasePreferenceFrame(name, subCategory, key, value, parent) // <--- Appel du constructeur parent
{

    m_pathLineEdit = new QLineEdit(value, this);
    m_browseBtn = new QPushButton("...", this);
    m_browseBtn->setCursor(Qt::PointingHandCursor);
    m_browseBtn->setFixedWidth(30);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->setContentsMargins(0, 0, 0, 0);
    pathLayout->setSpacing(5);
    pathLayout->addWidget(m_pathLineEdit);
    pathLayout->addWidget(m_browseBtn);

    setRightLayout(pathLayout);

    connect(m_browseBtn, &QPushButton::clicked, this, [this, isFolder]() {
        QString selectedPath = isFolder ? 
            QFileDialog::getExistingDirectory(this, "Sélectionner un dossier", m_pathLineEdit->text()) : 
            QFileDialog::getOpenFileName(this, "Sélectionner un fichier", m_pathLineEdit->text(), "Tous les fichiers (*.*)");

        if (!selectedPath.isEmpty() && selectedPath != m_prevValue) {
            m_pathLineEdit->setText(selectedPath);
            m_prevValue = selectedPath;
            emit updateJsonObjRequested(m_subCategory, m_key, selectedPath); 
        }
    });

    connect(m_pathLineEdit, &QLineEdit::editingFinished, this, [this]() {
        QString newPath = m_pathLineEdit->text();
        if (newPath != m_prevValue) {
            m_prevValue = newPath;
            emit updateJsonObjRequested(m_subCategory, m_key, newPath);
        }
    });
}

void FormPathEditFrame::clearPathUI(){
    if (m_pathLineEdit) {
        m_pathLineEdit->blockSignals(true); 
        m_prevValue = "";
        m_pathLineEdit->clear();     
        m_pathLineEdit->blockSignals(false);
    }
}

void FormPathEditFrame::setUIValue(const QString& path){
    if (m_pathLineEdit) {
        m_pathLineEdit->blockSignals(true); 
        m_prevValue = path;
        m_pathLineEdit->setText(path);
        m_pathLineEdit->blockSignals(false);
    }
}