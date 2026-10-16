#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QStringList>
#include <QProcessEnvironment>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    QStringList REL_SUL;

private slots:
    bool add_sul(QString sul);
    bool del_sul(QString sul);

    void on_pushButton_clicked();
    bool check_tools();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
