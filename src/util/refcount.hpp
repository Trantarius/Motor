#pragma once

struct _ref_count_init{};

#ifndef NDEBUG
//dummy ref prevents a constructor from not initializing the ref counted data holder
constexpr _ref_count_init _global_ref_count_dummy;
#define _REF_COUNT_DUMMY_REF const _ref_count_init& _ref_count_dummy_ref;
#define _REF_COUNT_DUMMY_REF_INIT :_ref_count_dummy_ref(_global_ref_count_dummy)
#else
#define _REF_COUNT_DUMMY_REF
#define _REF_COUNT_DUMMY_REF_INIT
#endif

#define REF_COUNTER(CLASS,DATA)                              \
private:                                                     \
	DATA* data=nullptr;                                        \
	_REF_COUNT_DUMMY_REF                                       \
	void onDestroy();                                          \
																														\
	void dispose(){                                            \
		onDestroy();                                             \
		delete data;                                             \
		data=nullptr;                                            \
	}                                                          \
																														\
	CLASS(_ref_count_init)_REF_COUNT_DUMMY_REF_INIT{           \
		data=new DATA();                                         \
		data->refcount+=1;                                       \
	}                                                          \
																														\
public:                                                      \
																														\
	CLASS()_REF_COUNT_DUMMY_REF_INIT{}                         \
																														\
																														\
	CLASS(const CLASS& b)_REF_COUNT_DUMMY_REF_INIT{            \
		*this=b;                                                 \
	}                                                          \
																														\
	void operator = (const CLASS& b){                          \
		if(data!=nullptr){                                       \
			data->refcount--;                                      \
			if(data->refcount==0){                                 \
				dispose();                                           \
			}                                                      \
		}                                                        \
																														\
		data=b.data;                                             \
		if(data!=nullptr){                                       \
			data->refcount++;                                      \
		}                                                        \
	}                                                          \
																														\
	bool isNull() const{                                       \
		return data==nullptr;                                    \
	}                                                          \
																														\
	~CLASS(){                                                  \
		if(data!=nullptr){                                       \
			data->refcount--;                                      \
			if(data->refcount==0){                                 \
				dispose();                                           \
			}                                                      \
		}                                                        \
	}
