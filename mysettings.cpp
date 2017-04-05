#include <QStandardItem>
#include <Qdebug>

#include "mysettings.h"
#include "myitem.h"
#include "mytree.h"
#include "treenode.h"


MySettings::MySettings(const QString &organization, const QString &application, QObject *parent):
                                                     QSettings(organization, application, parent)
                                                   , m_subtree(0)

{
}

MySettings::MySettings(QSettings::Scope scope, const QString &organization, const QString &application, QObject *parent):
                                                                      QSettings(scope, organization, application, parent)
                                                                    , m_subtree(0)
{
}

MySettings::MySettings(QSettings::Format format, QSettings::Scope scope, const QString &organization, const QString &application, QObject *parent):
                                                                                        QSettings(format, scope, organization, application, parent)
                                                                                      , m_subtree(0)
{
}

MySettings::MySettings(const QString &fileName, QSettings::Format format, QObject *parent):
                                                        QSettings(fileName, format, parent)
                                                      , m_subtree(0)
{
}

MySettings::MySettings(QObject *parent): QSettings(parent)
{
}

void MySettings::setCurrentParameterParents(const QStringList &currentParameterParents)
{
    m_curentFolderPath = currentParameterParents;
}

QStringList MySettings::currentFolderPath() const
{
    return m_curentFolderPath;
}

void MySettings::changeItem(QStandardItem *item)
{
    Q_ASSERT(item);
    if (!item || item->text().isEmpty())
        return;
    switch(item->column())
    {
        case 0:
        {
            MyItem* myItem = dynamic_cast<MyItem*>(item);

            QString previousName = myItem->previousValue().toString();
            if (!myItem || myItem->text() == previousName)
                return;

            QString itemName = myItem->text();

            // доходим до нужного раздела
            for (QString &parent: m_curentFolderPath)
                beginGroup(parent);

            // папка
            if (eTypeOfItem::folder == myItem->thisItemType())
            {
                endGroup();
                // создается бэкап поддерева

                makeTree(QStringList(previousName));
                if (m_subtree->renameCurrentNode(itemName))
                {
                    // если удачно переименован нужный узел, удаляем из хранилища старую версию и заливаем из дерева актуальную
                    remove(previousName);
                    // восстановить из дерева
                    restoreFromTree();
                    sync();
                }
            }
            // параметр
            else
            {
                if (false == childKeys().contains(itemName))
                    // ЕСЛИ С ТАКИМ ИМЕНЕМ ЕСТЬ - НЕ ПОДХОДИТ
                {
                    QVariant itemValue = value(previousName);
                    remove(previousName);
                    setValue(itemName, itemValue);
                    sync();
                }
            }
            // возвращаемся к исходному разделу
            for (int i = 0; i < m_curentFolderPath.count(); i++)
                endGroup();

        }
            break;
        case 1:
        {
            Q_ASSERT(false);
        }
            break;
        case 2:
        {
            MyItem *valueItem = dynamic_cast<MyItem*>(item);

            // пустое значение и совпадение с предыдущим
            if (!valueItem
                || valueItem->text().isEmpty()
                || valueItem->previousValue() == valueItem->text())
                return;

            // имя параметра
            MyItem* nameItem = dynamic_cast<MyItem*>(item->model()->item(item->row(), 0));
            QString itemKey = nameItem->text();

            // доходим до нужного раздела
            for (QString &parent: m_curentFolderPath)
                beginGroup(parent);

                setValue(itemKey, valueItem->text());
                sync();

            // возвращаемся к исходному разделу
            for (int i = 0; i < m_curentFolderPath.count(); i++)
                endGroup();          
        }
            break;
        default:
        {
            Q_ASSERT(false);
        }
    };
}

bool MySettings::makeTree(const QStringList &nodes)
{
    if (nodes.empty())
        return false;
    if (0 != m_subtree)
        delete m_subtree;
    m_subtree = new MyTree;

    // контроль глубины на случай возникновения ошибок.
    // если возникла ошибка при добавлении в дерево, этот параметр поможет вернуться к исходному узлу
    int depth = 0;
    bool result = true;
    for(const QString &node: nodes)
    {
        result &= addNodes(node, depth);
        if (!result)
            break;
    }
    for (; depth > 0; --depth)
        endGroup();
    return result;
}

void MySettings::restoreFromTree()
{
    m_subtree->resetToRoot();
    restoreNodes();
    delete m_subtree;
    m_subtree = 0;
}

void MySettings::restoreNodes()
{
    if (!m_subtree)
        return;
    qDebug() << "restoring " << m_subtree->currentNodeName();
    beginGroup(m_subtree->currentNodeName());
    QStringList childrenList = m_subtree->childKeys();
    if (childrenList.empty())
    {
        // пустые папки иначе не создаются!
        setValue("1", 0);
        remove("1");
    }
    for (QString &key : childrenList)
        setValue(key, m_subtree->value(key));
    childrenList = m_subtree->childNodes();
    for(QString &node: childrenList)
    {
        m_subtree->startGroup(node);
        restoreNodes();
        m_subtree->endGroup();
    }
    endGroup();
}

bool MySettings::addNodes(const QString &nodeName, int &depth)
{
    if (nodeName.isEmpty())
        return false;
    beginGroup(nodeName);
    ++depth;
    bool resultOk = true;
    resultOk &= m_subtree->addNodeToCurrent(new TreeNode(nodeName));
    QStringList childrenKeysList = childKeys();
    for (QString &key : childrenKeysList)
    {
        if (resultOk)
            resultOk &= m_subtree->addKeyToCurrent(key, value(key));
        else
        {
            delete m_subtree;
            return resultOk;
        }
    }
    QStringList childrenGroupsList = childGroups();
    for (QString &group: childrenGroupsList)
    {
        if (resultOk)
        {
            resultOk &= addNodes(group, depth);
            m_subtree->endGroup();
        }
        else
        {
            delete m_subtree;
            return resultOk;
        }
    }
    --depth;
    endGroup();
    return resultOk;
}
