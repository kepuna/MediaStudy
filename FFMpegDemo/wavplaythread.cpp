#include "wavplaythread.h"
#include <QDebug>
#include <QFile>
#include <SDL2/SDL.h>

#define WAV_FILENAME  "/Users/momo/Desktop/Resource/004.wav"

WAVPlayThread::WAVPlayThread(QObject *parent) : QThread(parent)
{
    connect(this, &WAVPlayThread::finished,
            this, &WAVPlayThread::deleteLater);
}

WAVPlayThread::~WAVPlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();
     qDebug() << this << "析构了";
}

// 存放WAV的PCM数据 和 数据长度
typedef struct {
    Uint32 len = 0; // 数据长度
    Uint8  *data = nullptr;
    int pullLen = 0; // 已经接收的PCM长度

} WAVAudioBuffer;

// 回调函数 （等待音频设备回调(会回调多次)）
//  stream ：需要往stream中填充PCM数据
// len： // 希望填充的大小(samples * format * channels / 8)
void pull_wav_audio_data (void *userdata, Uint8 * stream,
                                    int len) {

    qDebug() << "接受到字节✅ " << len;
    // clear stream
    SDL_memset(stream, 0, len);

    WAVAudioBuffer *buffer = (WAVAudioBuffer *)userdata;

    // 文件数据还没准备好
    if (buffer->len <= 0) {
        return;
    }

    // 取len、bufferLen的最小值
    buffer->pullLen = (len > (int) buffer->len) ? buffer->len : len;

    // 填充数据
    SDL_MixAudio(stream, buffer->data, buffer->pullLen, SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}

void WAVPlayThread::run() {

    // 初始化AUdio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "SDL_Init error❌:" << SDL_GetError();
        return;
    }

    // WAV中PCM数据
    Uint8 *data;
    // WAV 中PCM数据大小（字节）
    Uint32 len;

    // 音频参数
    SDL_AudioSpec spec;
    // 加载wav文件
    if( !SDL_LoadWAV(WAV_FILENAME, &spec, &data, &len)) {
        qDebug() << "SDL_LoadWAV error:❌" << SDL_GetError();
        // 清除所有的子系统
        SDL_Quit();
        return;
    }

    spec.callback = pull_wav_audio_data;
    // 传递给回调函数的userdata
    WAVAudioBuffer buffer;
    buffer.len = len;
    buffer.data = data;
    spec.userdata = &buffer;

    // 打开设备 ⚠️
    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio error:" << SDL_GetError();
        // 释放文件数据
        SDL_FreeWAV(data);
        // 清除所有的子系统
        SDL_Quit();
        return;
    }

    // 开始播放
    SDL_PauseAudio(0);
    qDebug() << "开始播放 ✅" << buffer.len ;
    while (!isInterruptionRequested()) {
             qDebug() << "不断结构 ✅" << buffer.len ;
        if(buffer.len > 0) {
            continue;
        }
        // 每一个样本的大小
        int size = spec.channels * SDL_AUDIO_BITSIZE(spec.format) / 8;

        // 最后一次播放的样本数量
        int samples = buffer.pullLen / size;
        // 最后一次播放的时长
        int ms = samples * 1000 / spec.freq;
        SDL_Delay(ms);
        break;
    }

     qDebug() << "播放结束 ✅" << buffer.len ;
    // 释放WAV文件数据
    SDL_FreeWAV(data);

    // 关闭设别
    SDL_CloseAudio();

    // 清除所有子系统
    SDL_Quit();

}
