#ifndef ENCRYPTORDECRYPTFILTER_H
#define ENCRYPTORDECRYPTFILTER_H
#include<QFile>
#include<QByteArray>
class EncryptOrDecryptFilter
{
public:
    EncryptOrDecryptFilter(const QString& a,const QString& key);
    static const int MODE_ENCRYPT=0;
    static const int MODE_DECRYPT=1;
    virtual ~EncryptOrDecryptFilter();
    virtual void reset()=0;
    virtual bool hasNext()=0;
    virtual int processedByteCount()=0;
    virtual int readByte()=0;
    virtual int mode()=0;
    bool isOpened();
    qint64 totalByteCount();
    int fillByte() const;
    void setFillByte(int fillByte);
    const QFile& file() const;
    int escapeByte() const;
    void setEscapeByte(int escapeByte);
    int alignLength() const;
    void setAlignLength(int alignLength);
    QByteArray key() const;
    void setKey(const QByteArray &key);
protected:
    QFile mFile;
    int mFillByte=0x00;
    int mEscapeByte=0x10;
    QByteArray mKey;
    int mAlignLength=8;
};

#endif // ENCRYPTORDECRYPTFILTER_H
