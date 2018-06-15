#include "PCH.h"

URealSenseDevice::URealSenseDevice(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	REALSENSE_TRACE(TEXT("+URealSenseDevice_%X"), this);
}

URealSenseDevice::~URealSenseDevice()
{
	REALSENSE_TRACE(TEXT("~URealSenseDevice_%X"), this);

	SetHandle(nullptr);
}

void URealSenseDevice::SetHandle(rs2_device* Handle)
{
	if (RsDevice) rs2_delete_device(RsDevice);
	RsDevice = Handle;
}

rs2_device* URealSenseDevice::GetHandle()
{
	return RsDevice;
}

FString URealSenseDevice::GetName()
{
	rs2_error* e = nullptr;
	auto Str = rs2_get_device_info(RsDevice, RS2_CAMERA_INFO_NAME, &e);
	rs2::error::handle(e);

	return FString(ANSI_TO_TCHAR(Str));
}

FString URealSenseDevice::GetSerial()
{
	rs2_error* e = nullptr;
	auto Str = rs2_get_device_info(RsDevice, RS2_CAMERA_INFO_SERIAL_NUMBER, &e);
	rs2::error::handle(e);

	return FString(ANSI_TO_TCHAR(Str));
}

TArray<URealSenseSensor*> URealSenseDevice::QuerySensors()
{
	TArray<URealSenseSensor*> Result;

	rs2_error* e = nullptr;
	rs2_sensor_list* List = rs2_query_sensors(RsDevice, &e);
	rs2::error::handle(e);

	if (List)
	{
		const int Count = rs2_get_sensors_count(List, &e);
		rs2::error::handle(e);

		for (int i = 0; i < Count; i++)
		{
			rs2_sensor* RsSensor = rs2_create_sensor(List, i, &e);
			rs2::error::handle(e);

			if (RsSensor)
			{
				auto Object = NewObject<URealSenseSensor>(this);
				Object->SetHandle(RsSensor);
				Result.Add(Object);
			}
		}

		rs2_delete_sensor_list(List);
	}

	return Result;
}
