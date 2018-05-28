#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "encryptfilter.h"
#include "decryptfilter.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    EncryptOrDecryptFilter* mFilter = nullptr;
    int mAlignByteLength;
    int mFillByte;
    int mEscapeByte;
    QString mKey;
    QString mFilePath;
    bool checkInput();
    void initFilter();
    void doWork();
private slots:
    void onPressOpenFileButton();
    void onPressDecryptButton();
    void onPressEncryptButton();
    void onPressRandomKeyButton();
    void onError(const QString&);
};

#endif // MAINWINDOW_H
