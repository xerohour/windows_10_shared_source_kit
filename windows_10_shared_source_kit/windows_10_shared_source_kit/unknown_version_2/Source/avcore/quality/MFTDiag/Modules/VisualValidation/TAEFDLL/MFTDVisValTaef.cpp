#include "stdafx.h"
#include <initguid.h>
#include "MFTestSinkInterface.h"
#include "MFTDVisValTaef.h"
#include "MFTVisValTests.h"
#include "HMFTUnit.h"
#include <sstream>
#include "TestCommon.h"
DWORD   g_dwLastTestID = 0;
DBTestInfo  *g_pDBTestInfo = NULL;


#define SAFECOMFREE(x) \
    { \
        if((x) != NULL) \
        { \
            CoTaskMemFree(x); \
            (x) = NULL; \
        } \
    }


static  const TCHAR StoreMFTRegKey[] = TEXT("SOFTWARE\\Microsoft\\Windows Media Foundation\\StoreMFT");

void AllowVPxDecoderForClassicApps()
{
    HKEY    hkey;
    DWORD dwKeyValue = 1;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, StoreMFTRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hkey, TEXT("EnableForwin32App"), 0, REG_DWORD, (BYTE *)&dwKeyValue, sizeof(dwKeyValue));
        RegCloseKey(hkey);
    }
}

using namespace std;
using namespace WEX::Logging;

HRESULT VerifyH264HardwareEncoderPresent(void)
{
    HRESULT                 hr                  = S_OK;
    MFT_REGISTER_TYPE_INFO  mfrtiOutputType     = {GUID_NULL};
    IMFActivate**           ppMFTActivates      = NULL;
    UINT32                  un32NumActivates    = 0;

    do
    {
        mfrtiOutputType.guidMajorType   = MFMediaType_Video;
        mfrtiOutputType.guidSubtype     = MFVideoFormat_H264;

        hr = MFTEnumEx(
            MFT_CATEGORY_VIDEO_ENCODER,
            MFT_ENUM_FLAG_HARDWARE,
            NULL,
            &mfrtiOutputType,
            &ppMFTActivates,
            &un32NumActivates
            );
        if(FAILED(hr))
        {
            break;
        }

        if(un32NumActivates == 0)
        {
            // No H264 Hardware encoders are present
            hr = S_FALSE;
            break;
        }
    }while(false);

    for(UINT32 un32CurrActivate = 0; un32CurrActivate < un32NumActivates; un32CurrActivate++)
    {
        SAFERELEASE(ppMFTActivates[un32CurrActivate]);
    }

    SAFECOMFREE(ppMFTActivates);

    return hr;
}

namespace MFTDVisVal
{
    static const WCHAR g_pszDefaultLogName[] = L"MFTDiag_VisualValidation.log";
};

CMFTDiagVisValTAEF::CMFTDiagVisValTAEF(void)
{
    m_pszDefaultLogFileName = MFTDVisVal::g_pszDefaultLogName;

    m_dwVerifyHWKeys        = (VERIFY_HW_DECODE_REG_KEY | VERIFY_HW_ENCODE_REG_KEY | VERIFY_HW_AT_LEAST_ONE);
}

CMFTDiagVisValTAEF::~CMFTDiagVisValTAEF(void)
{
    // Nothing to do
}

bool CMFTDiagVisValTAEF::InitializeTestClass(void)
{
    bool                    bContinueTests = false;
    WEX::Common::String     sCABAC;
    WEX::Common::String     sDisableMP4pt2PostProcessing;

    do
    {
        bContinueTests = InitializeBaseTestClass();
        if(bContinueTests == false)
        {
            break;
        }

        if(m_pXMLConfig == NULL)
        {
            if(FAILED(InitializeVisValTestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
                break;
            }
        }

        AllowVPxDecoderForClassicApps();

        if(SUCCEEDED(WEX::TestExecution::RuntimeParameters::TryGetValue(
            L"QualityTestCABAC", 
            sCABAC)))
        {
            if(_wcsicmp((const WCHAR*)sCABAC, L"TRUE") == 0)
            {
                g_bUseCabacForQualityTests  = TRUE;
            }
        }

        if(SUCCEEDED(WEX::TestExecution::RuntimeParameters::TryGetValue(
            L"DisableMPEG4pt2DecoderPostProcessing", 
            sDisableMP4pt2PostProcessing)))
        {
            if(_wcsicmp((const WCHAR*)sDisableMP4pt2PostProcessing, L"TRUE") == 0)
            {
                g_bDisableMPEG4pt2DecoderPostProcessing = TRUE;
            }
        }
    }while(false);

    return bContinueTests;
}

bool CMFTDiagVisValTAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}
  
void CMFTDiagVisValTAEF::VisValPlaybackTest61(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(61)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest62(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(62)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest64(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(64)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest65(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(65)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest67(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(67)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest68(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(68)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest69(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(69)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest70(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(70)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest71(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(71)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest72(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(72)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest73(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(73)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest74(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(74)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest77(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(77)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest78(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(78)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest79(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(79)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest81(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(81)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest82(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(82)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest83(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(83)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest84(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(84)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest85(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(85)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest86(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(86)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest87(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(87)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest88(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(88)); 
} 
  
void CMFTDiagVisValTAEF::VisValPlaybackTest89(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(89)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest93(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(93)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest94(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(94)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest95(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(95)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest96(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(96)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest97(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(97)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest98(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(98)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest99(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(99)); 
} 

void CMFTDiagVisValTAEF::VisValPlaybackTest100(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(100)); 
} 
  
void CMFTDiagVisValTAEF::VisValEncoderSettingsTest201(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(201)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest202(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(202)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest203(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(203)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest204(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(204)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest205(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(205)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest206(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(206)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest207(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(207)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest208(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(208)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest209(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(209)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest210(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(210)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest211(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(211)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest212(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(212)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest213(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(213)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest214(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(214)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest215(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(215)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest216(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(216)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest217(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(217)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest218(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(218)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest219(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(219)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest220(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(220)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest221(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(221)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest222(void) 
{ 
  VERIFY_SUCCEEDED(RunTest(222)); 
}

void CMFTDiagVisValTAEF::VisValSeekTest376(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(376)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest377(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(377)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest378(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(378)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest379(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(379)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest380(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(380)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest381(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(381)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest382(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(382)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest383(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(383)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest384(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(384)); 
} 
  
void CMFTDiagVisValTAEF::VisValSeekTest385(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(385)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest406(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(406)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest407(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(407)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest408(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(408)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest409(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(409)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest410(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(410)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest411(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(411)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest412(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(412)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest413(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(413)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest414(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(414)); 
} 
  
void CMFTDiagVisValTAEF::VisValRateTest415(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(415)); 
} 
  
void CMFTDiagVisValTAEF::VisValEncoderSettingsTest426(void)
{
    VERIFY_SUCCEEDED(RunTest(426)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest427(void)
{
    VERIFY_SUCCEEDED(RunTest(427)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest428(void)
{
    VERIFY_SUCCEEDED(RunTest(428)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest429(void)
{
    VERIFY_SUCCEEDED(RunTest(429)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest430(void)
{
    VERIFY_SUCCEEDED(RunTest(430)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest431(void)
{
    VERIFY_SUCCEEDED(RunTest(431)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest432(void)
{
    VERIFY_SUCCEEDED(RunTest(432)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest433(void)
{
    VERIFY_SUCCEEDED(RunTest(433)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest434(void)
{
    VERIFY_SUCCEEDED(RunTest(434)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest435(void)
{
    VERIFY_SUCCEEDED(RunTest(435)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest436(void)
{
    VERIFY_SUCCEEDED(RunTest(436)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest437(void)
{
    VERIFY_SUCCEEDED(RunTest(437)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest438(void)
{
    VERIFY_SUCCEEDED(RunTest(438)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest439(void)
{
    VERIFY_SUCCEEDED(RunTest(439)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest440(void)
{
    VERIFY_SUCCEEDED(RunTest(440)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest441(void)
{
    VERIFY_SUCCEEDED(RunTest(441)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest442(void)
{
    VERIFY_SUCCEEDED(RunTest(442)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest443(void)
{
    VERIFY_SUCCEEDED(RunTest(443)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest444(void)
{
    VERIFY_SUCCEEDED(RunTest(444)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest445(void)
{
    VERIFY_SUCCEEDED(RunTest(445)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest446(void)
{
    VERIFY_SUCCEEDED(RunTest(446)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest447(void)
{
    VERIFY_SUCCEEDED(RunTest(447)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest448(void)
{
    VERIFY_SUCCEEDED(RunTest(448)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest449(void)
{
    VERIFY_SUCCEEDED(RunTest(449)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest450(void)
{
    VERIFY_SUCCEEDED(RunTest(450)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest451(void)
{
    VERIFY_SUCCEEDED(RunTest(451)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest452(void)
{
    VERIFY_SUCCEEDED(RunTest(452)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest453(void)
{
    VERIFY_SUCCEEDED(RunTest(453)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest454(void)
{
    VERIFY_SUCCEEDED(RunTest(454)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest455(void)
{
    VERIFY_SUCCEEDED(RunTest(455)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest456(void)
{
    VERIFY_SUCCEEDED(RunTest(456)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest457(void)
{
    VERIFY_SUCCEEDED(RunTest(457)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest458(void)
{
    VERIFY_SUCCEEDED(RunTest(458)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest459(void)
{
    VERIFY_SUCCEEDED(RunTest(459)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest460(void)
{
    VERIFY_SUCCEEDED(RunTest(460)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest461(void)
{
    VERIFY_SUCCEEDED(RunTest(461)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest462(void)
{
    VERIFY_SUCCEEDED(RunTest(462)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest463(void)
{
    VERIFY_SUCCEEDED(RunTest(463)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest464(void)
{
    VERIFY_SUCCEEDED(RunTest(464)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest465(void)
{
    VERIFY_SUCCEEDED(RunTest(465)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest466(void)
{
    VERIFY_SUCCEEDED(RunTest(466)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest467(void)
{
    VERIFY_SUCCEEDED(RunTest(467)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest468(void)
{
    VERIFY_SUCCEEDED(RunTest(468)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest469(void)
{
    VERIFY_SUCCEEDED(RunTest(469)); 
}

void CMFTDiagVisValTAEF::VisValEncoderSettingsTest470(void)
{
    VERIFY_SUCCEEDED(RunTest(470)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest471(void)
{
    VERIFY_SUCCEEDED(RunTest(471)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest472(void)
{
    VERIFY_SUCCEEDED(RunTest(472)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest473(void)
{
    VERIFY_SUCCEEDED(RunTest(473)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest474(void)
{
    VERIFY_SUCCEEDED(RunTest(474)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest475(void)
{
    VERIFY_SUCCEEDED(RunTest(475)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest476(void)
{
    VERIFY_SUCCEEDED(RunTest(476)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest477(void)
{
    VERIFY_SUCCEEDED(RunTest(477)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest478(void)
{
    VERIFY_SUCCEEDED(RunTest(478)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest479(void)
{
    VERIFY_SUCCEEDED(RunTest(479)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest480(void)
{
    VERIFY_SUCCEEDED(RunTest(480)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest481(void)
{
    VERIFY_SUCCEEDED(RunTest(481)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest482(void)
{
    VERIFY_SUCCEEDED(RunTest(482)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest483(void)
{
    VERIFY_SUCCEEDED(RunTest(483)); 
}


void CMFTDiagVisValTAEF::VisValQualityTest485(void)
{
    VERIFY_SUCCEEDED(RunTest(485)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest486(void)
{
    VERIFY_SUCCEEDED(RunTest(486)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest487(void)
{
    VERIFY_SUCCEEDED(RunTest(487)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest488(void)
{
    VERIFY_SUCCEEDED(RunTest(488)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest489(void)
{
    VERIFY_SUCCEEDED(RunTest(489)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest490(void)
{
    VERIFY_SUCCEEDED(RunTest(490)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest491(void)
{
    VERIFY_SUCCEEDED(RunTest(491)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest492(void)
{
    VERIFY_SUCCEEDED(RunTest(492)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest493(void)
{
    VERIFY_SUCCEEDED(RunTest(493)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest494(void)
{
    VERIFY_SUCCEEDED(RunTest(494)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest495(void)
{
    VERIFY_SUCCEEDED(RunTest(495)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest496(void)
{
    VERIFY_SUCCEEDED(RunTest(496)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest497(void)
{
    VERIFY_SUCCEEDED(RunTest(497)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest498(void)
{
    VERIFY_SUCCEEDED(RunTest(498)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest499(void)
{
    VERIFY_SUCCEEDED(RunTest(499)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest500(void)
{
    VERIFY_SUCCEEDED(RunTest(500)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest501(void)
{
    VERIFY_SUCCEEDED(RunTest(501)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest502(void)
{
    VERIFY_SUCCEEDED(RunTest(502)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest503(void)
{
    VERIFY_SUCCEEDED(RunTest(503)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest504(void)
{
    VERIFY_SUCCEEDED(RunTest(504)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest505(void)
{
    VERIFY_SUCCEEDED(RunTest(505)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest506(void)
{
    VERIFY_SUCCEEDED(RunTest(506)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest507(void)
{
    VERIFY_SUCCEEDED(RunTest(507)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest508(void)
{
    VERIFY_SUCCEEDED(RunTest(508)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest509(void)
{
    VERIFY_SUCCEEDED(RunTest(509)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest510(void)
{
    VERIFY_SUCCEEDED(RunTest(510)); 
}


void CMFTDiagVisValTAEF::VisValQualityTest512(void)
{
    VERIFY_SUCCEEDED(RunTest(512)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest513(void)
{
    VERIFY_SUCCEEDED(RunTest(513)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest514(void)
{
    VERIFY_SUCCEEDED(RunTest(514)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest515(void)
{
    VERIFY_SUCCEEDED(RunTest(515)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest516(void)
{
    VERIFY_SUCCEEDED(RunTest(516)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest517(void)
{
    VERIFY_SUCCEEDED(RunTest(517)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest518(void)
{
    VERIFY_SUCCEEDED(RunTest(518)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest519(void)
{
    VERIFY_SUCCEEDED(RunTest(519)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest520(void)
{
    VERIFY_SUCCEEDED(RunTest(520)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest521(void)
{
    VERIFY_SUCCEEDED(RunTest(521)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest522(void)
{
    VERIFY_SUCCEEDED(RunTest(522)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest523(void)
{
    VERIFY_SUCCEEDED(RunTest(523)); 
}

void CMFTDiagVisValTAEF::VisValQualityTest524(void)
{
    VERIFY_SUCCEEDED(RunTest(524)); 
}

void CMFTDiagVisValTAEF::VisValEncoderPerformanceTest525(void)
{
    VERIFY_SUCCEEDED(RunTest(525)); 
}

void CMFTDiagVisValTAEF::VisValEncoderPerformanceTestWOA526(void)
{
    VERIFY_SUCCEEDED(RunTest(526)); 
}

void CMFTDiagVisValTAEF::VisValEncoderPerformanceTest527(void)
{
    VERIFY_SUCCEEDED(RunTest(527)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest600(void)
{
    VERIFY_SUCCEEDED(RunTest(600)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest601(void)
{
    VERIFY_SUCCEEDED(RunTest(601)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest602(void)
{
    VERIFY_SUCCEEDED(RunTest(602)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest603(void)
{
    VERIFY_SUCCEEDED(RunTest(603)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest604(void)
{
    VERIFY_SUCCEEDED(RunTest(604)); 
}

void CMFTDiagVisValTAEF::VisValEncoderDynamicSettingsTest605(void)
{
    VERIFY_SUCCEEDED(RunTest(605)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest606(void) 
{
    VERIFY_SUCCEEDED(RunTest(606)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest607(void) 
{
    VERIFY_SUCCEEDED(RunTest(607)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest608(void) 
{
    VERIFY_SUCCEEDED(RunTest(608)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest609(void) 
{
    VERIFY_SUCCEEDED(RunTest(609)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest610(void) 
{
    VERIFY_SUCCEEDED(RunTest(610)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest611(void) 
{
    VERIFY_SUCCEEDED(RunTest(611)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest612(void) 
{
    VERIFY_SUCCEEDED(RunTest(612)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest613(void) 
{
    VERIFY_SUCCEEDED(RunTest(613)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest614(void) 
{
    VERIFY_SUCCEEDED(RunTest(614)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest615(void) 
{
    VERIFY_SUCCEEDED(RunTest(615)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest616(void) 
{
    VERIFY_SUCCEEDED(RunTest(616)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest617(void) 
{
    VERIFY_SUCCEEDED(RunTest(617)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest618(void) 
{
    VERIFY_SUCCEEDED(RunTest(618)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest619(void) 
{
    VERIFY_SUCCEEDED(RunTest(619)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest620(void) 
{
    VERIFY_SUCCEEDED(RunTest(620)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest621(void) 
{
    VERIFY_SUCCEEDED(RunTest(621)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest622(void) 
{
    VERIFY_SUCCEEDED(RunTest(622)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest623(void) 
{
    VERIFY_SUCCEEDED(RunTest(623)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest624(void) 
{
    VERIFY_SUCCEEDED(RunTest(624)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest625(void) 
{
    VERIFY_SUCCEEDED(RunTest(625)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest626(void) 
{
    VERIFY_SUCCEEDED(RunTest(626)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest627(void) 
{
    VERIFY_SUCCEEDED(RunTest(627)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest628(void) 
{
    VERIFY_SUCCEEDED(RunTest(628)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest629(void) 
{
    VERIFY_SUCCEEDED(RunTest(629)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest630(void) 
{
    VERIFY_SUCCEEDED(RunTest(630)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest631(void) 
{
    VERIFY_SUCCEEDED(RunTest(631)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest632(void) 
{
    VERIFY_SUCCEEDED(RunTest(632)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest633(void) 
{
    VERIFY_SUCCEEDED(RunTest(633)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest634(void) 
{
    VERIFY_SUCCEEDED(RunTest(634)); 
}

void CMFTDiagVisValTAEF::VisValEncoderTemporalLayersTest640(void) 
{
    VERIFY_SUCCEEDED(RunTest(640)); 
}

void CMFTDiagVisValTAEF::VisValEncoderTemporalLayersTest641(void) 
{
    VERIFY_SUCCEEDED(RunTest(641)); 
}

void CMFTDiagVisValTAEF::VisValEncoderTemporalLayersTest642(void) 
{
    VERIFY_SUCCEEDED(RunTest(642)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest650(void)
{
    VERIFY_SUCCEEDED(RunTest(650)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest651(void) 
{
    VERIFY_SUCCEEDED(RunTest(651)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest652(void) 
{
    VERIFY_SUCCEEDED(RunTest(652)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest653(void) 
{
    VERIFY_SUCCEEDED(RunTest(653)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest654(void) 
{
    VERIFY_SUCCEEDED(RunTest(654)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest655(void) 
{
    VERIFY_SUCCEEDED(RunTest(655)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest656(void) 
{
    VERIFY_SUCCEEDED(RunTest(656)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRRDCurveTest657(void) 
{
    VERIFY_SUCCEEDED(RunTest(657)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRRDCurveTest658(void) 
{
    VERIFY_SUCCEEDED(RunTest(658)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRRDCurveTest659(void) 
{
    VERIFY_SUCCEEDED(RunTest(659)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest660(void) 
{
    VERIFY_SUCCEEDED(RunTest(660)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest661(void) 
{
    VERIFY_SUCCEEDED(RunTest(661)); 
}

void CMFTDiagVisValTAEF::VisValEncoderLTRTest662(void) 
{
    VERIFY_SUCCEEDED(RunTest(662)); 
}

void CMFTDiagVisValTAEF::VisValEncoderTemporalLayersTest675(void) 
{
    VERIFY_SUCCEEDED(RunTest(675)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest676(void) 
{
    VERIFY_SUCCEEDED(RunTest(676)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest677(void) 
{
    VERIFY_SUCCEEDED(RunTest(677)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest678(void) 
{
    VERIFY_SUCCEEDED(RunTest(678)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest679(void) 
{
    VERIFY_SUCCEEDED(RunTest(679)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest680(void) 
{
    VERIFY_SUCCEEDED(RunTest(680)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest681(void) 
{
    VERIFY_SUCCEEDED(RunTest(681)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest682(void) 
{
    VERIFY_SUCCEEDED(RunTest(682)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest683(void) 
{
    VERIFY_SUCCEEDED(RunTest(683)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest684(void) 
{
    VERIFY_SUCCEEDED(RunTest(684)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest685(void) 
{
    VERIFY_SUCCEEDED(RunTest(685)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest686(void) 
{
    VERIFY_SUCCEEDED(RunTest(686)); 
}

void CMFTDiagVisValTAEF::VisValEncoderTest801(void)
{
    // We are calling into SkypeLync tests
    // directly to ensure we are only running once

    DWORD dwTestID = 801;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;
    
    do
    {
        hr = VerifyH264HardwareEncoderPresent();
        if(FAILED(hr))
        {
            eCompletionType = TEST_FAILED;
            break;
        }
        else if(hr == S_FALSE)
        {
            // No HW H264 Encoder present, skip test
            Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            break;
        }

        hr = HMFTTestVideoEncodeDynamicControl(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr);
}

void CMFTDiagVisValTAEF::VisValEncoderTest802(void)
{
    // We are calling into SkypeLync tests 
    // directly to ensure we are only running once

    DWORD dwTestID = 802;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;

    do
    {
        hr = VerifyH264HardwareEncoderPresent();
        if(FAILED(hr))
        {
            eCompletionType = TEST_FAILED;
            break;
        }
        else if(hr == S_FALSE)
        {
            // No HW H264 Encoder present, skip test
            Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            break;
        }

        hr  = HMFTTestVideoEncodeLTR(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr); 
}

void CMFTDiagVisValTAEF::VisValEncoderTest803(void)
{
    // We are calling into SkypeLync tests 
    // directly to ensure we are only running once

    DWORD dwTestID = 803;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;
    
    do
    {
        // This test should run on all platforms, no need for VerifyH264HardwareEncoderPresent

        hr = HMFTTestVideoEncodeSimulcast(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr); 
}

void CMFTDiagVisValTAEF::VisValEncoderTest851(void)
{
    // We are calling into SkypeLync tests 
    // directly to ensure we are only running once

    DWORD dwTestID = 851;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;
    
    do
    {
        hr = VerifyH264HardwareEncoderPresent();
        if(FAILED(hr))
        {
            eCompletionType = TEST_FAILED;
            break;
        }
        else if(hr == S_FALSE)
        {
            // No HW H264 Encoder present, skip test
            Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            break;
        }

        hr = HMFTTestVideoCodecE2ESingleStream(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr); 
}

void CMFTDiagVisValTAEF::VisValEncoderTest852(void)
{
    // We are calling into SkypeLync tests 
    // directly to ensure we are only running once

    DWORD dwTestID = 852;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;
    
    do
    {
        hr = VerifyH264HardwareEncoderPresent();
        if(FAILED(hr))
        {
            eCompletionType = TEST_FAILED;
            break;
        }
        else if(hr == S_FALSE)
        {
            // No HW H264 Encoder present, skip test
            Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            break;
        }

        hr = HMFTTestVideoEncodeMemoryUsage(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr); 
}

void CMFTDiagVisValTAEF::VisValEncoderTest853(void)
{
    // We are calling into SkypeLync tests 
    // directly to ensure we are only running once

    DWORD dwTestID = 853;
    COMPLETION_TYPE eCompletionType = TEST_PASSES;

    stdext::hash_map<wstring, wstring> parameters;
    std::wstringstream wStringStream;

    wStringStream << dwTestID;
    parameters[L"TestNum"] = wStringStream.str();

    wStringStream.str(L"");
    wStringStream << L"VisValEncoderTest" << dwTestID;
    parameters[L"TestName"] = wStringStream.str();

    BeginTestCase(0, dwTestID);

    HRESULT hr = S_OK;
    
    do
    {
        hr = VerifyH264HardwareEncoderPresent();
        if(FAILED(hr))
        {
            eCompletionType = TEST_FAILED;
            break;
        }
        else if(hr == S_FALSE)
        {
            // No HW H264 Encoder present, skip test
            Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            break;
        }
        
        hr = HMFTTestVideoCodecE2EStability(this, parameters);

        if (SUCCEEDED(hr))
        {
            if(hr == S_FALSE)
            {
                // No HW H264 Encoder present or multiple graphic cards in the system, skip test
                Log(NORMAL, _T("INFO>> No Hardware H264 Encoder found, skipping test"));
            }
            else
            {
                Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            }
        }
        else
        {
            Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
    }while(false);

    EndTestCase(eCompletionType);

    VERIFY_SUCCEEDED(hr); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1200(void)
{
    VERIFY_SUCCEEDED(RunTest(1200));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1201(void)
{
    VERIFY_SUCCEEDED(RunTest(1201));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1202(void)
{
    VERIFY_SUCCEEDED(RunTest(1202));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1203(void)
{
    VERIFY_SUCCEEDED(RunTest(1203));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1204(void)
{
    VERIFY_SUCCEEDED(RunTest(1204));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1205(void)
{
    VERIFY_SUCCEEDED(RunTest(1205));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1206(void)
{
    VERIFY_SUCCEEDED(RunTest(1206));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1207(void)
{
    VERIFY_SUCCEEDED(RunTest(1207));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1208(void)
{
    VERIFY_SUCCEEDED(RunTest(1208));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1209(void)
{
    VERIFY_SUCCEEDED(RunTest(1209));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1210(void)
{
    VERIFY_SUCCEEDED(RunTest(1210));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1211(void)
{
    VERIFY_SUCCEEDED(RunTest(1211));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1212(void)
{
    VERIFY_SUCCEEDED(RunTest(1212));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1213(void)
{
    VERIFY_SUCCEEDED(RunTest(1213));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1214(void)
{
    VERIFY_SUCCEEDED(RunTest(1214));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1215(void)
{
    VERIFY_SUCCEEDED(RunTest(1215));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1300(void) 
{
    VERIFY_SUCCEEDED(RunTest(1300)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1301(void) 
{
    VERIFY_SUCCEEDED(RunTest(1301)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1302(void) 
{
    VERIFY_SUCCEEDED(RunTest(1302)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1303(void) 
{
    VERIFY_SUCCEEDED(RunTest(1303)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1304(void) 
{
    VERIFY_SUCCEEDED(RunTest(1304)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1305(void) 
{
    VERIFY_SUCCEEDED(RunTest(1305)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1306(void) 
{
    VERIFY_SUCCEEDED(RunTest(1306)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1307(void) 
{
    VERIFY_SUCCEEDED(RunTest(1307)); 
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1400(void)
{
    VERIFY_SUCCEEDED(RunTest(1400));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1401(void)
{
    VERIFY_SUCCEEDED(RunTest(1401));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1402(void)
{
    VERIFY_SUCCEEDED(RunTest(1402));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1403(void)
{
    VERIFY_SUCCEEDED(RunTest(1403));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1404(void)
{
    VERIFY_SUCCEEDED(RunTest(1404));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1405(void)
{
    VERIFY_SUCCEEDED(RunTest(1405));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1406(void)
{
    VERIFY_SUCCEEDED(RunTest(1406));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1407(void)
{
    VERIFY_SUCCEEDED(RunTest(1407));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1408(void)
{
    VERIFY_SUCCEEDED(RunTest(1408));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1409(void)
{
    VERIFY_SUCCEEDED(RunTest(1409));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1500(void)
{
    VERIFY_SUCCEEDED(RunTest(1500));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1501(void)
{
    VERIFY_SUCCEEDED(RunTest(1501));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1502(void)
{
    VERIFY_SUCCEEDED(RunTest(1502));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1503(void)
{
    VERIFY_SUCCEEDED(RunTest(1503));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1504(void)
{
    VERIFY_SUCCEEDED(RunTest(1504));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1505(void)
{
    VERIFY_SUCCEEDED(RunTest(1505));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1506(void)
{
    VERIFY_SUCCEEDED(RunTest(1506));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1507(void)
{
    VERIFY_SUCCEEDED(RunTest(1507));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1508(void)
{
    VERIFY_SUCCEEDED(RunTest(1508));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1509(void)
{
    VERIFY_SUCCEEDED(RunTest(1509));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1510(void)
{
    VERIFY_SUCCEEDED(RunTest(1510));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1511(void)
{
    VERIFY_SUCCEEDED(RunTest(1511));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1512(void)
{
    VERIFY_SUCCEEDED(RunTest(1512));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1600(void)
{
    VERIFY_SUCCEEDED(RunTest(1600));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1601(void)
{
    VERIFY_SUCCEEDED(RunTest(1601));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1602(void)
{
    VERIFY_SUCCEEDED(RunTest(1602));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1603(void)
{
    VERIFY_SUCCEEDED(RunTest(1603));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1604(void)
{
    VERIFY_SUCCEEDED(RunTest(1604));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1605(void)
{
    VERIFY_SUCCEEDED(RunTest(1605));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1606(void)
{
    VERIFY_SUCCEEDED(RunTest(1606));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1607(void)
{
    VERIFY_SUCCEEDED(RunTest(1607));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1608(void)
{
    VERIFY_SUCCEEDED(RunTest(1608));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1609(void)
{
    VERIFY_SUCCEEDED(RunTest(1609));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1610(void)
{
    VERIFY_SUCCEEDED(RunTest(1610));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1611(void)
{
    VERIFY_SUCCEEDED(RunTest(1611));
}

void CMFTDiagVisValTAEF::VisValEncoderWinBlueTest1612(void)
{
    VERIFY_SUCCEEDED(RunTest(1612));
}

void CMFTDiagVisValTAEF::VisValEncoderGradualIntraRefresh1800(void)
{
    VERIFY_SUCCEEDED(RunTest(1800));
}

void CMFTDiagVisValTAEF::VisValEncoderGradualIntraRefresh1801(void)
{
    VERIFY_SUCCEEDED(RunTest(1801));
}

void CMFTDiagVisValTAEF::VisValEncoderGradualIntraRefresh1802(void)
{
    VERIFY_SUCCEEDED(RunTest(1802));
}

void CMFTDiagVisValTAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}
    
HRESULT CMFTDiagVisValTAEF::RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL)
{
    HRESULT hr = S_OK;
    IMFActivate* pActivate = NULL;

    do
    {
        hr = m_pActivateHelper->InitializeActivateByGUID(pGUIDMFTUnderTest, pszHWURL);
        if(FAILED(hr))
        {
            Log(HIGH, L"ERROR>> MFActivateHelper class failed to find GUID");
            break;
        }

        hr = m_pActivateHelper->GetActivate(pGUIDMFTUnderTest, pszHWURL, &pActivate);
        if(FAILED(hr))
        {
            Log(HIGH, L"ERROR>> Failed to get IMFActivate from ActivateHelper class (hr=0x%x)", hr);
            break;
        }

        hr = RunVisValTest(dwTestID, this, m_pXMLConfig, pActivate);
    }while(false);

    if(pActivate != NULL)
    {
        pActivate->ShutdownObject();

        pActivate->Release();
    }

    return hr;
}
