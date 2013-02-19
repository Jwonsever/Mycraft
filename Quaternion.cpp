//Quaternion implementation.

#include "Quaternion.h"

// Convert from Axis Angle
void Quaternion::FromAxis(const vec3 &v, float angle)
{
	float sinAngle;
	angle *= 0.5f;
	vec3 vn = glm::normalize(v);
 
	sinAngle = sin(angle);
 
	x = (vn.x * sinAngle);
	y = (vn.y * sinAngle);
	z = (vn.z * sinAngle);
	w = cos(angle);

}

// Convert to Matrix
mat4 Quaternion::getMatrix()
{
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
 
	// This calculation would be a lot more complicated for non-unit length quaternions
	// Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
	//   OpenGL
	return mat4( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
		     2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
		     2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
		     0.0f, 0.0f, 0.0f, 1.0f);
}

// Multiplying q1 with q2 applies the rotation q2 to q1
Quaternion Quaternion::operator* (const Quaternion &rq)
{
	// the constructor takes its arguments as (x, y, z, w)
        Quaternion product;
	product.x = w * rq.x + x * rq.w + y * rq.z - z * rq.y;
	product.y = w * rq.y + y * rq.w + z * rq.x - x * rq.z;
	product.z = w * rq.z + z * rq.w + x * rq.y - y * rq.x;
	product.w = w * rq.w - x * rq.x - y * rq.y - z * rq.z;
	return product;
}

// We need to get the inverse of a quaternion to properly apply a quaternion-rotation to a vector
// The conjugate of a quaternion is the same as the inverse, as long as the quaternion is unit-length
Quaternion Quaternion::getConjugate()
{
        Quaternion conjugate;
	conjugate.x = -x;
	conjugate.y = -y;
	conjugate.z = -z;
	conjugate,w = -w;
	return conjugate;
}

// Multiplying a quaternion q with a vector v applies the q-rotation to v
vec3 Quaternion::operator* (const vec3 &vec)
{
        vec3 vn = glm::normalize(vec);
 
	Quaternion vecQuat, resQuat;
	vecQuat.x = vn.x;
	vecQuat.y = vn.y;
	vecQuat.z = vn.z;
	vecQuat.w = 0.0f;
 
	resQuat = vecQuat * getConjugate();
	resQuat = *this * resQuat;
 
	return (vec3(resQuat.x, resQuat.y, resQuat.z));
}

Quaternion::Quaternion()
{

}
