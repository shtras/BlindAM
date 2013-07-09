/**
 * Linear algebra vectors
 * Author: Gregory Shtrasberg
 * Date: 01.2010
 **/
#pragma once

class Matrix4;

class Vector3
{
public:
  ENGINE_API Vector3();
  ENGINE_API Vector3(double* cont);
  ENGINE_API Vector3(double x, double y, double z);
  ENGINE_API Vector3(const Vector3& other);
  ENGINE_API ~Vector3();

  ENGINE_API void normalize();
  ENGINE_API Vector3 getNormalized() const;
  ENGINE_API double dot(const Vector3& other) const;
  ENGINE_API double getLength();
  ENGINE_API Vector3 operator+ (const Vector3& other) const;
  ENGINE_API Vector3 operator* (const Vector3& other) const;
  ENGINE_API Vector3 operator- (const Vector3& other) const;
  ENGINE_API Vector3 operator+ (const double f) const;
  ENGINE_API Vector3 operator* (const double f) const;
  ENGINE_API Vector3 operator- (const double f) const;
  ENGINE_API Vector3& operator+= (const Vector3& other) {return *this = *this + other;}
  ENGINE_API Vector3& operator*= (const Vector3& other) {return *this = *this * other;}
  ENGINE_API Vector3& operator-= (const Vector3& other) {return *this = *this - other;}
  ENGINE_API Vector3& operator+= (const double f) {return *this = *this + f;}
  ENGINE_API Vector3& operator*= (const double f) {return *this = *this * f;}
  ENGINE_API Vector3& operator-= (const double f) {return *this = *this - f;}

  ENGINE_API double& operator[] (const int i) {assert (i>=0 && i<3); return cont_[i];}
  ENGINE_API const double operator[] (const int i) const {assert (i>=0 && i<3); return cont_[i];}
  ENGINE_API operator double*() {return cont_;}
  ENGINE_API double getAngle();
protected:
  double cont_[3];
};

class Vector4
{
public:
  ENGINE_API Vector4();
  ENGINE_API Vector4(double* cont);
  ENGINE_API Vector4(double x, double y, double z, double w);
  ENGINE_API Vector4(const Vector4& other);
  ENGINE_API ~Vector4();

  ENGINE_API void normalize();
  ENGINE_API Vector4 getNormalized() const;
  ENGINE_API double dot(const Vector4& other) const;
  ENGINE_API Vector4 operator+ (const Vector4& other) const;
  ENGINE_API Vector4 operator- (const Vector4& other) const;
  ENGINE_API Vector4 operator+ (const double f) const;
  ENGINE_API Vector4 operator* (const double f) const;
  ENGINE_API Vector4 operator- (const double f) const;
  ENGINE_API Vector4& operator+= (const Vector4& other) {return *this = *this + other;}
  ENGINE_API Vector4& operator-= (const Vector4& other) {return *this = *this - other;}
  ENGINE_API Vector4& operator+= (const double f) {return *this = *this + f;}
  ENGINE_API Vector4& operator*= (const double f) {return *this = *this * f;}
  ENGINE_API Vector4& operator-= (const double f) {return *this = *this - f;}

  ENGINE_API Vector4 operator* (const Matrix4& mat) const;

  ENGINE_API double& operator[] (const int i) {assert (i>=0 && i<4); return cont_[i];}
  ENGINE_API const double operator[] (const int i) const {assert (i>=0 && i<4); return cont_[i];}
  ENGINE_API operator double*() {return cont_;}
private:
  double cont_[4];
};
