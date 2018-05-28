#include "encryptordecryptfilter.h"


EncryptOrDecryptFilter::EncryptOrDecryptFilter(const QString &a,const QString &key)
    :mFile(a),mKey(key.toLatin1())
{

}

EncryptOrDecryptFilter::~EncryptOrDecryptFilter()
{

}

bool EncryptOrDecryptFilter::isOpened()
{
    return mFile.isOpen();
}

qint64 EncryptOrDecryptFilter::totalByteCount()
{
    return mFile.size();
}

int EncryptOrDecryptFilter::fillByte() const
{
    return mFillByte;
}

void EncryptOrDecryptFilter::setFillByte(int fillByte)
{
    mFillByte = fillByte;
}

const QFile& EncryptOrDecryptFilter::file() const
{
    return mFile;
}

int EncryptOrDecryptFilter::escapeByte() const
{
    return mEscapeByte;
}

void EncryptOrDecryptFilter::setEscapeByte(int escapeByte)
{
    mEscapeByte = escapeByte;
}

int EncryptOrDecryptFilter::alignLength() const
{
    return mAlignLength;
}

void EncryptOrDecryptFilter::setAlignLength(int alignLength)
{
    mAlignLength = alignLength;
}

QByteArray EncryptOrDecryptFilter::key() const
{
    return mKey;
}

void EncryptOrDecryptFilter::setKey(const QByteArray &key)
{
    mKey = key;
}
