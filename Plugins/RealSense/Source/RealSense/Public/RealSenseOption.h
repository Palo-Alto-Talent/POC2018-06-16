#pragma once

#include "RealSenseObject.h"
#include "RealSenseOption.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class REALSENSE_API URealSenseOption : public URealSenseObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseOption();

	struct rs2_options* GetHandle();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	ERealSenseOptionType GetType();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FString GetName();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FString GetDescription();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FRealSenseOptionRange GetRange();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	float GetValue();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	void SetValue(float Value);

private:

	friend class URealSenseSensor;
	void SetHandle(struct rs2_options* Handle);
	void SetType(ERealSenseOptionType Type);

private:

	struct rs2_options* RsOptions = nullptr;
	ERealSenseOptionType OptionType;
};
