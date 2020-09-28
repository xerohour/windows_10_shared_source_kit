typedef enum _CFRAME_LINES
{
    CFRAME_LINES_ODD    = 0,
    CFRAME_LINES_EVEN   = 1,
    CFRAME_LINES_ALL    = 2
}   CFRAME_LINES;

typedef enum _CFRAME_TYPE
{
    CFRAME_TYPE_MAIN    = 0,
    CFRAME_TYPE_FUTURE  = 1,
    CFRAME_TYPE_PAST    = 2
} CFRAME_TYPE;

typedef enum _CCONTENT_TYPE
{
    CCONTENT_TYPE_NONE        = 0,
    CCONTENT_TYPE_COLOR_BARS  = 1,
    CCONTENT_TYPE_SOLID_COLOR = 2,
    CCONTENT_TYPE_LINES_V     = 3,
    CCONTENT_TYPE_LINES_H     = 4,
    CCONTENT_TYPE_BOX         = 5
}   CCONTENT_TYPE;

typedef enum _CCONTENT_MOVEMENT_TYPE
{
    CCONTENT_MOVEMENT_TYPE_NONE         = 0,
    CCONTENT_MOVEMENT_TYPE_HORIZONTAL   = 1,
    CCONTENT_MOVEMENT_TYPE_VERTICAL     = 2,
    CCONTENT_MOVEMENT_TYPE_CIRCULAR     = 3,
    CCONTENT_MOVEMENT_TYPE_DIAGONALLY   = 4
} CCONTENT_MOVEMENT_TYPE;

typedef enum _CCONTENT_SIZE_CHANGE_TYPE
{
    CCONTENT_SIZE_CHANGE_TYPE_NONE      = 0,
    CCONTENT_SIZE_CHANGE_TYPE_SHRINK    = 1,
    CCONTENT_SIZE_CHANGE_TYPE_STRETCH   = 2,
    CCONTENT_SIZE_CHANGE_TYPE_ANGLE     = 3
}   CCONTENT_SIZE_CHANGE_TYPE;

typedef	struct	_CCONTENT_DESC
{
	CCONTENT_TYPE               Type;
    DXVAHD_FRAME_FORMAT         FrameFormat;        //  progressive/ interlaced even first/ odd first
    float                       flAlpha;            //  value to fill the alpha channel with.
    UINT                        uiWidth;            //  the width of the content.
    UINT                        uiHeight;           //  the height of the content.
    SIZE                        siCoordinates;      //  where the content should be drawn on the surface.
    float                       flSizePercentage;   //  the size based on a percentage of the actual surface.
    BOOL                        bMoving;            //  whether the content will be moving across the surface.
    CCONTENT_MOVEMENT_TYPE      MovementType;       //  how the content will move and which way.
	float                       flMovementRate;     //  the percentage of the surface to move per frame.                 
    BOOL                        bSizeChanging;      //  whether the content will change size across frames.
    CCONTENT_SIZE_CHANGE_TYPE   SizeChangingType;   //  how the size should change Shrink or stretch.
    float                       flSizeChangingRate; //  the percentage of the current size of the content to adjust.
    BOOL                        bColorChanging;     //  whether the content will change in color.
    UINT                        uiColorChangingRate;//  how often in frames the color should change.
}	CCONTENT_DESC;

