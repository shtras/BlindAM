#include "StdAfx.h"
#include "SoundManager.h"

#define NUM_OF_DYNBUF 2  // num buffers in queue
#define DYNBUF_SIZE  10024 // Buffer size

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  File->read((char *)ptr, size * nmemb);
  return (size_t)File->gcount();
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  ios_base::seekdir Dir;
  File->clear();
  switch (whence) {
  case SEEK_SET: Dir = ios::beg;  break;
  case SEEK_CUR: Dir = ios::cur;  break;
  case SEEK_END: Dir = ios::end;  break;
  default: return -1;
  }
  File->seekg((streamoff)offset, Dir);
  return (File->fail() ? -1 : 0);
}

long TellOgg(void *datasource)
{
  istream *File = reinterpret_cast<istream*>(datasource);
  return (long)File->tellg();
}

int CloseOgg(void *datasource)
{
  return 0;
}

SoundManager& SoundManager::getInstance()
{
  static SoundManager instance;
  return instance;
}

SoundManager::SoundManager()
{

}

SoundManager::~SoundManager()
{
  for (auto i = sounds_.begin(); i != sounds_.end(); i++) {
    Snd* snd = (*i).second;
    snd->close();
    delete snd;
  }
  for (auto i = buffers_.begin(); i != buffers_.end(); i++) {
    alDeleteBuffers(1, &i->second.ID);
  }
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context_);
  alcCloseDevice(device_);
}

bool SoundManager::init()
{
  ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
  ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
  ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
  device_ = alcOpenDevice(NULL);
  if (!device_) {
    Logger::getInstance().log(ERROR_LOG_NAME, "Default sound device not found");
    return false;
  }
  context_ = alcCreateContext(device_, NULL);

  alcMakeContextCurrent(context_);
  alListenerfv(AL_POSITION,    ListenerPos);
  alListenerfv(AL_VELOCITY,    ListenerVel);
  alListenerfv(AL_ORIENTATION, ListenerOri);

  Snd* snd = new Snd();
  snd->open("intro.ogg", true, true);
  sounds_[Ambient] = snd;
  
  snd = new Snd();
  snd->open("res/button.wav", false, false);
  sounds_[Test] = snd;
  checkReleaseError("");
  return true;
}

void SoundManager::update()
{
  for (auto itr = sounds_.begin(); itr != sounds_.end(); ++itr) {
    Snd* snd = itr->second;
    snd->update();
  }
}

void SoundManager::playSound( SoundType type )
{
  assert (sounds_.count(type) == 1);
  sounds_[type]->play();
}

void SoundManager::stopSound( SoundType type )
{
  assert (sounds_.count(type) == 1);
  sounds_[type]->stop();
}

Snd::Snd():mVF_(NULL),srcID_(-1),wavBufferID_(-1)
{
  for (int i = 0; i < 3; i++) {
    pos_[i] = pos_[i] = 0.0f;
  }
}

Snd::~Snd()
{

}

bool Snd::open( CString fileName, bool looped, bool streamed )
{
  // Check file existance
  ifstream a(fileName);
  if (!a.is_open()) {
    return false;
  }
  a.close();

  looped_ = looped;
  streamed_ = streamed;

  alGenSources(1, &srcID_);
  checkReleaseError("");

  alSourcef (srcID_, AL_PITCH,  1.0f);
  alSourcef (srcID_, AL_GAIN,  1.0f);
  alSourcefv(srcID_, AL_POSITION, pos_);
  alSourcefv(srcID_, AL_VELOCITY, vel_);
  alSourcei (srcID_, AL_LOOPING, looped_);
  if (fileName.contains(".wav")) {
    streamed_ = false;
    return loadWav(fileName);
  } else if (fileName.contains(".ogg")) {
    return loadOGG(fileName, streamed);
  }
  return false;
}

void Snd::play()
{
  alSourcePlay(srcID_);
}

void Snd::stop()
{
  alSourceStop(srcID_);
}

void Snd::close()
{
  stop();
  if (alIsSource(srcID_)) {
    alDeleteSources(1, &srcID_);
  }
  if (mVF_) { 
    ov_clear(mVF_);
    delete mVF_;
  }
}

bool Snd::loadWav( CString fileName )
{
  ALenum    format;
  ALsizei    size;
  ALsizei    freq;
  ALuint    BufID = 0;

  bool res = loadWavFile(fileName, &wavBufferID_, &size, &freq, &format);
  if (!res) {
    return false;
  }

  alSourcei(srcID_, AL_BUFFER, wavBufferID_);

  return true;
}

bool Snd::loadOGG( CString fileName, bool streamed )
{
  int    i, DynBuffs = 1, BlockSize;
  // OAL specific
  SndInfo   buffer;
  ALuint   BufID = 0;
  // OggVorbis specific structures
  ov_callbacks cb;
  streamed_ = streamed;

  // Fill cb struct
  cb.close_func = CloseOgg;
  cb.read_func = ReadOgg;
  cb.seek_func = SeekOgg;
  cb.tell_func = TellOgg;

  // Create OggVorbis_File struct
  mVF_ = new OggVorbis_File;

  // Open Ogg file
  oggFile_.open(fileName, ios_base::in | ios_base::binary);

  // Generate local buffers
  if (ov_open_callbacks(&oggFile_, mVF_, NULL, -1, cb) < 0)
  {
    // This is not ogg bitstream. Return
    return false;
  }

  // Check for existance of sound
  auto& buffers = SoundManager::getInstance().getBuffers();
  if (!streamed) {
    for (auto i = buffers.begin(); i != buffers.end(); i++) {
      if (i->second.Filename == fileName) BufID = i->first;
    }
    BlockSize = (int)ov_pcm_total(mVF_, -1) * 4;
  } else {
    BlockSize = DYNBUF_SIZE;
    DynBuffs = NUM_OF_DYNBUF;
    alSourcei(srcID_, AL_LOOPING, AL_FALSE);
  }

  // Return vorbis_comment and vorbis_info structures
  comment_  = ov_comment(mVF_, -1);
  info_   = ov_info(mVF_, -1);

  // Fill buffer infos
  buffer.Rate  = info_->rate;
  buffer.Filename = fileName;
  buffer.Format = (info_->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  // Fill buffers with data each block by DYNBUF_SIZE bytes
  if (streamed || !BufID) {
    for (i = 0; i < DynBuffs; i++) {
      // Create buffers
      alGenBuffers(1, &buffer.ID);
      buffers[buffer.ID] = buffer;
      // Read amount (DYNBUF_SIZE) data into each buffer
      readOggBlock(buffer.ID, BlockSize);
      if (streamed) {
        alSourceQueueBuffers(srcID_, 1, &buffer.ID);
      } else {
        alSourcei(srcID_, AL_BUFFER, buffer.ID);
      }
    }
  } else {
    alSourcei(srcID_, AL_BUFFER, buffers[BufID].ID);
  }

  return true;
}

bool Snd::readOggBlock( ALuint BufID, size_t Size )
{
  // vars
  char  eof = 0;
  int   current_section;
  unsigned long  TotalRet = 0, ret;
  char  *PCM;
  auto& buffers = SoundManager::getInstance().getBuffers();
  if (Size < 1) return false;
  PCM = new char[Size];

  // Read loop
  while (TotalRet < Size) {
    ret = ov_read(mVF_, PCM + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

    // if end of file or read limit exceeded
    if (ret == 0) {
      break;
    }
    else if (ret < 0) {
      //
    } else {
      TotalRet += ret;
    }
  }
  if (TotalRet > 0) {
    alBufferData(BufID, buffers[BufID].Format, (void *)PCM, TotalRet, buffers[BufID].Rate);
  }
  delete [] PCM;
  return (ret > 0);
}

void Snd::update()
{
  if (!streamed_) {
    return;
  }
  int Processed = 0;
  ALuint BufID;
  alGetSourcei(srcID_, AL_BUFFERS_PROCESSED, &Processed);
  checkReleaseError("");
  while (Processed--) {
    alSourceUnqueueBuffers(srcID_, 1, &BufID);
    checkReleaseError("");
    if (readOggBlock(BufID, DYNBUF_SIZE) != 0) {
      alSourceQueueBuffers(srcID_, 1, &BufID);
    } else {
      ov_pcm_seek(mVF_, 0);
      alSourceQueueBuffers(srcID_, 1, &BufID);
      if (!looped_) {
        stop();
      }
    }
  }
}
