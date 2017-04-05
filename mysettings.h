#ifndef MYSETTINGS_H
#define MYSETTINGS_H

#include <QSettings>
#include <QStringList>
class QStandardItem;
class MyTree;

class MySettings : public QSettings
{
    Q_OBJECT
public:
    MySettings(const QString &organization, const QString &application = QString(), QObject *parent = Q_NULLPTR);
    MySettings(Scope scope, const QString &organization, const QString &application = QString(), QObject *parent = Q_NULLPTR);
    MySettings(Format format, Scope scope, const QString &organization, const QString &application = QString(), QObject *parent = Q_NULLPTR);
    MySettings(const QString &fileName, Format format, QObject *parent = Q_NULLPTR);
    MySettings(QObject *parent = Q_NULLPTR);

    void setCurrentParameterParents(const QStringList& currentFolderPath);
    QStringList currentFolderPath() const;

public slots:
    void changeItem(QStandardItem *item);
public:
    bool makeTree(const QStringList &nodes);
    void restoreFromTree();
private:
    void restoreNodes();
    bool addNodes(const QString &nodeName, int &depth);
private:
    QStringList m_curentFolderPath;
    MyTree *m_subtree;
};

#endif // MYSETTINGS_H
