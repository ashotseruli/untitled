#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include<QString>
#include<QMap>
#include<QVariant>
#include<QScopedPointer>

class QXmlStreamWriter;
class QSettings;

class XmlHandler
{
public:
    XmlHandler();

    // загрузка из xml в QSettings.
    // return true, если загрузка прошла успешно
    bool loadXmlToSettingsTree(const QString &xmlPath, QSettings *settings);

    // загрузка из QSettings в xml
    // return true, если загрузка прошла успешно
    bool loadSettingsTreeToXml(const QString &xmlPath, QSettings *settings);

    // Возврат последней ошибки
    QString getLastError() const;

private:
    // запрещаем копирование
    XmlHandler(const XmlHandler&) = delete;
    void operator=(const XmlHandler&) = delete;

private:
    // непосредственный обход дерева и загрузка в XML
    void processLoadSettingsTreeToXml();

    // записывает параметр в m_settings
    /// ПРОВЕРИТЬ ПОВТОРЫ!!!!
    void writeParameterToSettings(const QString &key, const QVariant &value);

    // установка значения последней ошибки
    void setLastError(const QString &lastError);
    // сброс значения последней ошибки
    void resetLastError();

private:
    QScopedPointer<QXmlStreamWriter> m_xmlWriter;

    // описание последней ошибки
    QString m_lastError;

    // указатель на данные
    QSettings *m_settings;

    // бэкап для settings на случай попытки чтения из повреждённого xml
    QMap<QString, QVariant> backup;
};

#endif // XMLHANDLER_H
