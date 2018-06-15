#include "PCH.h"

URealSenseOption::URealSenseOption(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//REALSENSE_TRACE(TEXT("+URealSenseOption_%X"), this);
}

URealSenseOption::~URealSenseOption()
{
	//REALSENSE_TRACE(TEXT("~URealSenseOption_%X"), this);
}

void URealSenseOption::SetHandle(rs2_options* Handle)
{
	RsOptions = Handle;
}

void URealSenseOption::SetType(ERealSenseOptionType Type)
{
	OptionType = Type;
}

rs2_options* URealSenseOption::GetHandle()
{
	return RsOptions;
}

ERealSenseOptionType URealSenseOption::GetType()
{
	return OptionType;
}

FString URealSenseOption::GetName()
{
	return FString(ANSI_TO_TCHAR(rs2_option_to_string((rs2_option)OptionType)));
}

FString URealSenseOption::GetDescription()
{
	rs2_error* e = nullptr;
	auto Str = rs2_get_option_description(RsOptions, (rs2_option)OptionType, &e);
	rs2::error::handle(e);

	return FString(ANSI_TO_TCHAR(Str));
}

FRealSenseOptionRange URealSenseOption::GetRange()
{
	FRealSenseOptionRange Range;

	rs2_error* e = nullptr;
	rs2_get_option_range(RsOptions, (rs2_option)OptionType, &Range.Min, &Range.Max, &Range.Step, &Range.Default, &e);
	rs2::error::handle(e);

	return Range;
}

float URealSenseOption::GetValue()
{
	rs2_error* e = nullptr;
	float Value = rs2_get_option(RsOptions, (rs2_option)OptionType, &e);
	rs2::error::handle(e);

	return Value;
}

void URealSenseOption::SetValue(float Value)
{
	rs2_error* e = nullptr;
	rs2_set_option(RsOptions, (rs2_option)OptionType, Value, &e);
	rs2::error::handle(e);
}
