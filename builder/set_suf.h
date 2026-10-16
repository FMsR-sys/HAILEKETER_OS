#ifndef SET_SUF_H
#define SET_SUF_H

#include <QDialog>
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>

namespace Ui {
class set_suf;
}

class set_suf : public QDialog
{
    Q_OBJECT

public:
    explicit set_suf(QWidget *parent = nullptr);
    ~set_suf();
    QStringList get_suf();
    void set_suf_list(const QStringList &lst);

private slots:
    void on_pushButton_clicked();
    void on_buttonBox_accepted();
    void refresh_suf();

    void on_pushButton_2_clicked();

private:
    Ui::set_suf *ui;
    QStringList m_suf;
};

#endif // SET_SUF_H
