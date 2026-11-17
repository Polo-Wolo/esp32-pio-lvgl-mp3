#include "music_library.h"
#include <SD_MMC.h>

// Broches carte SD (mode SDMMC 1-bit) - Waveshare ESP32-S3-Touch-AMOLED-1.91
#define SD_CLK 9
#define SD_CMD 42
#define SD_D0  8

namespace
{
    struct Mp3FrameInfo
    {
        uint32_t bitrate = 0;
        uint32_t sampleRate = 0;
        uint16_t samplesPerFrame = 0;
        uint16_t frameSize = 0;
        uint8_t version = 0;
        uint8_t channels = 0;
        bool hasCrc = false;
    };

    uint32_t readBe32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
               ((uint32_t)p[2] << 8) | p[3];
    }

    bool parseMp3Header(const uint8_t *p, Mp3FrameInfo &info)
    {
        const uint32_t h = readBe32(p);
        if ((h & 0xFFE00000) != 0xFFE00000)
            return false;

        const uint8_t versionBits = (h >> 19) & 3;
        const uint8_t layerBits = (h >> 17) & 3;
        const uint8_t bitrateIndex = (h >> 12) & 15;
        const uint8_t sampleRateIndex = (h >> 10) & 3;
        if (versionBits == 1 || layerBits != 1 || bitrateIndex == 0 ||
            bitrateIndex == 15 || sampleRateIndex == 3)
            return false;

        static const uint16_t mpeg1Bitrates[] =
            {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
        static const uint16_t mpeg2Bitrates[] =
            {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160};
        static const uint32_t baseSampleRates[] = {44100, 48000, 32000};

        info.version = versionBits; // 3=MPEG1, 2=MPEG2, 0=MPEG2.5
        info.sampleRate = baseSampleRates[sampleRateIndex];
        if (versionBits == 2) info.sampleRate /= 2;
        if (versionBits == 0) info.sampleRate /= 4;
        const uint32_t kbps = versionBits == 3 ? mpeg1Bitrates[bitrateIndex]
                                               : mpeg2Bitrates[bitrateIndex];
        info.bitrate = kbps * 1000;
        info.samplesPerFrame = versionBits == 3 ? 1152 : 576;
        info.channels = ((h >> 6) & 3) == 3 ? 1 : 2;
        info.hasCrc = ((h >> 16) & 1) == 0;
        const uint8_t padding = (h >> 9) & 1;
        info.frameSize = (versionBits == 3 ? 144 : 72) * info.bitrate /
                         info.sampleRate + padding;
        return info.frameSize >= 24;
    }

    uint32_t id3v2End(File &file)
    {
        uint8_t h[10];
        if (!file.seek(0) || file.read(h, sizeof(h)) != sizeof(h) ||
            h[0] != 'I' || h[1] != 'D' || h[2] != '3')
            return 0;
        if ((h[6] | h[7] | h[8] | h[9]) & 0x80)
            return 0; // taille synchsafe invalide
        const uint32_t body = ((uint32_t)h[6] << 21) | ((uint32_t)h[7] << 14) |
                              ((uint32_t)h[8] << 7) | h[9];
        return 10 + body + ((h[5] & 0x10) ? 10 : 0);
    }

    bool findFirstMp3Frame(File &file, uint32_t start, uint32_t &position,
                           Mp3FrameInfo &info)
    {
        const uint32_t fileSize = file.size();
        const uint32_t limit = min(fileSize, start + 64U * 1024U);
        uint8_t bytes[4] = {};
        if (start + 4 > limit || !file.seek(start) || file.read(bytes, 4) != 4)
            return false;

        for (uint32_t pos = start; pos + 4 <= limit; ++pos)
        {
            Mp3FrameInfo candidate;
            if (parseMp3Header(bytes, candidate))
            {
                // Un second en-tete a la taille de trame attendue elimine la
                // plupart des faux mots de synchronisation.
                uint8_t next[4];
                const uint32_t nextPos = pos + candidate.frameSize;
                if (nextPos + 4 <= fileSize && file.seek(nextPos) &&
                    file.read(next, 4) == 4)
                {
                    Mp3FrameInfo nextInfo;
                    if (parseMp3Header(next, nextInfo) &&
                        nextInfo.version == candidate.version &&
                        nextInfo.sampleRate == candidate.sampleRate)
                    {
                        position = pos;
                        info = candidate;
                        return true;
                    }
                }
                file.seek(pos + 4);
            }
            bytes[0] = bytes[1];
            bytes[1] = bytes[2];
            bytes[2] = bytes[3];
            if (pos + 4 == limit || file.read(&bytes[3], 1) != 1)
                break;
        }
        return false;
    }

    uint32_t mp3Duration(File &file)
    {
        const uint32_t fileSize = file.size();
        if (fileSize < 8)
            return 0;

        const uint32_t audioStartHint = id3v2End(file);
        uint32_t framePosition = 0;
        Mp3FrameInfo frame;
        if (!findFirstMp3Frame(file, audioStartHint, framePosition, frame))
            return 0;

        uint8_t firstFrame[160] = {};
        if (!file.seek(framePosition))
            return 0;
        const size_t readCount = file.read(firstFrame, sizeof(firstFrame));

        // Xing/Info se trouve apres l'en-tete, le CRC eventuel et les side-info.
        const uint32_t sideInfo = frame.version == 3
            ? (frame.channels == 1 ? 17 : 32)
            : (frame.channels == 1 ? 9 : 17);
        const uint32_t xing = 4 + (frame.hasCrc ? 2 : 0) + sideInfo;
        if (xing + 12 <= readCount &&
            ((!memcmp(firstFrame + xing, "Xing", 4)) ||
             (!memcmp(firstFrame + xing, "Info", 4))))
        {
            const uint32_t flags = readBe32(firstFrame + xing + 4);
            if (flags & 1)
            {
                const uint32_t frames = readBe32(firstFrame + xing + 8);
                return (uint32_t)(((uint64_t)frames * frame.samplesPerFrame +
                                   frame.sampleRate / 2) / frame.sampleRate);
            }
        }

        // VBRI est normalement place 32 octets apres l'en-tete MPEG.
        for (size_t i = 4; i + 18 <= readCount && i < 80; ++i)
        {
            if (!memcmp(firstFrame + i, "VBRI", 4))
            {
                const uint32_t frames = readBe32(firstFrame + i + 14);
                return (uint32_t)(((uint64_t)frames * frame.samplesPerFrame +
                                   frame.sampleRate / 2) / frame.sampleRate);
            }
        }

        // Repli CBR : pour un VBR sans index, cette valeur reste une estimation.
        uint32_t audioEnd = fileSize;
        uint8_t tag[3];
        if (fileSize >= 128 && file.seek(fileSize - 128) && file.read(tag, 3) == 3 &&
            tag[0] == 'T' && tag[1] == 'A' && tag[2] == 'G')
            audioEnd -= 128;
        const uint32_t audioBytes = audioEnd > framePosition ? audioEnd - framePosition : 0;
        return frame.bitrate
            ? (uint32_t)(((uint64_t)audioBytes * 8 + frame.bitrate / 2) / frame.bitrate)
            : 0;
    }

    bool isMp3(const String &name)
    {
        String n = name;
        n.toLowerCase();
        return n.endsWith(".mp3");
    }

    // "01 - One More Time.mp3" -> "01 - One More Time"
    String basenameNoExt(const String &name)
    {
        int dot = name.lastIndexOf('.');
        return dot > 0 ? name.substring(0, dot) : name;
    }

    size_t scanDir(Jukebox &pm, const String &path, bool recursive)
    {
        size_t added = 0;

        File dir = SD_MMC.open(path);
        if (!dir || !dir.isDirectory())
        {
            Serial.printf("Impossible d'ouvrir %s\n", path.c_str());
            return 0;
        }

        File f = dir.openNextFile();
        while (f)
        {
            String fname = f.name();
            int slash = fname.lastIndexOf('/');
            if (slash >= 0) fname = fname.substring(slash + 1);

            if (f.isDirectory())
            {
                if (recursive && fname != "." && fname != "..")
                {
                    String sub = path;
                    if (!sub.endsWith("/")) sub += "/";
                    sub += fname;
                    added += scanDir(pm, sub, recursive);
                }
            }
            else if (isMp3(fname))
            {
                String fullPath = path;
                if (!fullPath.endsWith("/")) fullPath += "/";
                fullPath += fname;

                // Titre par defaut = nom de fichier. Sera remplace par le vrai
                // tag ID3 des que la piste sera lue (cf. etape audio).
                const uint32_t duration = mp3Duration(f);
                Music track(fullPath, basenameNoExt(fname), "", "", 0, duration);
                if (duration)
                    Serial.printf("[Library] %s : %u:%02u\n", fname.c_str(),
                                  (unsigned)(duration / 60), (unsigned)(duration % 60));

                if (pm.add(track)) added++;
                else Serial.println("File de lecture pleine, piste ignoree.");
            }

            f = dir.openNextFile();
        }
        dir.close();

        return added;
    }
}

bool MusicLibrary::begin()
{
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);
    return SD_MMC.begin("/sdcard", true); // true = mode 1-bit (seul D0 est cable)
}

size_t MusicLibrary::loadFolder(Jukebox &pm, const String &folderPath, bool recursive)
{
    return scanDir(pm, folderPath, recursive);
}
