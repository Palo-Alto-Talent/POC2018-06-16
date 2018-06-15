#pragma once

#include "RealSenseTypes.generated.h"

UENUM(Blueprintable)
enum class ERealSenseStreamType : uint8
{
	Depth = 0,
	Color,
	Infrared,
};

// rs2_option
UENUM(Blueprintable)
enum class ERealSenseOptionType : uint8
{
    BACKLIGHT_COMPENSATION                     , /**< Enable / disable color backlight compensation*/
    BRIGHTNESS                                 , /**< Color image brightness*/
    CONTRAST                                   , /**< Color image contrast*/
    EXPOSURE                                   , /**< Controls exposure time of color camera. Setting any value will disable auto exposure*/
    GAIN                                       , /**< Color image gain*/
    GAMMA                                      , /**< Color image gamma setting*/
    HUE                                        , /**< Color image hue*/
    SATURATION                                 , /**< Color image saturation setting*/
    SHARPNESS                                  , /**< Color image sharpness setting*/
    WHITE_BALANCE                              , /**< Controls white balance of color image. Setting any value will disable auto white balance*/
    ENABLE_AUTO_EXPOSURE                       , /**< Enable / disable color image auto-exposure*/
    ENABLE_AUTO_WHITE_BALANCE                  , /**< Enable / disable color image auto-white-balance*/
    VISUAL_PRESET                              , /**< Provide access to several recommend sets of option presets for the depth camera */
    LASER_POWER                                , /**< Power of the F200 / SR300 projector, with 0 meaning projector off*/
    ACCURACY                                   , /**< Set the number of patterns projected per frame. The higher the accuracy value the more patterns projected. Increasing the number of patterns help to achieve better accuracy. Note that this control is affecting the Depth FPS */
    MOTION_RANGE                               , /**< Motion vs. Range trade-off, with lower values allowing for better motion sensitivity and higher values allowing for better depth range*/
    FILTER_OPTION                              , /**< Set the filter to apply to each depth frame. Each one of the filter is optimized per the application requirements*/
    CONFIDENCE_THRESHOLD                       , /**< The confidence level threshold used by the Depth algorithm pipe to set whether a pixel will get a valid range or will be marked with invalid range*/
    EMITTER_ENABLED                            , /**< Laser Emitter enabled */
    FRAMES_QUEUE_SIZE                          , /**< Number of frames the user is allowed to keep per stream. Trying to hold-on to more frames will cause frame-drops.*/
    TOTAL_FRAME_DROPS                          , /**< Total number of detected frame drops from all streams */
    AUTO_EXPOSURE_MODE                         , /**< Auto-Exposure modes: Static, Anti-Flicker and Hybrid */
    POWER_LINE_FREQUENCY                       , /**< Power Line Frequency control for anti-flickering Off/50Hz/60Hz/Auto */
    ASIC_TEMPERATURE                           , /**< Current Asic Temperature */
    ERROR_POLLING_ENABLED                      , /**< disable error handling */
    PROJECTOR_TEMPERATURE                      , /**< Current Projector Temperature */
    OUTPUT_TRIGGER_ENABLED                     , /**< Enable / disable trigger to be outputed from the camera to any external device on every depth frame */
    MOTION_MODULE_TEMPERATURE                  , /**< Current Motion-Module Temperature */
    DEPTH_UNITS                                , /**< Number of meters represented by a single depth unit */
    ENABLE_MOTION_CORRECTION                   , /**< Enable/Disable automatic correction of the motion data */
    AUTO_EXPOSURE_PRIORITY                     , /**< Allows sensor to dynamically ajust the frame rate depending on lighting conditions */
    COLOR_SCHEME                               , /**< Color scheme for data visualization */
    HISTOGRAM_EQUALIZATION_ENABLED             , /**< Perform histogram equalization post-processing on the depth data */
    MIN_DISTANCE                               , /**< Minimal distance to the target */
    MAX_DISTANCE                               , /**< Maximum distance to the target */
    TEXTURE_SOURCE                             , /**< Texture mapping stream unique ID */
    FILTER_MAGNITUDE                           , /**< The 2D-filter effect. The specific interpretation is given within the context of the filter */
    FILTER_SMOOTH_ALPHA                        , /**< 2D-filter parameter controls the weight/radius for smoothing.*/
    FILTER_SMOOTH_DELTA                        , /**< 2D-filter range/validity threshold*/
    HOLES_FILL                                 , /**< Enhance depth data post-processing with holes filling where appropriate*/
    STEREO_BASELINE                            , /**< The distance in mm between the first and the second imagers in stereo-based depth cameras*/
    AUTO_EXPOSURE_CONVERGE_STEP                , /**< Allows dynamically ajust the converge step value of the target exposure in Auto-Exposure algorithm*/
};

USTRUCT(BlueprintType)
struct FRealSenseOptionRange
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Min;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Max;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Step;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Default;
};
