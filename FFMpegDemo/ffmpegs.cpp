#include "ffmpegs.h"
#include <QFile>
#include <QtDebug>

#define PCM_FILENAME  "/Users/momo/Desktop/Resource/004.pcm"

FFmpegs::FFmpegs()
{

}

void FFmpegs::pcm2wav(WAVHeader &header, const char *pcmFilename, const char *wavFilename) {
    header.blockAlign = (header.bitsPerSample * header.numChannels) / 8;
    header.byteRate = header.sampleRate * header.blockAlign;

    // 打开PCM
    QFile pcmFile(pcmFilename);
    if(!pcmFile.open(QFile::ReadOnly) ) {
        qDebug() << "Open File Faild" << pcmFilename;
        return;
    }
    header.dataChunkDataSize = pcmFile.size();
    header.riffChunkDataSize = header.dataChunkDataSize + sizeof(WAVHeader) - 8;

    // 打开wav文件
    QFile wavFile(wavFilename);
    if(!wavFile.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << wavFilename;
        return;
    }

    // 写入头部
    wavFile.write((const char *)&header, sizeof (WAVHeader));

    // 写入pcm数据
    char buf[1024];
    int size;
    while ((size = pcmFile.read(buf, sizeof (buf))) > 0 ) {
        wavFile.write(buf, size);
    }

    pcmFile.close();
    wavFile.close();
}
