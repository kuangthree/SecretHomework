#ifndef DECRYPTFILTER_H
#define DECRYPTFILTER_H
#include "encryptordecryptfilter.h"
#include <QQueue>
class DecryptFilter : public EncryptOrDecryptFilter
{
public:
    DecryptFilter(const QString& file,const QString& key);
    virtual void reset();
    virtual bool hasNext();
    virtual int processedByteCount();
    virtual int readByte();
    virtual int mode();
    virtual ~DecryptFilter();
private:
    bool mEscape = false;
    int mProcessedByteCount = 0;
    QByteArray mKeyRefresh;
    QQueue<char> mEscapeByteQueue;
    QQueue<char> mDecryptQueue;
    void decrypt(char* buffer);
    void readIntoDecryptQueue();
    void readIntoEscapeByteQueue();
};

#endif // DECRYPTFILTER_H
