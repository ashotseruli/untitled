#include "myitem.h"
#include<QDebug>

MyItem::MyItem(eTypeOfItem thisItemType): m_thisItemType(thisItemType)
{    
}

MyItem::MyItem(const QString &text, eTypeOfItem thisItemType): QStandardItem(text)
                                                             , m_PreviousValue(text)
                                                             , m_thisItemType(thisItemType)
{
}

MyItem::MyItem(const QIcon &icon, const QString &text, eTypeOfItem thisItemType): QStandardItem(icon, text)
                                                                                , m_PreviousValue(text)
                                                                                , m_thisItemType(thisItemType)
{
}

MyItem::MyItem(int rows, int columns, eTypeOfItem thisItemType): QStandardItem(rows, columns)
                                                               , m_thisItemType(thisItemType)
{
}

void MyItem::setData(const QVariant &value, int role)
{
    qDebug() << "value - " << value;
    m_PreviousValue = this->text();
    QStandardItem::setData(value, role);
}

QVariant MyItem::previousValue() const
{
    return m_PreviousValue;
}

eTypeOfItem MyItem::thisItemType() const
{
    return m_thisItemType;
}
