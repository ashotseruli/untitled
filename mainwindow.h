#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// forward declarations
class QSettings;
class QString;
class QXmlStreamReader;
class QFile;
class QStandardItemModel;
class QStandardItem;

class MySettings;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:


private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_actionexit_triggered();

    void on_addButton_clicked();

    void on_deleteParamButton_clicked();

    void on_actionImport_triggered();

    void on_actionExport_triggered();

    void on_deleteFolderButton_clicked();

    void on_addFolderButton_clicked();

private:
    // инициализация формы и обработчиков
    void init();

    // загрузка данных о разделах в модель отображения. Прикрепляет к ветки модели AppendTo
    void loadDataToTreeViewModel(QStandardItem * appendTo = 0);

    // загрузка параметров в модель отображения
    void loadDataToParamViewModel();

    // вспомогательаня функция, прикрепляющая элемент append к appendTo (либо к model, если 0)
    void appendRowToViewModel(QStandardItemModel* model, QStandardItem * append, QStandardItem * appendTo = 0);

    // Создание нового параметра и добавление в текущую папку
    void addNewParameter();
    // удалить последний кликнутый параметр
    void deleteCurrentParameter();

    // Создание новой папки и добавление в текущую
    void addNewFolder();
    // удалить последнюю кликнутую папку
    void deleteCurrentFolder();

    // показывает окошко с сообщением
    void showMessage(const QString &message);

    void importFromXml();

    void exportToXml();

private:
    Ui::MainWindow *ui;

    // объект для получения данных из реестра/ini-файла
    MySettings *treeData;

    // модель отображения разделов
    QStandardItemModel *treeViewModel;

    // модель отображения параметров
    QStandardItemModel *paramViewModel;

    // читает из XML
    QXmlStreamReader *xmlReader;

    // путь к корневому элементу программы в реестре
    QString defaultRegPath;

};

#endif // MAINWINDOW_H
