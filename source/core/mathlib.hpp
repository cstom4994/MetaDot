// Copyright(c) 2022-2023, KaoruXun All rights reserved.

// This source file may include
// Polypartition (MIT) by Ivan Fratric
// MarchingSquares(Tom Gibara) (MIT) by Juha Reunanen

#ifndef _METADOT_MATH_HPP_
#define _METADOT_MATH_HPP_

#include <cassert>
#include <list>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "core/core.hpp"
#include "core/mathlib_base.hpp"
#include "libs/imgui/imgui.h"

#ifdef ME_PLATFORM_WIN32
#undef far
#undef near
#endif

typedef struct U16Point {
    U16 x;
    U16 y;
} U16Point;

typedef struct metadot_rect {
    float x, y;
    float w, h;
} metadot_rect;

typedef signed int metadot_bool;

#define VECTOR3_ZERO \
    MEvec3 { 0.0f, 0.0f, 0.0f }
#define VECTOR3_FORWARD \
    MEvec3 { 1.0f, 0.0f, 0.0f }
#define VECTOR3_UP \
    MEvec3 { 0.0f, 0.0f, 1.0f }
#define VECTOR3_DOWN \
    MEvec3 { 0.0f, 0.0f, -1.0f }
#define VECTOR3_LEFT \
    MEvec3 { 0.0f, 1.0f, 0.0f }

#define INT_INFINITY 0x3f3f3f3f

// #define sign(x) (x > 0 ? 1 : (x < 0 ? -1 : 0))
#define UTIL_clamp(x, m, M) (x < m ? m : (x > M ? M : x))
#define FRAC0(x) (x - floorf(x))
#define FRAC1(x) (1 - x + floorf(x))

#define UTIL_cross(u, v) \
    (MEvec3) { (u).y *(v).z - (u).z *(v).y, (u).z *(v).x - (u).x *(v).z, (u).x *(v).y - (u).y *(v).x }
#define UTIL_dot(u, v) ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
// #define norm(v) sqrt(dot(v, v))// norm = length of  vector

F32 math_perlin(F32 x, F32 y, F32 z, int x_wrap = 0, int y_wrap = 0, int z_wrap = 0);

static_inline ImVec4 vec4_to_imvec4(const MEvec4 &v4) { return {v4.x, v4.y, v4.z, v4.w}; }

#pragma region NewMATH

namespace NewMaths {

constexpr std::size_t hash_combine(std::size_t l, std::size_t r) noexcept { return l ^ (r + 0x9e3779b9 + (l << 6) + (l >> 2)); }

F32 vec22angle(MEvec2 v2);
F32 clamp(F32 input, F32 min, F32 max);
int rand_range(int min, int max);
uint64_t rand_XOR();
inline F64 random_double();
inline F64 random_double(F64 min, F64 max);
struct v2;
struct RandState;
F32 v2_distance_2Points(v2 A, v2 B);
v2 unitvec_AtoB(v2 A, v2 B);
F32 signed_angle_v2(v2 A, v2 B);
v2 Rotate2D(v2 P, F32 sine, F32 cosine);
v2 Rotate2D(v2 P, F32 Angle);
v2 Rotate2D(v2 p, v2 o, F32 angle);
v2 Reflection2D(v2 P, v2 N);
bool PointInRectangle(v2 P, v2 A, v2 B, v2 C);
int sign(F32 x);
static F32 dot(v2 A, v2 B);
static F32 perpdot(v2 A, v2 B);
static bool operator==(v2 A, v2 B);

MEvec3 NormalizeVector(MEvec3 v);
MEvec3 Add(MEvec3 a, MEvec3 b);
MEvec3 Subtract(MEvec3 a, MEvec3 b);
MEvec3 ScalarMult(MEvec3 v, F32 s);
F64 Distance(MEvec3 a, MEvec3 b);
MEvec3 VectorProjection(MEvec3 a, MEvec3 b);
MEvec3 Reflection(MEvec3 *v1, MEvec3 *v2);

F64 DistanceFromPointToLine2D(MEvec3 lP1, MEvec3 lP2, MEvec3 p);

typedef struct Matrix3x3 {
    F32 m[3][3];
} Matrix3x3;

Matrix3x3 Transpose(Matrix3x3 m);
Matrix3x3 Identity();
MEvec3 Matrix3x3ToEulerAngles(Matrix3x3 m);
Matrix3x3 EulerAnglesToMatrix3x3(MEvec3 rotation);
MEvec3 RotateVector(MEvec3 v, Matrix3x3 m);
MEvec3 RotatePoint(MEvec3 p, MEvec3 r, MEvec3 pivot);
Matrix3x3 MultiplyMatrix3x3(Matrix3x3 a, Matrix3x3 b);

typedef struct Matrix4x4 {
    F32 m[4][4];
} Matrix4x4;

Matrix4x4 Identity4x4();
Matrix4x4 GetProjectionMatrix(F32 right, F32 left, F32 top, F32 bottom, F32 near, F32 far);

F32 Lerp(F64 t, F32 a, F32 b);
int Step(F32 edge, F32 x);
F32 Smoothstep(F32 edge0, F32 edge1, F32 x);
int Modulus(int a, int b);
F32 fModulus(F32 a, F32 b);
}  // namespace NewMaths

#pragma endregion NewMATH

#pragma region PCG

struct pcg_state_setseq_64 {  // Internals are *Private*.
    uint64_t state;           // RNG state.  All values are possible.
    uint64_t inc;             // Controls which RNG sequence (stream) is
                              // selected. Must *always* be odd.
};
typedef struct pcg_state_setseq_64 pcg32_random_t;

// If you *must* statically initialize it, here's one.

#define PCG32_INITIALIZER \
    { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

// pcg32_srandom(initstate, initseq)
// pcg32_srandom_r(rng, initstate, initseq):
//     Seed the rng.  Specified in two parts, state initializer and a
//     sequence selection constant (a.k.a. stream id)

void pcg32_srandom(uint64_t initstate, uint64_t initseq);
void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initseq);

// pcg32_random()
// pcg32_random_r(rng)
//     Generate a uniformly distributed 32-bit random number

uint32_t pcg32_random(void);
uint32_t pcg32_random_r(pcg32_random_t *rng);

// pcg32_boundedrand(bound):
// pcg32_boundedrand_r(rng, bound):
//     Generate a uniformly distributed number, r, where 0 <= r < bound

uint32_t pcg32_boundedrand(uint32_t bound);
uint32_t pcg32_boundedrand_r(pcg32_random_t *rng, uint32_t bound);

#pragma endregion PCG

#pragma region PORO

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "core/const.h"

namespace MetaEngine {
namespace math {

//=============================================================================
// Still in the works templated vector class

template <class Type>
class CVector2 {
public:
    typedef Type unit_type;
    //=========================================================================

    CVector2() : x(Type()), y(Type()) {}

    CVector2(Type x, Type y) : x(x), y(y) {}

    template <class T>
    explicit CVector2(const T &other) : x((Type)(other.x)), y((Type)(other.y)) {}

    void Set(Type x_, Type y_) {
        x = x_;
        y = y_;
    }

    //=========================================================================

    bool operator==(const CVector2<Type> &other) const { return (this->x == other.x && this->y == other.y); }
    bool operator!=(const CVector2<Type> &other) const { return !operator==(other); }

    //=========================================================================

    CVector2<Type> operator-() const { return CVector2<Type>(-x, -y); }

    CVector2<Type> &operator+=(const CVector2<Type> &v) {
        x += v.x;
        y += v.y;

        return *this;
    }

    CVector2<Type> &operator-=(const CVector2<Type> &v) {
        x -= v.x;
        y -= v.y;

        return *this;
    }

    CVector2<Type> &operator*=(float a) {
        x = (Type)(x * a);
        y = (Type)(y * a);

        return *this;
    }

    CVector2<Type> &operator/=(float a) {
        x = (Type)(x / a);
        y = (Type)(y / a);

        return *this;
    }

    //-------------------------------------------------------------------------

    CVector2<Type> operator+(const CVector2<Type> &other) const { return CVector2<Type>(this->x + other.x, this->y + other.y); }

    CVector2<Type> operator-(const CVector2<Type> &other) const { return CVector2<Type>(this->x - other.x, this->y - other.y); }

    CVector2<Type> operator*(float t) const { return CVector2<Type>((Type)(this->x * t), (Type)(this->y * t)); }

    CVector2<Type> operator/(float t) const { return CVector2<Type>((Type)(this->x / t), (Type)(this->y / t)); }

    //=========================================================================

    Type Dot(const CVector2<Type> &a, const CVector2<Type> &b) const { return a.x * b.x + a.y * b.y; }

    Type Cross(const CVector2<Type> &a, const CVector2<Type> &b) const { return a.x * b.y - a.y * b.x; }

    CVector2<Type> Cross(const CVector2<Type> &a, const Type &s) const { return CVector2<Type>(s * a.y, -s * a.x); }

    CVector2<Type> Cross(const Type &s, const CVector2<Type> &a) const { return CVector2<Type>(-s * a.y, s * a.x); }

    //=========================================================================

    Type LengthSquared() const { return (x * x + y * y); }

    float Length() const { return sqrtf((float)LengthSquared()); }

    CVector2<Type> Normalize() const {
        float d = Length();

        if (d > 0)
            return CVector2<Type>(Type(x / d), Type(y / d));
        else
            return CVector2<Type>(0, 0);
    }

    //=========================================================================
    // ripped from:
    // http://forums.indiegamer.com/showthread.php?t=10459

    Type Angle() const {
        CVector2<Type> normal = Normalize();
        Type angle = atan2(normal.y, normal.x);

        return angle;
    }

    Type Angle(const CVector2<Type> &x) const {
        Type dot = Dot(*this, x);
        Type cross = Cross(*this, x);

        // angle between segments
        Type angle = (Type)atan2(cross, dot);

        return angle;
    }

    CVector2<Type> &Rotate(Type angle_rad) {
        Type tx = x;
        x = (Type)x * (Type)cos(angle_rad) - y * (Type)sin(angle_rad);
        y = (Type)tx * (Type)sin(angle_rad) + y * (Type)cos(angle_rad);

        return *this;
    }

    CVector2<Type> &Rotate(const CVector2<Type> &centre, Type angle_rad) {
        CVector2<Type> D = *this - centre;
        D.Rotate(angle_rad);

        // *this = xCentre + D;
        D += centre;
        Set(D.x, D.y);

        return *this;
    }

    //=========================================================================

    Type x;
    Type y;
};

}  // namespace math
}  // end of namespace MetaEngine

// ---------- types ----------
namespace types {
typedef MetaEngine::math::CVector2<float> vector2;
typedef MetaEngine::math::CVector2<double> dvector2;
typedef MetaEngine::math::CVector2<int> ivector2;

typedef MetaEngine::math::CVector2<int> point;

}  // end of namespace types

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

namespace MetaEngine {
namespace math {

// typedef CVector2< float > Vec2;

//-----------------------------------------------------------------------------
template <class Type>
struct CMat22 {
    CMat22() { SetIdentity(); }
    CMat22(float angle) {
        Type c = (Type)cos(angle), s = (Type)sin(angle);
        col1.x = c;
        col2.x = -s;
        col1.y = s;
        col2.y = c;
    }

    CMat22(const CVector2<Type> &col1, const CVector2<Type> &col2) : col1(col1), col2(col2) {}

    CMat22(Type c1x, Type c1y, Type c2x, Type c2y) : col1(c1x, c1y), col2(c2x, c2y) {}

    CMat22<Type> Transpose() const { return CMat22<Type>(CVector2<Type>(col1.x, col2.x), CVector2<Type>(col1.y, col2.y)); }

    /// Initialize this matrix using columns.
    void Set(const CVector2<Type> &c1, const CVector2<Type> &c2) {
        col1 = c1;
        col2 = c2;
    }

    /// Initialize this matrix using an angle. This matrix becomes
    /// an orthonormal rotation matrix.
    void Set(float angle) {
        float c = cosf((float)angle), s = sinf((float)angle);
        col1.x = (Type)c;
        col2.x = (Type)(-s);
        col1.y = (Type)s;
        col2.y = (Type)c;
    }

    /// Set this to the identity matrix.
    void SetIdentity() {
        col1.x = (Type)1;
        col2.x = (Type)0;
        col1.y = (Type)0;
        col2.y = (Type)1;
    }

    /// Set this matrix to all zeros.
    void SetZero() {
        col1.x = 0;
        col2.x = 0;
        col1.y = 0;
        col2.y = 0;
    }

    /// Extract the angle from this matrix (assumed to be
    /// a rotation matrix).
    float GetAngle() const { return static_cast<float>(atan2(col1.y, col1.x)); }

    void SetAngle(float angle) { Set(angle); }

    CMat22<Type> Invert() const {
        Type a = col1.x, b = col2.x, c = col1.y, d = col2.y;
        CMat22<Type> B;
        Type det = a * d - b * c;
        assert(det != 0.0f);
        float fdet = 1.0f / (float)det;
        B.col1.x = Type(fdet * d);
        B.col2.x = Type(-fdet * b);
        B.col1.y = Type(-fdet * c);
        B.col2.y = Type(fdet * a);
        return B;
    }

    CVector2<Type> col1, col2;
};

}  // end of namespace math
}  // end of namespace MetaEngine

//----------------- types --------------------------------------------

namespace types {
typedef MetaEngine::math::CMat22<float> mat22;
}  // end of namespace types

namespace MetaEngine {
namespace math {

/// A transform contains translation and rotation. It is used to represent
/// the position and orientation of rigid frames.
template <typename Type>
struct CXForm {
    /// The default constructor does nothing (for performance).
    CXForm() : scale((Type)1, (Type)1) {}

    /// Initialize using a position vector and a rotation matrix.
    CXForm(const CVector2<Type> &position, const CMat22<Type> &R, const CVector2<Type> &scale) : position(position), R(R), scale(scale) {}

    const CXForm<Type> &operator=(const CXForm<Type> &other) {
        position = other.position;
        R = other.R;
        scale = other.scale;
        return *this;
    }

    /// Set this to the identity transform.
    void SetIdentity() {
        position.Set((Type)0, (Type)0);
        R.SetIdentity();
        scale.Set((Type)1, (Type)1);
    }

    CVector2<Type> position;
    CMat22<Type> R;
    CVector2<Type> scale;
};

}  // end of namespace math
}  // End of namespace MetaEngine

// -------------- types --------------------------

namespace types {
typedef MetaEngine::math::CXForm<float> xform;
}  // end of namespace types

namespace MetaEngine {
namespace math {

//-----------------------------------------------------------------------------
// returns a rounded value, if the value is less than 0, rounds it "down"
// otherwise rounds it rounds it up by 0.5f
float Round(float value);
double Round(double value);

inline float Round(float value) {
    if (value < 0)
        value -= 0.5f;
    else
        value += 0.5f;
    return std::floor(value);
}

inline double Round(double value) {
    if (value < 0)
        value -= 0.5;
    else
        value += 0.5;
    return std::floor(value);
}

//=============================================================================
// Min -function returns the smaller one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template <typename Type>
inline const Type &Min(const Type &c1, const Type &c2) {
    return (c1 < c2) ? c1 : c2;
}

template <typename Type>
inline const Type &Min(const Type &c1, const Type &c2, const Type &c3) {
    return (c1 < Min(c2, c3)) ? c1 : Min(c2, c3);
}

template <typename Type>
inline const Type &Min(const Type &c1, const Type &c2, const Type &c3, const Type &c4) {
    return (c1 < Min(c2, c3, c4)) ? c1 : Min(c2, c3, c4);
}

//=============================================================================
// Max -function returns the bigger one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template <typename Type>
inline const Type &Max(const Type &c1, const Type &c2) {
    return (c1 < c2) ? c2 : c1;
}

template <typename Type>
inline const Type &Max(const Type &c1, const Type &c2, const Type &c3) {
    return (c1 < Max(c2, c3)) ? Max(c2, c3) : c1;
}

template <typename Type>
inline const Type &Max(const Type &c1, const Type &c2, const Type &c3, const Type &c4) {
    return (c1 < Max(c2, c3, c4)) ? Max(c2, c3, c4) : c1;
}

//=============================================================================
// Returns the absolute value of the variable
//
// Required operators: < operator and - operator.
//.............................................................................

template <typename Type>
inline Type Absolute(const Type &n) {
    /*
if( n < 0 )
return -n;
else
return n;
*/

    return MetaEngine::math::Max(n, -n);
}

//=============================================================================
// Clams a give value to the desired
//
// Required operators: < operator
//.............................................................................

template <typename Type>
inline Type Clamp(const Type &a, const Type &low, const Type &high) {
    return MetaEngine::math::Max(low, MetaEngine::math::Min(a, high));
}

//=============================================================================
// Swaps to elements with each other.
//
// Required operators: = operator
//.............................................................................

template <typename Type>
inline void Swap(Type &a, Type &b) {
    Type temp = a;
    a = b;
    b = temp;
}

//=============================================================================
// Returns Square value of the value
//
// Required operators: * operator
//.............................................................................

template <typename Type>
inline Type Square(Type x) {
    return x * x;
}

//=============================================================================

template <typename Type>
inline Type ConvertRadToAngle(const Type &rad) {
    return rad * (180.0f / PI);
}

template <typename Type>
inline Type ConvertAngleToRad(const Type &angle) {
    return angle * (PI / 180.0f);
}

//=============================================================================

template <typename Type>
inline float Distance(const Type &vector1, const Type &vector2) {
    Type d;
    d.x = vector1.x - vector2.x;
    d.y = vector1.y - vector2.y;

    return sqrtf((float)(d.x * d.x + d.y * d.y));
}

template <typename Type>
inline float Distance(const Type &vector1) {
    return sqrtf((float)(vector1.x * vector1.x + vector1.y * vector1.y));
}

template <typename Type>
inline float DistanceSquared(const Type &vector1, const Type &vector2) {
    Type d;
    d.x = vector1.x - vector2.x;
    d.y = vector1.y - vector2.y;

    return (float)(d.x * d.x + d.y * d.y);
}

template <typename Type>
inline float DistanceSquared(const Type &vector1) {
    return (float)(vector1.x * vector1.x + vector1.y * vector1.y);
}

//=============================================================================

template <typename Type>
inline Type Lerp(const Type &a, const Type &b, float t) {
    return (Type)(a + t * (b - a));
}

//=============================================================================

//-----------------------------------------------------------------------------

template <class Type>
Type Dot(const CVector2<Type> &a, const CVector2<Type> &b) {
    return a.x * b.x + a.y * b.y;
}

template <class Type>
Type Cross(const CVector2<Type> &a, const CVector2<Type> &b) {
    return a.x * b.y - a.y * b.x;
}

template <class Type>
CVector2<Type> Cross(const CVector2<Type> &a, Type s) {
    return CVector2<Type>(s * a.y, -s * a.x);
}
template <class Type>
CVector2<Type> Cross(Type s, const CVector2<Type> &a) {
    return CVector2<Type>(-s * a.y, s * a.x);
}

// ----------------------------------------------------------------------------

template <class Type>
CVector2<Type> operator*(const CMat22<Type> &A, const CVector2<Type> &v) {
    return CVector2<Type>(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

template <class Type>
CVector2<Type> operator*(Type s, const CVector2<Type> &v) {
    return CVector2<Type>(s * v.x, s * v.y);
}

template <class Type>
CMat22<Type> operator+(const CMat22<Type> &A, const CMat22<Type> &B) {
    return CMat22<Type>(A.col1 + B.col1, A.col2 + B.col2);
}

template <class Type>
CMat22<Type> operator*(const CMat22<Type> &A, const CMat22<Type> &B) {
    return CMat22<Type>(A * B.col1, A * B.col2);
}

// ----------------------------------------------------------------------------

template <class Type>
CVector2<Type> Abs(const CVector2<Type> &a) {
    return CVector2<Type>(fabsf(a.x), fabsf(a.y));
}

template <class Type>
CMat22<Type> Abs(const CMat22<Type> &A) {
    return CMat22<Type>(Abs(A.col1), Abs(A.col2));
}

// ----------------------------------------------------------------------------

template <class Type>
inline Type Sign(Type value) {
    return value < (Type)0 ? (Type)-1 : (Type)1;
}

// ----------------------------------------------------------------------------

template <class Type>
CVector2<Type> Mul(const CVector2<Type> &a, const CVector2<Type> &b) {
    CVector2<Type> result(a.x * b.x, a.y * b.y);
    return result;
}

/// Multiply a matrix times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another.
template <class Type, class Vector2>
Vector2 Mul(const CMat22<Type> &A, const Vector2 &v) {
    Vector2 u;
    u.x = A.col1.x * v.x + A.col2.x * v.y;
    u.y = A.col1.y * v.x + A.col2.y * v.y;
    // u.Set( A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y );
    return u;
}

/// Multiply a matrix transpose times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another (inverse transform).
template <class Type>
CVector2<Type> MulT(const CMat22<Type> &A, const CVector2<Type> &v) {
    CVector2<Type> u;
    u.Set(Dot(v, A.col1), Dot(v, A.col2));
    return u;
}

template <class Type>
CVector2<Type> Mul(const CXForm<Type> &T, const CVector2<Type> &v) {
    return T.position + Mul(T.R, v);
}

#if 0
template< class Type, class Vector2 >
Vector2 Mul( const CXForm< Type >& T, const Vector2& v )
{
    Vector2 result;
    result.x = v.x * T.scale.x;
    result.y = v.y * T.scale.y;
    result = Mul( T.R, result );
    result.x += T.position.x;
    result.y += T.position.y;
    return result;
}
#endif

template <class Type, class Vector2>
Vector2 MulWithScale(const CXForm<Type> &T, const Vector2 &v) {
    Vector2 result;
    result.x = v.x * T.scale.x;
    result.y = v.y * T.scale.y;
    result = Mul(T.R, result);
    result.x += T.position.x;
    result.y += T.position.y;
    return result;
}

template <class Type>
CXForm<Type> Mul(const CXForm<Type> &T, const CXForm<Type> &v) {

    CXForm<Type> result;

    result.position.x = T.scale.x * v.position.x;
    result.position.y = T.scale.y * v.position.y;
    result.position = Mul(T.R, result.position);
    result.position += T.position;

    result.R = Mul(T.R, v.R);
    result.scale.x = T.scale.x * v.scale.x;
    result.scale.y = T.scale.y * v.scale.y;

    return result;
}

template <class Type>
CVector2<Type> MulT(const CXForm<Type> &T, const CVector2<Type> &v) {
    return MulT(T.R, v - T.position);
}

template <class Type>
CXForm<Type> MulT(const CXForm<Type> &T, const CXForm<Type> &v) {

    CXForm<Type> result;

    result.scale.x = v.scale.x / T.scale.x;
    result.scale.y = v.scale.y / T.scale.y;
    result.R = Mul(T.R.Invert(), v.R);

    result.position.x = (v.position.x - T.position.x) / T.scale.x;
    result.position.y = (v.position.y - T.position.y) / T.scale.y;
    // result.position = v.position - T.position;
    result.position = MulT(T.R, result.position);

    return result;
}

template <class Type>
CVector2<Type> MulTWithScale(const CXForm<Type> &T, const CVector2<Type> &v) {
    CVector2<Type> result(v);
    if (T.scale.x != 0) result.x /= T.scale.x;
    if (T.scale.y != 0) result.y /= T.scale.y;
    return MulT(T.R, result - T.position);
}

// A * B
template <class Type>
inline CMat22<Type> Mul(const CMat22<Type> &A, const CMat22<Type> &B) {
    CMat22<Type> C;
    C.Set(Mul(A, B.col1), Mul(A, B.col2));
    return C;
}

// ----------------------------------------------------------------------------

template <class Type>
inline CVector2<float> ClosestPointOnLineSegment(const CVector2<Type> &a, const CVector2<Type> &b, const CVector2<Type> &p) {
    CVector2<float> c(p - a);
    CVector2<float> v(b - a);
    float distance = v.Length();

    // optimized normalized
    // v = v.Normalise();
    if (distance != 0) {
        v.x /= distance;
        v.y /= distance;
    }

    float t = (float)Dot(v, c);

    if (t < 0) return CVector2<float>(a);

    if (t > distance) return CVector2<float>(b);

    v *= t;

    return CVector2<float>(a) + v;
}

// ----------------------------------------------------------------------------

template <class Type>
inline float DistanceFromLineSquared(const CVector2<Type> &a, const CVector2<Type> &b, const CVector2<Type> &p) {
    CVector2<float> delta = ClosestPointOnLineSegment(a, b, p) - CVector2<float>(p);

    return delta.LengthSquared();
}

// ----------------------------------------------------------------------------

template <class Type>
inline float DistanceFromLine(const CVector2<Type> &a, const CVector2<Type> &b, const CVector2<Type> &p) {
    return sqrtf((float)DistanceFromLineSquared(a, b, p));
}

// ----------------------------------------------------------------------------

template <class T>
bool LineIntersection(const CVector2<T> &startA, const CVector2<T> &endA, const CVector2<T> &startB, const CVector2<T> &endB, CVector2<T> &result) {

    // TODO: reuse mathutil.intersect
    float d = (endB.y - startB.y) * (endA.x - startA.x) - (endB.x - startB.x) * (endA.y - startA.y);

    if (d == 0)  // parallel lines
        return false;

    float uA = (endB.x - startB.x) * (startA.y - startB.y) - (endB.y - startB.y) * (startA.x - startB.x);
    uA /= d;
    float uB = (endA.x - startA.x) * (startA.y - startB.y) - (endA.y - startA.y) * (startA.x - startB.x);
    uB /= d;

    if (uA < 0 || uA > 1 || uB < 0 || uB > 1) return false;  // intersection point isn't between the start and endpoints

    result.Set(startA.x + uA * (endA.x - startA.x), startA.y + uA * (endA.y - startA.y));

    // result = position;
    return true;
}

// ----------------------------------------------------------------------------

template <class PType>
float DistanceFromAABB(const CVector2<PType> &point, const CVector2<PType> &aabb_min, const CVector2<PType> &aabb_max) {
    if (IsPointInsideAABB(point, aabb_min, aabb_max)) return 0;

    float lowest = 0;
    float temp = MetaEngine::math::DistanceFromLineSquared(aabb_min, CVector2<PType>(aabb_max.x, aabb_min.y), point);
    if (temp < lowest) lowest = temp;

    temp = MetaEngine::math::DistanceFromLineSquared(CVector2<PType>(aabb_max.x, aabb_min.y), CVector2<PType>(aabb_max.x, aabb_max.y), point);
    if (temp < lowest) lowest = temp;

    temp = MetaEngine::math::DistanceFromLineSquared(CVector2<PType>(aabb_max.x, aabb_max.y), CVector2<PType>(aabb_min.x, aabb_max.y), point);
    if (temp < lowest) lowest = temp;

    temp = MetaEngine::math::DistanceFromLineSquared(CVector2<PType>(aabb_min.x, aabb_max.y), CVector2<PType>(aabb_min.x, aabb_min.y), point);
    if (temp < lowest) lowest = temp;

    return sqrtf(lowest);
}

template <class T>
class CAverager {
public:
    CAverager() : value(T()), current_value(T()), count(0) {}

    virtual ~CAverager() {}

    virtual void Reset() {
        value = T();
        count = 0;
    }

    virtual T Add(const T &other) {
        current_value = other;
        value += other;
        count++;

        return GetAverage();
    }

    T operator+=(const T &other) { return Add(other); }

    T GetAverage() const {
        if (count == 0) return T();

        return (T)(value / (T)count);
    }

    T GetCurrent() const { return current_value; }
    T GetTotal() const { return value; }
    unsigned int GetCount() const { return count; }

private:
    T value;
    T current_value;
    unsigned int count;
};

///////////////////////////////////////////////////////////////////////////////

typedef float PointType;

bool IsPointInsideRect(const CVector2<PointType> &point, const CVector2<PointType> &position, const CVector2<PointType> &width, float rotation);

bool IsPointInsideCircle(const CVector2<PointType> &point, const CVector2<PointType> &position, float d);

bool IsPointInsidePolygon(const CVector2<PointType> &point, const std::vector<CVector2<PointType>> &polygon);
bool IsPointInsidePolygon(const CVector2<int> &point, const std::vector<CVector2<int>> &polygon);

bool IsPointInsidePolygon_Better(const CVector2<PointType> &point, const std::vector<CVector2<PointType>> &pgon);

template <class Type>
bool IsPointInsideAABB(const Type &point, const Type &rect_low, const Type &rect_high) {
    return (rect_low.x <= point.x && rect_high.x > point.x && rect_low.y <= point.y && rect_high.y > point.y);
}

bool DoesLineAndBoxCollide(const CVector2<PointType> &p1, const CVector2<PointType> &p2, const CVector2<PointType> &rect_low, const CVector2<PointType> &rect_high);

bool TestLineAABB(const CVector2<PointType> &p1, const CVector2<PointType> &p2, const CVector2<PointType> &rect_low, const CVector2<PointType> &rect_high);

template <class PType>
bool TestAABBAABBIntersection(const CVector2<PType> &a_min, const CVector2<PType> &a_max, const CVector2<PType> &b_min, const CVector2<PType> &b_max) {
    if (a_max.x < b_min.x || a_max.y < b_min.y || a_min.x > b_max.x || a_min.y > b_max.y) {
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

template <class T>
class CStatisticsHelper : public math::CAverager<T> {
public:
    CStatisticsHelper() : CAverager<T>(), minValue(std::numeric_limits<T>::max()), maxValue(std::numeric_limits<T>::min()) {}

    virtual void Reset() {
        minValue = std::numeric_limits<T>::max();
        maxValue = std::numeric_limits<T>::min();

        CAverager<T>::Reset();
    }

    virtual T Add(const T &other) {
        if (other < minValue) minValue = other;
        if (other > maxValue) maxValue = other;

        return CAverager<T>::Add(other);
    }

    T GetMin() const { return minValue; }

    T GetMax() const { return maxValue; }

private:
    T minValue;
    T maxValue;
};
///////////////////////////////////////////////////////////////////////////////

template <typename T>
class CAngle {
    static const T TWO_PI;

    T AngleMod(T value) {
        while (value < 0) {
            value += TWO_PI;
        }

        value = static_cast<T>(fmod((double)value, (double)TWO_PI));

        return value;
    }

public:
    CAngle() : mAng(T()) {}
    CAngle(const CAngle &other) : mAng(other.mAng) {}
    CAngle(T value) { SetValue(value); }

    T GetValue() const { return mAng; }
    void SetValue(T value) { mAng = AngleMod(value); }

    void operator+=(const CAngle &other) {
        mAng += other.mAng;
        if (mAng >= TWO_PI) mAng -= TWO_PI;

        ME_ASSERT_E(mAng >= 0 && mAng <= TWO_PI);
    }

    void operator-=(const CAngle &other) {
        mAng -= other.mAng;
        if (mAng < 0) mAng += TWO_PI;

        ME_ASSERT_E(mAng >= 0 && mAng <= TWO_PI);
    }

    CAngle operator+(const CAngle &other) const {
        CAngle result;
        result.mAng = mAng + other.mAng;
        if (result.mAng >= TWO_PI) result.mAng -= TWO_PI;

        return result;
    }

    CAngle operator-(const CAngle &other) const {
        CAngle result;
        result.mAng = mAng - other.mAng;
        if (result.mAng < 0) result.mAng += TWO_PI;

        return result;
    }

    bool operator==(const CAngle &other) const { return GetValue() == other.GetValue(); }

private:
    T mAng;
};

template <typename T>
const T CAngle<T>::TWO_PI = (T)(2 * PI);

}  // end of namespace math
}  // namespace MetaEngine

// ---------------- types ---------------------

namespace types {
typedef MetaEngine::math::CAngle<float> angle;
}  // end of namespace types

#pragma endregion PORO

#pragma region c2

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// 2d vector.
typedef struct METAENGINE_V2 {
    float x;
    float y;
} METAENGINE_V2;

// Use this to create a v2 struct.
// The C++ API uses V2(x, y).
ME_INLINE METAENGINE_V2 metadot_v2(float x, float y) {
    METAENGINE_V2 result;
    result.x = x;
    result.y = y;
    return result;
}

// Rotation about an axis composed of cos/sin pair.
typedef struct METAENGINE_SinCos {
    float s;
    float c;
} METAENGINE_SinCos;

// 2x2 matrix.
typedef struct METAENGINE_M2x2 {
    METAENGINE_V2 x;
    METAENGINE_V2 y;
} METAENGINE_M2x2;

// 2d transformation, mostly useful for graphics and not physics colliders, since it supports scale.
typedef struct METAENGINE_M3x2 {
    METAENGINE_M2x2 m;
    METAENGINE_V2 p;
} METAENGINE_M3x2;

// 2d transformation, mostly useful for physics colliders since there's no scale.
typedef struct METAENGINE_Transform {
    METAENGINE_SinCos r;
    METAENGINE_V2 p;
} METAENGINE_Transform;

// 2d plane, aka line.
typedef struct METAENGINE_Halfspace {
    METAENGINE_V2 n;  // normal
    float d;          // distance to origin; d = ax + by = dot(n, p)
} METAENGINE_Halfspace;

// A ray is a directional line segment. It starts at an endpoint and extends into another direction
// for a specified distance (defined by t).
typedef struct METAENGINE_Ray {
    METAENGINE_V2 p;  // position
    METAENGINE_V2 d;  // direction (normalized)
    float t;          // distance along d from position p to find endpoint of ray
} METAENGINE_Ray;

// The results for a raycast query.
typedef struct METAENGINE_Raycast {
    float t;          // time of impact
    METAENGINE_V2 n;  // normal of surface at impact (unit length)
} METAENGINE_Raycast;

typedef struct METAENGINE_Circle {
    METAENGINE_V2 p;
    float r;
} METAENGINE_Circle;

// Axis-aligned bounding box. A box that cannot rotate.
typedef struct METAENGINE_Aabb {
    METAENGINE_V2 min;
    METAENGINE_V2 max;
} METAENGINE_Aabb;

// Box that cannot rotate defined with integers instead of floats. Not used for collision detection,
// but still sometimes useful.
typedef struct METAENGINE_Rect {
    int w, h, x, y;
} METAENGINE_Rect;

#define METAENGINE_PI 3.14159265f

//--------------------------------------------------------------------------------------------------
// Scalar float ops.

ME_INLINE float metadot_min(float a, float b) { return a < b ? a : b; }
ME_INLINE float metadot_max(float a, float b) { return b < a ? a : b; }
ME_INLINE float metadot_clamp(float a, float lo, float hi) { return metadot_max(lo, metadot_min(a, hi)); }
ME_INLINE float metadot_clamp01(float a) { return metadot_max(0.0f, metadot_min(a, 1.0f)); }
ME_INLINE float metadot_sign(float a) { return a < 0 ? -1.0f : 1.0f; }
ME_INLINE float metadot_intersect(float da, float db) { return da / (da - db); }
ME_INLINE float metadot_safe_invert(float a) { return a != 0 ? 1.0f / a : 0; }
ME_INLINE float metadot_lerp(float a, float b, float t) { return a + (b - a) * t; }
ME_INLINE float metadot_remap(float t, float lo, float hi) { return (hi - lo) != 0 ? (t - lo) / (hi - lo) : 0; }
ME_INLINE float metadot_mod(float x, float m) { return x - (int)(x / m) * m; }
ME_INLINE float metadot_fract(float x) { return x - floorf(x); }

ME_INLINE int metadot_sign_int(int a) { return a < 0 ? -1 : 1; }
#define metadot_min(a, b) ((a) < (b) ? (a) : (b))
#define metadot_max(a, b) ((b) < (a) ? (a) : (b))
ME_INLINE float metadot_abs(float a) { return fabsf(a); }
ME_INLINE int metadot_abs_int(int a) {
    int mask = a >> ((sizeof(int) * 8) - 1);
    return (a + mask) ^ mask;
}
ME_INLINE int metadot_clamp_int(int a, int lo, int hi) { return metadot_max(lo, metadot_min(a, hi)); }
ME_INLINE int metadot_clamp01_int(int a) { return metadot_max(0, metadot_min(a, 1)); }
ME_INLINE bool metadot_is_even(int x) { return (x % 2) == 0; }
ME_INLINE bool metadot_is_odd(int x) { return !metadot_is_even(x); }

//--------------------------------------------------------------------------------------------------
// Bit manipulation.

ME_INLINE bool metadot_is_power_of_two(int a) { return a != 0 && (a & (a - 1)) == 0; }
ME_INLINE bool metadot_is_power_of_two_uint(uint64_t a) { return a != 0 && (a & (a - 1)) == 0; }
ME_INLINE int metadot_fit_power_of_two(int a) {
    a--;
    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;
    a++;
    return a;
}

//--------------------------------------------------------------------------------------------------
// Easing functions.
// Adapted from Noel Berry: https://github.com/NoelFB/blah/blob/master/include/blah_ease.h

ME_INLINE float metadot_smoothstep(float x) { return x * x * (3.0f - 2.0f * x); }
ME_INLINE float metadot_quad_in(float x) { return x * x; }
ME_INLINE float metadot_quad_out(float x) { return -(x * (x - 2.0f)); }
ME_INLINE float metadot_quad_in_out(float x) {
    if (x < 0.5f)
        return 2.0f * x * x;
    else
        return (-2.0f * x * x) + (4.0f * x) - 1.0f;
}
ME_INLINE float metadot_cube_in(float x) { return x * x * x; }
ME_INLINE float metadot_cube_out(float x) {
    float f = (x - 1);
    return f * f * f + 1.0f;
}
ME_INLINE float metadot_cube_in_out(float x) {
    if (x < 0.5f)
        return 4.0f * x * x * x;
    else {
        float f = ((2.0f * x) - 2.0f);
        return 0.5f * x * x * x + 1.0f;
    }
}
ME_INLINE float metadot_quart_in(float x) { return x * x * x * x; }
ME_INLINE float metadot_quart_out(float x) {
    float f = (x - 1.0f);
    return f * f * f * (1.0f - x) + 1.0f;
}
ME_INLINE float metadot_quart_in_out(float x) {
    if (x < 0.5f)
        return 8.0f * x * x * x * x;
    else {
        float f = (x - 1);
        return -8.0f * f * f * f * f + 1.0f;
    }
}
ME_INLINE float metadot_quint_in(float x) { return x * x * x * x * x; }
ME_INLINE float metadot_quint_out(float x) {
    float f = (x - 1);
    return f * f * f * f * f + 1.0f;
}
ME_INLINE float metadot_quint_in_out(float x) {
    if (x < 0.5f)
        return 16.0f * x * x * x * x * x;
    else {
        float f = ((2.0f * x) - 2.0f);
        return 0.5f * f * f * f * f * f + 1.0f;
    }
}
ME_INLINE float metadot_sin_in(float x) { return sinf((x - 1.0f) * METAENGINE_PI * 0.5f) + 1.0f; }
ME_INLINE float metadot_sin_out(float x) { return sinf(x * (METAENGINE_PI * 0.5f)); }
ME_INLINE float metadot_sin_in_out(float x) { return 0.5f * (1.0f - cosf(x * METAENGINE_PI)); }
ME_INLINE float metadot_circle_in(float x) { return 1.0f - sqrtf(1.0f - (x * x)); }
ME_INLINE float metadot_circle_out(float x) { return sqrtf((2.0f - x) * x); }
ME_INLINE float metadot_circle_in_out(float x) {
    if (x < 0.5f)
        return 0.5f * (1.0f - sqrtf(1.0f - 4.0f * (x * x)));
    else
        return 0.5f * (sqrtf(-((2.0f * x) - 3.0f) * ((2.0f * x) - 1.0f)) + 1.0f);
}
ME_INLINE float metadot_back_in(float x) { return x * x * x - x * sinf(x * METAENGINE_PI); }
ME_INLINE float metadot_back_out(float x) {
    float f = (1.0f - x);
    return 1.0f - (x * x * x - x * sinf(f * METAENGINE_PI));
}
ME_INLINE float metadot_back_in_out(float x) {
    if (x < 0.5f) {
        float f = 2.0f * x;
        return 0.5f * (f * f * f - f * sinf(f * METAENGINE_PI));
    } else {
        float f = (1.0f - (2.0f * x - 1.0f));
        return 0.5f * (1.0f - (f * f * f - f * sinf(f * METAENGINE_PI))) + 0.5f;
    }
}

//--------------------------------------------------------------------------------------------------
// 2D vector ops.

ME_INLINE METAENGINE_V2 metadot_add_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_v2(a.x + b.x, a.y + b.y); }
ME_INLINE METAENGINE_V2 metadot_sub_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_v2(a.x - b.x, a.y - b.y); }

ME_INLINE float metadot_dot(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x * b.x + a.y * b.y; }

ME_INLINE METAENGINE_V2 metadot_mul_v2_f(METAENGINE_V2 a, float b) { return metadot_v2(a.x * b, a.y * b); }
ME_INLINE METAENGINE_V2 metadot_mul_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_v2(a.x * b.x, a.y * b.y); }
ME_INLINE METAENGINE_V2 metadot_div_v2_f(METAENGINE_V2 a, float b) { return metadot_v2(a.x / b, a.y / b); }

ME_INLINE METAENGINE_V2 metadot_skew(METAENGINE_V2 a) { return metadot_v2(-a.y, a.x); }
ME_INLINE METAENGINE_V2 metadot_cw90(METAENGINE_V2 a) { return metadot_v2(a.y, -a.x); }
ME_INLINE float metadot_det2(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x * b.y - a.y * b.x; }
ME_INLINE float metadot_cross(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_det2(a, b); }
ME_INLINE METAENGINE_V2 metadot_cross_v2_f(METAENGINE_V2 a, float b) { return metadot_v2(b * a.y, -b * a.x); }
ME_INLINE METAENGINE_V2 metadot_cross_f_v2(float a, METAENGINE_V2 b) { return metadot_v2(-a * b.y, a * b.x); }
ME_INLINE METAENGINE_V2 metadot_min_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_v2(metadot_min(a.x, b.x), metadot_min(a.y, b.y)); }
ME_INLINE METAENGINE_V2 metadot_max_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return metadot_v2(metadot_max(a.x, b.x), metadot_max(a.y, b.y)); }
ME_INLINE METAENGINE_V2 metadot_clamp_v2(METAENGINE_V2 a, METAENGINE_V2 lo, METAENGINE_V2 hi) { return metadot_max_v2(lo, metadot_min_v2(a, hi)); }
ME_INLINE METAENGINE_V2 metadot_clamp01_v2(METAENGINE_V2 a) { return metadot_max_v2(metadot_v2(0, 0), metadot_min_v2(a, metadot_v2(1, 1))); }
ME_INLINE METAENGINE_V2 metadot_abs_v2(METAENGINE_V2 a) { return metadot_v2(fabsf(a.x), fabsf(a.y)); }
ME_INLINE float metadot_hmin(METAENGINE_V2 a) { return metadot_min(a.x, a.y); }
ME_INLINE float metadot_hmax(METAENGINE_V2 a) { return metadot_max(a.x, a.y); }
ME_INLINE float metadot_len(METAENGINE_V2 a) { return sqrtf(metadot_dot(a, a)); }
ME_INLINE float metadot_distance(METAENGINE_V2 a, METAENGINE_V2 b) {
    METAENGINE_V2 d = metadot_sub_v2(b, a);
    return sqrtf(metadot_dot(d, d));
}
ME_INLINE METAENGINE_V2 metadot_norm(METAENGINE_V2 a) { return metadot_div_v2_f(a, metadot_len(a)); }
ME_INLINE METAENGINE_V2 metadot_safe_norm(METAENGINE_V2 a) {
    float sq = metadot_dot(a, a);
    return sq ? metadot_div_v2_f(a, sqrtf(sq)) : metadot_v2(0, 0);
}
ME_INLINE float metadot_safe_norm_f(float a) { return a == 0 ? 0 : metadot_sign(a); }
ME_INLINE int metadot_safe_norm_int(int a) { return a == 0 ? 0 : metadot_sign_int(a); }
ME_INLINE METAENGINE_V2 metadot_neg_v2(METAENGINE_V2 a) { return metadot_v2(-a.x, -a.y); }
ME_INLINE METAENGINE_V2 metadot_lerp_v2(METAENGINE_V2 a, METAENGINE_V2 b, float t) { return metadot_add_v2(a, metadot_mul_v2_f(metadot_sub_v2(b, a), t)); }
ME_INLINE METAENGINE_V2 metadot_bezier(METAENGINE_V2 a, METAENGINE_V2 c0, METAENGINE_V2 b, float t) { return metadot_lerp_v2(metadot_lerp_v2(a, c0, t), metadot_lerp_v2(c0, b, t), t); }
ME_INLINE METAENGINE_V2 metadot_bezier2(METAENGINE_V2 a, METAENGINE_V2 c0, METAENGINE_V2 c1, METAENGINE_V2 b, float t) {
    return metadot_bezier(metadot_lerp_v2(a, c0, t), metadot_lerp_v2(c0, c1, t), metadot_lerp_v2(c1, b, t), t);
}
ME_INLINE int metadot_lesser_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x < b.x && a.y < b.y; }
ME_INLINE int metadot_greater_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x > b.x && a.y > b.y; }
ME_INLINE int metadot_lesser_equal_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x <= b.x && a.y <= b.y; }
ME_INLINE int metadot_greater_equal_v2(METAENGINE_V2 a, METAENGINE_V2 b) { return a.x >= b.x && a.y >= b.y; }
ME_INLINE METAENGINE_V2 metadot_floor(METAENGINE_V2 a) { return metadot_v2(floorf(a.x), floorf(a.y)); }
ME_INLINE METAENGINE_V2 metadot_round(METAENGINE_V2 a) { return metadot_v2(roundf(a.x), roundf(a.y)); }
ME_INLINE METAENGINE_V2 metadot_safe_invert_v2(METAENGINE_V2 a) { return metadot_v2(metadot_safe_invert(a.x), metadot_safe_invert(a.y)); }
ME_INLINE METAENGINE_V2 metadot_sign_v2(METAENGINE_V2 a) { return metadot_v2(metadot_sign(a.x), metadot_sign(a.y)); }

ME_INLINE int metadot_parallel(METAENGINE_V2 a, METAENGINE_V2 b, float tol) {
    float k = metadot_len(a) / metadot_len(b);
    b = metadot_mul_v2_f(b, k);
    if (fabs(a.x - b.x) < tol && fabs(a.y - b.y) < tol) return 1;
    return 0;
}

//--------------------------------------------------------------------------------------------------
// METAENGINE_SinCos rotation ops.

ME_INLINE METAENGINE_SinCos metadot_sincos_f(float radians) {
    METAENGINE_SinCos r;
    r.s = sinf(radians);
    r.c = cosf(radians);
    return r;
}
ME_INLINE METAENGINE_SinCos metadot_sincos() {
    METAENGINE_SinCos r;
    r.c = 1.0f;
    r.s = 0;
    return r;
}
ME_INLINE METAENGINE_V2 metadot_x_axis(METAENGINE_SinCos r) { return metadot_v2(r.c, r.s); }
ME_INLINE METAENGINE_V2 metadot_y_axis(METAENGINE_SinCos r) { return metadot_v2(-r.s, r.c); }
ME_INLINE METAENGINE_V2 metadot_mul_sc_v2(METAENGINE_SinCos a, METAENGINE_V2 b) { return metadot_v2(a.c * b.x - a.s * b.y, a.s * b.x + a.c * b.y); }
ME_INLINE METAENGINE_V2 metadot_mulT_sc_v2(METAENGINE_SinCos a, METAENGINE_V2 b) { return metadot_v2(a.c * b.x + a.s * b.y, -a.s * b.x + a.c * b.y); }
ME_INLINE METAENGINE_SinCos metadot_mul_sc(METAENGINE_SinCos a, METAENGINE_SinCos b) {
    METAENGINE_SinCos c;
    c.c = a.c * b.c - a.s * b.s;
    c.s = a.s * b.c + a.c * b.s;
    return c;
}
ME_INLINE METAENGINE_SinCos metadot_mulT_sc(METAENGINE_SinCos a, METAENGINE_SinCos b) {
    METAENGINE_SinCos c;
    c.c = a.c * b.c + a.s * b.s;
    c.s = a.c * b.s - a.s * b.c;
    return c;
}

// Remaps the result from atan2f to the continuous range of 0, 2*PI.
ME_INLINE float metadot_atan2_360(float y, float x) { return atan2f(-y, -x) + METAENGINE_PI; }
ME_INLINE float metadot_atan2_360_sc(METAENGINE_SinCos r) { return metadot_atan2_360(r.s, r.c); }
ME_INLINE float metadot_atan2_360_v2(METAENGINE_V2 v) { return atan2f(-v.y, -v.x) + METAENGINE_PI; }

// Computes the shortest angle to rotate the vector a to the vector b.
ME_INLINE float metadot_shortest_arc(METAENGINE_V2 a, METAENGINE_V2 b) {
    float c = metadot_dot(a, b);
    float s = metadot_det2(a, b);
    float theta = acosf(c);
    if (s > 0) {
        return theta;
    } else {
        return -theta;
    }
}

ME_INLINE float metadot_angle_diff(float radians_a, float radians_b) { return metadot_mod((radians_b - radians_a) + METAENGINE_PI, 2.0f * METAENGINE_PI) - METAENGINE_PI; }
ME_INLINE METAENGINE_V2 metadot_from_angle(float radians) { return metadot_v2(cosf(radians), sinf(radians)); }

//--------------------------------------------------------------------------------------------------
// m2 ops.
// 2D graphics matrix for only scale + rotate.

ME_INLINE METAENGINE_M2x2 metadot_mul_m2_f(METAENGINE_M2x2 a, float b) {
    METAENGINE_M2x2 c;
    c.x = metadot_mul_v2_f(a.x, b);
    c.y = metadot_mul_v2_f(a.y, b);
    return c;
}
ME_INLINE METAENGINE_V2 metadot_mul_m2_v2(METAENGINE_M2x2 a, METAENGINE_V2 b) {
    METAENGINE_V2 c;
    c.x = a.x.x * b.x + a.y.x * b.y;
    c.y = a.x.y * b.x + a.y.y * b.y;
    return c;
}
ME_INLINE METAENGINE_M2x2 metadot_mul_m2(METAENGINE_M2x2 a, METAENGINE_M2x2 b) {
    METAENGINE_M2x2 c;
    c.x = metadot_mul_m2_v2(a, b.x);
    c.y = metadot_mul_m2_v2(a, b.y);
    return c;
}

//--------------------------------------------------------------------------------------------------
// m3x2 ops.
// General purpose 2D graphics matrix; scale + rotate + translate.

ME_INLINE METAENGINE_V2 metadot_mul_m32_v2(METAENGINE_M3x2 a, METAENGINE_V2 b) { return metadot_add_v2(metadot_mul_m2_v2(a.m, b), a.p); }
ME_INLINE METAENGINE_M3x2 metadot_mul_m32(METAENGINE_M3x2 a, METAENGINE_M3x2 b) {
    METAENGINE_M3x2 c;
    c.m = metadot_mul_m2(a.m, b.m);
    c.p = metadot_add_v2(metadot_mul_m2_v2(a.m, b.p), a.p);
    return c;
}
ME_INLINE METAENGINE_M3x2 metadot_make_identity() {
    METAENGINE_M3x2 m;
    m.m.x = metadot_v2(1, 0);
    m.m.y = metadot_v2(0, 1);
    m.p = metadot_v2(0, 0);
    return m;
}
ME_INLINE METAENGINE_M3x2 metadot_make_translation_f(float x, float y) {
    METAENGINE_M3x2 m;
    m.m.x = metadot_v2(1, 0);
    m.m.y = metadot_v2(0, 1);
    m.p = metadot_v2(x, y);
    return m;
}
ME_INLINE METAENGINE_M3x2 metadot_make_translation(METAENGINE_V2 p) { return metadot_make_translation_f(p.x, p.y); }
ME_INLINE METAENGINE_M3x2 metadot_make_scale(METAENGINE_V2 s) {
    METAENGINE_M3x2 m;
    m.m.x = metadot_v2(s.x, 0);
    m.m.y = metadot_v2(0, s.y);
    m.p = metadot_v2(0, 0);
    return m;
}
ME_INLINE METAENGINE_M3x2 metadot_make_scale_f(float s) { return metadot_make_scale(metadot_v2(s, s)); }
ME_INLINE METAENGINE_M3x2 metadot_make_scale_translation(METAENGINE_V2 s, METAENGINE_V2 p) {
    METAENGINE_M3x2 m;
    m.m.x = metadot_v2(s.x, 0);
    m.m.y = metadot_v2(0, s.y);
    m.p = p;
    return m;
}
ME_INLINE METAENGINE_M3x2 metadot_make_scale_translation_f(float s, METAENGINE_V2 p) { return metadot_make_scale_translation(metadot_v2(s, s), p); }
ME_INLINE METAENGINE_M3x2 metadot_make_scale_translation_f_f(float sx, float sy, METAENGINE_V2 p) { return metadot_make_scale_translation(metadot_v2(sx, sy), p); }
ME_INLINE METAENGINE_M3x2 metadot_make_rotation(float radians) {
    METAENGINE_SinCos sc = metadot_sincos_f(radians);
    METAENGINE_M3x2 m;
    m.m.x = metadot_v2(sc.c, -sc.s);
    m.m.y = metadot_v2(sc.s, sc.c);
    m.p = metadot_v2(0, 0);
    return m;
}
ME_INLINE METAENGINE_M3x2 metadot_make_transform_TSR(METAENGINE_V2 p, METAENGINE_V2 s, float radians) {
    METAENGINE_SinCos sc = metadot_sincos_f(radians);
    METAENGINE_M3x2 m;
    m.m.x = metadot_mul_v2_f(metadot_v2(sc.c, -sc.s), s.x);
    m.m.y = metadot_mul_v2_f(metadot_v2(sc.s, sc.c), s.y);
    m.p = p;
    return m;
}

ME_INLINE METAENGINE_M3x2 metadot_invert(METAENGINE_M3x2 a) {
    float id = metadot_safe_invert(metadot_det2(a.m.x, a.m.y));
    METAENGINE_M3x2 b;
    b.m.x = metadot_v2(a.m.y.y * id, -a.m.x.y * id);
    b.m.y = metadot_v2(-a.m.y.x * id, a.m.x.x * id);
    b.p.x = (a.m.y.x * a.p.y - a.p.x * a.m.y.y) * id;
    b.p.y = (a.p.x * a.m.x.y - a.m.x.x * a.p.y) * id;
    return b;
}

//--------------------------------------------------------------------------------------------------
// Transform ops.
// No scale factor allowed here, good for physics + colliders.

ME_INLINE METAENGINE_Transform metadot_make_transform() {
    METAENGINE_Transform x;
    x.p = metadot_v2(0, 0);
    x.r = metadot_sincos();
    return x;
}
ME_INLINE METAENGINE_Transform metadot_make_transform_TR(METAENGINE_V2 p, float radians) {
    METAENGINE_Transform x;
    x.r = metadot_sincos_f(radians);
    x.p = p;
    return x;
}
ME_INLINE METAENGINE_V2 metadot_mul_tf_v2(METAENGINE_Transform a, METAENGINE_V2 b) { return metadot_add_v2(metadot_mul_sc_v2(a.r, b), a.p); }
ME_INLINE METAENGINE_V2 metadot_mulT_tf_v2(METAENGINE_Transform a, METAENGINE_V2 b) { return metadot_mulT_sc_v2(a.r, metadot_sub_v2(b, a.p)); }
ME_INLINE METAENGINE_Transform metadot_mul_tf(METAENGINE_Transform a, METAENGINE_Transform b) {
    METAENGINE_Transform c;
    c.r = metadot_mul_sc(a.r, b.r);
    c.p = metadot_add_v2(metadot_mul_sc_v2(a.r, b.p), a.p);
    return c;
}
ME_INLINE METAENGINE_Transform metadot_mulT_tf(METAENGINE_Transform a, METAENGINE_Transform b) {
    METAENGINE_Transform c;
    c.r = metadot_mulT_sc(a.r, b.r);
    c.p = metadot_mulT_sc_v2(a.r, metadot_sub_v2(b.p, a.p));
    return c;
}

//--------------------------------------------------------------------------------------------------
// Halfspace (plane/line) ops.
// Functions for infinite lines.

ME_INLINE METAENGINE_Halfspace metadot_plane(METAENGINE_V2 n, float d) {
    METAENGINE_Halfspace h;
    h.n = n;
    h.d = d;
    return h;
}
ME_INLINE METAENGINE_Halfspace metadot_plane2(METAENGINE_V2 n, METAENGINE_V2 p) {
    METAENGINE_Halfspace h;
    h.n = n;
    h.d = metadot_dot(n, p);
    return h;
}
ME_INLINE METAENGINE_V2 metadot_origin(METAENGINE_Halfspace h) { return metadot_mul_v2_f(h.n, h.d); }
ME_INLINE float metadot_distance_hs(METAENGINE_Halfspace h, METAENGINE_V2 p) { return metadot_dot(h.n, p) - h.d; }
ME_INLINE METAENGINE_V2 metadot_project(METAENGINE_Halfspace h, METAENGINE_V2 p) { return metadot_sub_v2(p, metadot_mul_v2_f(h.n, metadot_distance_hs(h, p))); }
ME_INLINE METAENGINE_Halfspace metadot_mul_tf_hs(METAENGINE_Transform a, METAENGINE_Halfspace b) {
    METAENGINE_Halfspace c;
    c.n = metadot_mul_sc_v2(a.r, b.n);
    c.d = metadot_dot(metadot_mul_tf_v2(a, metadot_origin(b)), c.n);
    return c;
}
ME_INLINE METAENGINE_Halfspace metadot_mulT_tf_hs(METAENGINE_Transform a, METAENGINE_Halfspace b) {
    METAENGINE_Halfspace c;
    c.n = metadot_mulT_sc_v2(a.r, b.n);
    c.d = metadot_dot(metadot_mulT_tf_v2(a, metadot_origin(b)), c.n);
    return c;
}
ME_INLINE METAENGINE_V2 metadot_intersect_halfspace(METAENGINE_V2 a, METAENGINE_V2 b, float da, float db) { return metadot_add_v2(a, metadot_mul_v2_f(metadot_sub_v2(b, a), (da / (da - db)))); }
ME_INLINE METAENGINE_V2 metadot_intersect_halfspace2(METAENGINE_Halfspace h, METAENGINE_V2 a, METAENGINE_V2 b) {
    return metadot_intersect_halfspace(a, b, metadot_distance_hs(h, a), metadot_distance_hs(h, b));
}

//--------------------------------------------------------------------------------------------------
// AABB helpers.

ME_INLINE METAENGINE_Aabb metadot_make_aabb(METAENGINE_V2 min, METAENGINE_V2 max) {
    METAENGINE_Aabb bb;
    bb.min = min;
    bb.max = max;
    return bb;
}
ME_INLINE METAENGINE_Aabb metadot_make_aabb_pos_w_h(METAENGINE_V2 pos, float w, float h) {
    METAENGINE_Aabb bb;
    METAENGINE_V2 he = metadot_mul_v2_f(metadot_v2(w, h), 0.5f);
    bb.min = metadot_sub_v2(pos, he);
    bb.max = metadot_add_v2(pos, he);
    return bb;
}
ME_INLINE METAENGINE_Aabb metadot_make_aabb_center_half_extents(METAENGINE_V2 center, METAENGINE_V2 half_extents) {
    METAENGINE_Aabb bb;
    bb.min = metadot_sub_v2(center, half_extents);
    bb.max = metadot_add_v2(center, half_extents);
    return bb;
}
ME_INLINE METAENGINE_Aabb metadot_make_aabb_from_top_left(METAENGINE_V2 top_left, float w, float h) {
    return metadot_make_aabb(metadot_add_v2(top_left, metadot_v2(0, -h)), metadot_add_v2(top_left, metadot_v2(w, 0)));
}
ME_INLINE float metadot_width(METAENGINE_Aabb bb) { return bb.max.x - bb.min.x; }
ME_INLINE float metadot_height(METAENGINE_Aabb bb) { return bb.max.y - bb.min.y; }
ME_INLINE float metadot_half_width(METAENGINE_Aabb bb) { return metadot_width(bb) * 0.5f; }
ME_INLINE float metadot_half_height(METAENGINE_Aabb bb) { return metadot_height(bb) * 0.5f; }
ME_INLINE METAENGINE_V2 metadot_half_extents(METAENGINE_Aabb bb) { return (metadot_mul_v2_f(metadot_sub_v2(bb.max, bb.min), 0.5f)); }
ME_INLINE METAENGINE_V2 metadot_extents(METAENGINE_Aabb aabb) { return metadot_sub_v2(aabb.max, aabb.min); }
ME_INLINE METAENGINE_Aabb metadot_expand_aabb(METAENGINE_Aabb aabb, METAENGINE_V2 v) { return metadot_make_aabb(metadot_sub_v2(aabb.min, v), metadot_add_v2(aabb.max, v)); }
ME_INLINE METAENGINE_Aabb metadot_expand_aabb_f(METAENGINE_Aabb aabb, float v) {
    METAENGINE_V2 factor = metadot_v2(v, v);
    return metadot_make_aabb(metadot_sub_v2(aabb.min, factor), metadot_add_v2(aabb.max, factor));
}
ME_INLINE METAENGINE_V2 metadot_min_aabb(METAENGINE_Aabb bb) { return bb.min; }
ME_INLINE METAENGINE_V2 metadot_max_aabb(METAENGINE_Aabb bb) { return bb.max; }
ME_INLINE METAENGINE_V2 metadot_midpoint(METAENGINE_Aabb bb) { return metadot_mul_v2_f(metadot_add_v2(bb.min, bb.max), 0.5f); }
ME_INLINE METAENGINE_V2 metadot_center(METAENGINE_Aabb bb) { return metadot_mul_v2_f(metadot_add_v2(bb.min, bb.max), 0.5f); }
ME_INLINE METAENGINE_V2 metadot_top_left(METAENGINE_Aabb bb) { return metadot_v2(bb.min.x, bb.max.y); }
ME_INLINE METAENGINE_V2 metadot_top_right(METAENGINE_Aabb bb) { return metadot_v2(bb.max.x, bb.max.y); }
ME_INLINE METAENGINE_V2 metadot_bottom_left(METAENGINE_Aabb bb) { return metadot_v2(bb.min.x, bb.min.y); }
ME_INLINE METAENGINE_V2 metadot_bottom_right(METAENGINE_Aabb bb) { return metadot_v2(bb.max.x, bb.min.y); }
ME_INLINE bool metadot_contains_point(METAENGINE_Aabb bb, METAENGINE_V2 p) { return metadot_greater_equal_v2(p, bb.min) && metadot_lesser_equal_v2(p, bb.max); }
ME_INLINE bool metadot_contains_aabb(METAENGINE_Aabb a, METAENGINE_Aabb b) { return metadot_lesser_equal_v2(a.min, b.min) && metadot_greater_equal_v2(a.max, b.max); }
ME_INLINE float metadot_surface_area_aabb(METAENGINE_Aabb bb) { return 2.0f * metadot_width(bb) * metadot_height(bb); }
ME_INLINE float metadot_area_aabb(METAENGINE_Aabb bb) { return metadot_width(bb) * metadot_height(bb); }
ME_INLINE METAENGINE_V2 metadot_clamp_aabb_v2(METAENGINE_Aabb bb, METAENGINE_V2 p) { return metadot_clamp_v2(p, bb.min, bb.max); }
ME_INLINE METAENGINE_Aabb metadot_clamp_aabb(METAENGINE_Aabb a, METAENGINE_Aabb b) { return metadot_make_aabb(metadot_clamp_v2(a.min, b.min, b.max), metadot_clamp_v2(a.max, b.min, b.max)); }
ME_INLINE METAENGINE_Aabb metadot_combine(METAENGINE_Aabb a, METAENGINE_Aabb b) { return metadot_make_aabb(metadot_min_v2(a.min, b.min), metadot_max_v2(a.max, b.max)); }

ME_INLINE int metadot_overlaps(METAENGINE_Aabb a, METAENGINE_Aabb b) {
    int d0 = b.max.x < a.min.x;
    int d1 = a.max.x < b.min.x;
    int d2 = b.max.y < a.min.y;
    int d3 = a.max.y < b.min.y;
    return !(d0 | d1 | d2 | d3);
}

ME_INLINE int metadot_collide_aabb(METAENGINE_Aabb a, METAENGINE_Aabb b) { return metadot_overlaps(a, b); }

ME_INLINE METAENGINE_Aabb metadot_make_aabb_verts(METAENGINE_V2 *verts, int count) {
    METAENGINE_V2 vmin = verts[0];
    METAENGINE_V2 vmax = vmin;
    for (int i = 0; i < count; ++i) {
        vmin = metadot_min_v2(vmin, verts[i]);
        vmax = metadot_max_v2(vmax, verts[i]);
    }
    return metadot_make_aabb(vmin, vmax);
}

ME_INLINE void metadot_aabb_verts(METAENGINE_V2 *out, METAENGINE_Aabb bb) {
    out[0] = bb.min;
    out[1] = metadot_v2(bb.max.x, bb.min.y);
    out[2] = bb.max;
    out[3] = metadot_v2(bb.min.x, bb.max.y);
}

//--------------------------------------------------------------------------------------------------
// Circle helpers.

ME_INLINE float metadot_area_circle(METAENGINE_Circle c) { return 3.14159265f * c.r * c.r; }
ME_INLINE float metadot_surface_area_circle(METAENGINE_Circle c) { return 2.0f * 3.14159265f * c.r; }
ME_INLINE METAENGINE_Circle metadot_mul_tf_circle(METAENGINE_Transform tx, METAENGINE_Circle a) {
    METAENGINE_Circle b;
    b.p = metadot_mul_tf_v2(tx, a.p);
    b.r = a.r;
    return b;
}

//--------------------------------------------------------------------------------------------------
// Ray ops.
// Full raycasting suite is farther down below in this file.

ME_INLINE METAENGINE_V2 metadot_impact(METAENGINE_Ray r, float t) { return metadot_add_v2(r.p, metadot_mul_v2_f(r.d, t)); }
ME_INLINE METAENGINE_V2 metadot_endpoint(METAENGINE_Ray r) { return metadot_add_v2(r.p, metadot_mul_v2_f(r.d, r.t)); }

ME_INLINE int metadot_ray_to_halfpsace(METAENGINE_Ray A, METAENGINE_Halfspace B, METAENGINE_Raycast *out) {
    float da = metadot_distance_hs(B, A.p);
    float db = metadot_distance_hs(B, metadot_impact(A, A.t));
    if (da * db > 0) return 0;
    out->n = metadot_mul_v2_f(B.n, metadot_sign(da));
    out->t = metadot_intersect(da, db);
    return 1;
}

// http://www.randygaul.net/2014/07/23/distance-point-to-line-segment/
ME_INLINE float metadot_distance_sq(METAENGINE_V2 a, METAENGINE_V2 b, METAENGINE_V2 p) {
    METAENGINE_V2 n = metadot_sub_v2(b, a);
    METAENGINE_V2 pa = metadot_sub_v2(a, p);
    float c = metadot_dot(n, pa);

    // Closest point is a
    if (c > 0.0f) return metadot_dot(pa, pa);

    // Closest point is b
    METAENGINE_V2 bp = metadot_sub_v2(p, b);
    if (metadot_dot(n, bp) > 0.0f) return metadot_dot(bp, bp);

    // Closest point is between a and b
    METAENGINE_V2 e = metadot_sub_v2(pa, metadot_mul_v2_f(n, (c / metadot_dot(n, n))));
    return metadot_dot(e, e);
}

//--------------------------------------------------------------------------------------------------
// Collision detection.

// It's quite common to limit the number of verts on polygons to a low number. Feel free to adjust
// this number if needed, but be warned: higher than 8 and shapes generally start to look more like
// circles/ovals; it becomes pointless beyond a certain point.
#define METAENGINE_POLY_MAX_VERTS 8

// 2D polygon. Verts are ordered in counter-clockwise order (CCW).
typedef struct METAENGINE_Poly {
    int count;
    METAENGINE_V2 verts[METAENGINE_POLY_MAX_VERTS];
    METAENGINE_V2 norms[METAENGINE_POLY_MAX_VERTS];  // Pointing perpendicular along the poly's surface.
                                                     // Rotated vert[i] to vert[i + 1] 90 degrees CCW + normalized.
} METAENGINE_Poly;

// 2D capsule shape. It's like a shrink-wrap of 2 circles connected by a rod.
typedef struct METAENGINE_Capsule {
    METAENGINE_V2 a;
    METAENGINE_V2 b;
    float r;
} METAENGINE_Capsule;

// Contains all information necessary to resolve a collision, or in other words
// this is the information needed to separate shapes that are colliding. Doing
// the resolution step is *not* included.
typedef struct METAENGINE_Manifold {
    int count;
    float depths[2];
    METAENGINE_V2 contact_points[2];

    // Always points from shape A to shape B.
    METAENGINE_V2 n;
} METAENGINE_Manifold;

// Boolean collision detection functions.
// These versions are slightly faster/simpler than the manifold versions, but only give a YES/NO result.
bool METADOT_CDECL metadot_circle_to_circle(METAENGINE_Circle A, METAENGINE_Circle B);
bool METADOT_CDECL metadot_circle_to_aabb(METAENGINE_Circle A, METAENGINE_Aabb B);
bool METADOT_CDECL metadot_circle_to_capsule(METAENGINE_Circle A, METAENGINE_Capsule B);
bool METADOT_CDECL metadot_aabb_to_aabb(METAENGINE_Aabb A, METAENGINE_Aabb B);
bool METADOT_CDECL metadot_aabb_to_capsule(METAENGINE_Aabb A, METAENGINE_Capsule B);
bool METADOT_CDECL metadot_capsule_to_capsule(METAENGINE_Capsule A, METAENGINE_Capsule B);
bool METADOT_CDECL metadot_circle_to_poly(METAENGINE_Circle A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx);
bool METADOT_CDECL metadot_aabb_to_poly(METAENGINE_Aabb A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx);
bool METADOT_CDECL metadot_capsule_to_poly(METAENGINE_Capsule A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx);
bool METADOT_CDECL metadot_poly_to_poly(const METAENGINE_Poly *A, const METAENGINE_Transform *ax, const METAENGINE_Poly *B, const METAENGINE_Transform *bx);

// Ray casting.
// Output is placed into the `METAENGINE_Raycast` struct, which represents the hit location
// of the ray. The out param contains no meaningful information if these funcs
// return false.
bool METADOT_CDECL metadot_ray_to_circle(METAENGINE_Ray A, METAENGINE_Circle B, METAENGINE_Raycast *out);
bool METADOT_CDECL metadot_ray_to_aabb(METAENGINE_Ray A, METAENGINE_Aabb B, METAENGINE_Raycast *out);
bool METADOT_CDECL metadot_ray_to_capsule(METAENGINE_Ray A, METAENGINE_Capsule B, METAENGINE_Raycast *out);
bool METADOT_CDECL metadot_ray_to_poly(METAENGINE_Ray A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx_ptr, METAENGINE_Raycast *out);

// Manifold generation.
// These functions are (generally) slower + more complex than bool versions, but compute one
// or two points that represent the plane of contact. This information is
// is usually needed to resolve and prevent shapes from colliding. If no coll-
// ision occured the `count` member of the manifold typedef struct is set to 0.
void METADOT_CDECL metadot_circle_to_circle_manifold(METAENGINE_Circle A, METAENGINE_Circle B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_circle_to_aabb_manifold(METAENGINE_Circle A, METAENGINE_Aabb B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_circle_to_capsule_manifold(METAENGINE_Circle A, METAENGINE_Capsule B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_aabb_to_aabb_manifold(METAENGINE_Aabb A, METAENGINE_Aabb B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_aabb_to_capsule_manifold(METAENGINE_Aabb A, METAENGINE_Capsule B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_capsule_to_capsule_manifold(METAENGINE_Capsule A, METAENGINE_Capsule B, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_circle_to_poly_manifold(METAENGINE_Circle A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_aabb_to_poly_manifold(METAENGINE_Aabb A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_capsule_to_poly_manifold(METAENGINE_Capsule A, const METAENGINE_Poly *B, const METAENGINE_Transform *bx, METAENGINE_Manifold *m);
void METADOT_CDECL metadot_poly_to_poly_manifold(const METAENGINE_Poly *A, const METAENGINE_Transform *ax, const METAENGINE_Poly *B, const METAENGINE_Transform *bx, METAENGINE_Manifold *m);

#define METAENGINE_SHAPE_TYPE_DEFS         \
    METAENGINE_ENUM(SHAPE_TYPE_NONE, 0)    \
    METAENGINE_ENUM(SHAPE_TYPE_CIRCLE, 1)  \
    METAENGINE_ENUM(SHAPE_TYPE_AABB, 2)    \
    METAENGINE_ENUM(SHAPE_TYPE_CAPSULE, 3) \
    METAENGINE_ENUM(SHAPE_TYPE_POLY, 4)

typedef enum METAENGINE_ShapeType {
#define METAENGINE_ENUM(K, V) METAENGINE_##K = V,
    METAENGINE_SHAPE_TYPE_DEFS
#undef METAENGINE_ENUM
} METAENGINE_ShapeType;

ME_INLINE const char *metadot_shape_type_to_string(METAENGINE_ShapeType type) {
    switch (type) {
#define METAENGINE_ENUM(K, V) \
    case METAENGINE_##K:      \
        return METAENGINE_STRINGIZE(METAENGINE_##K);
        METAENGINE_SHAPE_TYPE_DEFS
#undef METAENGINE_ENUM
        default:
            return NULL;
    }
}

// This typedef struct is only for advanced usage of the `metadot_gjk` function. See comments inside of the
// `metadot_gjk` function for more details.
typedef struct METAENGINE_GjkCache {
    float metric;
    int count;
    int iA[3];
    int iB[3];
    float div;
} METAENGINE_GjkCache;

// This is an advanced function, intended to be used by people who know what they're doing.
//
// Runs the GJK algorithm to find closest points, returns distance between closest points.
// outA and outB can be NULL, in this case only distance is returned. ax_ptr and bx_ptr
// can be NULL, and represent local to world transformations for shapes A and B respectively.
// use_radius will apply radii for capsules and circles (if set to false, spheres are
// treated as points and capsules are treated as line segments i.e. rays). The cache parameter
// should be NULL, as it is only for advanced usage (unless you know what you're doing, then
// go ahead and use it). iterations is an optional parameter.
//
//
// IMPORTANT NOTE
//
//     The GJK function is sensitive to large shapes, since it internally will compute signed area
//     values. `metadot_gjk` is called throughout this file in many ways, so try to make sure all of your
//     collision shapes are not gigantic. For example, try to keep the volume of all your shapes
//     less than 100.0f. If you need large shapes, you should use tiny collision geometry for all
//     function here, and simply render the geometry larger on-screen by scaling it up.
//
float METADOT_CDECL metadot_gjk(const void *A, METAENGINE_ShapeType typeA, const METAENGINE_Transform *ax_ptr, const void *B, METAENGINE_ShapeType typeB, const METAENGINE_Transform *bx_ptr,
                                METAENGINE_V2 *outA, METAENGINE_V2 *outB, int use_radius, int *iterations, METAENGINE_GjkCache *cache);

// Stores results of a time of impact calculation done by `metadot_toi`.
typedef struct METAENGINE_ToiResult {
    int hit;          // 1 if shapes were touching at the TOI, 0 if they never hit.
    float toi;        // The time of impact between two shapes.
    METAENGINE_V2 n;  // Surface normal from shape A to B at the time of impact.
    METAENGINE_V2 p;  // Point of contact between shapes A and B at time of impact.
    int iterations;   // Number of iterations the solver underwent.
} METAENGINE_ToiResult;

// This is an advanced function, intended to be used by people who know what they're doing.
//
// Computes the time of impact from shape A and shape B. The velocity of each shape is provided
// by vA and vB respectively. The shapes are *not* allowed to rotate over time. The velocity is
// assumed to represent the change in motion from time 0 to time 1, and so the return value will
// be a number from 0 to 1. To move each shape to the colliding configuration, multiply vA and vB
// each by the return value. ax_ptr and bx_ptr are optional parameters to transforms for each shape,
// and are typically used for polygon shapes to transform from model to world space. Set these to
// NULL to represent identity transforms. iterations is an optional parameter. use_radius
// will apply radii for capsules and circles (if set to false, spheres are treated as points and
// capsules are treated as line segments i.e. rays).
//
// IMPORTANT NOTE:
// The metadot_toi function can be used to implement a "swept character controller", but it can be
// difficult to do so. Say we compute a time of impact with `metadot_toi` and move the shapes to the
// time of impact, and adjust the velocity by zeroing out the velocity along the surface normal.
// If we then call `metadot_toi` again, it will fail since the shapes will be considered to start in
// a colliding configuration. There are many styles of tricks to get around this problem, and
// all of them involve giving the next call to `metadot_toi` some breathing room. It is recommended
// to use some variation of the following algorithm:
//
// 1. Call metadot_toi.
// 2. Move the shapes to the TOI.
// 3. Slightly inflate the size of one, or both, of the shapes so they will be intersecting.
//    The purpose is to make the shapes numerically intersecting, but not visually intersecting.
//    Another option is to call metadot_toi with slightly deflated shapes.
//    See the function `metadot_inflate` for some more details.
// 4. Compute the collision manifold between the inflated shapes (for example, use poly_ttoPolyManifold).
// 5. Gently push the shapes apart. This will give the next call to metadot_toi some breathing room.
METAENGINE_ToiResult METADOT_CDECL metadot_toi(const void *A, METAENGINE_ShapeType typeA, const METAENGINE_Transform *ax_ptr, METAENGINE_V2 vA, const void *B, METAENGINE_ShapeType typeB,
                                               const METAENGINE_Transform *bx_ptr, METAENGINE_V2 vB, int use_radius);

// Inflating a shape.
//
// This is useful to numerically grow or shrink a polytope. For example, when calling
// a time of impact function it can be good to use a slightly smaller shape. Then, once
// both shapes are moved to the time of impact a collision manifold can be made from the
// slightly larger (and now overlapping) shapes.
//
// IMPORTANT NOTE
// Inflating a shape with sharp corners can cause those corners to move dramatically.
// Deflating a shape can avoid this problem, but deflating a very small shape can invert
// the planes and result in something that is no longer convex. Make sure to pick an
// appropriately small skin factor, for example 1.0e-6f.
void METADOT_CDECL metadot_inflate(void *shape, METAENGINE_ShapeType type, float skin_factor);

// Computes 2D convex hull. Will not do anything if less than two verts supplied. If
// more than METAENGINE_POLY_MAX_VERTS are supplied extras are ignored.
int METADOT_CDECL metadot_hull(METAENGINE_V2 *verts, int count);
void METADOT_CDECL metadot_norms(METAENGINE_V2 *verts, METAENGINE_V2 *norms, int count);

// runs metadot_hull and metadot_norms, assumes p->verts and p->count are both set to valid values
void METADOT_CDECL metadot_make_poly(METAENGINE_Poly *p);
METAENGINE_V2 METADOT_CDECL metadot_centroid(const METAENGINE_V2 *verts, int count);

// Generic collision detection routines, useful for games that want to use some poly-
// morphism to write more generic-styled code. Internally calls various above functions.
// For AABBs/Circles/Capsules ax and bx are ignored. For polys ax and bx can define
// model to world transformations (for polys only), or be NULL for identity transforms.
int METADOT_CDECL metadot_collided(const void *A, const METAENGINE_Transform *ax, METAENGINE_ShapeType typeA, const void *B, const METAENGINE_Transform *bx, METAENGINE_ShapeType typeB);
void METADOT_CDECL metadot_collide(const void *A, const METAENGINE_Transform *ax, METAENGINE_ShapeType typeA, const void *B, const METAENGINE_Transform *bx, METAENGINE_ShapeType typeB,
                                   METAENGINE_Manifold *m);
bool METADOT_CDECL metadot_cast_ray(METAENGINE_Ray A, const void *B, const METAENGINE_Transform *bx, METAENGINE_ShapeType typeB, METAENGINE_Raycast *out);

#ifdef __cplusplus
}
#endif  // __cplusplus

//--------------------------------------------------------------------------------------------------
// C++ API

namespace MetaEngine {

using v2 = METAENGINE_V2;

ME_INLINE v2 V2(float x, float y) {
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

ME_INLINE v2 operator+(v2 a, v2 b) { return V2(a.x + b.x, a.y + b.y); }
ME_INLINE v2 operator-(v2 a, v2 b) { return V2(a.x - b.x, a.y - b.y); }
ME_INLINE v2 &operator+=(v2 &a, v2 b) { return a = a + b; }
ME_INLINE v2 &operator-=(v2 &a, v2 b) { return a = a - b; }

ME_INLINE v2 operator*(v2 a, float b) { return V2(a.x * b, a.y * b); }
ME_INLINE v2 operator*(v2 a, v2 b) { return V2(a.x * b.x, a.y * b.y); }
ME_INLINE v2 &operator*=(v2 &a, float b) { return a = a * b; }
ME_INLINE v2 &operator*=(v2 &a, v2 b) { return a = a * b; }
ME_INLINE v2 operator/(v2 a, float b) { return V2(a.x / b, a.y / b); }
ME_INLINE v2 operator/(v2 a, v2 b) { return V2(a.x / b.x, a.y / b.y); }
ME_INLINE v2 &operator/=(v2 &a, float b) { return a = a / b; }
ME_INLINE v2 &operator/=(v2 &a, v2 b) { return a = a / b; }

ME_INLINE v2 operator-(v2 a) { return V2(-a.x, -a.y); }

ME_INLINE int operator<(v2 a, v2 b) { return a.x < b.x && a.y < b.y; }
ME_INLINE int operator>(v2 a, v2 b) { return a.x > b.x && a.y > b.y; }
ME_INLINE int operator<=(v2 a, v2 b) { return a.x <= b.x && a.y <= b.y; }
ME_INLINE int operator>=(v2 a, v2 b) { return a.x >= b.x && a.y >= b.y; }

using SinCos = METAENGINE_SinCos;
using M2x2 = METAENGINE_M2x2;
using M3x2 = METAENGINE_M3x2;
using MTransform = METAENGINE_Transform;
using Halfspace = METAENGINE_Halfspace;
using Ray = METAENGINE_Ray;
using Raycast = METAENGINE_Raycast;
using Circle = METAENGINE_Circle;
using Aabb = METAENGINE_Aabb;
using Rect = METAENGINE_Rect;
using Poly = METAENGINE_Poly;
using Capsule = METAENGINE_Capsule;
using Manifold = METAENGINE_Manifold;
using GjkCache = METAENGINE_GjkCache;
using ToiResult = METAENGINE_ToiResult;

using ShapeType = METAENGINE_ShapeType;
#define METAENGINE_ENUM(K, V) ME_INLINE constexpr ShapeType K = METAENGINE_##K;
METAENGINE_SHAPE_TYPE_DEFS
#undef METAENGINE_ENUM

ME_INLINE const char *to_string(ShapeType type) {
    switch (type) {
#define METAENGINE_ENUM(K, V) \
    case METAENGINE_##K:      \
        return #K;
        METAENGINE_SHAPE_TYPE_DEFS
#undef METAENGINE_ENUM
        default:
            return NULL;
    }
}

ME_INLINE float min(float a, float b) { return metadot_min(a, b); }
ME_INLINE float max(float a, float b) { return metadot_max(a, b); }
ME_INLINE float clamp(float a, float lo, float hi) { return metadot_clamp(a, lo, hi); }
ME_INLINE float clamp01(float a) { return metadot_clamp01(a); }
ME_INLINE float sign(float a) { return metadot_sign(a); }
ME_INLINE float intersect(float da, float db) { return metadot_intersect(da, db); }
ME_INLINE float safe_invert(float a) { return metadot_safe_invert(a); }
ME_INLINE float lerp_f(float a, float b, float t) { return metadot_lerp(a, b, t); }
ME_INLINE float remap(float t, float lo, float hi) { return metadot_remap(t, lo, hi); }
ME_INLINE float mod(float x, float m) { return metadot_mod(x, m); }
ME_INLINE float fract(float x) { return metadot_fract(x); }

ME_INLINE int sign(int a) { return metadot_sign_int(a); }
ME_INLINE int min(int a, int b) { return metadot_min(a, b); }
ME_INLINE int max(int a, int b) { return metadot_max(a, b); }
ME_INLINE uint64_t min(uint64_t a, uint64_t b) { return metadot_min(a, b); }
ME_INLINE uint64_t max(uint64_t a, uint64_t b) { return metadot_max(a, b); }
ME_INLINE float abs(float a) { return metadot_abs(a); }
ME_INLINE int abs(int a) { return metadot_abs_int(a); }
ME_INLINE int clamp(int a, int lo, int hi) { return metadot_clamp_int(a, lo, hi); }
ME_INLINE int clamp01(int a) { return metadot_clamp01_int(a); }
ME_INLINE bool is_even(int x) { return metadot_is_even(x); }
ME_INLINE bool is_odd(int x) { return metadot_is_odd(x); }

ME_INLINE bool is_power_of_two(int a) { return metadot_is_power_of_two(a); }
ME_INLINE bool is_power_of_two(uint64_t a) { return metadot_is_power_of_two_uint(a); }
ME_INLINE int fit_power_of_two(int a) { return metadot_fit_power_of_two(a); }

ME_INLINE float smoothstep(float x) { return metadot_smoothstep(x); }
ME_INLINE float quad_in(float x) { return metadot_quad_in(x); }
ME_INLINE float quad_out(float x) { return metadot_quad_out(x); }
ME_INLINE float quad_in_out(float x) { return metadot_quad_in_out(x); }
ME_INLINE float cube_in(float x) { return metadot_cube_in(x); }
ME_INLINE float cube_out(float x) { return metadot_cube_out(x); }
ME_INLINE float cube_in_out(float x) { return metadot_cube_in_out(x); }
ME_INLINE float quart_in(float x) { return metadot_quart_in(x); }
ME_INLINE float quart_out(float x) { return metadot_quart_out(x); }
ME_INLINE float quart_in_out(float x) { return metadot_quart_in_out(x); }
ME_INLINE float quint_in(float x) { return metadot_quint_in(x); }
ME_INLINE float quint_out(float x) { return metadot_quint_out(x); }
ME_INLINE float quint_in_out(float x) { return metadot_quint_in_out(x); }
ME_INLINE float sin_in(float x) { return metadot_sin_in(x); }
ME_INLINE float sin_out(float x) { return metadot_sin_out(x); }
ME_INLINE float sin_in_out(float x) { return metadot_sin_in_out(x); }
ME_INLINE float circle_in(float x) { return metadot_circle_in(x); }
ME_INLINE float circle_out(float x) { return metadot_circle_out(x); }
ME_INLINE float circle_in_out(float x) { return metadot_circle_in_out(x); }
ME_INLINE float back_in(float x) { return metadot_back_in(x); }
ME_INLINE float back_out(float x) { return metadot_back_out(x); }
ME_INLINE float back_in_out(float x) { return metadot_back_in_out(x); }

ME_INLINE float dot(v2 a, v2 b) { return metadot_dot(a, b); }

ME_INLINE v2 skew(v2 a) { return metadot_skew(a); }
ME_INLINE v2 cw90(v2 a) { return metadot_cw90(a); }
ME_INLINE float det2(v2 a, v2 b) { return metadot_det2(a, b); }
ME_INLINE float cross(v2 a, v2 b) { return metadot_cross(a, b); }
ME_INLINE v2 cross(v2 a, float b) { return metadot_cross_v2_f(a, b); }
ME_INLINE v2 cross(float a, v2 b) { return metadot_cross_f_v2(a, b); }
ME_INLINE v2 min(v2 a, v2 b) { return metadot_min_v2(a, b); }
ME_INLINE v2 max(v2 a, v2 b) { return metadot_max_v2(a, b); }
ME_INLINE v2 clamp(v2 a, v2 lo, v2 hi) { return metadot_clamp_v2(a, lo, hi); }
ME_INLINE v2 clamp01(v2 a) { return metadot_clamp01_v2(a); }
ME_INLINE v2 abs(v2 a) { return metadot_abs_v2(a); }
ME_INLINE float hmin(v2 a) { return metadot_hmin(a); }
ME_INLINE float hmax(v2 a) { return metadot_hmax(a); }
ME_INLINE float len(v2 a) { return metadot_len(a); }
ME_INLINE float distance(v2 a, v2 b) { return metadot_distance(a, b); }
ME_INLINE v2 norm(v2 a) { return metadot_norm(a); }
ME_INLINE v2 safe_norm(v2 a) { return metadot_safe_norm(a); }
ME_INLINE float safe_norm(float a) { return metadot_safe_norm_f(a); }
ME_INLINE int safe_norm(int a) { return metadot_safe_norm_int(a); }

ME_INLINE v2 lerp_v2(v2 a, v2 b, float t) { return metadot_lerp_v2(a, b, t); }
ME_INLINE v2 bezier(v2 a, v2 c0, v2 b, float t) { return metadot_bezier(a, c0, b, t); }
ME_INLINE v2 bezier(v2 a, v2 c0, v2 c1, v2 b, float t) { return metadot_bezier2(a, c0, c1, b, t); }
ME_INLINE v2 floor(v2 a) { return metadot_floor(a); }
ME_INLINE v2 round(v2 a) { return metadot_round(a); }
ME_INLINE v2 safe_invert(v2 a) { return metadot_safe_invert_v2(a); }
ME_INLINE v2 sign(v2 a) { return metadot_sign_v2(a); }

ME_INLINE int parallel(v2 a, v2 b, float tol) { return metadot_parallel(a, b, tol); }

ME_INLINE SinCos sincos(float radians) { return metadot_sincos_f(radians); }
ME_INLINE SinCos sincos() { return metadot_sincos(); }
ME_INLINE v2 x_axis(SinCos r) { return metadot_x_axis(r); }
ME_INLINE v2 y_axis(SinCos r) { return metadot_y_axis(r); }
ME_INLINE v2 mul(SinCos a, v2 b) { return metadot_mul_sc_v2(a, b); }
ME_INLINE v2 mulT(SinCos a, v2 b) { return metadot_mulT_sc_v2(a, b); }
ME_INLINE SinCos mul(SinCos a, SinCos b) { return metadot_mul_sc(a, b); }
ME_INLINE SinCos mulT(SinCos a, SinCos b) { return metadot_mulT_sc(a, b); }

ME_INLINE float atan2_360(float y, float x) { return metadot_atan2_360(y, x); }
ME_INLINE float atan2_360(v2 v) { return metadot_atan2_360_v2(v); }
ME_INLINE float atan2_360(SinCos r) { return metadot_atan2_360_sc(r); }

ME_INLINE float shortest_arc(v2 a, v2 b) { return metadot_shortest_arc(a, b); }

ME_INLINE float angle_diff(float radians_a, float radians_b) { return metadot_angle_diff(radians_a, radians_b); }
ME_INLINE v2 from_angle(float radians) { return metadot_from_angle(radians); }

ME_INLINE v2 mul(M2x2 a, v2 b) { return metadot_mul_m2_v2(a, b); }
ME_INLINE M2x2 mul(M2x2 a, M2x2 b) { return metadot_mul_m2(a, b); }

ME_INLINE v2 mul(M3x2 a, v2 b) { return metadot_mul_m32_v2(a, b); }
ME_INLINE M3x2 mul(M3x2 a, M3x2 b) { return metadot_mul_m32(a, b); }
ME_INLINE M3x2 make_identity() { return metadot_make_identity(); }
ME_INLINE M3x2 make_translation(float x, float y) { return metadot_make_translation_f(x, y); }
ME_INLINE M3x2 make_translation(v2 p) { return metadot_make_translation(p); }
ME_INLINE M3x2 make_scale(v2 s) { return metadot_make_scale(s); }
ME_INLINE M3x2 make_scale(float s) { return metadot_make_scale_f(s); }
ME_INLINE M3x2 make_scale(v2 s, v2 p) { return metadot_make_scale_translation(s, p); }
ME_INLINE M3x2 make_scale(float s, v2 p) { return metadot_make_scale_translation_f(s, p); }
ME_INLINE M3x2 make_scale(float sx, float sy, v2 p) { return metadot_make_scale_translation_f_f(sx, sy, p); }
ME_INLINE M3x2 make_rotation(float radians) { return metadot_make_rotation(radians); }
ME_INLINE M3x2 make_transform(v2 p, v2 s, float radians) { return metadot_make_transform_TSR(p, s, radians); }
ME_INLINE M3x2 invert(M3x2 m) { return metadot_invert(m); }

ME_INLINE MTransform make_transform() { return metadot_make_transform(); }
ME_INLINE MTransform make_transform(v2 p, float radians) { return metadot_make_transform_TR(p, radians); }
ME_INLINE v2 mul(MTransform a, v2 b) { return metadot_mul_tf_v2(a, b); }
ME_INLINE v2 mulT(MTransform a, v2 b) { return metadot_mulT_tf_v2(a, b); }
ME_INLINE MTransform mul(MTransform a, MTransform b) { return metadot_mul_tf(a, b); }
ME_INLINE MTransform mulT(MTransform a, MTransform b) { return metadot_mulT_tf(a, b); }

ME_INLINE Halfspace plane(v2 n, float d) { return metadot_plane(n, d); }
ME_INLINE Halfspace plane(v2 n, v2 p) { return metadot_plane2(n, p); }
ME_INLINE v2 origin(Halfspace h) { return metadot_origin(h); }
ME_INLINE float distance(Halfspace h, v2 p) { return metadot_distance_hs(h, p); }
ME_INLINE v2 project(Halfspace h, v2 p) { return metadot_project(h, p); }
ME_INLINE Halfspace mul(MTransform a, Halfspace b) { return metadot_mul_tf_hs(a, b); }
ME_INLINE Halfspace mulT(MTransform a, Halfspace b) { return metadot_mulT_tf_hs(a, b); }
ME_INLINE v2 intersect(v2 a, v2 b, float da, float db) { return metadot_intersect_halfspace(a, b, da, db); }
ME_INLINE v2 intersect(Halfspace h, v2 a, v2 b) { return metadot_intersect_halfspace2(h, a, b); }

ME_INLINE Aabb make_aabb(v2 min, v2 max) { return metadot_make_aabb(min, max); }
ME_INLINE Aabb make_aabb(v2 pos, float w, float h) { return metadot_make_aabb_pos_w_h(pos, w, h); }
ME_INLINE Aabb make_aabb_center_half_extents(v2 center, v2 half_extents) { return metadot_make_aabb_center_half_extents(center, half_extents); }
ME_INLINE Aabb make_aabb_from_top_left(v2 top_left, float w, float h) { return metadot_make_aabb_from_top_left(top_left, w, h); }
ME_INLINE float width(Aabb bb) { return metadot_width(bb); }
ME_INLINE float height(Aabb bb) { return metadot_height(bb); }
ME_INLINE float half_width(Aabb bb) { return metadot_half_width(bb); }
ME_INLINE float half_height(Aabb bb) { return metadot_half_height(bb); }
ME_INLINE v2 half_extents(Aabb bb) { return metadot_half_extents(bb); }
ME_INLINE v2 extents(Aabb aabb) { return metadot_extents(aabb); }
ME_INLINE Aabb expand(Aabb aabb, v2 v) { return metadot_expand_aabb(aabb, v); }
ME_INLINE Aabb expand(Aabb aabb, float v) { return metadot_expand_aabb_f(aabb, v); }
ME_INLINE v2 min(Aabb bb) { return metadot_min_aabb(bb); }
ME_INLINE v2 max(Aabb bb) { return metadot_max_aabb(bb); }
ME_INLINE v2 midpoint(Aabb bb) { return metadot_midpoint(bb); }
ME_INLINE v2 center(Aabb bb) { return metadot_center(bb); }
ME_INLINE v2 top_left(Aabb bb) { return metadot_top_left(bb); }
ME_INLINE v2 top_right(Aabb bb) { return metadot_top_right(bb); }
ME_INLINE v2 bottom_left(Aabb bb) { return metadot_bottom_left(bb); }
ME_INLINE v2 bottom_right(Aabb bb) { return metadot_bottom_right(bb); }
ME_INLINE bool contains(Aabb bb, v2 p) { return metadot_contains_point(bb, p); }
ME_INLINE bool contains(Aabb a, Aabb b) { return metadot_contains_aabb(a, b); }
ME_INLINE float surface_area(Aabb bb) { return metadot_surface_area_aabb(bb); }
ME_INLINE float area(Aabb bb) { return metadot_area_aabb(bb); }
ME_INLINE v2 clamp(Aabb bb, v2 p) { return metadot_clamp_aabb_v2(bb, p); }
ME_INLINE Aabb clamp(Aabb a, Aabb b) { return metadot_clamp_aabb(a, b); }
ME_INLINE Aabb combine(Aabb a, Aabb b) { return metadot_combine(a, b); }

ME_INLINE int overlaps(Aabb a, Aabb b) { return metadot_overlaps(a, b); }
ME_INLINE int collide(Aabb a, Aabb b) { return metadot_collide_aabb(a, b); }

ME_INLINE Aabb make_aabb(v2 *verts, int count) { return metadot_make_aabb_verts((METAENGINE_V2 *)verts, count); }
ME_INLINE void aabb_verts(v2 *out, Aabb bb) { return metadot_aabb_verts((METAENGINE_V2 *)out, bb); }

ME_INLINE float area(Circle c) { return metadot_area_circle(c); }
ME_INLINE float surface_area(Circle c) { return metadot_surface_area_circle(c); }
ME_INLINE Circle mul(MTransform tx, Circle a) { return metadot_mul_tf_circle(tx, a); }

ME_INLINE v2 impact(Ray r, float t) { return metadot_impact(r, t); }
ME_INLINE v2 endpoint(Ray r) { return metadot_endpoint(r); }

ME_INLINE int ray_to_halfpsace(Ray A, Halfspace B, Raycast *out) { return metadot_ray_to_halfpsace(A, B, out); }
ME_INLINE float distance_sq(v2 a, v2 b, v2 p) { return metadot_distance_sq(a, b, p); }

ME_INLINE bool circle_to_circle(Circle A, Circle B) { return metadot_circle_to_circle(A, B); }
ME_INLINE bool circle_to_aabb(Circle A, Aabb B) { return metadot_circle_to_aabb(A, B); }
ME_INLINE bool circle_to_capsule(Circle A, Capsule B) { return metadot_circle_to_capsule(A, B); }
ME_INLINE bool aabb_to_aabb(Aabb A, Aabb B) { return metadot_aabb_to_aabb(A, B); }
ME_INLINE bool aabb_to_capsule(Aabb A, Capsule B) { return metadot_aabb_to_capsule(A, B); }
ME_INLINE bool capsule_to_capsule(Capsule A, Capsule B) { return metadot_capsule_to_capsule(A, B); }
ME_INLINE bool circle_to_poly(Circle A, const Poly *B, const MTransform *bx) { return metadot_circle_to_poly(A, B, bx); }
ME_INLINE bool aabb_to_poly(Aabb A, const Poly *B, const MTransform *bx) { return metadot_aabb_to_poly(A, B, bx); }
ME_INLINE bool capsule_to_poly(Capsule A, const Poly *B, const MTransform *bx) { return metadot_capsule_to_poly(A, B, bx); }
ME_INLINE bool poly_to_poly(const Poly *A, const MTransform *ax, const Poly *B, const MTransform *bx) { return metadot_poly_to_poly(A, ax, B, bx); }

ME_INLINE bool ray_to_circle(Ray A, Circle B, Raycast *out) { return metadot_ray_to_circle(A, B, out); }
ME_INLINE bool ray_to_aabb(Ray A, Aabb B, Raycast *out) { return metadot_ray_to_aabb(A, B, out); }
ME_INLINE bool ray_to_capsule(Ray A, Capsule B, Raycast *out) { return metadot_ray_to_capsule(A, B, out); }
ME_INLINE bool ray_to_poly(Ray A, const Poly *B, const MTransform *bx_ptr, Raycast *out) { return metadot_ray_to_poly(A, B, bx_ptr, out); }

ME_INLINE void circle_to_circle_manifold(Circle A, Circle B, Manifold *m) { return metadot_circle_to_circle_manifold(A, B, m); }
ME_INLINE void circle_to_aabb_manifold(Circle A, Aabb B, Manifold *m) { return metadot_circle_to_aabb_manifold(A, B, m); }
ME_INLINE void circle_to_capsule_manifold(Circle A, Capsule B, Manifold *m) { return metadot_circle_to_capsule_manifold(A, B, m); }
ME_INLINE void aabb_to_aabb_manifold(Aabb A, Aabb B, Manifold *m) { return metadot_aabb_to_aabb_manifold(A, B, m); }
ME_INLINE void aabb_to_capsule_manifold(Aabb A, Capsule B, Manifold *m) { return metadot_aabb_to_capsule_manifold(A, B, m); }
ME_INLINE void capsule_to_capsule_manifold(Capsule A, Capsule B, Manifold *m) { return metadot_capsule_to_capsule_manifold(A, B, m); }
ME_INLINE void circle_to_poly_manifold(Circle A, const Poly *B, const MTransform *bx, Manifold *m) { return metadot_circle_to_poly_manifold(A, B, bx, m); }
ME_INLINE void aabb_to_poly_manifold(Aabb A, const Poly *B, const MTransform *bx, Manifold *m) { return metadot_aabb_to_poly_manifold(A, B, bx, m); }
ME_INLINE void capsule_to_poly_manifold(Capsule A, const Poly *B, const MTransform *bx, Manifold *m) { return metadot_capsule_to_poly_manifold(A, B, bx, m); }
ME_INLINE void poly_to_poly_manifold(const Poly *A, const MTransform *ax, const Poly *B, const MTransform *bx, Manifold *m) { return metadot_poly_to_poly_manifold(A, ax, B, bx, m); }

ME_INLINE float gjk(const void *A, ShapeType typeA, const MTransform *ax_ptr, const void *B, ShapeType typeB, const MTransform *bx_ptr, v2 *outA, v2 *outB, int use_radius, int *iterations,
                    GjkCache *cache) {
    return metadot_gjk(A, typeA, ax_ptr, B, typeB, bx_ptr, (METAENGINE_V2 *)outA, (METAENGINE_V2 *)outB, use_radius, iterations, cache);
}

ME_INLINE ToiResult toi(const void *A, ShapeType typeA, const MTransform *ax_ptr, v2 vA, const void *B, ShapeType typeB, const MTransform *bx_ptr, v2 vB, int use_radius, int *iterations) {
    return metadot_toi(A, typeA, ax_ptr, vA, B, typeB, bx_ptr, vB, use_radius);
}

ME_INLINE void inflate(void *shape, ShapeType type, float skin_factor) { return metadot_inflate(shape, type, skin_factor); }

ME_INLINE int hull(v2 *verts, int count) { return metadot_hull((METAENGINE_V2 *)verts, count); }
ME_INLINE void norms(v2 *verts, v2 *norms, int count) { return metadot_norms((METAENGINE_V2 *)verts, (METAENGINE_V2 *)norms, count); }

ME_INLINE void make_poly(Poly *p) { return metadot_make_poly(p); }
ME_INLINE v2 centroid(const v2 *verts, int count) { return metadot_centroid((METAENGINE_V2 *)verts, count); }

ME_INLINE int collided(const void *A, const MTransform *ax, ShapeType typeA, const void *B, const MTransform *bx, ShapeType typeB) { return metadot_collided(A, ax, typeA, B, bx, typeB); }
ME_INLINE void collide(const void *A, const MTransform *ax, ShapeType typeA, const void *B, const MTransform *bx, ShapeType typeB, Manifold *m) {
    return metadot_collide(A, ax, typeA, B, bx, typeB, m);
}
ME_INLINE bool cast_ray(Ray A, const void *B, const MTransform *bx, ShapeType typeB, Raycast *out) { return metadot_cast_ray(A, B, bx, typeB, out); }

}  // namespace MetaEngine

#pragma endregion c2

#ifdef ME_PLATFORM_WIN32
#define far
#define near
#endif

#endif
