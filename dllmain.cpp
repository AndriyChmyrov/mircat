// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "mircat.h"

#define VALUE	plhs[0]


/*	Driver mutually exclusive access lock (SEMAPHORE).

The semaphore is granted by the mexEnter() function. The owner of the
semaphore must release it with mexLeave().
*/
static HANDLE driver;


/*	Handle of the DLL module itself.
*/
static HANDLE self;


/*	Library initialization and termination.

This function creates a semaphore when a process is attaching to the
library.
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	UNREFERENCED_PARAMETER(ul_reason_for_call);
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		self = hModule;
		driver = CreateSemaphore(NULL, 1, 1, NULL);
		return driver != NULL;
	case DLL_THREAD_ATTACH:
		self = hModule;
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		// next line is actually excessive, because mexCleanup is registered in mexStartup
		//mexCleanup();
		break;
	}
	return TRUE;
}


/*	Request exclusive access.

This function uses a semaphore for granting mutually exclusive access to a
code section between mexEnter()/mexLeave(). If the semaphore is locked, the
function waits up to 10ms before giving up.

The semaphore is created once upon initialization of the library and persists
until the library is unloaded. Code that may raise an exception or lead to a
MEX abort on error must not execute within a mexEnter()/mexLeave section,
otherwise the access may lock permanently.
*/
#ifndef NDEBUG
void mexEnter(const char* file, const int line)
#else
void mexEnter(void)
#endif
{
	switch (WaitForSingleObject(driver, 10))
	{
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:					// access granted
		return;
	default:
#ifndef NDEBUG
		mexPrintf("%s:%d - ", file, line);
#endif
		mexErrMsgTxt("Locked.");
	}
}


/*	Release exclusive access.
*/
void mexLeave(void)
{
	ReleaseSemaphore(driver, 1, NULL);
}


static const unsigned nErrorCodes = 41;
/*	Error codes.
*/
static const unsigned codes[nErrorCodes] = 
{
	1,
	32,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102
};

/*	Error descriptions.
*/
static const char* messages[nErrorCodes+1] =
{
	"MIRcatSDK_RET_UNSUPPORTED_TRANSPORT",
	"MIRcatSDK_RET_INITIALIZATION_FAILURE",
	"MIRcatSDK_RET_ARMDISARM_FAILURE",
	"MIRcatSDK_RET_STARTTUNE_FAILURE",
	"MIRcatSDK_RET_INTERLOCKS_KEYSWITCH_NOTSET",
	"MIRcatSDK_RET_STOP_SCAN_FAILURE",
	"MIRcatSDK_RET_PAUSE_SCAN_FAILURE",
	"MIRcatSDK_RET_RESUME_SCAN_FAILURE",
	"MIRcatSDK_RET_MANUAL_STEP_SCAN_FAILURE",
	"MIRcatSDK_RET_START_SWEEPSCAN_FAILURE",
	"MIRcatSDK_RET_START_STEPMEASURESCAN_FAILURE",
	"MIRcatSDK_RET_INDEX_OUTOFBOUNDS",
	"MIRcatSDK_RET_START_MULTISPECTRALSCAN_FAILURE",
	"MIRcatSDK_RET_TOO_MANY_ELEMENTS",
	"MIRcatSDK_RET_NOT_ENOUGH_ELEMENTS",
	"MIRcatSDK_RET_BUFFER_TOO_SMALL",
	"MIRcatSDK_RET_FAVORITE_NAME_NOTRECOGNIZED",
	"MIRcatSDK_RET_FAVORITE_RECALL_FAILURE",
	"MIRcatSDK_RET_WW_OUTOFTUNINGRANGE",
	"MIRcatSDK_RET_NO_SCAN_INPROGRESS",
	"MIRcatSDK_RET_EMISSION_ON_FAILURE",
	"MIRcatSDK_RET_EMISSION_ALREADY_OFF",
	"MIRcatSDK_RET_EMISSION_OFF_FAILURE",
	"MIRcatSDK_RET_EMISSION_ALREADY_ON",
	"MIRcatSDK_RET_PULSERATE_OUTOFRANGE",
	"MIRcatSDK_RET_PULSEWIDTH_OUTOFRANGE",
	"MIRcatSDK_RET_CURRENT_OUTOFRANGE",
	"MIRcatSDK_RET_SAVE_SETTINGS_FAILURE",
	"MIRcatSDK_RET_QCL_NUM_OUTOFRANGE",
	"MIRcatSDK_RET_LASER_ALREADY_ARMED",
	"MIRcatSDK_RET_LASER_ALREADY_DISARMED",
	"MIRcatSDK_RET_LASER_NOT_ARMED",
	"MIRcatSDK_RET_LASER_NOT_TUNED",
	"MIRcatSDK_RET_TECS_NOT_AT_SET_TEMPERATURE",
	"MIRcatSDK_RET_CW_NOT_ALLOWED_ON_QCL",
	"MIRcatSDK_RET_INVALID_LASER_MODE",
	"MIRcatSDK_RET_TEMPERATURE_OUT_OF_RANGE",
	"MIRcatSDK_RET_LASER_POWER_OFF_ERROR",
	"MIRcatSDK_RET_COMM_ERROR",
	"MIRcatSDK_RET_NOT_INITIALIZED",
	"MIRcatSDK_RET_ALREADY_CREATED",
	"Unknown error" 
};


/*	Check for an error and print the message.

This function is save within a mexEnter()/mexLeave section.
*/
#ifndef NDEBUG
void mexMessage(const char* file, const int line, uint32_t error)
#else
void mexMessage(unsigned error)
#endif
{
	if (error != MIRcatSDK_RET_SUCCESS)
	{
		int n = 0;
		while (n < nErrorCodes)
		{
			if (codes[n] == error) break;
			++n;
		}
		MEXLEAVE;
#ifndef NDEBUG
		mexPrintf("%s:%d ", file, line);
#endif
		char  errbuffer[StringLength];
		sprintf_s(errbuffer, "Error code %d - %s", error, messages[n]);
		mexErrMsgTxt(errbuffer);
	}
}


/*	Free the laser and the driver.
*/
void mexCleanup(void)
{
	if ( laser )
	{
		MEXENTER;
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsEmissionOn(&bRes));
		if (bRes)
		{
			MEXMESSAGE(MIRcatSDK_TurnEmissionOff());
		}
		bool IsArmed = false;
		MEXMESSAGE(MIRcatSDK_IsLaserArmed(&IsArmed));
		if (IsArmed)
		{
			MEXMESSAGE(MIRcatSDK_DisarmLaser());
		}
		MEXMESSAGE(MIRcatSDK_DeInitialize());
		MEXLEAVE;
#ifndef NDEBUG
		mexPrintf("MIRcat SDK deinitialized!\n");
#endif
	}


}


/*	Initialize MIRcat driver and get lasers.
*/
int mexStartup(void)
{
	mexAtExit(mexCleanup);

	uint16_t major, minor, patch;
	uint32_t ret;
	MEXMESSAGE(MIRcatSDK_GetAPIVersion(&major, &minor, &patch));
#ifndef NDEBUG
	mexPrintf(" MIRcatSDK_GetAPIVersion: major = %d, minor = %d, patch = %d\n", major, minor, patch);
#endif

	ret = MIRcatSDK_Initialize();
	if (MIRcatSDK_RET_SUCCESS == ret)
	{
		bool bConnected = false;
		MEXMESSAGE(MIRcatSDK_IsConnectedToLaser(&bConnected));
		if (bConnected)
		{
			laser = 1;
		}
		else
		{
			mexPrintf("MIRcat SDK is initialized, but laser connection is not estableshed!\n");
		}
	}
	else
	{
		int n = 0;
		while (n < nErrorCodes)
		{
			if (codes[n] == ret) break;
			++n;
		}
		mexPrintf("An error occurred during initialization of the laser!\nError code:%d - %s\n", ret, messages[n]);
		return 0;
	}

	bool bIlockSet = false;
	MEXMESSAGE(MIRcatSDK_IsInterlockedStatusSet(&bIlockSet));
	bool bKeySwitchSet = false;
	MEXMESSAGE(MIRcatSDK_IsKeySwitchStatusSet(&bKeySwitchSet));
#ifndef NDEBUG
	mexPrintf("InterLocked Status: %d\n", bIlockSet);
	mexPrintf("Key Switch Status: %d\n", bKeySwitchSet);
#endif

	return static_cast<int>(laser);
}


extern "C" void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs == 0 && nlhs == 0)
	{
		mexPrintf("\nDaylights Solutions MIRcat laser interface.\n\n\tAndriy Chmyrov © 17.08.2016\n\n");
		return;
	}

	if (driver == NULL) mexErrMsgTxt("Semaphore not initialized.");

	if (laser == -1)
	{
		if (mexStartup() == 0)
		{
			VALUE = mxCreateDoubleScalar(0); 
			return;
		}
		laser = 1;
	}

	int n = 0;
	while (n < nrhs)
	{
		SHORT index;
		int field;
		switch (mxGetClassID(prhs[n]))
		{
		default:
			mexErrMsgTxt("Parameter name expected as string.");
		case mxCHAR_CLASS:
		{
			char read[StringLength];
			if (mxGetString(prhs[n], read, StringLength)) mexErrMsgTxt("Unknown parameter.");
			if (++n < nrhs)
			{
				setParameter(read, prhs[n]);
				break;
			}
			if (nlhs > 1) mexErrMsgTxt("Too many output arguments.");
			VALUE = getParameter(read);
			return;
		}
		case mxSTRUCT_CLASS:
			for (index = 0; index < static_cast<int>(mxGetNumberOfElements(prhs[n])); index++)
				for (field = 0; field < mxGetNumberOfFields(prhs[n]); field++)
					setParameter(mxGetFieldNameByNumber(prhs[n], field), mxGetFieldByNumber(prhs[n], index, field));
			;
		}
		n++;
	}
	switch (nlhs)
	{
	default:
		mexErrMsgTxt("Too many output arguments.");
	case 1:
		VALUE = mxCreateDoubleScalar(static_cast<double>(laser));
	case 0:;
	}

}