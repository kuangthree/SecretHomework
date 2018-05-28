#include "encryptfilter.h"
#include<QSet>
#include<QMap>
#include<QVector>

EncryptFilter::EncryptFilter(const QString &file,const QString& key)
    :EncryptOrDecryptFilter(file,key),mKeyRefresh(key.toLatin1())
{
    mFile.open(QIODevice::ReadOnly);
}

void EncryptFilter::reset()
{
    mFile.close();
    mBegin=false;
    mEscapeFillCount=0;
    mEscapeFillQueue.clear();
    mEncryptedQueue.clear();
    mKeyRefresh = mKey;
    mProcessedByteCount = 0;
    mFile.open(QIODevice::ReadOnly);
}

bool EncryptFilter::hasNext()
{
    return !(mFile.atEnd() && mEncryptedQueue.size()==0 && mEscapeFillQueue.size()==0);
}

int EncryptFilter::processedByteCount()
{
    return mProcessedByteCount;
}

int EncryptFilter::readByte()
{
    if(!hasNext())
        exception("Cannot read.");
    if(mEncryptedQueue.size()==0){
        readEncryptedBytes();
    }
    if(mEncryptedQueue.size()==0){
        exception("Read Error.");
    }
    int ret = mEncryptedQueue.first();
    mEncryptedQueue.pop_front();
    return ret;
}

int EncryptFilter::mode()
{
    return MODE_ENCRYPT;
}

EncryptFilter::~EncryptFilter()
{
    if(mFile.isOpen())mFile.close();
}

void EncryptFilter::readEscapeFillBytes()
{
    if(mFile.atEnd())return;
    char buf[20];
    int length = mFile.read(buf,20);
    if(mFile.atEnd() && length<=0){
        return;
    }
    for(int i=0;i<length;i++){
        if(mEscape){
            if(buf[i]!=(char)mEscapeByte){
                mEscape = false;
            }
        }else{
            if(buf[i]==(char)mEscapeByte){
                mEscape=true;
                mEscapeFillQueue.append(mEscapeByte);
            }else if(buf[i]==(char)mFillByte){
                mEscapeFillQueue.append(mEscapeByte);
            }
        }
        //if(buf[i]==(char)mEscapeByte || buf[i]==(char)mFillByte){
        //    if(buf[i]==(char)mEscapeByte) mEscape = true;
        //    mEscapeFillQueue.append(mEscapeByte);
        //}
        mEscapeFillQueue.append(buf[i]);
        mProcessedByteCount++;
    }
}

void EncryptFilter::readEncryptedBytes()
{
    char* buffer = new char[mAlignLength];
    int size = mAlignLength;
    for(int i=0;i<mAlignLength;i++){
        if(mEscapeFillQueue.size()==0){
            readEscapeFillBytes();
        }
        if(mEscapeFillQueue.size()==0){
            size = i;
            break;
        }
        buffer[i]=mEscapeFillQueue.first();
        mEscapeFillQueue.pop_front();
    }
    for(int i=size;i<mAlignLength;i++){
        buffer[i] = mFillByte & 0xFF;
    }
    encrypt(buffer);
    for(int i=0;i<mAlignLength;i++){
        mEncryptedQueue.push_back(buffer[i]);
    }
    delete[] buffer;
}

void EncryptFilter::encrypt(char *buffer)
{
    QByteArray origin(buffer,mAlignLength);
    for(int i=0;i<mAlignLength;i++){
        buffer[i] = buffer[i] ^ (char)mKeyRefresh[i];
    }
    QMap<char,char> charMap;
    QVector<char> charVector;
    for(int i=0;i<mAlignLength;i++){
        char m = mKeyRefresh[i];
        int p = 1;
        int minus = 1;
        while(charMap.contains(m)){
            m = (char)((int)m + p * minus);
            minus*=-1;
            p++;
        }
        charMap.insert(m,buffer[i]);
        charVector.append(m);
    }
    std::sort(charVector.begin(),charVector.end());
    for(int i=0;i<mAlignLength;i++){
        buffer[i]=charMap[charVector[i]];
    }
    for(int i=0,p=1;i<mAlignLength;i++,p*=-1){
        mKeyRefresh[i]=~((char)((int)origin[i]+((int)mKeyRefresh[i]*p)));
    }
}

void EncryptFilter::exception(const QString &msg)
{
    throw msg;
}
