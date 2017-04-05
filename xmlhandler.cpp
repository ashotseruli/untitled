#include "xmlhandler.h"

#include<QSettings>
#include<QFile>
#include<QXmlStreamWriter>
#include<QXmlStreamReader>
#include<QstringList>
#include<QMap>

XmlHandler::XmlHandler() : m_lastError("")
{
}

bool XmlHandler::loadXmlToSettingsTree(const QString &xmlPath, QSettings *settings)
{
    Q_ASSERT(settings != 0);

    if (0 == settings)
    {
        setLastError("Приёмник данных не корректен");
        return false;
    }

    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        setLastError("Не удалось открыть файл!");
        return false;
    }
    m_settings = settings;
    m_settings->clear();
    QXmlStreamReader xml(&xmlFile);

    bool isCorrect = true;
    while(isCorrect && !xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        // шапка xml, ничего не делаем
        switch (token) {
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::StartElement:
            {
                QXmlStreamAttributes attributes = xml.attributes();
                QString elementName = xml.name().toString();
                if ("document" == elementName)
                    continue;

                if ("folder" == elementName)
                {
                    if (!attributes.hasAttribute("name"))
                    {
                       isCorrect = false;
                       break;
                    }
                    m_settings->beginGroup(attributes.value("name").toString());

                    /// это костыль, чтобы загружались пустые директории
                    m_settings->setValue("1", "");
                    m_settings->remove("1");
                }
                else if ("parameter" == elementName)
                {
                    if (!(attributes.hasAttribute("name") && attributes.hasAttribute("value")) )
                    {
                        isCorrect = false;
                        break;
                    }
                    writeParameterToSettings(attributes.value("name").toString(), attributes.value("value").toString());
                }
                else
                {
                    isCorrect = false;
                    break;
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            {
                if ("folder" == xml.name().toString())
                    m_settings->endGroup();
            }
            break;
        case QXmlStreamReader::Invalid:
            {
                isCorrect = false;
            }
            break;
        default:
            break;
        }
    }
    // файл надо закрыть в любом случае
    xmlFile.close();

    if (!isCorrect)
    {
        while("" != m_settings->group())
            m_settings->endGroup();
        setLastError("XML is incorrect!");
        return false;
    }
    m_settings->sync();
    resetLastError();
    return true;
}

bool XmlHandler::loadSettingsTreeToXml(const QString &xmlPath, QSettings *settings)
{
    Q_ASSERT(settings != 0);

    if (0 == settings)
    {
        setLastError("Приёмник данных не корректен");
        return false;
    }


    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QIODevice::WriteOnly))
    {
        setLastError("Не удалось открыть файл!");
        return false;
    }
    m_settings = settings;
    m_xmlWriter.reset(new QXmlStreamWriter(&xmlFile));
    m_xmlWriter->setAutoFormatting(true);

    // непосредственно запись в xml
    m_xmlWriter->writeStartDocument();

    processLoadSettingsTreeToXml();

    m_xmlWriter->writeEndDocument();

    xmlFile.close();
    m_xmlWriter.reset(); // больше здесь не нужен
    resetLastError();
    return true;
}

QString XmlHandler::getLastError() const
{
    return m_lastError;
}

void XmlHandler::processLoadSettingsTreeToXml()
{
    m_xmlWriter->writeStartElement("document");
    QStringList childrenList = m_settings->childGroups();
    // обработка поддиректорий
    for (QString &groupName: childrenList)
    {
        m_xmlWriter->writeStartElement("folder");
        m_xmlWriter->writeAttribute("name", groupName);

        m_settings->beginGroup(groupName);
        processLoadSettingsTreeToXml();
        m_settings->endGroup();
        m_xmlWriter->writeEndElement();
    }
    // обработка параметров внутри текущей директории
    childrenList = m_settings->childKeys();
    for (QString &childName: childrenList)
    {
        m_xmlWriter->writeStartElement("parameter");

        m_xmlWriter->writeAttribute("name", childName);
        m_xmlWriter->writeAttribute("value", m_settings->value(childName).toString());

        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
    resetLastError();
}

void XmlHandler::setLastError(const QString &value)
{
    m_lastError = value;
}

void XmlHandler::resetLastError()
{
    m_lastError = "";
}

void XmlHandler::writeParameterToSettings(const QString &key, const QVariant &value)
{
    // если удастся привести к целому, запишется как целое
    bool isInt = true;
    int intValue = value.toInt(&isInt);
    m_settings->setValue(key, isInt? intValue : value);
}
