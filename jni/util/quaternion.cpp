#include "main.h"

void CQuaternion::ConvertMatrixToQuaternion(PQUATERNION q, PMATRIX4X4 m)
{
	if(!q || !m) return;

	q->W = sqrt( std::max( (float)0, 1.0f + m->right.X + m->up.Y + m->at.Z ) ) * 0.5f;
    q->X = sqrt( std::max( (float)0, 1.0f + m->right.X - m->up.Y - m->at.Z ) ) * 0.5f;
    q->Y = sqrt( std::max( (float)0, 1.0f - m->right.X + m->up.Y - m->at.Z ) ) * 0.5f;
    q->Z = sqrt( std::max( (float)0, 1.0f - m->right.X - m->up.Y + m->at.Z ) ) * 0.5f;

    q->X = static_cast < float > ( copysign( q->X, m->at.Y - m->up.Z ) );
    q->Y = static_cast < float > ( copysign( q->Y, m->right.Z - m->at.X ) );
    q->Z = static_cast < float > ( copysign( q->Z, m->up.X - m->right.Y ) );
}

void CQuaternion::ConvertQuaternionToMatrix(PMATRIX4X4 m, PQUATERNION q)
{
	float sqw = q->W*q->W; // v13 = a1 * a1;
    float sqx = q->X*q->X; // v14 = a2 * a2;
    float sqy = q->Y*q->Y; // v15 = a3 * a3;
    float sqz = q->Z*q->Z; // v16 = a4 * a4;

    m->right.X = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
    m->up.Y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
    m->at.Z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;
    
    float tmp1 = q->X*q->Y;					// v17 = a2 * a3;
    float tmp2 = q->Z*q->W;					// v18 = a1 * a4;
    m->up.X = 2.0 * (tmp1 + tmp2);			// a8 = v18 + v17 + v18 + v17;
    m->right.Y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;
    
    tmp1 = q->X*q->Z;						// v20 = a2 * a4;
    tmp2 = q->Y*q->W;						// v21 = a1 * a3;
    m->at.X = 2.0 * (tmp1 - tmp2);			// a11 = v20 - v21 + v20 - v21;
    m->right.Z 	= 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
    tmp1 = q->Y*q->Z;						// v22 = a3 * a4;
    tmp2 = q->X*q->W;						// v23 = a1 * a2;
    m->at.Y = 2.0 * (tmp1 + tmp2);			// a12 = v23 + v22 + v23 + v22;
    m->up.Z = 2.0 * (tmp1 - tmp2);			// a10 = v22 - v23 + v22 - v23;
}

void CQuaternion::Normalize(PQUATERNION q)
{
	double n = sqrt(q->X*q->X + q->Y*q->Y + q->Z*q->Z + q->W*q->W);
	q->X /= n;
	q->Y /= n;
	q->Z /= n;
	q->W /= n;
}

void CQuaternion::Slerp(PQUATERNION out, QUATERNION a, QUATERNION b, float t) 
{
  // benchmarks:
  //    http://jsperf.com/quaternion-slerp-implementations

  float ax = a.X, ay = a.Y, az = a.Z, aw = a.W,
    bx = b.X, by = b.Y, bz = b.Z, bw = b.W;

  float omega, cosom, sinom, scale0, scale1;

  // calc cosine
  cosom = ax * bx + ay * by + az * bz + aw * bw;
  // adjust signs (if necessary)
  if (cosom < 0.0) 
  {
    cosom = -cosom;
    bx = -bx;
    by = -by;
    bz = -bz;
    bw = -bw;
  }
  // calculate coefficients
  if ((1.0 - cosom) > 0.000001) 
  {
    // standard case (slerp)
    omega = acosf(cosom);
    sinom = sinf(omega);
    scale0 = sinf((1.0 - t) * omega) / sinom;
    scale1 = sinf(t * omega) / sinom;
  } 
  else 
  {
    // "from" and "to" quaternions are very close
    //  ... so we can do a linear interpolation
    scale0 = 1.0 - t;
    scale1 = t;
  }

  // calculate final values
  out->X = scale0 * ax + scale1 * bx;
  out->Y = scale0 * ay + scale1 * by;
  out->Z = scale0 * az + scale1 * bz;
  out->W = scale0 * aw + scale1 * bw;
}
