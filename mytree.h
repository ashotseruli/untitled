#ifndef MYTREE_H
#define MYTREE_H

#include<QString>
#include<QVariant>
class TreeNode;

// Класс, имплементирующий дерево.
// Поддерживает добавление и редактирование и удаление узлов и значений вида <ключ-значение> внутри узлов.

class MyTree
{
public:
    MyTree();
    ~MyTree();

    // запрещаем копирование
    MyTree(const MyTree&) = delete;
    void operator=(const MyTree&) = delete;

    // Добавляет узел на текущий уровень и возвращает true, если узла с таким именем нет на текущем уровне,
    // иначе оставит состояние без изменений и вернёт false
    bool addNodeToCurrent(TreeNode *node);

    // Добавляет в список значний текущего узла
    bool addKeyToCurrent(const QString &key, const QVariant &value);


    // Возвращает true, если у текущего узла есть потомок с именем name
    bool containsNode(const QString & name) const;

    // Возвращает true, если у текущего узла есть значение для ключа key
    bool containsKey(const QString &key) const;

    // Возвращает значение по ключу key в текущем узле
    QVariant value(const QString &key) const;

    // Возвращает список значений текущего узла
    QStringList childKeys() const;

    // Возвращает список имён потомков текущего узла
    QStringList childNodes() const;

    // Переименовывает  узел с именем oldName в newName и возвращает true
    // если на текущем уровне присутствует узел с именем oldName и отсутствует узел с именем newName,
    // иначе оставляет состояние без ищменений и возвращает false
    bool renameNode(const QString &oldName, const QString &newName);

    // Переименовывает текущий узел в newName, если не существует узлов с таким же именем среди родственников текущего узла
    bool renameCurrentNode(const QString &newName);

    // Переименовывает значение oldName в newName и возвращает true
    // если на текущем уровне присутствует значение с именем oldName и отсутствует значение с именем newName,
    // иначе оставляет состояние без изменений и возвращает false
    bool renameKey(const QString &oldName, const QString &newName);


    // Удаляет узел с именем name среди потомков текущего узла, если такой есть
    // вместе с узлом удаляются все его дочерние узлы и значения
    void deleteChildNode(const QString &name);

    // Удаляет текущий узел, если это не корень вместе со всеми потомками и значениями
    //Текущим становится предок удаляемого узла
    void deleteCurrentNode();

    // Удаляет значение с именем name из текущего узла, если такое есть
    void deleteKey(const QString &name);

    // Открывает папку с именем name, если такая есть в текущем расположении, либо создаёт новую, если нет
    void startGroup(const QString &name);

    // Переходит к родительскому узлу. Если достигнут корень - остаётся на нём
    void endGroup();

    // Возвращает имя текущего узла
    QString currentNodeName() const;

    // Устанавливает указатель текущего узла на старший по иерархии узел
    void resetToRoot();
private:
    // Инициализация дерева
    void init();
    // Уничтожение дерева
    void destroy();
private:
    // Указатель на текущий узел
    TreeNode *m_pCurrentNode;
    // Указатель на корневой узел дерева. Недоступен извне
    TreeNode *m_pRoot;
};

#endif // MYTREE_H
