#ifndef ENCRYPTFILTER_H
#define ENCRYPTFILTER_H
#include "encryptordecryptfilter.h"
#include <QQueue>
#include <QByteArray>
class EncryptFilter : public EncryptOrDecryptFilter
{
public:
    EncryptFilter(const QString& file,const QString& key);
    virtual void reset();
    virtual bool hasNext();
    virtual int processedByteCount();
    virtual int readByte();
    virtual int mode();
    virtual ~EncryptFilter();
private:
    bool mBegin=false;
    bool mEscape = false;
    QQueue<int> mEscapeFillQueue;
    QQueue<int> mEncryptedQueue;
    qint64 mEscapeFillCount=0;
    int mProcessedByteCount=0;
    void readEscapeFillBytes();
    void readEncryptedBytes();
    QByteArray mKeyRefresh;
    void encrypt(char*);
    void exception(const QString& msg);
};

#endif // ENCRYPTFILTER_H
