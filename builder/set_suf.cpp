#include "set_suf.h"
#include "ui_set_suf.h"

set_suf::set_suf(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::set_suf)
{
    ui->setupUi(this);
    m_suf = {};
}

set_suf::~set_suf()
{
    delete ui;
}

void set_suf::on_pushButton_clicked()
{
    bool ok;
    QString new_suf = QInputDialog::getText(this , "新增白名单后缀" , "请输入后缀（禁止带“.”）：" , QLineEdit::Normal , "" , &ok);
    if (!ok)
    {
        QMessageBox::information(this , "提示" , "用户取消操作");
        return;
    }

    new_suf = new_suf.trimmed().toLower();
    if (new_suf.isEmpty())
    {
        QMessageBox::warning(this , "错误" , "后缀不能为空");
        return;
    }
    if (m_suf.contains(new_suf))
    {
        QMessageBox::warning(this , "错误" , "后缀已存在");
        return;
    }
    m_suf.append(new_suf);
    refresh_suf();
}

void set_suf::refresh_suf()
{
    ui->listWidget->clear();
    for (const QString& s : m_suf)
    {
        ui->listWidget->addItem(s);
    }
    return;
}

QStringList set_suf::get_suf()
{
    return m_suf;
}

void set_suf::on_buttonBox_accepted()
{
    accept();
}

void set_suf::set_suf_list(const QStringList &lst)
{
    m_suf = lst;
    refresh_suf();
    return;
}
void set_suf::on_pushButton_2_clicked()
{
    bool ok;
    QString def_suf = QInputDialog::getText(this , "删除白名单后缀" , "请输入后缀（禁止带“.”）：" , QLineEdit::Normal , "" , &ok);
    if (!ok)
    {
        QMessageBox::information(this, "提示" ,"用户取消操作");
        return;
    }
    def_suf = def_suf.trimmed().toLower();
    if(def_suf.isEmpty())
    {
        QMessageBox::warning(this , "错误" , "后缀不能为空");
        return;
    }
    m_suf.removeAll(def_suf);
    refresh_suf();
    return;
}

