#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "set_suf.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    check_tools();
    REL_SUL = {"cpp" , "asm" , "hpp" , "ld"};
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::add_sul(QString sul)
{
    sul = sul.toLower();
    if(!REL_SUL.contains(sul))
    {
        REL_SUL.append(sul);
        return true;
    }
    return false;
}

bool MainWindow::del_sul(QString sul)
{
    sul = sul.toLower();
    int ret = REL_SUL.removeAll(sul);
    return ret > 0;
}

void MainWindow::on_pushButton_clicked()
{
    QString exe_PATH = QCoreApplication::applicationFilePath();
    QString exe_Dir = QCoreApplication::applicationDirPath();

    ui->label->setText("当前目录：" + exe_PATH + "/" + exe_Dir);

    QMessageBox::information(this,"设置完成","当前完整目录：" + exe_PATH + "/" + exe_Dir);
}

bool MainWindow::check_tools()
{
    bool allOk = true;

    // 检测 NASM
    QString nasm = "nasm";
    QProcess proc;
    proc.start(nasm,QStringList() << "--version");
    bool nasmOk = false;
    if (proc.waitForFinished(1000))
    {
        QString output = proc.readAllStandardOutput();
        QString err = proc.readAllStandardError();
        QString all_output = output + err;
        if(!all_output.isEmpty())
        {
            nasmOk = true;
        }
    }
    if(nasmOk)
    {
        ui->NASM->setText("<font color='green'>NASM状态：已安装</font>");
    }
    else
    {
        ui->NASM->setText("<font color='red'>NASM状态：未安装</font>");
        allOk = false;
    }

    // 检测 i686-elf-gcc
    QString gcc = "i686-elf-gcc";
    QProcess prod;
    prod.start(gcc,QStringList() << "--version");
    bool gccOk = false;
    if (prod.waitForFinished(1000))
    {
        QString output = prod.readAllStandardOutput();
        QString err = prod.readAllStandardError();
        QString all_output = output + err;
        if(!all_output.isEmpty())
        {
            gccOk = true;
        }
    }
    if(gccOk)
    {
        ui->GCC->setText("<font color='green'>i686-elf-gcc状态：已安装</font>");
    }
    else
    {
        ui->GCC->setText("<font color='red'>i686-elf-gcc状态：未安装</font>");
        allOk = false;
    }

    // 检测 QEMU
    QString qemu = "qemu-system-i386";
    QProcess qemuProc;
    qemuProc.start(qemu, QStringList() << "--version");
    bool qemuOk = false;
    if(qemuProc.waitForFinished(2000))
    {
        QString output = qemuProc.readAllStandardOutput();
        QString err = qemuProc.readAllStandardError();
        QString all_output = output + err;
        if(!all_output.isEmpty())
        {
            qemuOk = true;
        }
    }
    if(qemuOk)
    {
        ui->QEMU->setText("<font color='green'>qemu-system-i386状态: 已安装</font>");
    }
    else
    {
        ui->QEMU->setText("<font color='red'>qemu-system-i386状态: 未安装</font>");
        allOk = false;
    }

    // 全部检测完，统一返回
    return allOk;
}


void MainWindow::on_pushButton_2_clicked()
{
    check_tools();
}

void MainWindow::on_pushButton_3_clicked()
{
    set_suf dlg(this);
    dlg.set_suf_list(REL_SUL);

    if(dlg.exec() == QDialog::Accepted)
    {
        REL_SUL = dlg.get_suf();
    }
}

