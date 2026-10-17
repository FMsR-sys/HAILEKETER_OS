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

void MainWindow::find_all(QStringList const &REL_SUL , QStringList &result)
{
    //想换currentPath就改这里
    QString dirPATH = QCoreApplication::applicationDirPath();
    find(dirPATH, REL_SUL, result);
}
void MainWindow::find(QString const &PATH, const QStringList &REL_SUL, QStringList &result)
{
    QDir dir(PATH);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList entryList = dir.entryInfoList();

    for (const QFileInfo &info : entryList)
    {
        if (info.isDir())
        {
            find(info.absoluteFilePath(), REL_SUL, result);
        }
        else if (info.isFile())
        {
            QString suf = info.suffix().toLower();
            if (REL_SUL.contains(suf))
            {
                result.append(info.absoluteFilePath());
            }
        }
    }
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

    ui->ab->setText("当前目录：" + exe_PATH);

    QMessageBox::information(this,"设置完成","当前完整目录：" + exe_PATH);
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


void MainWindow::on_pushButton_4_clicked()
{
    QStringList fileResult;
    find_all(REL_SUL, fileResult);
    qDebug() << "扫描到文件列表：" << fileResult;
    // 新增：填充UI列表
    fillFileList(fileResult);
}

void MainWindow::fillFileList(const QStringList &absFileList)
{
    // 获取扫描根目录（exe所在目录）
    QString rootPath = QCoreApplication::applicationDirPath();
    QDir rootDir(rootPath);
    // 清空列表旧内容
    ui->abc->clear();

    for(const QString &absPath : absFileList)
    {
        // 计算相对路径
        QString relPath = rootDir.relativeFilePath(absPath);

        relPath.replace("/", "\\");

        // 创建列表条目
        QListWidgetItem *item = new QListWidgetItem(relPath, ui->abc);
        // 开启复选框
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        // 默认未勾选
        item->setCheckState(Qt::Unchecked);

        // 【关键】把完整绝对路径存到item的data里面，后面编译要用
        item->setData(Qt::UserRole, absPath);
    }
}
void MainWindow::on_pushButton_5_clicked()
{
    QStringList selected = get_check_file();
    qDebug() << "选中文件绝对路径：" << selected;
}

QStringList MainWindow::get_check_file()
{
    QStringList check;
    for (int i = 0 ; i < ui->abc->count() ; i++)
    {
        QListWidgetItem* item = ui->abc->item(i);
        if(item->checkState() == Qt::Checked)
        {
            QString z = item->data(Qt::UserRole).toString();
            check << z;
        }
    }
    return check;
}