#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogRealSense, Log, All);
#define REALSENSE_ERR(Format, ...) UE_LOG(LogRealSense, Error, Format, __VA_ARGS__)
#define REALSENSE_TRACE(Format, ...) UE_LOG(LogRealSense, Warning, Format, __VA_ARGS__)

class RealSenseException : public std::runtime_error
{
public:
	RealSenseException() : runtime_error("") {}
	explicit RealSenseException(const char* Msg) : runtime_error(Msg) {}
};

inline const TCHAR* operator*(std::string& str) { return ANSI_TO_TCHAR(str.c_str()); }

template<typename T>
inline T* NewInstance(const TCHAR* Name, UObject* Owner = nullptr)
{
	static int InstanceId = 0;
	auto ObjName = FString::Printf(TEXT("%s_%d"), Name, InstanceId++);
	auto Object = NewObject<T>((Owner ? Owner : GetTransientPackage()), FName(*ObjName));
	return Object;
}

namespace rs2 {
	inline void release_context_ref(rs2_context* Handle) {} // do nothing
	class context_ref : public context {
	public:
		context_ref(rs2_context* Handle) : context(std::shared_ptr<rs2_context>(Handle, release_context_ref)) { }
	};
}
