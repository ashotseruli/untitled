#ifndef TREENODE_H
#define TREENODE_H

#include<QList>
#include<QVariant>
#include<QString>
#include<QMap>

class TreeNode
{
public:
    // name - имя узла
    // parent - указатель на родительский узел
    TreeNode(const QString &name, TreeNode *parent = 0);

    // Запрещаем копирование
    TreeNode(const TreeNode&) = delete;
    void operator=(const TreeNode&) = delete;

    // Добавление дочернего узла. Если узел с именем, как у child уже есть,
    // либо указатель нулевой, оставит состояние неизменным и вернет false
    // Родителем для child устанавливается текущий узел
    bool addChild(TreeNode * child);

    // Добавление значения в текущий узел. Если уже есть узел с ключом key, либо ключ пустой,
    // оставит состояние неизменным и вернет false
    bool addValue(const QString &key, const QVariant &value);

    // Удаляет дочерний узел с именем name, если такой есть
    void deleteChild(const QString &name);

    // Удаляет ключ с именем name, если такой есть
    void deleteKey(const QString &key);

    // Возвращает true, если содержит значение с ключом key
    bool containsKey(const QString &key) const;

    // Возвращает true, если среди потомков есть узел с именем name
    bool containsChild(const QString name) const;

    // Устанавливает имя узла в newName (кроме пустой строки). Вернёт true в случае успеха
    bool setName(const QString &newName);

    // Устанавливает новое имя newName для ключа key
    // Если ключ с таким именем уже существует, оставит состояние без зименения и вернет false
    bool renameKey(const QString &key, const QString &newName);

    // Устанавливает родителя узла в parent (0 = корень)
    //  Операция на предке не отражается
    void setParent(TreeNode *parent);

    // Рекурсивно удаляет всех потомков узла вместе с их значениями и очищает список потомков
    void clearChildren();

    // Удаляет все значения узла
    void clearKeys();

    // Возвращает true, если имеет потомков
    bool hasChildren() const;

    // Возвращает true, если имеет значения
    bool hasKeys() const;

    // Возвращает указатель на потомка текущего узла с именем name
    // либо 0, если такого потомка нет
    TreeNode *child(const QString &name) const;

    // Возвращает список имён потомков узла
    QStringList childrenNodes() const;

    // Возвращает список имён значений узла
    QStringList childrenKeys() const;

    // Возвращает указатель на родитель узла.
    TreeNode *parent() const;

    // Возвращает имя узла
    QString name() const;

    // Возвращает значение, соответствующее ключу key. Если такого ключа нет, вернет пустую строку.
    QVariant value(const QString &key) const;

    ~TreeNode();
private:
    void destroy();
    QString m_name;
private:
    // Указатель на родительский узел
    TreeNode *m_pParent = 0;
    // Список узлов-потомков
    QList<TreeNode*> m_childrenList;
    // Список значений внутри узла
    QMap<QString, QVariant> m_values;
};
#endif // TREENODE_H
