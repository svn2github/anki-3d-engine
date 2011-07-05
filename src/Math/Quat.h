#ifndef M_QUAT_H
#define M_QUAT_H

#include "Common.h"


namespace M {


/// Used in rotations
class Quat
{
	public:
		/// @name Constructors & destructors
		/// @{
		explicit Quat();
		explicit Quat(float f);
		explicit Quat(float x, float y, float z, float w);
		explicit Quat(const Vec2& v2, float z, float w);
		explicit Quat(const Vec3& v3, float w);
		explicit Quat(const Vec4& v4);
		         Quat(const Quat& b);
		explicit Quat(const Mat3& m3);
		explicit Quat(const Euler& eu);
		explicit Quat(const Axisang& axisang);
		/// @}

		/// @name Accessors
		/// @{
		float x() const;
		float& x();
		float y() const;
		float& y();
		float z() const;
		float& z();
		float w() const;
		float& w();
		/// @}

		/// Operators with same
		/// @{
		Quat& operator=(const Quat& b);
		Quat operator*(const Quat& b) const; ///< 16 muls, 12 adds
		Quat& operator*=(const Quat& b);
		bool operator==(const Quat& b) const;
		bool operator!=(const Quat& b) const;
		/// @}

		/// @name Other
		/// @{

		/// Calculates a quat from v0 to v1
		void  setFrom2Vec3(const Vec3& v0, const Vec3& v1);
		float getLength() const;
		Quat  getInverted() const;
		void  invert();
		void  conjugate();
		Quat  getConjugated() const;
		void  normalize();
		Quat  getNormalized() const;
		float dot(const Quat& b) const;
		/// Returns slerp(this, q1, t)
		Quat  slerp(const Quat& q1, float t) const;
		Quat  getRotated(const Quat& b) const; ///< The same as Quat * Quat
		void  rotate(const Quat& b); ///< @see getRotated
		void  setIdentity();
		static const Quat& getIdentity();
		/// @}

	private:
		/// @name Data
		/// @{
		struct
		{
			float x, y, z, w;
		} vec;
		/// @}
};


/// @name Other operators
/// @{
extern std::ostream& operator<<(std::ostream& s, const Quat& q);
/// @}


} // end namespace


#include "Quat.inl.h"


#endif
