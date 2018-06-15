#include "PCH.h"

URealSenseSensor::URealSenseSensor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	REALSENSE_TRACE(TEXT("+URealSenseSensor_%X"), this);
}

URealSenseSensor::~URealSenseSensor()
{
	REALSENSE_TRACE(TEXT("~URealSenseSensor_%X"), this);

	SetHandle(nullptr);
}

void URealSenseSensor::SetHandle(rs2_sensor* Handle)
{
	if (RsSensor) rs2_delete_sensor(RsSensor);
	RsSensor = Handle;
}

rs2_sensor* URealSenseSensor::GetHandle()
{
	return RsSensor;
}

FString URealSenseSensor::GetName()
{
	rs2_error* e = nullptr;
	auto Str = rs2_get_sensor_info(RsSensor, RS2_CAMERA_INFO_NAME, &e);
	rs2::error::handle(e);

	return FString(ANSI_TO_TCHAR(Str));
}

bool URealSenseSensor::Supports(ERealSenseOptionType Option)
{
	rs2_error* e = nullptr;
	int Value = rs2_supports_option((const rs2_options*)RsSensor, (rs2_option)Option, &e);
	rs2::error::handle(e);

	return (Value > 0);
}

TArray<URealSenseOption*> URealSenseSensor::QueryOptions()
{
	TArray<URealSenseOption*> Result;

	for (int i = 0; i < RS2_OPTION_COUNT; ++i)
	{
		if (Supports((ERealSenseOptionType)i))
		{
			auto Object = NewObject<URealSenseOption>(this);
			Object->SetHandle((rs2_options*)RsSensor);
			Object->SetType((ERealSenseOptionType)i);
			Result.Add(Object);
		}
	}

	return Result;
}
