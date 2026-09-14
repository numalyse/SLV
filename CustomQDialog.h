#ifndef CUSTOMQDIALOG_H
#define CUSTOMQDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;

class CustomQDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomQDialog(
        const QString& titre,
        const QString& texte,
        const QString& bouton1,
        const QString& bouton2 = QString(),
        const QString& path = QString(),
        QWidget* parent = nullptr
    );

    ~CustomQDialog() override = default;

private slots:
    void onCloseClicked();
    void onPathClicked();

private:
    void setupUi();
    void setupColors();
    void setupStyle();

private:
    QString m_titre;
    QString m_texte;
    QString m_bouton1;
    QString m_bouton2;
    QString m_path;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_textLabel = nullptr;

    QPushButton* m_button1 = nullptr;
    QPushButton* m_button2 = nullptr;

    bool m_isDarkMode = false;
    QColor m_palbtnColor;
    QString m_palbtnColorStr;
};

#endif // CUSTOMQDIALOG_H
