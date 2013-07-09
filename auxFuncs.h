#pragma once

class WLayout;

struct WAVE_Data {
  char subChunkID[4];
  long subChunk2Size;
};

struct WAVE_Format {
  char subChunkID[4];
  long subChunkSize;
  short audioFormat;
  short numChannels;
  long sampleRate;
  long byteRate;
  short blockAlign;
  short bitsPerSample;
};

struct RIFF_Header {
  char chunkID[4];
  long chunkSize;
  char format[4];
};

ENGINE_API double RadToDeg(double rad);
ENGINE_API double DegToRad(double deg);

ENGINE_API bool isNumberChar(int key);
ENGINE_API bool isLetterChar(int key);

ENGINE_API void checkErrorDebug(CString errorMsg = "");

ENGINE_API void checkReleaseError(CString errorMsg);

ENGINE_API vector<CString> getFileNames(CString dir);

ENGINE_API int parseInt(CString str);

ENGINE_API void writeStringToFile(FILE* file, CString str);

ENGINE_API void writeToFile(void* buffer, size_t size, int count, FILE* file);

ENGINE_API bool readFromFile(void* buffer, size_t size, int count, FILE* file);

ENGINE_API CString readStringFromFile(FILE* file);

class FilePointer
{
public:
  FilePointer(const char* fileName, std::ios_base::openmode mode);
  ~FilePointer();
  fstream& operator->();
private:
  fstream file_;
};
