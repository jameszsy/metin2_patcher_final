#include "metin2_patcher.h"
#include "ui_metin2_patcher.h"

metin2_patcher::metin2_patcher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::metin2_patcher)
{
    ui->setupUi(this);

    ui->button_config->setVisible(false);
    ui->button_start->setVisible(false);

    file_get = 0;
    dir_get = 0;
    file_dow = 0;
    licz_dow = 0;
    dow_get = 0;
    sprawdz_pliki();

    file = new QFile("log_ftp.txt");
    file->open(QIODevice::WriteOnly);

    file_dir = new QFile("log_dir.txt");
    file_dir->open(QIODevice::WriteOnly);

    ftp = new QFtp(this);
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(dataProcess(qint64, qint64)));
    connect(ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(printFinished(int, bool)));

    ftp->connectToHost("ip.serwera", 21);
    ftp->login("login", "hasło");
    ftp->cd("nazwa folderu z plikami");
    file_get = ftp->get("log_ftp.txt", file);
}

metin2_patcher::~metin2_patcher()
{
    delete ui;
}

void metin2_patcher::dataProcess(qint64 act, qint64 size)
{
    if(size != 0)
    {
        QString rozmiar = QString::number((act / 1024) / 1024) + "mb z " + QString::number((size / 1024) / 1024) + "mb";
        ui->file_size->setText(rozmiar);
        ui->progressBar->setValue((act * 100) / size);
    }
    //qDebug() << "Pobrano : " << act << " z : " << size;
}

void metin2_patcher::printFinished(int id, bool error)
{
    if(id == file_get)
    {
        file->close();
        dir_get = ftp->get("log_dir.txt", file_dir);
    }

    if(id == dir_get)
    {
        file_dir->close();
        check_log();
    }

    if(id == file_dow)
    {
        download->close();
        dow_get += 1;

        next_download_file();
    }
}

void metin2_patcher::next_download_file()
{
    for(int s = dow_get; s < 1024; s++)
    {
        if(spr[s] == true && check_all_log_name[s] != "")
        {
            download_patch_file(check_all_log_name[s]);
            qDebug() << "licz_dow_next = " << s;
            QString log_str = "Trwa pobieranie " + check_all_log_name[s] + "...";
            QScrollBar *defil = ui->listWidget->verticalScrollBar();
            ui->listWidget->addItem(new QListWidgetItem(log_str));
            defil->setValue(defil->maximum());
            ui->file_name->setText(check_all_log_name[s]);
            dow_get = s;

            return;
        }
    }

    ui->button_config->setVisible(true);
    ui->button_start->setVisible(true);
}

void metin2_patcher::check_log()
{
    QFile list_dir("log_dir.txt");
    QFile check("log_ftp.txt");
    QFile check2("log.txt");

    if(!check.open(QIODevice::ReadOnly))
        qDebug() << "Nie można otworzyć pliku log_ftp.txt";

    if(!check2.open(QIODevice::ReadOnly))
        qDebug() << "Nie można otworzyć pliku log.txt";

    if(!list_dir.open(QIODevice::ReadOnly))
        qDebug() << "Nie można otworzyć pliku log_dir.txt";

    QTextStream in(&check);
    QTextStream in_2(&check2);
    QTextStream in_dir(&list_dir);

    while(!in_dir.atEnd())
    {
        QString line = in_dir.readLine();

        if(line != "")
        {
            QDir dir(line);

            if(!dir.exists())
                dir.mkpath(line);
        }
    }

    int licznik = 0;

    QString check_all_log_size[1024];
    QString check_all_log_sum[1024];

    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList list = line.split("\t");

        check_all_log_name[licznik] = list.at(0);
        check_all_log_size[licznik] = list.at(1);
        check_all_log_sum[licznik] = list.at(2);
        licznik++;
    }

    licznik = 0;
    QString check_2_all_log_name[1024];
    QString check_2_all_log_size[1024];
    QString check_2_all_log_sum[1024];

    while(!in_2.atEnd())
    {
        QString line = in_2.readLine();
        QStringList list = line.split("\t");

        check_2_all_log_name[licznik] = list.at(0);
        check_2_all_log_size[licznik] = list.at(1);
        check_2_all_log_sum[licznik] = list.at(2);
        licznik++;
    }

    for(int i = 0; i < 1024; i++)
    {
        spr[i] = true;
    }

    for(int i = 0; i < 1024; i++)
    {
        for(int j = 0; j < 1024; j++)
        {
            if(check_all_log_name[i] == check_2_all_log_name[j] &&
               check_all_log_size[i] == check_2_all_log_size[j] &&
               check_all_log_sum[i] == check_2_all_log_sum[j])
            {
                spr[i] = false;
            }
        }
    }

    for(int k = 0; k < 1024; k++)
    {
        if(spr[k] == true && check_all_log_name[k] != ""){
            download_patch_file(check_all_log_name[k]);
            dow_get = k;
            qDebug() << "licz_dow = " << dow_get;
            QString log_str = "Trwa pobieranie " + check_all_log_name[k] + "...";
            ui->file_name->setText(check_all_log_name[k]);
            ui->listWidget->addItem(new QListWidgetItem(log_str));
            return;
        }
    }

    ui->button_config->setVisible(true);
    ui->button_start->setVisible(true);

    check.close();
    check2.close();
}

void metin2_patcher::sprawdz_pliki()
{
    QFile log("log.txt");
    if(!log.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Nie można otworzyć pliku log.txt";
    }

    QTextStream out(&log);

    QString lnk = "";

    QDir dir(lnk);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);             // Ustawienie filtrów
    dir.setSorting(QDir::Name);                                                                     // Ustawienie sortowania

    QFileInfoList dir_list = dir.entryInfoList();                                                   // Utworzenie listy folderów

    for(int i = 0; i < dir_list.size(); ++i)
    {
        QFileInfo dirInfo = dir_list.at(i);

        QString lnk_file = lnk + dirInfo.fileName();

        QDir dir2(lnk_file);

        dir2.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir2.setSorting(QDir::Name);

        QFileInfoList file_list = dir2.entryInfoList();

        for(int j = 0; j < file_list.size(); ++j)
        {
            QFileInfo fileInfo = file_list.at(j);
            QString lnk_file_sum = lnk_file + "/" + fileInfo.fileName();
            QFile *file = new QFile(lnk_file_sum);
            file->open(QIODevice::ReadOnly);

            QByteArray byte = file->read(524288);

            quint16 crcSum = qChecksum(byte.data(), byte.length());

            file->close();

            out << lnk_file << "/" << fileInfo.fileName() << "\t" << fileInfo.size() << "\t" << QString::number(crcSum, 16) << endl;
        }
    }

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QFileInfoList file_default_folder = dir.entryInfoList();

    for(int k = 0; k < file_default_folder.size(); ++k)
    {
        QFileInfo fileInfo = file_default_folder.at(k);
        QFile *file = new QFile(lnk + fileInfo.fileName());
        file->open(QIODevice::ReadOnly);
        QByteArray byte = file->read(524288);

        quint16 crcSum = qChecksum(byte.data(), byte.length());

        file->close();

        if(fileInfo.fileName() != "log.txt" &&
           fileInfo.fileName() != "QtCore4.dll" &&
           fileInfo.fileName() != "QtNetwork4.dll" &&
           fileInfo.fileName() != "libgcc_s_dw2-1.dll" &&
           fileInfo.fileName() != "QtGui4.dll" &&
           fileInfo.fileName() != "log_ftp.txt")
            out << lnk << fileInfo.fileName() << "\t" << fileInfo.size() << "\t" << QString::number(crcSum, 16) << endl;
    }

    log.close();
}

void metin2_patcher::download_patch_file(QString name)
{
    qDebug() << "Download : " << name;

    download = new QFile(name);

    download->open(QIODevice::WriteOnly);

    file_dow = ftp->get(name, download);
}

void metin2_patcher::on_button_home_clicked()
{
    system("start nazwa_strony_www.pl");
}

void metin2_patcher::on_button_start_clicked()
{
    QProcess::startDetached("plik.exe");
}

void metin2_patcher::on_button_config_clicked()
{
    QProcess::startDetached("plik.exe");
}
void metin2_patcher::on_button_register_clicked()
{
    system("start nazwa_strony_www.pl");
}

void metin2_patcher::on_button_ranking_clicked()
{
    system("start nazwa_strony_www.pl");
}

void metin2_patcher::on_button_forum_clicked()
{
    system("start nazwa_strony_www.pl");
}

void metin2_patcher::on_button_support_clicked()
{
    system("start nazwa_strony_www.pl");
}

void metin2_patcher::on_button_exit_clicked()
{
    close();
}
