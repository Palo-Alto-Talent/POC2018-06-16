#include "PCH.h"

URealSenseContext::URealSenseContext(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	REALSENSE_TRACE(TEXT("+URealSenseContext_%X"), this);
}

URealSenseContext::~URealSenseContext()
{
	REALSENSE_TRACE(TEXT("~URealSenseContext_%X"), this);

	SetHandle(nullptr);
}

void URealSenseContext::SetHandle(rs2_context* Handle)
{
	RsContext = Handle;
}

rs2_context* URealSenseContext::GetHandle()
{
	return RsContext;
}

URealSenseContext* URealSenseContext::GetRealSense() // static
{
	return IRealSensePlugin::Get().GetContext();
}

TArray<URealSenseDevice*> URealSenseContext::QueryDevices()
{
	TArray<URealSenseDevice*> Result;

	rs2_error* e = nullptr;
	rs2_device_list* List = rs2_query_devices(RsContext, &e);
	rs2::error::handle(e);

	if (List)
	{
		const int Count = rs2_get_device_count(List, &e);
		rs2::error::handle(e);

		for (int i = 0; i < Count; i++)
		{
			rs2_device* RsDevice = rs2_create_device(List, i, &e);
			rs2::error::handle(e);

			if (RsDevice)
			{
				auto Object = NewObject<URealSenseDevice>(this);
				Object->SetHandle(RsDevice);
				Result.Add(Object);
			}
		}

		rs2_delete_device_list(List);
	}

	return Result;
}

URealSenseTextureSource* URealSenseContext::CreateTextureSource()
{
	return NewObject<URealSenseTextureSource>(this);
}
