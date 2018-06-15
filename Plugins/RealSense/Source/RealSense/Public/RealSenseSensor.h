#pragma once

#include "RealSenseObject.h"
#include "RealSenseSensor.generated.h"

UCLASS(ClassGroup="RealSense", BlueprintType)
class REALSENSE_API URealSenseSensor : public URealSenseObject
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~URealSenseSensor();

	struct rs2_sensor* GetHandle();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	FString GetName();

	UFUNCTION(Category="RealSense", BlueprintCallable)
	bool Supports(ERealSenseOptionType Option);

	UFUNCTION(Category="RealSense", BlueprintCallable)
	TArray<class URealSenseOption*> QueryOptions();

private:

	friend class URealSenseDevice;
	void SetHandle(struct rs2_sensor* Handle);

private:

	struct rs2_sensor* RsSensor = nullptr;
};
