#include "decryptfilter.h"
#include<QMap>
#include<QVector>
DecryptFilter::DecryptFilter(const QString &file, const QString &key)
    :EncryptOrDecryptFilter(file,key),
      mKeyRefresh(key.toLatin1())
{
    mFile.open(QIODevice::ReadOnly);
}

void DecryptFilter::reset()
{
    mEscape = false;
    mFile.close();
    mProcessedByteCount = 0;
    mKeyRefresh = mKey;
    mEscapeByteQueue.clear();
    mDecryptQueue.clear();
    mFile.open(QIODevice::ReadOnly);
}

bool DecryptFilter::hasNext()
{
    return (!mFile.atEnd()) || (!mEscapeByteQueue.isEmpty()) || (!mDecryptQueue.isEmpty());
}

int DecryptFilter::processedByteCount()
{
    return mProcessedByteCount;
}

int DecryptFilter::readByte()
{
    if(mEscapeByteQueue.empty()){
        readIntoEscapeByteQueue();
    }
    if(mEscapeByteQueue.empty()){
        return -1;
    }
    char m = mEscapeByteQueue.first();
    mEscapeByteQueue.pop_front();
    return m;
}

int DecryptFilter::mode()
{
    return MODE_DECRYPT;
}

DecryptFilter::~DecryptFilter()
{
    if(mFile.isOpen())
        mFile.close();
}

void DecryptFilter::decrypt(char *buffer)
{
    QMap<char,int> charMap;
    QVector<char> charVector;
    for(int i=0;i<mAlignLength;i++){
        char m = mKeyRefresh[i];
        int p=1;
        int minus=1;
        while(charMap.contains(m)){
            m =(char)((int)m + p * minus);
            minus*=-1;
            p++;
        }
        charMap.insert(m,i);
        charVector.append(m);
    }
    std::sort(charVector.begin(),charVector.end());
    QByteArray origin;
    origin.resize(mAlignLength);
    for(int i=0;i<mAlignLength;i++){
        int index = charMap[charVector[i]];
        origin[index]=buffer[i];
    }
    for(int i=0;i<mAlignLength;i++){
        origin[i] = origin[i] ^ (char)mKeyRefresh[i];
        buffer[i] = origin[i];
    }
    for(int i=0,p=1;i<mAlignLength;i++,p*=-1){
        mKeyRefresh[i]=~((char)((int)origin[i]+((int)mKeyRefresh[i]*p)));
    }
}

void DecryptFilter::readIntoDecryptQueue()
{
    if(mFile.atEnd())return;
    char* buf = new char[mAlignLength];
    int length = mFile.read(buf,mAlignLength);
    if(length!=mAlignLength){
        throw QString("Error File");
    }
    decrypt(buf);
    for(int i=0;i<mAlignLength;i++){
        mDecryptQueue.append(buf[i]);
    }
    delete[] buf;
}

void DecryptFilter::readIntoEscapeByteQueue()
{
    if(mDecryptQueue.isEmpty()){
        readIntoDecryptQueue();
    }
    mProcessedByteCount+=mDecryptQueue.size();
    while(!mDecryptQueue.isEmpty()){
        char m = mDecryptQueue.first();
        mDecryptQueue.pop_front();
        if(mEscape){
            mEscapeByteQueue.append(m);
            if(m == mFillByte || m!=mEscapeByte){
                mEscape = false;
            }
        }else if(m==mEscapeByte){
            mEscape = true;
        }else if(m!=mFillByte){
            mEscapeByteQueue.append(m);
        }
    }
}
