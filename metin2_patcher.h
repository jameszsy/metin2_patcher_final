#ifndef METIN2_PATCHER_H
#define METIN2_PATCHER_H

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QFtp>
#include <QByteArray>
#include <QEventLoop>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QProcess>
#include <QScrollBar>

namespace Ui {
class metin2_patcher;
}

class metin2_patcher : public QMainWindow
{
    Q_OBJECT

public:
    explicit metin2_patcher(QWidget *parent = 0);
    int file_get;
    int dir_get;
    int file_dow;
    int licz_dow;
    int dow_get;
    ~metin2_patcher();

private slots:
    void dataProcess(qint64 act, qint64 size);
    void printFinished(int id, bool erro);
    void check_log();
    void sprawdz_pliki();
    void download_patch_file(QString name);
    void next_download_file();

    void on_button_home_clicked();

    void on_button_start_clicked();

    void on_button_config_clicked();

    void on_button_register_clicked();

    void on_button_ranking_clicked();

    void on_button_forum_clicked();

    void on_button_support_clicked();

    void on_button_exit_clicked();

private:
    Ui::metin2_patcher *ui;
    QFtp *ftp;
    QFile *file;
    QFile *file_dir;
    QFile *download;
    QString check_all_log_name[1024];
    bool spr[1024];
};

#endif // METIN2_PATCHER_H
