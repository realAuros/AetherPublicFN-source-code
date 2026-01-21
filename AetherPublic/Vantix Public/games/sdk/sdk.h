#include <corecrt_math_defines.h>
#include <random>
#include <d3d9types.h>
#include <xmmintrin.h>
#include <immintrin.h>

class fvector2d
{
public:
	fvector2d() : x(0.f), y(0.f)
	{

	}

	fvector2d(double _x, double _y) : x(_x), y(_y)
	{

	}
	~fvector2d()
	{

	}

	fvector2d operator-(fvector2d v)
	{
		return fvector2d(x - v.x, y - v.y);
	}

	double x;
	double y;
};

class fvector
{
public:
	fvector() : x(0.f), y(0.f), z(0.f)
	{

	}

	fvector(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~fvector()
	{

	}

	double x;
	double y;
	double z;

	inline double dot(fvector v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double distance(fvector v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double length() {
		return sqrt(x * x + y * y + z * z);
	}

	fvector operator+(fvector v)
	{
		return fvector(x + v.x, y + v.y, z + v.z);
	}

	fvector operator-(fvector v)
	{
		return fvector(x - v.x, y - v.y, z - v.z);
	}

	fvector operator/(double flNum)
	{
		return fvector(x / flNum, y / flNum, z / flNum);
	}

	fvector operator*(double flNum) { return fvector(x * flNum, y * flNum, z * flNum); }
};
struct fquat
{
	double x;
	double y;
	double z;
	double w;
};
struct frotator
{
	double Pitch;
	double Yaw;
	double Roll;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline bool isValid() const {
		return isfinite(x) && isfinite(y) && isfinite(z);
	}

	inline double distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double length() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 operator+(const Vector3& other) const { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
	Vector3 operator-(const Vector3& other) const { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
	Vector3 operator*(float offset) const { return { this->x * offset, this->y * offset, this->z * offset }; }
	Vector3 operator/(float offset) const { return { this->x / offset, this->y / offset, this->z / offset }; }

	Vector3& operator*=(const double other) { this->x *= other; this->y *= other; this->z *= other; return *this; }
	Vector3& operator/=(const double other) { this->x /= other; this->y /= other; this->z /= other; return *this; }

	Vector3& operator=(const Vector3& other) { this->x = other.x; this->y = other.y; this->z = other.z; return *this; }
	Vector3& operator+=(const Vector3& other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
	Vector3& operator-=(const Vector3& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
	Vector3& operator*=(const Vector3& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }


	operator bool() { return bool(this->x || this->y || this->z); }
	friend bool operator==(const Vector3& a, const Vector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

};

struct FTransform
{
	fquat rotation;
	fvector translation;
	uint8_t pad[8];
	fvector scale;
	uint8_t pad2[8];

	D3DMATRIX to_matrix_with_scale() const {
		fvector scale_3d
		(
			(scale.x == 0.0) ? 1.0 : scale.x,
			(scale.y == 0.0) ? 1.0 : scale.y,
			(scale.z == 0.0) ? 1.0 : scale.z
		);

		double x2 = rotation.x + rotation.x;
		double y2 = rotation.y + rotation.y;
		double z2 = rotation.z + rotation.z;
		double xx2 = rotation.x * x2;
		double yy2 = rotation.y * y2;
		double zz2 = rotation.z * z2;
		double yz2 = rotation.y * z2;
		double wx2 = rotation.w * x2;
		double xy2 = rotation.x * y2;
		double wz2 = rotation.w * z2;
		double xz2 = rotation.x * z2;
		double wy2 = rotation.w * y2;

		D3DMATRIX m;

		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		m._11 = (1.0 - (yy2 + zz2)) * scale_3d.x;
		m._22 = (1.0 - (xx2 + zz2)) * scale_3d.y;
		m._33 = (1.0 - (xx2 + yy2)) * scale_3d.z;

		m._32 = (yz2 - wx2) * scale_3d.z;
		m._23 = (yz2 + wx2) * scale_3d.y;

		m._21 = (xy2 - wz2) * scale_3d.y;
		m._12 = (xy2 + wz2) * scale_3d.x;

		m._31 = (xz2 + wy2) * scale_3d.z;
		m._13 = (xz2 - wy2) * scale_3d.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
inline D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
#define PI 3.14159265358979323846f
struct _MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};
inline _MATRIX Matrix(fvector Vec4, fvector origin = fvector(0, 0, 0))
{
	double radPitch = (Vec4.x * double(PI) / 180.f);
	double radYaw = (Vec4.y * double(PI) / 180.f);
	double radRoll = (Vec4.z * double(PI) / 180.f);

	double SP = sinf(radPitch);
	double CP = cosf(radPitch);
	double SY = sinf(radYaw);
	double CY = cosf(radYaw);
	double SR = sinf(radRoll);
	double CR = cosf(radRoll);

	_MATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

static float powf_(float _X, float _Y)
{
	return (_mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y))));
}

static float sqrtf_(float _X) {
	return (_mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(_X))));
}

static double get_cross_distance(double x1, double y1, double x2, double y2) {
	return sqrtf(powf((x2 - x1), 2) + powf_((y2 - y1), 2));
}

