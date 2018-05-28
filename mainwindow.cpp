#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include<QMessageBox>
#include<QRegExp>
#include<QTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mProcessBar->setValue(0);
    ui->mStateLineEdit->setText("");
    connect(ui->mFileOpenButton,SIGNAL(clicked(bool)),this,SLOT(onPressOpenFileButton()));
    connect(ui->mDecryptButton,SIGNAL(clicked(bool)),this,SLOT(onPressDecryptButton()));
    connect(ui->mEncryptButton,SIGNAL(clicked(bool)),this,SLOT(onPressEncryptButton()));
    connect(ui->mRandomKeyButton,SIGNAL(clicked(bool)),this,SLOT(onPressRandomKeyButton()));
    QTime time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::checkInput()
{
    mFilePath = ui->mFileOpenLineEdit->text();
    if(mFilePath.length()==0){
        onError(tr("未填写文件路径"));
        return false;
    }
    mKey = ui->mKeyLineEdit->text();
    if(mKey.length()==0){
        onError(tr("未填写密码"));
        return false;
    }
    QFile file(mFilePath);
    if(!file.exists()){
        onError(tr("文件不存在"));
        return false;
    }
    mAlignByteLength = ui->mByteAlignLengthSpinBox->value();
    if(mKey.length()<mAlignByteLength){
        QString expandKey = mKey;
        for(int i=mKey.length(),j=0;i<mAlignByteLength;i++,j++){
           j%=mKey.length();
           expandKey.append(mKey[j]);
        }
        mKey = expandKey;
    }else if(mKey.length()>mAlignByteLength){
        mKey = mKey.mid(0,mAlignByteLength);
    }
    QString fb = ui->mFillByteLineEdit->text();
    QString eb = ui->mEscapeByteLineEdit->text();
    QRegExp regExp("0x[0-9][0-9]?");
    if(!regExp.exactMatch(fb) || !regExp.exactMatch(eb)){
        onError(tr("错误的16进制格式。"));
        return false;
    }
    fb = fb.mid(2);
    eb = eb.mid(2);
    mFillByte = fb.toInt();
    mEscapeByte = eb.toInt();
    return true;
}

void MainWindow::initFilter()
{
    mFilter->setAlignLength(mAlignByteLength);
    mFilter->setEscapeByte(mEscapeByte);
    mFilter->setFillByte(mFillByte);
}

void MainWindow::doWork()
{
    try{
    ui->mStateLineEdit->setText(tr("正在创建文件"));
    QString outFilePath;
    if(mFilter->mode()==EncryptOrDecryptFilter::MODE_ENCRYPT){
        outFilePath = mFilePath + ".ksec";
    }else if(mFilePath.endsWith(".ksec")){
        outFilePath = mFilePath.mid(0,mFilePath.length()-5);
    }else{
        outFilePath = mFilePath.mid(0,mFilePath.lastIndexOf("\\")+1)+"未知格式解密文件.unknown";
    }
    QFile outFile(outFilePath);
    outFile.open(QIODevice::WriteOnly);
    ui->mStateLineEdit->setText(tr("创建文件：")+outFilePath);
    while(mFilter->hasNext()){
        char byte = mFilter->readByte();
        outFile.write(&byte,1);
        int process = 100 * mFilter->processedByteCount()/mFilter->totalByteCount();
        ui->mProcessBar->setValue(process);
    }
    outFile.close();
    ui->mStateLineEdit->setText(tr("文件加密/解密完成。"));
    delete mFilter;
    mFilter = nullptr;
    }catch(QString& e){
        onError(e);
    }
}

void MainWindow::onPressOpenFileButton()
{
    QString path = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("所有文件(*)"));
    ui->mFileOpenLineEdit->setText(path);
}

void MainWindow::onPressDecryptButton()
{
    if(!checkInput()){
        return;
    }
    mFilter = new DecryptFilter(mFilePath,mKey);
    initFilter();
    doWork();
}

void MainWindow::onPressEncryptButton()
{
    if(!checkInput()){
        return;
    }
    mFilter = new EncryptFilter(mFilePath,mKey);
    initFilter();
    doWork();
}

void MainWindow::onPressRandomKeyButton()
{
    QString pool = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    QString key;
    int l = ui->mByteAlignLengthSpinBox->value();
    for(int i=0;i<l;i++){
        key.append(pool[qrand()%pool.length()]);
    }
    ui->mKeyLineEdit->setText(key);
}

void MainWindow::onError(const QString &message)
{
    QMessageBox::information(nullptr,tr("错误"),message);
}
