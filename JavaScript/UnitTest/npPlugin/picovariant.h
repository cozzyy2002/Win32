#ifndef picovariant_h
#define picovariant_h

#include <npapi.h>
#include <npruntime.h>
#include <string>

namespace picovariant {

	struct null {};

	class value {
	public:
		value() : m_exact(false) { VOID_TO_NPVARIANT(m_variant); };
		explicit value(bool b) : m_exact(false) { BOOLEAN_TO_NPVARIANT(b, m_variant); };
		explicit value(double d, bool exact = false) : m_exact(exact) { DOUBLE_TO_NPVARIANT(d, m_variant); };
		explicit value(int32_t i, bool exact = false) : m_exact(exact) { INT32_TO_NPVARIANT(i, m_variant); };
		explicit value(LPCSTR s) : m_exact(false) { STRINGZ_TO_NPVARIANT(s, m_variant); };
		explicit value(LPCSTR s, uint32_t n) : m_exact(false) { STRINGN_TO_NPVARIANT(s, n, m_variant); };
		explicit value(const std::string& s) : m_exact(false) { STRINGN_TO_NPVARIANT(s.c_str(), s.size(), m_variant); };
		explicit value(NPObject* o) : m_exact(false) { OBJECT_TO_NPVARIANT(o, m_variant); };
		explicit value(NPVariant& var, bool exact = false) : m_exact(exact) { m_variant = var; };
		virtual ~value(void) {};

		void setExact(bool exact) { m_exact = exact; };
		template<typename T> bool is() const;
		template<typename T> T get() const;

		bool isNumber() const { return NPVARIANT_IS_INT32(m_variant) || NPVARIANT_IS_DOUBLE(m_variant); };
		bool isNullString() const { return NPVARIANT_IS_STRING(m_variant) && !NPVARIANT_TO_STRING(m_variant).UTF8Characters; };
		bool isEmptyString() const { return NPVARIANT_IS_STRING(m_variant) && !isNullString() && !NPVARIANT_TO_STRING(m_variant).UTF8Length; };
		bool isNullObject() const { return NPVARIANT_IS_OBJECT(m_variant) && !NPVARIANT_TO_OBJECT(m_variant); };

		const char* getTypeName() const;

		void getProperty(NPVariant& var, NPP npp, const NPUTF8* propertyName);
		template<typename T> void getProperty(T& ret, NPP npp, const NPUTF8* propertyName);
		template<typename T> void getProperty(T& ret, NPP npp, const NPUTF8* propertyName, const T& defaultValue);
		template<typename T> T getProperty(NPP npp, const NPUTF8* propertyName);
		template<typename T> T getProperty(NPP npp, const NPUTF8* propertyName, const T& defaultValue);

	protected:
		NPVariant m_variant;
		bool m_exact;
	};

#define IS(ctype, nptype) \
	template<> inline bool value::is<ctype>() const { return NPVARIANT_IS_##nptype(m_variant); }

	IS(void, VOID)
	IS(null, NULL)
	IS(bool, BOOLEAN)
	IS(std::string, STRING)
	IS(NPObject*, OBJECT)

#undef IS

	template<>
	bool value::is<double>() const {
		return m_exact ? NPVARIANT_IS_DOUBLE(m_variant) : isNumber();
	}
	template<>
	bool value::is<int>() const {
		return m_exact ? NPVARIANT_IS_INT32(m_variant) : isNumber();
	}

#define GET(ctype, nptype) \
	template<> inline ctype value::get<ctype>() const { return NPVARIANT_TO_##nptype(m_variant); }

	GET(bool, BOOLEAN)
	GET(NPObject*, OBJECT)

#undef GET

	template<>
	double value::get<double>() const {
		return (m_exact || NPVARIANT_IS_DOUBLE(m_variant)) ? NPVARIANT_TO_DOUBLE(m_variant) : NPVARIANT_TO_INT32(m_variant);
	}
	template<>
	int value::get<int>() const {
		return (m_exact || NPVARIANT_IS_INT32(m_variant)) ? NPVARIANT_TO_INT32(m_variant) : (int)NPVARIANT_TO_DOUBLE(m_variant);
	}
	template<>
	std::string value::get<std::string>() const {
		const NPString& npstr = NPVARIANT_TO_STRING(m_variant);
		return std::string(npstr.UTF8Characters, npstr.UTF8Length);
	}

	class PicovariantException : public std::exception {};
	class NotNPObjectException : public PicovariantException {};
	class NPObjectNullException : public PicovariantException {};
	class HasNotPropertyException : public PicovariantException {};
	class GetPropertyFailedException : public PicovariantException {};
	class PropertyTypeMismatchException : public PicovariantException {};

	void value::getProperty(NPVariant& var, NPP npp, const NPUTF8* propertyName)
	{
		if(!is<NPObject*>()) throw NotNPObjectException();
		NPObject* obj = get<NPObject*>();
		if(!obj) throw NPObjectNullException();

		NPIdentifier nameId = NPN_GetStringIdentifier(propertyName);
		if(!NPN_HasProperty(npp, obj, nameId)) throw HasNotPropertyException();
		VOID_TO_NPVARIANT(var);
		if(!NPN_GetProperty(npp, obj, nameId, &var)) throw GetPropertyFailedException();
	}

	template<typename T>
	void value::getProperty(T& ret, NPP npp, const NPUTF8* propertyName)
	{
		NPVariant var;
		getProperty(npp, propertyName, var);
		value v(var);
		if(!v.is<T>()) {
			NPN_ReleaseVariantValue(&var);
			throw PropertyTypeMismatchException();
		}
		ret = v.get<T>();
		if(v.is<std::string>()) {
			NPN_ReleaseVariantValue(&var);
		}
	}

	template<typename T>
	void value::getProperty(T& ret, NPP npp, const NPUTF8* propertyName, const T& defaultValue)
	{
		try {
			getProperty<T>(ret, npp, propertyName);
		} catch(HasNotPropertyException) {
			ret = defaultValue;
		}
	}

	template<typename T>
	T getProperty(NPP npp, const NPUTF8* propertyName)
	{
		T ret;
		getProperty(ret, npp, propertyName);
		return T;
	}

	template<typename T>
	T getProperty(NPP npp, const NPUTF8* propertyName, const T& defaultValue)
	{
		T ret;
		getProperty(ret, npp, propertyName, defaultValue);
		return T;
	}

	const char* value::getTypeName() const {
		const char* name = "unknown";
		switch(m_variant.type) {
#define NAME(T) case NPVariantType_##T: name= #T; break
			NAME(Void);
			NAME(Null);
			NAME(Bool);
			NAME(Double);
			NAME(Int32);
			NAME(String);
			NAME(Object);
#undef NAME
		}
		return name;
	}
}

#endif // picovariant_h
