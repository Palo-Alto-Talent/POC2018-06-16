#include "PCH.h"
#include "RealSensePluginImpl.h"

DEFINE_LOG_CATEGORY(LogRealSense);

FRealSensePlugin::FRealSensePlugin()
{
	REALSENSE_TRACE(TEXT("+FRealSensePlugin_%X"), this);
}

FRealSensePlugin::~FRealSensePlugin()
{
	REALSENSE_TRACE(TEXT("~FRealSensePlugin_%X"), this);

	ReleaseContext();
}

void FRealSensePlugin::StartupModule()
{
	REALSENSE_TRACE(TEXT("FRealSensePlugin_%X::StartupModule"), this);

	#if defined(PROF_ENABLE)
	HiresTimer::Init();
	Profiler::InitInstance();
	#endif
}

void FRealSensePlugin::ShutdownModule()
{
	REALSENSE_TRACE(TEXT("FRealSensePlugin_%X::ShutdownModule"), this);

	ReleaseContext();

	#if defined(PROF_ENABLE)
	Profiler::GetInstance()->LogSummary();
	Profiler::DestroyInstance();
	#endif
}

URealSenseContext* FRealSensePlugin::GetContext()
{
	if (!Context)
	{
		FScopeLock Lock(&ContextMx);
		if (!Context)
		{
			REALSENSE_TRACE(TEXT("rs2_create_context"));
			rs2_error* e = nullptr;
			RsContext = rs2_create_context(RS2_API_VERSION, &e);
			rs2::error::handle(e);
			REALSENSE_TRACE(TEXT("rs2_context -> %X"), RsContext);

			Context = NewInstance<URealSenseContext>(TEXT("RealSenseContext"));
			Context->SetHandle(RsContext);
			//Context->AddToRoot();
		}
	}
	return Context;
}

void FRealSensePlugin::ReleaseContext()
{
	FScopeLock Lock(&ContextMx);

	if (RsContext)
	{
		REALSENSE_TRACE(TEXT("rs2_delete_context %X"), RsContext);
		rs2_delete_context(RsContext);
		RsContext = nullptr;
	}

	//Context->RemoveFromRoot();
	Context = nullptr;
}

IMPLEMENT_MODULE(FRealSensePlugin, RealSense)
