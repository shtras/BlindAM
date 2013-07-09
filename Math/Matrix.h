/**
 * Linear algebra matrices
 * Author: Gregory Shtrasberg
 * Date: 01.2010
 **/
#pragma once
#include "Vector.h"

class Matrix4
{
public:
  ENGINE_API Matrix4();
  ENGINE_API Matrix4(double* cont);
  ENGINE_API Matrix4(double a00, double a01, double a02, double a03,
           double a10, double a11, double a12, double a13,
           double a20, double a21, double a22, double a23,
           double a30, double a31, double a32, double a33);
  ENGINE_API Matrix4(const Matrix4& other);
  ENGINE_API ~Matrix4();

  ENGINE_API void loadIdentity();

  ENGINE_API Vector4 getRow(const int i) const;
  ENGINE_API Vector4 getCol(const int i) const;

  ENGINE_API Matrix4 operator+ (const Matrix4& other) const;
  ENGINE_API Matrix4 operator- (const Matrix4& other) const;
  ENGINE_API Matrix4 operator* (const Matrix4& other) const;

  ENGINE_API Matrix4 operator+ (const double f) const;
  ENGINE_API Matrix4 operator- (const double f) const;
  ENGINE_API Matrix4 operator* (const double f) const;

  ENGINE_API Matrix4& operator+= (const Matrix4& other) {return *this = *this + other;}
  ENGINE_API Matrix4& operator-= (const Matrix4& other) {return *this = *this - other;}
  ENGINE_API Matrix4& operator*= (const Matrix4& other) {return *this = *this * other;}

  ENGINE_API Matrix4& operator+= (const double f) {return *this = *this + f;}
  ENGINE_API Matrix4& operator-= (const double f) {return *this = *this - f;}
  ENGINE_API Matrix4& operator*= (const double f) {return *this = *this * f;}

  ENGINE_API double* operator[](const int i);
  ENGINE_API const double* operator[](const int i) const;

  ENGINE_API double det() const;
  ENGINE_API operator double*() {return cont_;}
private:
  double cont_[16];
};
