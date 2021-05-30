#include "Dialog.h"
#include "ui_Dialog.h"
#include <QFile>
#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QClipboard>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //选择待编码文件
    connect(ui->pushButton_selectEncodeSource,&QPushButton::clicked,this,[=]()
    {
        //支持自动定位到弹窗原来选择的文件路径

        QFileInfo oldFileInfo(ui->lineEdit_encodeSourceFile->text());
        QString defaultFilePath = oldFileInfo.absolutePath().isEmpty()?
                    QApplication::applicationDirPath():oldFileInfo.absolutePath();
        QString fileName = QFileDialog::getOpenFileName(this,
            QString("请选择待编码源文件"), defaultFilePath);
        ui->lineEdit_encodeSourceFile->setText(fileName);
        ui->lineEdit_encodeSourceFile->setToolTip(fileName);
    },Qt::QueuedConnection);

    //单击编码按钮
    connect(ui->pushButton_encode,&QPushButton::clicked,this,[=]()
    {
        ui->pushButton_encode->setDisabled(true);

        //输出文件已存在则先remove掉
        QFileInfo outputFileInfo;
        outputFileInfo.setFile(QApplication::applicationDirPath(),ui->lineEdit_encodeOutputFile->text());
        QFile output(outputFileInfo.absoluteFilePath());
        if(output.exists())
        {
            output.remove();
        }
        if(!output.open(QIODevice::Append))
        {
            QString messsage = QString("文件打开失败.文件名:%1.错误信息:%2 \n").arg(output.fileName()).arg(output.errorString());
            ui->textBrowser_encodeResult->append(messsage);
            return;
        }
        QDataStream out(&output);

        QFile input(ui->lineEdit_encodeSourceFile->text());
        if(!input.open(QIODevice::ReadWrite))
        {
            QString messsage = QString("文件打开失败.文件名:%1.错误信息:%2 \n").arg(input.fileName()).arg(input.errorString());
            ui->textBrowser_encodeResult->append(messsage);
            return;
        }
        QDataStream in(&input);

        QString strBuffer;
        while(!input.atEnd())
        {
            QByteArray byteArray = input.readAll();
            QByteArray base64ByteArray = byteArray.toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
            output.write(base64ByteArray);

            if(ui->checkBox_toClipBoard->checkState() == Qt::CheckState::Checked)
            {
                QClipboard * cb = QApplication::clipboard();
                cb->setText(base64ByteArray);
            }
        }

        input.close();
        output.close();

        //写入到Base64解码文件源路径
        ui->lineEdit_decodeSourceFile->setText(outputFileInfo.absoluteFilePath());

        QString messsage;
        if(ui->checkBox_toClipBoard->checkState() == Qt::CheckState::Checked)
        {
            messsage = QString("Base64编码成功,编码后文件内容已写入剪切板\n");
        }
        else
        {
            messsage = QString("Base64编码成功\n");
        }
        ui->textBrowser_encodeResult->append(messsage);
        ui->pushButton_encode->setDisabled(false);
    },Qt::QueuedConnection);

    //单击打开目录
    connect(ui->pushButton_openEncodeDirectory,&QPushButton::clicked,this,[=]()
    {
        QDesktopServices::openUrl(QUrl(QApplication::applicationDirPath()));
    },Qt::QueuedConnection);


    //选择待编码文件
    connect(ui->pushButton_selectDecodeSource,&QPushButton::clicked,this,[=]()
    {
        //支持自动定位到弹窗原来选择的文件路径

        QFileInfo oldFileInfo(ui->lineEdit_decodeSourceFile->text());
        QString defaultFilePath = oldFileInfo.absolutePath().isEmpty()?
                    QApplication::applicationDirPath():oldFileInfo.absolutePath();
        QString fileName = QFileDialog::getOpenFileName(this,
            QString("请选择待解码源文件"), defaultFilePath);
        ui->lineEdit_decodeSourceFile->setText(fileName);
        ui->lineEdit_decodeSourceFile->setToolTip(fileName);
    },Qt::QueuedConnection);

    //单击解码按钮
    connect(ui->pushButton_decode,&QPushButton::clicked,this,[=]()
    {
        ui->pushButton_decode->setDisabled(true);

        //输出文件已存在则先remove掉
        QFileInfo outputFileInfo;
        outputFileInfo.setFile(QApplication::applicationDirPath(),ui->lineEdit_decodeOutputFile->text());
        QFile output(outputFileInfo.absoluteFilePath());
        if(output.exists())
        {
            output.remove();
        }

        if(!output.open(QIODevice::Append))
        {
            QString messsage = QString("文件打开失败.文件名:%1.错误信息:%2 \n").arg(output.fileName()).arg(output.errorString());
            ui->textBrowser_decodeResult->append(messsage);
            return;
        }
        QDataStream out(&output);

        QFile input(ui->lineEdit_decodeSourceFile->text());
        if(!input.open(QIODevice::ReadOnly))
        {
            QString messsage = QString("文件打开失败.文件名:%1.错误信息:%2 \n").arg(input.fileName()).arg(input.errorString());
            ui->textBrowser_decodeResult->append(messsage);
            return;
        }
        QDataStream in(&input);

        QString strBuffer;
        while(!input.atEnd())
        {
            QByteArray byteArray = input.readAll();
            QByteArray base64ByteArray = QByteArray::fromBase64(byteArray,QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
            output.write(base64ByteArray);
        }

        input.close();
        output.close();

        QString messsage = QString("Base64解码成功\n");
        ui->textBrowser_decodeResult->append(messsage);
        ui->pushButton_decode->setDisabled(false);
    },Qt::QueuedConnection);

    connect(ui->pushButton_openDecodeDirectory,&QPushButton::clicked,this,[=]()
    {
        QDesktopServices::openUrl(QUrl(QApplication::applicationDirPath()));
    },Qt::QueuedConnection);

    //单击从剪切板解码按钮
    connect(ui->pushButton_decodeFromClipBoard,&QPushButton::clicked,this,[=]()
    {
        ui->pushButton_decode->setDisabled(true);

        QClipboard * cb = QApplication::clipboard();
        QString content = cb->text();

        //输出文件已存在则先remove掉
        QFileInfo outputFileInfo;
        outputFileInfo.setFile(QApplication::applicationDirPath(),ui->lineEdit_decodeOutputFile->text());
        QFile output(outputFileInfo.absoluteFilePath());
        if(output.exists())
        {
            output.remove();
        }

        if(!output.open(QIODevice::Append))
        {
            QString messsage = QString("文件打开失败.文件名:%1.错误信息:%2 \n").arg(output.fileName()).arg(output.errorString());
            ui->textBrowser_decodeResult->append(messsage);
            return;
        }
        QDataStream out(&output);


        QByteArray byteArray = content.toUtf8();
        QByteArray base64ByteArray = QByteArray::fromBase64(byteArray,QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
        output.write(base64ByteArray);

        output.close();

        QString messsage = QString("Base64解码成功\n");
        ui->textBrowser_decodeResult->append(messsage);
        ui->pushButton_decode->setDisabled(false);
    },Qt::QueuedConnection);

}

Dialog::~Dialog()
{
    delete ui;
}

