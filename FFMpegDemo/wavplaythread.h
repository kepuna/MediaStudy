#ifndef WAVPLAYTHREAD_H
#define WAVPLAYTHREAD_H

#include <QObject>
#include <QThread>

class WAVPlayThread : public QThread
{
    Q_OBJECT

private:
    void run();
public:
    explicit WAVPlayThread(QObject *parent = nullptr);
    ~WAVPlayThread();

signals:

};

#endif // WAVPLAYTHREAD_H
