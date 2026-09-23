#include "ContentBase.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QVector>
#include <QHeaderView>
#include <QLabel>
#include <QScrollArea>

class ImageLabel : public QLabel {
public:
    explicit ImageLabel(const QPixmap &pixmap, QWidget *parent = nullptr) : QLabel(parent), m_pixmap(pixmap){
        setAlignment(Qt::AlignCenter);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void setWidthRatio(double ratio){
        m_widthRatio = qBound(0.0, ratio, 1.0);
        updateGeometry();
        updatePixmap();
    }

    QSize sizeHint() const override{
        if (m_pixmap.isNull())
            return QSize(0, 0);

        int width = 100;

        if (m_widthRatio > 0.0 && parentWidget())
            width = parentWidget()->width() * m_widthRatio;

        width = qMax(width, 1);
        width = qMin(width, m_pixmap.width());

        const int height = width * m_pixmap.height() / m_pixmap.width();

        return QSize(width, height);
    }

protected:
    void resizeEvent(QResizeEvent *event) override{
        QLabel::resizeEvent(event);
        updatePixmap();
    }

private:
    void updatePixmap(){
        if (m_pixmap.isNull())
            return;

        int width = contentsRect().width();

        if (width <= 0)
            return;

        if (m_widthRatio > 0.0 && parentWidget()){
            width = qMin(
                width,
                static_cast<int>(parentWidget()->contentsRect().width() * m_widthRatio)
            );
        }

        width = qMin(width, m_pixmap.width());

        const int height = width * m_pixmap.height() / m_pixmap.width();

        setPixmap(m_pixmap.scaled(
            width,
            height,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation)
        );
    }

private:
    QPixmap m_pixmap;
    double m_widthRatio = 0.0;
};

ContentBase::ContentBase(QWidget *parent, const QString& categoryName, const QString& subcategoryName)
    : QWidget(parent) , pref(PrefManager::instance()), fileformat(FileFormatManager::instance())
{
    pref = PrefManager::instance();
    fileformat = FileFormatManager::instance();
    theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : ""; 
    
#ifdef Q_OS_MAC
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        backgroundFillColor = "palette(mid)";
    } else {
        backgroundFillColor = "palette(base)";
    }
#else
    backgroundFillColor = "palette(base)";
#endif

    //m_imageWidth = 800;

    // Setup barre de défilement    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 

    // Setup contenu
    m_contentWidget = new QWidget();
    m_contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(10);
    m_contentLayout->addStretch();

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Titre & sous-titre
    setCategoryName(categoryName);
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    addContent(separator);
    setsubcategoryName(subcategoryName);

}

// Add this widget to the Content page
void ContentBase::addContent(QWidget* widget)
{
    if (!widget)
        return;

    const int nbWidget = m_contentLayout->count();
    const int index = (nbWidget > 0) ? nbWidget - 1 : 0;

    m_contentLayout->insertWidget(index, widget);
}

// Add a layout 
void ContentBase::addLayout(QLayout* layout)
{
    m_contentLayout->addLayout(layout);
}

// Add a vertical space 
void ContentBase::addVSpacing(int spacing)
{
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);
    layout->addSpacing(spacing);

    addContent(widget);
}

// [] 
// Set the name of the Category name at the beginning of the Content page
void ContentBase::setCategoryName(const QString& categoryName){
    auto* name = new QLabel(pref.getText(categoryName));

    QFont nameFont = name->font();
    nameFont.setPointSize(20);
    nameFont.setBold(true);
    name->setFont(nameFont);

    addContent(name);
}
// [] 
// Set the name of the subcategory name at the beginning of the Content page
void ContentBase::setsubcategoryName(const QString& subcategoryName){
    auto* name = new QLabel(pref.getText(subcategoryName));

    QFont nameFont = name->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    name->setFont(nameFont);

    addContent(name);
}

// Add a section with a bold font
void ContentBase::addSectionName(const QString& sectionName){
    auto* name = new QLabel(pref.getText(sectionName));

    QFont nameFont = name->font();
    nameFont.setPointSize(11);
    nameFont.setBold(true);
    name->setFont(nameFont);

    addContent(name);
}

// Create a table depending of a list of widgets
QWidget* ContentBase::createTable(const QString& tableName, const QList<QWidget*>& rows)
{
    auto* widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* layout = new QVBoxLayout(widget);
    
    if(pref.getText(tableName) != "[none]"){
        auto* titleTable = new QLabel(pref.getText(tableName));
        layout->addWidget(titleTable);
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    for (QWidget* row : rows)
    {
        //row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(row);
    }

    return widget;
}

QWidget* ContentBase::createButtonDescription(const QString& iconName, const QString& buttonLabel, const QString& buttonDescription)
{
    auto* widget = new QWidget(this);
    widget->setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 1px; border-radius: 5px;");

    auto* layout = new QHBoxLayout(widget);
    //layout->setContentsMargins(20, 20, 20, 20);

    auto* icon = new QLabel();
    icon->setPixmap(QPixmap(iconName + theme).scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //icon->setStyleSheet("padding: 1px");
    icon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* label = new QLabel("<b>" + pref.getText(buttonLabel) + "</b>");
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* description = new QLabel(pref.getText(buttonDescription));
    description->setWordWrap(true);
    description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->addWidget(icon, 1, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(label, 2);
    layout->addSpacing(20);
    layout->addWidget(description, 6);

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    widget->adjustSize();

    return widget;
}

void ContentBase::addButtonDescriptionTable(const QString& tableName, std::initializer_list<QString> buttons)
{
    QList<QWidget*> rows;

    for (const auto& button : buttons)
    {
        rows << createButtonDescription(
            ":/icons/" + button,
            "help_menu_" + button + "_label",
            "help_menu_" + button + "_description"
        );
    }

    addContent(createTable(tableName, rows));
}

void ContentBase::addImage(const QString& imageName, double widthRatio)
{
    auto* widget = new QWidget(this);

    //widget->setStyleSheet("background-color: red;");
    auto* layout = new QHBoxLayout(widget);

    layout->setContentsMargins(0, 0, 0, 0);

    QPixmap illustration(
        ":/help_dialog_illustrations/" + imageName
    );

    auto* illustrationLabel = new ImageLabel(illustration);

    if (widthRatio > 0.0)
        illustrationLabel->setWidthRatio(widthRatio);


    layout->addWidget(illustrationLabel);

    widget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    addContent(widget);
}



void ContentBase::addImages(const QList<QString>& imageNames)
{
    auto* widget = new QWidget(this);

    //widget->setStyleSheet("background-color: green;");

    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    for (const QString& imageName : imageNames)
    {
        QPixmap illustration(
            ":/help_dialog_illustrations/" + imageName
        );

        auto* illustrationLabel = new ImageLabel(illustration);

        layout->addWidget(illustrationLabel, 1);
    }

    widget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Minimum
    );

    addContent(widget);
}

void ContentBase::addTextFromLangJSON(const QString& descriptionKey)
{
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);

    QLabel* descriptionLabel = new QLabel(pref.getText(descriptionKey));

    QFont descriptionFont = descriptionLabel->font();
    descriptionFont.setPointSize(11);
    descriptionLabel->setFont(descriptionFont);
    
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignLeft);
    descriptionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    
    layout->addWidget(descriptionLabel);

    addContent(widget);
}

void ContentBase::addTextFromLangQMAP(QMap<QString, QString> texts)
{
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);

    QString langCode = pref.getLangCode();
    QString textToDisplay = texts.contains(langCode) ? texts[langCode] : "[Missing translation for : " + langCode + "]";
    QLabel* textLabel = new QLabel(textToDisplay);

    QFont textFont = textLabel->font();
    textFont.setPointSize(11);
    textLabel->setFont(textFont);
    
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignLeft);
    textLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    
    layout->addWidget(textLabel);

    addContent(widget);
}

void ContentBase::addQMAPTexts(QMap<QString, QPair<QString, QString>> texts)
{
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);

    QString langCode = pref.getLangCode();

    // Title
    QString titleToDisplay = texts.contains(langCode) ? texts[langCode].first : "[Missing translation for : " + langCode + "]";
    QLabel* titleLabel = new QLabel(titleToDisplay);

    QFont qmapFont = titleLabel->font();
    qmapFont.setPointSize(11);
    qmapFont.setBold(true);

    titleLabel->setFont(qmapFont);
    titleLabel->setAlignment(Qt::AlignLeft);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Text
    QString textToDisplay = texts.contains(langCode) ? texts[langCode].second : "[Missing translation for : " + langCode + "]";
    QLabel* textLabel = new QLabel(textToDisplay);

    qmapFont.setBold(false);
    textLabel->setFont(qmapFont);

    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignLeft);
    textLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    if(titleToDisplay != ""){
        layout->addWidget(titleLabel);
        layout->addSpacing(10);
        
    }    
    if(textToDisplay != "") {
        layout->addWidget(textLabel);
    }

    addContent(widget);
}

QString ContentBase::setTextFromLangQMAP(QMap<QString, QString> texts)
{
    QString langCode = pref.getLangCode();
    QString textToDisplay = texts.contains(langCode) ? texts[langCode] : "[Missing translation for : " + langCode + "]";    
    return textToDisplay;
}

void ContentBase::addMails(const QStringList& mails)
{
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);

    widget->setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 1px; border-radius: 5px;");

    for (auto &&mail : mails)
    {
        QLabel *label_mail = new QLabel("<a href=mailto:" + mail + ">" + mail + "</a>");
        label_mail->setAlignment(Qt::AlignCenter);
        label_mail->setTextFormat(Qt::RichText);
        label_mail->setTextInteractionFlags(Qt::TextBrowserInteraction);
        label_mail->setOpenExternalLinks(true);
        label_mail->setStyleSheet("a { text-decoration: none; }");
        layout->addWidget(label_mail);
    }

    addContent(widget);
}

void ContentBase::getFormatsAvailables(){
    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);


    QStringList categoriesList = fileformat.getAllCatergories();

    for (auto &&category : categoriesList)
    {
        QLabel *label_category = new QLabel(pref.getText(category));
        label_category->setAlignment(Qt::AlignLeft);
        layout->addWidget(label_category);
        
        // Les formats de la catégorie sont affichés et séparés par des virgules, le tout centré dans le widget
        QString formats = fileformat.getFormats(category);

        QLabel *label_format = new QLabel(formats);
        label_format->setAlignment(Qt::AlignCenter);
        label_format->setTextFormat(Qt::RichText);
        label_format->setTextInteractionFlags(Qt::TextBrowserInteraction);
        label_format->setStyleSheet("border: none; background-color: " + backgroundFillColor + "; padding: 5px; border-radius: 5px;");
        //label_format->setStyleSheet("a { text-decoration: none; }");
        layout->addWidget(label_format);
    }

    addContent(widget);
    
}

