#include "sd_card.h"
#include <string.h>

int photo_file_num = 0;
char file_name_list[IMAGE_FILE_NUM][IMAGE_FILE_NAME_MAX_LEN];

void SdCard::init()
{

  SPIClass *sd_spi = new SPIClass(HSPI); // another SPI
  if (!SD.begin(15, *sd_spi))            // SD-Card SS pin is 15
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void SdCard::listDir(const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  int dir_len = strlen(dirname) + 1;

  File file = root.openNextFile();
  while (file && photo_file_num < IMAGE_FILE_NUM)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      strncpy(file_name_list[photo_file_num], file.name() + dir_len, IMAGE_FILE_NAME_MAX_LEN - 1);
      file_name_list[photo_file_num][strlen(file_name_list[photo_file_num]) - 4] = 0;
      Serial.print(file_name_list[photo_file_num]);
      ++photo_file_num;
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  Serial.println(photo_file_num);
}

void SdCard::createDir(const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (SD.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void SdCard::removeDir(const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (SD.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void SdCard::readFile(const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = SD.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

String SdCard::readFileLine(const char *path, int num)
{
  Serial.printf("Reading file: %s line: %d\n", path, num);

  File file = SD.open(path);
  if (!file)
  {
    return ("Failed to open file for reading");
  }

  char *p = buf;
  while (file.available())
  {
    char c = file.read();
    if (c == '\n')
    {
      num--;
      if (num == 0)
      {
        *(p++) = '\0';
        String s(buf);
        s.trim();
        return s;
      }
    }
    else if (num == 1)
    {
      *(p++) = c;
    }
  }
  file.close();

  return String("error parameter!");
}

void SdCard::writeFile(const char *path, const char *info)
{
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.println(info))
  {
    Serial.println("Write succ");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

File SdCard::open(const String &path, const char *mode)
{
  return SD.open(path, FILE_WRITE);
}

void SdCard::appendFile(const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void SdCard::renameFile(const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (SD.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

boolean SdCard::deleteFile(const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (SD.remove(path))
  {
    Serial.println("File deleted");
    return true;
  }
  else
  {
    Serial.println("Delete failed");
  }
  return false;
}

boolean SdCard::deleteFile(const String &path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (SD.remove(path))
  {
    Serial.println("File deleted");
    return true;
  }
  else
  {
    Serial.println("Delete failed");
  }
  return false;
}

void SdCard::readBinFromSd(const char *path, uint8_t *buf)
{
  File file = SD.open(path);
  size_t len = 0;
  if (file)
  {
    len = file.size();

    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }

    file.close();
  }
  else
  {
    Serial.println("Failed to open file for reading");
  }
}

void SdCard::writeBinToSd(const char *path, uint8_t *buf)
{
  File file = SD.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  file.close();
}

void SdCard::fileIO(const char *path)
{
  File file = SD.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("Failed to open file for reading");
  }

  file = SD.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}
