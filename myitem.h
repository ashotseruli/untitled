#ifndef MYITEM_H
#define MYITEM_H

#include<QStandardItem>
#include<QVariant>

enum eTypeOfItem
{
    folder,
    parameter
};

class MyItem : public QStandardItem
{

public:
    MyItem(eTypeOfItem thisItemType = folder);

    MyItem(const QString &text, eTypeOfItem thisItemType = folder);
    MyItem(const QIcon &icon, const QString &text, eTypeOfItem thisItemType = folder);
    MyItem(int rows, int columns = 1, eTypeOfItem thisItemType = folder);

    // Переопределение из QstandardIem
    virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);

    // Возвращает предыдущее значение
    QVariant previousValue() const;

    // Возвращает тип этого элемента
    eTypeOfItem thisItemType() const;

private:
    // предыдущее значение
    QVariant m_PreviousValue;
    // тип этого элемента
    eTypeOfItem m_thisItemType;
};

#endif // MYITEM_H
