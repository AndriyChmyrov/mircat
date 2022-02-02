// mircat.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mircat.h"

/*	Selected laser.
*/
int laser = -1;


/*	Check for and read a scalar value.
*/
double getScalar(const mxArray* array)
{
	if (!mxIsNumeric(array) || mxGetNumberOfElements(array) != 1) mexErrMsgTxt("Not a scalar.");
	return mxGetScalar(array);
}


/*	Check for and read a matrix
*/
double* getArray(const mxArray* array, int size)
{
	if (!mxIsNumeric(array) || mxGetNumberOfElements(array) != size) mexErrMsgTxt("Supplied wrong number of elements in array.");
	return mxGetPr(array);
}


/*	Get a parameter or status value.
*/
mxArray* getParameter(const char* name)
{
	if (_stricmp("arm", name) == 0)
	{
		MEXMESSAGE(MIRcatSDK_ArmLaser());
		return mxCreateDoubleScalar(static_cast<double>(1));
	}

	if (_stricmp("disarm", name) == 0)
	{
		MEXMESSAGE(MIRcatSDK_DisarmLaser());
		return mxCreateDoubleScalar(static_cast<double>(1));
	}

	if (_stricmp("isarmed", name) == 0)
	{
		bool IsArmed = false;
		MEXMESSAGE(MIRcatSDK_IsLaserArmed(&IsArmed));
		return mxCreateDoubleScalar(static_cast<double>(IsArmed));
	}

	if (_stricmp("isconnected", name) == 0)
	{
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsConnectedToLaser(&bRes));
		return mxCreateDoubleScalar(static_cast<double>(bRes));
	}

	if (_stricmp("isinterlocked", name) == 0)
	{
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsInterlockedStatusSet(&bRes));
		return mxCreateDoubleScalar(static_cast<double>(bRes));
	}

	if (_stricmp("istuned", name) == 0)
	{
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsTuned(&bRes));
		return mxCreateDoubleScalar(static_cast<double>(bRes));
	}

	if (_stricmp("temperature", name) == 0)
	{
		uint8_t numQcls;
		MEXMESSAGE(MIRcatSDK_GetNumInstalledQcls(&numQcls));
#ifndef NDEBUG
		mexPrintf(" Number of installed QCLs: %u\n", numQcls);
#endif

		mxArray* output;
		output = mxCreateDoubleMatrix(1, static_cast<mwSize>(numQcls), mxREAL);
		double  *ptr;
		ptr = (double *)mxGetPr(output);

		for (uint8_t i = 1; i <= numQcls; i++)
		{
			float qclTemp;
			MEXMESSAGE(MIRcatSDK_GetQCLTemperature(i, &qclTemp));
			ptr[i-1] = static_cast<double>(qclTemp);
#ifndef NDEBUG
			mexPrintf(" Test Result: QCL%u Temp: %.3f C\n", i, qclTemp);
#endif
		}
		return output;
	}

	if (_stricmp("temperaturestable", name) == 0)
	{
		bool atTemp = false;
		MEXMESSAGE(MIRcatSDK_AreTECsAtSetTemperature(&atTemp));
		return mxCreateDoubleScalar(static_cast<double>(atTemp));
	}

	if (_stricmp("temperaturewait", name) == 0)
	{
		bool atTemp = false;
		MEXMESSAGE(MIRcatSDK_AreTECsAtSetTemperature(&atTemp));

		uint8_t numQcls;
		MEXMESSAGE(MIRcatSDK_GetNumInstalledQcls(&numQcls));
		mexPrintf(" Number of installed QCLs: %u\n", numQcls);
		while (!atTemp)
		{
			for (uint8_t i = 1; i <= numQcls; i++)
			{
				float qclTemp;
				MEXMESSAGE(MIRcatSDK_GetQCLTemperature(i, &qclTemp));
				mexPrintf(" Test Result: QCL%u Temp: %.3f C\n", i, qclTemp);
				uint16_t tecCur;
				MEXMESSAGE(MIRcatSDK_GetTecCurrent(i, &tecCur));
				mexPrintf(" Test Result: TEC%u Current: %u mA\n", i, tecCur);
			}

			MEXMESSAGE(MIRcatSDK_AreTECsAtSetTemperature(&atTemp));
			mexPrintf("TECs at Temperature: %d\n", atTemp);
			mexEvalString("drawnow");
			::Sleep(1000);
		}
		
		return mxCreateDoubleScalar(static_cast<double>(atTemp));
	}

	if (_stricmp("emission", name) == 0)
	{
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsEmissionOn(&bRes));
		return mxCreateDoubleScalar(static_cast<double>(bRes));
	}

	if (_stricmp("wavelength", name) == 0)
	{
		float actualWW;
		uint8_t units;
		bool lightValid;
		MEXMESSAGE(MIRcatSDK_GetActualWW(&actualWW, &units, &lightValid));

		mxArray *res;
		const char *field_names[] = { "value", "units" };
		res = mxCreateStructMatrix(1, 1, 2, field_names);
		mxSetFieldByNumber(res, 0, 0, mxCreateDoubleScalar(actualWW));

		switch (units)
		{
		case MIRcatSDK_UNITS_MICRONS:
			mxSetFieldByNumber(res, 0, 1, mxCreateString("microns"));
			break;
		case MIRcatSDK_UNITS_CM1:
			mxSetFieldByNumber(res, 0, 1, mxCreateString("cm-1"));
			break;
		}

		return res;
	}

	if (_stricmp("poweroff", name) == 0)
	{
		MEXMESSAGE(MIRcatSDK_PowerOffSystem());
		return mxCreateDoubleScalar(1);
	}

#ifndef NDEBUG
	mexPrintf("%s:%d - ", __FILE__, __LINE__);
#endif
	mexPrintf("\"%s\" unknown.\n", name);
	return NULL;
}


/*	Set a measurement parameter.
*/
void setParameter(const char* name, const mxArray* field)
{
	if (mxGetNumberOfElements(field) < 1) return;
	if (_stricmp("laser", name) == 0) return;
	if (_stricmp("lasers", name) == 0) return;
	if (laser < 1 ) mexErrMsgTxt("Invalid laser handle.");

	if (_stricmp("wavelength_mic", name) == 0)
	{
		float wavelength = static_cast<float>(getScalar(field));
		MEXMESSAGE(MIRcatSDK_TuneToWW(wavelength, MIRcatSDK_UNITS_MICRONS, 1));
		return;
	}

	if (_stricmp("wavelength_cm1", name) == 0)
	{
		float wavelength = static_cast<float>(getScalar(field));
		MEXMESSAGE(MIRcatSDK_TuneToWW(wavelength, MIRcatSDK_UNITS_CM1, 1));
		return;
	}

	if (_stricmp("emission", name) == 0)
	{
		double emission = getScalar(field);
		bool bRes = false;
		MEXMESSAGE(MIRcatSDK_IsEmissionOn(&bRes));

		if ( emission )
		{
			if (!bRes)
			{
				MEXMESSAGE(MIRcatSDK_TurnEmissionOn());
			}
		}
		else
		{
			if (bRes)
			{
				MEXMESSAGE(MIRcatSDK_TurnEmissionOff());
			}
		}
		return;
	}

	if (_stricmp("stepscan", name) == 0)
	{
		if (mxGetNumberOfElements(field) != 5)
			mexErrMsgTxt("You need to specify 5 parameters - [ScanStart, ScanStop, ScanStep, ScanUnits, ScanIterations]; ScanUnits = 1 for um and 2 for cm-1");
		double* arguments = getArray(field, 5);
		float fStart = static_cast<float>(arguments[0]);
		float fStop = static_cast<float>(arguments[1]);
		float fStepSize = static_cast<float>(arguments[2]);
		uint8_t bUnits = static_cast<uint8_t>(arguments[3]);
		uint16_t wNumScans = static_cast<uint16_t>(arguments[4]);
		MEXMESSAGE(MIRcatSDK_StartStepMeasureModeScan(fStart, fStop, fStepSize, bUnits, wNumScans));
		return;
	}

	if (_stricmp("wavelength_trigger", name) == 0)
	{
		if (mxGetNumberOfElements(field) != 5)
			mexErrMsgTxt("You need to specify 5 parameters - [PulseMode, TrigStart, TrigStop, TrigInterval, Units]");
		double* arguments = getArray(field, 5);
		uint8_t pbPulseMode    = static_cast<uint8_t>(arguments[0]);
		float pfWlTrigStart    = static_cast<float>(arguments[1]);
		float pfWlTrigStop     = static_cast<float>(arguments[2]);
		float pfWlTrigInterval = static_cast<float>(arguments[3]);
		uint8_t pbUnits        = static_cast<uint8_t>(arguments[4]);
		MEXMESSAGE(MIRcatSDK_SetWlTrigParams(pbPulseMode, pfWlTrigStart, pfWlTrigStop, pfWlTrigInterval, pbUnits));
		return;
	}

#ifndef NDEBUG
	mexPrintf("%s:%d - ", __FILE__, __LINE__);
#endif
	mexPrintf("\"%s\" unknown.\n", name);
	return;
}
