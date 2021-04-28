#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QObject>
#include <QThread>
//Stream #0:0: Audio: pcm_f32le, 48000 Hz, mono, flt, 1536 kb/s
class PlayThread : public QThread
{
    Q_OBJECT

private:
    void run();
public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();
signals:

};

#endif // PLAYTHREAD_H
