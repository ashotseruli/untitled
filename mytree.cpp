#include "mytree.h"

#include"treenode.h"

MyTree::MyTree()
{
    init();
}

MyTree::~MyTree()
{
    destroy();
}

bool MyTree::addNodeToCurrent(TreeNode *node)
{
    bool result = m_pCurrentNode->addChild(node);
    if (result)
        m_pCurrentNode = node;
    return result;

}

bool MyTree::addKeyToCurrent(const QString &key, const QVariant &value)
{
    return m_pCurrentNode->addValue(key, value);
}

bool MyTree::containsNode(const QString &name) const
{
    return m_pCurrentNode->containsChild(name);
}

bool MyTree::containsKey(const QString &key) const
{
    return m_pCurrentNode->containsKey(key);
}

QVariant MyTree::value(const QString &key) const
{
    return m_pCurrentNode->value(key);
}

QStringList MyTree::childKeys() const
{
    return m_pCurrentNode->childrenKeys();
}

QStringList MyTree::childNodes() const
{
    return m_pCurrentNode->childrenNodes();
}

bool MyTree::renameNode(const QString &oldName, const QString &newName)
{
    if (m_pCurrentNode->containsChild(newName))
        return false;
    return m_pCurrentNode->child(oldName)->setName(newName);
}

bool MyTree::renameCurrentNode(const QString &newName)
{
    if (m_pCurrentNode->parent() && m_pCurrentNode->parent()->containsChild(newName))
        return false;
    return m_pCurrentNode->setName(newName);
}

bool MyTree::renameKey(const QString &oldName, const QString &newName)
{
    return m_pCurrentNode->renameKey(oldName, newName);
}

void MyTree::deleteChildNode(const QString &name)
{
    if (!m_pCurrentNode->containsChild(name))
        return;
    delete m_pCurrentNode->child(name);
    m_pCurrentNode->deleteChild(name);
}

void MyTree::deleteCurrentNode()
{
    if (m_pCurrentNode == m_pRoot)
        return;
    TreeNode* parentNode = m_pCurrentNode->parent();
    delete m_pCurrentNode;
    m_pCurrentNode = parentNode;
}

void MyTree::deleteKey(const QString &name)
{
    m_pCurrentNode->deleteKey(name);
}

void MyTree::startGroup(const QString &name)
{
    if (name.isEmpty())
        return;

    TreeNode *group = m_pCurrentNode->child(name);
    if (0 != group)
        m_pCurrentNode = group;
    else
    {
        group = new TreeNode(name, m_pCurrentNode);
        m_pCurrentNode->addChild(group);
    }
}

void MyTree::endGroup()
{
    if (m_pCurrentNode != m_pRoot)
        m_pCurrentNode = m_pCurrentNode->parent();
}

QString MyTree::currentNodeName() const
{
    return m_pCurrentNode->name();
}

void MyTree::resetToRoot()
{
    // у корневого узла будет только 1 потомок - фактически корень с данными
    if (m_pRoot->hasChildren())
        m_pCurrentNode = m_pRoot->child(m_pRoot->childrenNodes().at(0));
    else
        m_pCurrentNode = m_pRoot;
}

void MyTree::init()
{
    m_pRoot = new TreeNode("root", 0);
    m_pCurrentNode = m_pRoot;
}

void MyTree::destroy()
{
    delete m_pRoot;
    m_pRoot = 0;
    m_pCurrentNode = 0;
}
