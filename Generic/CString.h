#pragma once

#include <string>
using namespace std;

class CString
{
  friend ENGINE_API CString operator+ (const char* s1, const CString& s2);
public:
  ENGINE_API CString();
  ENGINE_API CString(const CString& other);
  ENGINE_API CString(CString&& other);
  ENGINE_API CString(const char* str);
  ENGINE_API CString(char c);
  ENGINE_API CString(int val);
  ENGINE_API CString(int val, int minLen);
  ENGINE_API CString(float val);
  ENGINE_API CString(float val, int len);
  ENGINE_API CString(double val);
  ENGINE_API CString(double val, int len);
  ENGINE_API CString(long val);
  ENGINE_API ~CString();

  ENGINE_API CString left(int idx);
  ENGINE_API int getSize() const;
  ENGINE_API CString append(const CString& other) const;
  ENGINE_API int getIndex(char c) const;
  ENGINE_API bool contains(CString other) const;
  ENGINE_API bool contains(char c) const;
  ENGINE_API operator const char*() const;
  ENGINE_API CString operator+ (const CString& other) const;
  ENGINE_API CString& operator= (CString other);
  ENGINE_API CString& operator= (const char* str);
  ENGINE_API bool operator== (const CString& other) const;
  ENGINE_API bool operator== (const char* other) const;
  ENGINE_API bool operator!= (const CString& other) const;
  ENGINE_API bool operator!= (const char* other) const;
  ENGINE_API bool operator< (const CString& other) const;
  ENGINE_API CString& operator+= (const CString& other);
  ENGINE_API bool isEmpty();
  ENGINE_API CString toLower();
  ENGINE_API CString toUpper();
  ENGINE_API CString remove(char c);
  ENGINE_API CString substr(int begin, int end);
  ENGINE_API vector<CString> tokenize(char c);
  ENGINE_API void replace(char c, char to);
  ENGINE_API int hashCode();
private:
  char* cont_;
  int len_;

  //string str_;
};

ENGINE_API CString operator+ (const char* s1, const CString& s2);
