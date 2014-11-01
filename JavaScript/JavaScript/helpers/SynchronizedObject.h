#pragma once

#include <Windows.h>

template<typename T>
class SynchronizedObject {
	typedef struct _Object {
		T content;
		CRITICAL_SECTION lock;
	} Object;

public:
	// create T and lock
	SynchronizedObject() {
		obj = (Object*)::HeapAlloc(::GetProcessHeap(), 0, sizeof(Object));

		lock();
	};

	// lock/unlock T
	SynchronizedObject(void* p) : obj(NULL) {
		// FIXIT: lock until make sure that obj is valid
		if(valid(p)) {
			obj = (Object*)p;

			lock();
		}
	};
	~SynchronizedObject() { if(obj) unlock(); };

	// validate and return T*
	T* get() const { return valid(obj) ? &obj->content : NULL; };

	// delete T
	void destroy() {
		if(valid(obj)) {
			::HeapFree(::GetProcessHeap(), 0, obj);
		}
	};

protected:
	bool valid(void* p) const {
		return p ? (::HeapValidate(::GetProcessHeap(), 0, p) ? true : false) : false;
	};
	void lock() {};
	void unlock() {};

	Object* obj;
};
