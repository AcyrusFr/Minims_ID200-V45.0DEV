#pragma once
#include "camera.h"

#define GAIN_MIN			0	//dB
#define GAIN_MAX			18	//dB
#define EXPO_MIN			3		// µs
#define EXPO_MAX			3300	// µs
#define CHARFINSPYDER "\r"
#define TRAMEFINDALSA	">"

class CDalsaSpyder3CL :
	public CCamera
{
public:
	CDalsaSpyder3CL(bool b_att);
	~CDalsaSpyder3CL(void);

	bool SetGain(int gain);
	bool SetGainPourcent(int gain);

	bool SetTrigger(int mode);
	bool SetExposition(int expo);
	bool SetExpoMicroSec(int expoms);

	bool Save_CamConfig(void);
	bool Set_PartialArea(int startline, int endline);
	void Open_testFrame();

	bool SetFormat(int format);
	void AfficheRep(char CharRecv);
};
