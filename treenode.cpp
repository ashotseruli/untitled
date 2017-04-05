#include "treenode.h"


TreeNode::TreeNode(const QString &name, TreeNode *parent): m_name(name)
                                                         , m_pParent(parent)
{
}

TreeNode::~TreeNode()
{
    destroy();
}

QString TreeNode::name() const
{
    return m_name;
}

QVariant TreeNode::value(const QString &key) const
{
    return m_values.value(key).toString();
}

bool TreeNode::addChild(TreeNode *child)
{
    if (0 == child || containsChild(child->name()))
        return false;
    m_childrenList.append(child);
    child->setParent(this);
    return true;
}

bool TreeNode::addValue(const QString &key, const QVariant &value)
{
    if (key.isEmpty() || m_values.contains(key))
        return false;
    m_values.insert(key, value);
    return true;
}

void TreeNode::deleteChild(const QString &name)
{
    TreeNode* childToDelete = child(name);
    if (0 == childToDelete)
        return;
    m_childrenList.removeOne(childToDelete);
    delete childToDelete;
}

void TreeNode::deleteKey(const QString &key)
{
    m_values.remove(key);
}

bool TreeNode::containsKey(const QString &key) const
{
    if (key.isEmpty())
        return false;
    return m_values.contains(key);
}

bool TreeNode::containsChild(const QString name) const
{
    if (name.isEmpty())
        return false;

    for(QList<TreeNode*>::const_iterator it = m_childrenList.cbegin(); it != m_childrenList.cend(); ++it)
    {
        if ((*it)->name() == name)
            return true;
    }
    return false;
}

bool TreeNode::setName(const QString &newName)
{
    if (newName.isEmpty())
        return false;

    m_name = newName;
    return true;
}

bool TreeNode::renameKey(const QString &key, const QString &newName)
{
    if ( key.isEmpty() || newName.isEmpty() || containsKey(newName) )
        return false;
    QVariant valueBackup = value(key);
    m_values.remove(key);
    m_values.insert(newName, valueBackup);
    return true;
}

void TreeNode::setParent(TreeNode *parent)
{
    m_pParent = parent;
}

void TreeNode::clearChildren()
{
    for(QList<TreeNode*>::iterator it = m_childrenList.begin(); it != m_childrenList.end(); ++it)
        delete (*it);
    m_childrenList.clear();
}

void TreeNode::clearKeys()
{
    m_values.clear();
}

bool TreeNode::hasChildren() const
{
    return !m_childrenList.isEmpty();
}

bool TreeNode::hasKeys() const
{
    return !m_values.isEmpty();
}

TreeNode *TreeNode::child(const QString &name) const
{
    if (name.isEmpty())
        return 0;

    for(QList<TreeNode*>::const_iterator it = m_childrenList.cbegin(); it != m_childrenList.cend(); ++it)
        if ( (*it)->name() == name)
            return (*it);
    return 0;
}

QStringList TreeNode::childrenNodes() const
{
    QStringList childrenNames;
    for(TreeNode *node : m_childrenList)
    {
        childrenNames.append(node->name());
    }
    return childrenNames;
}

QStringList TreeNode::childrenKeys() const
{
    QStringList childrenKeys;
    for(QMap<QString, QVariant>::const_iterator keyValue = m_values.cbegin(); keyValue != m_values.cend(); ++keyValue)
    {
        childrenKeys.append(keyValue.key());
    }
    return childrenKeys;
}

TreeNode *TreeNode::parent() const
{
    return m_pParent;
}

void TreeNode::destroy()
{
    clearKeys();
    clearChildren();
    m_pParent = 0;
}
