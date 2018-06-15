#include "PCH.h"

DWORD WINAPI RealSenseThreadProc(_In_ LPVOID lpParameter)
{
	auto Context = (URealSenseTextureSource*)lpParameter;
	Context->ThreadProc();
	return 0;
}

URealSenseTextureSource::URealSenseTextureSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	REALSENSE_TRACE(TEXT("+URealSenseTextureSource_%X"), this);
}

URealSenseTextureSource::~URealSenseTextureSource()
{
	REALSENSE_TRACE(TEXT("~URealSenseTextureSource_%X"), this);

	Stop();
}

void URealSenseTextureSource::Configure(FString DeviceSerial, ERealSenseStreamType StreamType, int Width, int Height, int Rate)
{
	REALSENSE_TRACE(TEXT("URealSenseTextureSource_%X::Configure SN=\"%s\" Width=%d Height=%d Rate=%d"), 
		this, *DeviceSerial, Width, Height, Rate);

	this->DeviceSerial = DeviceSerial;
	this->StreamType = StreamType;
	this->Width = Width;
	this->Height = Height;
	this->Rate = Rate;
}

bool URealSenseTextureSource::Start()
{
	REALSENSE_TRACE(TEXT("URealSenseTextureSource_%X::Start SN=\"%s\" Width=%d Height=%d Rate=%d"), 
		this, *DeviceSerial, Width, Height, Rate);

	bool Result = false;
	try
	{
		if (IsStarted())
		{
			throw RealSenseException("Already started");
		}

		rs2::context_ref RsContext(GetContext()->GetHandle());

		REALSENSE_TRACE(TEXT("query_devices"));
		auto Devices = RsContext.query_devices();
		if (Devices.size() == 0)
		{
			throw RealSenseException("No devices available");
		}

		rs2::device Device;
		if (DeviceSerial.IsEmpty()) // take any device
		{
			Device = Devices.front();
		}
		else
		{
			for (auto Iter : Devices)
			{
				FString IterSerial(Iter.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
				if (DeviceSerial.Equals(IterSerial))
				{
					Device = Iter;
					break;
				}
			}
			if (!Device)
			{
				throw RealSenseException("Specified device not found");
			}
		}

		rs2::config RsConfig;

		REALSENSE_TRACE(TEXT("enable_device"));
		RsConfig.enable_device(Device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

		REALSENSE_TRACE(TEXT("enable_stream"));
		if (StreamType == ERealSenseStreamType::Depth)
		{
			Texture.Reset(new FDynamicTexture(TEXT("RS2_STREAM_DEPTH")));
			Texture->Resize(Width, Height, PF_R16F);
			RsConfig.enable_stream(RS2_STREAM_DEPTH, Width, Height, RS2_FORMAT_Z16, Rate);
		}
		else if (StreamType == ERealSenseStreamType::Color)
		{
			Texture.Reset(new FDynamicTexture(TEXT("RS2_STREAM_COLOR")));
			Texture->Resize(Width, Height, PF_B8G8R8A8);
			RsConfig.enable_stream(RS2_STREAM_COLOR, Width, Height, RS2_FORMAT_BGRA8, Rate);
		}
		else
		{
			throw RealSenseException("Unsupported StreamType");
		}

		REALSENSE_TRACE(TEXT("new pipeline"));
		RsPipeline.Reset(new rs2::pipeline());

		REALSENSE_TRACE(TEXT("start pipeline"));
		RsPipeline->start(RsConfig);

		if (UseBackgroundThread)
		{
			REALSENSE_TRACE(TEXT("start worker"));
			ThreadFlag = 1;
			ThreadHandle = ::CreateThread(0, 0, RealSenseThreadProc, this, 0, 0); // TODO: not cross platform but UE FRunnable is epic mess
			if (!ThreadHandle)
			{
				throw RealSenseException("CreateThread failed");
			}
		}

		Result = true;
	}
	catch (const std::exception& ex)
	{
		REALSENSE_ERR(TEXT("URealSenseTextureSource_%X::Start exception: %s"), this, ANSI_TO_TCHAR(ex.what()));
	}

	if (!Result)
	{
		Stop();
	}

	return Result;
}

void URealSenseTextureSource::Stop()
{
	if (RsPipeline.Get())
	{
		REALSENSE_TRACE(TEXT("URealSenseTextureSource_%X::Stop"), this);
	}

	ThreadFlag = 0;
	if (ThreadHandle)
	{
		REALSENSE_TRACE(TEXT("join worker"));
		::WaitForSingleObject(ThreadHandle, 1000);
		::CloseHandle(ThreadHandle);
		ThreadHandle = nullptr;
	}

	if (RsPipeline.Get())
	{
		REALSENSE_TRACE(TEXT("stop pipeline"));
		try { 
			RsPipeline->stop();
		} catch(...) {}
	}

	RsPipeline.Reset();
	Texture.Reset();
}

void URealSenseTextureSource::Update() // game thread context
{
	SCOPED_PROFILER;

	if (IsStarted() && !ThreadHandle) // ensure background thread not running
	{
		rs2::frameset Frameset;
		if (RsPipeline->poll_for_frames(&Frameset))
		{
			ProcessFrameset(Frameset);
		}
	}
}

void URealSenseTextureSource::ThreadProc() // background thread context
{
	REALSENSE_TRACE(TEXT("enter ThreadProc"));

	while (ThreadFlag)
	{
		rs2::frameset Frameset = RsPipeline->wait_for_frames();
		ProcessFrameset(Frameset);
	}

	REALSENSE_TRACE(TEXT("leave ThreadProc"));
}

void URealSenseTextureSource::ProcessFrameset(rs2::frameset& Frameset)
{
	SCOPED_PROFILER;

	try
	{
		if (StreamType == ERealSenseStreamType::Depth)
		{
			Texture->UpdateFrame(Frameset.get_depth_frame());
		}
		else if (StreamType == ERealSenseStreamType::Color)
		{
			Texture->UpdateFrame(Frameset.get_color_frame());
		}

		//REALSENSE_TRACE(TEXT("Frameset %d"), FramesetId);
		FramesetId++;
	}
	catch (const std::exception& ex)
	{
		REALSENSE_ERR(TEXT("URealSenseTextureSource_%X::ProcessFrameset exception: %s"), this, ANSI_TO_TCHAR(ex.what()));
		Stop();
	}
}

UTexture2D* URealSenseTextureSource::GetTexture()
{
	auto Tex = Texture.Get();
	return Tex ? Tex->GetTextureObject() : nullptr;
}

bool URealSenseTextureSource::IsStarted()
{
	return (RsPipeline.Get() ? true : false);
}
