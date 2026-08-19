#include "music_library.h"
#include <SD_MMC.h>

// Broches carte SD (mode SDMMC 1-bit) - Waveshare ESP32-S3-Touch-AMOLED-1.91
#define SD_CLK 9
#define SD_CMD 42
#define SD_D0  8

namespace
{
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
                Music track(fullPath, basenameNoExt(fname), "", "", 0);

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
