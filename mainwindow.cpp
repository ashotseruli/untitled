#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QDebug>///УБРАТЬ
#include<QStandardItem>
#include<QFile>
#include<QMessageBox>
#include<QSettings>
#include<QXmlStreamReader>
#include<QStandardItemModel>
#include<QFileDialog>

#include"xmlhandler.h"
#include"mysettings.h"
#include"myitem.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    treeData = new MySettings(QSettings::NativeFormat, QSettings::SystemScope, "NtcMks", "TestTree", this);

    // загрузка в модель представления дерева каталогов
    treeViewModel = new QStandardItemModel(this);
    loadDataToTreeViewModel();

    // отображение дерева каталогов в представлении
    ui->treeView->setModel(treeViewModel);
    ui->treeView->expandAll();

    paramViewModel = new QStandardItemModel(this);
    paramViewModel->setColumnCount(3);
    loadDataToParamViewModel();

    QList<QString> headers;
    headers << "name" << "type" << "value";
    paramViewModel->setHorizontalHeaderLabels(headers);
    ui->paramView->setModel(paramViewModel);
    ui->paramView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->paramView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->paramView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    ui->treeView->setCurrentIndex(treeViewModel->index(0,0));
    ///
    connect(treeViewModel, SIGNAL(itemChanged(QStandardItem*)), treeData, SLOT(changeItem(QStandardItem*)));
    connect(paramViewModel, SIGNAL(itemChanged(QStandardItem*)), treeData, SLOT(changeItem(QStandardItem*)));
}

void MainWindow::loadDataToTreeViewModel(QStandardItem * appendTo)
{
    // рекурсивно загружаем разделы
    QStringList childrenList = treeData->childGroups();
    for (QString &groupName: childrenList)
    {
        QStandardItem *item = new MyItem(groupName, folder);
        appendRowToViewModel(treeViewModel, item, appendTo);
        treeData->beginGroup(groupName);
            loadDataToTreeViewModel(item);
        treeData->endGroup();
    }
}

void MainWindow::loadDataToParamViewModel()
{
    // очистка таблицы
    paramViewModel->removeRows(0, paramViewModel->rowCount());
    //загружаем листья
    QStringList childrenList = treeData->childKeys();

    //загрузка параметров
    for (QString &paramName: childrenList)
    {
        QStandardItem *itemName = new MyItem(paramName, parameter);
        QStandardItem *itemType = new QStandardItem(treeData->value(paramName).typeName());
        itemType->setEditable(false);
        QStandardItem *itemValue = new MyItem(treeData->value(paramName).toString(), parameter);
        QList<QStandardItem *> items;
        items.push_back(itemName);
        items.push_back(itemType);
        items.push_back(itemValue);
        paramViewModel->appendRow(items);
    }
}

void MainWindow::appendRowToViewModel(QStandardItemModel* model, QStandardItem *append, QStandardItem *appendTo)
{
    if (!append || !model)
        return;
    if (appendTo)
        appendTo->appendRow(append);
    else
        model->invisibleRootItem()->appendRow(append);
}

void MainWindow::addNewParameter()
{
    // должна была быть выбрана папка, где создаётся параметр
    if (!(ui->treeView->currentIndex().isValid()))
        return;

    QStringList itemParents = treeData->currentFolderPath();
    // доходим до нужного раздела
    for (QString &parent: itemParents)
        treeData->beginGroup(parent);

    // исключим возможность создания параметра с существующим именем
    bool hasSuchName = false;
    int paramNumber = 0;
    QString name = "new param(";
    do
    {
        hasSuchName = false;
        ++paramNumber;
        for (QString &paramName : treeData->childKeys())
        {
            if ( paramName == QString(name + QString::number(paramNumber) + ")") )
            {
                hasSuchName = true;
                break;
            }
        }
    } while(hasSuchName);
    name += QString::number(paramNumber);
    name += ")";

    QStandardItem *itemName = new MyItem(name, parameter);
    QStandardItem *itemType = new QStandardItem("QString");
    itemType->setEditable(false);
    QString value = "0";
    QStandardItem *itemValue = new MyItem(value);
    QList<QStandardItem *> items;
    items.push_back(itemName);
    items.push_back(itemType);
    items.push_back(itemValue);
    paramViewModel->appendRow(items);

    // установка значения
    treeData->setValue(name, value);

    // возвращаемся к исходному разделу
    for (int i = 0; i < itemParents.count(); i++)
        treeData->endGroup();
}

void MainWindow::deleteCurrentParameter()
{
    QModelIndex paramIndex = ui->paramView->currentIndex();
    if (!paramIndex.isValid())
        return;
    QStandardItem *paramItem = paramViewModel->item(paramIndex.row(), 0);
    QString key = paramItem->text(); // имя текущего параметра

    // удаляем элемент из модели отборажения
    paramViewModel->removeRows(paramIndex.row(), 1);

    // получаем список предков кликнутого узла
    QStringList itemParents = treeData->currentFolderPath();
    // доходим до нужного раздела
    for (QString &parent: itemParents)
        treeData->beginGroup(parent);

    // удаляем элемент из хранилища
    treeData->remove(key);
    treeData->sync();

    // возвращаемся к исходному разделу
    for (int i = 0; i < itemParents.count(); i++)
        treeData->endGroup();
}

void MainWindow::addNewFolder()
{
    QModelIndex currentItemIndex = ui->treeView->currentIndex();

    QStringList itemParents = treeData->currentFolderPath();
    // доходим до нужного раздела
    for (QString &parent: itemParents)
        treeData->beginGroup(parent);

    // исключим возможность создания параметра с существующим именем
    bool hasSuchName = false;
    int folderNumber = 0;
    QString name = "new folder(";
    do
    {
        hasSuchName = false;
        ++folderNumber;
        for (QString &groupName : treeData->childGroups())
        {
            if ( groupName == QString(name + QString::number(folderNumber) + ")") )
            {
                hasSuchName = true;
                break;
            }
        }
    } while(hasSuchName);
    name += QString::number(folderNumber);
    name += ")";

    QStandardItem *item= new MyItem(name, parameter);
    appendRowToViewModel(treeViewModel, item, treeViewModel->itemFromIndex(currentItemIndex));

    treeData->beginGroup(name);
    // установка значения, чтобы создалась папка
    treeData->setValue("1", 0);
    treeData->remove("1");
    treeData->endGroup();

    // возвращаемся к исходному разделу
    for (int i = 0; i < itemParents.count(); i++)
        treeData->endGroup();
}

void MainWindow::deleteCurrentFolder()
{
    QModelIndex treeIndex = ui->treeView->currentIndex();
    if (!treeIndex.isValid())
        return;
    // получаем список предков кликнутого узла
    QStringList itemParents = treeData->currentFolderPath();
    // удаляем элемент из модели отборажения
    treeViewModel->removeRow(treeIndex.row(), treeIndex.parent());

    // доходим до нужного раздела
    for (QString &parent: itemParents)
        treeData->beginGroup(parent);
    treeData->endGroup();
    // удаляем элемент из хранилища
    treeData->remove(itemParents.back());
    treeData->sync();
    // актуализируем список предков
    itemParents.pop_back();
    loadDataToParamViewModel();

    // возвращаемся к исходному разделу
    for (int i = 0; i < itemParents.count(); i++)
        treeData->endGroup();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qDebug() << "12312";
    }
    QStandardItem * item = treeViewModel->itemFromIndex(index);
    qDebug() << "treeindexROW = " << index.row();
    qDebug() << "treeindexCOLUMN = " << index.column();
    qDebug() << "treeindexDATA = " << index.data().toString();
    // получаем список предков кликнутого узла
    QStringList itemParents;
    itemParents.push_front(item->text());
    while(item->parent() != 0)
    {
        itemParents.push_front(item->parent()->text());
        item = item->parent();
    }
    // путь с высокой вероятностью еще пригодится, поэтому сохраним, чтобы не вычислять несколько раз
    treeData->setCurrentParameterParents(itemParents);

    // доходим до нужного раздела
    for (QString &parent: itemParents)
        treeData->beginGroup(parent);
    // загружаем параметры в модель отображения
    loadDataToParamViewModel();

    // возвращаемся к исходному разделу
    for (int i = 0; i < itemParents.count(); i++)
        treeData->endGroup();
}

void MainWindow::showMessage(const QString &message)
{
    QMessageBox messageBox;
    messageBox.setText(message);
    messageBox.exec();
}

void MainWindow::importFromXml()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open XML"), "", tr("XML files (*.xml)"));
    if (filePath.isEmpty())
        return;
    QStringList nodes;
    QStringList currentFolderPath = treeData->currentFolderPath();
    if (!currentFolderPath.isEmpty())
        nodes.append(currentFolderPath.at(0));
    else
        nodes = treeData->childGroups();
    treeData->makeTree(nodes);

    XmlHandler xmlHandler;
    QString message;
    if (xmlHandler.loadXmlToSettingsTree(filePath, treeData))
    {
        message = "Импорт успешно завершен";
    }
    else
    {
        treeData->clear();
        treeData->restoreFromTree();
        message = xmlHandler.getLastError();
    }

    // перезагружаем новые данные в модель
    treeViewModel->clear();
    loadDataToTreeViewModel();
    ui->treeView->expandAll();
    showMessage(message);
}

void MainWindow::exportToXml()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Open XML"), "", tr("XML files (*.xml)")); //"c://mytree.xml";
    if (filePath.isEmpty())
        return;
    XmlHandler xmlHandler;
    if (xmlHandler.loadSettingsTreeToXml(filePath, treeData))
        showMessage("Экспорт успешно завершен");
    else
        showMessage(xmlHandler.getLastError());
}

void MainWindow::on_actionexit_triggered()
{
    this->close();
}

void MainWindow::on_addButton_clicked()
{
    addNewParameter();
}

void MainWindow::on_deleteParamButton_clicked()
{
    deleteCurrentParameter();
}

void MainWindow::on_actionImport_triggered()
{
    qDebug("Import triggered");
     importFromXml();
}

void MainWindow::on_actionExport_triggered()
{
    qDebug("Export triggered");
    exportToXml();
}

void MainWindow::on_deleteFolderButton_clicked()
{
    deleteCurrentFolder();
}

void MainWindow::on_addFolderButton_clicked()
{
    addNewFolder();
}
