#ifndef MTL_USER_DEFINED_VAR_RUNTIME_H
#define MTL_USER_DEFINED_VAR_RUNTIME_H

#include "MtlUserDefinedVar.h"


/// @todo
class MtlUserDefinedVarRuntime
{
	friend class ConstructVisitor;

	public:
		/// The data union. The Texture resource is readonly at runtime
		typedef boost::variant<float, Vec2, Vec3, Vec4, const RsrcPtr<Texture>*, MtlUserDefinedVar::Fai> DataVariant;

		MtlUserDefinedVarRuntime(const MtlUserDefinedVar& rsrc);

		/// @name Accessors
		/// @{
		const MtlUserDefinedVar& getMtlUserDefinedVar() const {return rsrc;}

		/// Get the value of the variant
		/// @exception boost::exception when you try to get the incorrect data type
		template<typename Type>
		const Type& get() const {return boost::get<Type>(data);}

		/// Get the value of the variant
		/// @exception boost::exception when you try to get the incorrect data type
		template<typename Type>
		Type& get() {return boost::get<Type>(data);}
		/// @}

	private:
		/// @todo
		class ConstructVisitor: public boost::static_visitor<void>
		{
			public:
				MtlUserDefinedVarRuntime& udvr;

				ConstructVisitor(MtlUserDefinedVarRuntime& udvr_): udvr(udvr_) {}

				void operator()(float x) const;
				void operator()(const Vec2& x) const;
				void operator()(const Vec3& x) const;
				void operator()(const Vec4& x) const;
				void operator()(const RsrcPtr<Texture>& x) const;
				void operator()(MtlUserDefinedVar::Fai x) const;
		};

		DataVariant data;
		const MtlUserDefinedVar& rsrc;
};


#endif
