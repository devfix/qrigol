#include "mlogger.h"
#include <QSemaphore>
#include <QDateTime>

MLogger::MLogger(QThread *parent) :
    QThread(parent)
{
}


void MLogger::prep(QFile *file, bool header, bool closeflag, int repeat)
{
    this->file=file;
    this->header=header;
    this->closeFlag=closeflag;
    this->repeat=repeat;
}

void MLogger::run()
{
    int i,j;
    while (repeat!=0)
    {
        int dbg=sample.available();
        sample.acquire();
        dbg=sample.available();
        if (file->exists()) header=false;
        if (!file->isOpen()&&!file->open(QIODevice::Append|QIODevice::Text))
        {
            emit finished();
            return; // spruce up soon
        }
        if (header)
        {
            file->write("Date,Time,Peak 1,Max 1, Min 1, Avg 1, Amp 1, RMS 1, Top 1, Base 1, +Width 1, -Width 1,Freq 1, Period 1, Over 1, Pres 1, Rise 1, Fall 1, +Duty 1, -Duty 1, +Delay 1, -Delay 1, Peak 2,Max 2, Min 2, Avg 2, Amp 2, RMS 2, Top 2, Base 2, +Width 2, -Width 2,Freq 2, Period 2, Over 2, Pres 2, Rise 2, Fall 2, +Duty 2, -Duty 2, +Delay 2, -Delay 2\n");
            header=false;
        }
        inuse.lock();
        QDateTime now=QDateTime::currentDateTime();
        file->write(now.toString("yyyyMMdd,hh:mm:ss.zzz,").toLatin1());
        for (i=0;i<1;i++)
            for (j=0;j<20;j++)
            {
                file->write(QString::number(data[i][j]).toLatin1());
                file->write(",");
            }

        file->write("\n");
        inuse.unlock();
        if (closeFlag) file->close(); else file->flush();
        if (repeat>0) repeat--;
        // Note: You never release the semaphore. You count them to zero and the other guy keeps adding them
    }
    file->close();
}
