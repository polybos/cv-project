#include "myBGSubMog2.h"


myBGSubMog2::myBGSubMog2(void)
{
}


myBGSubMog2::~myBGSubMog2(void)
{
}

void myBGSubMog2::setHistorySize(int p_historySize)
{
	history = p_historySize;
}

void myBGSubMog2::setNMixtures(int p_nmixtures)
{
	nmixtures = p_nmixtures;
}
	
void myBGSubMog2::setBackgroundRatio(float p_ratio)
{
	backgroundRatio = p_ratio;
}

void myBGSubMog2::setVarThresholdGen(float p_varThreshGen)
{
	varThresholdGen = p_varThreshGen;
}
void  myBGSubMog2::setVarThreshold(float p_varThreshold)
{
	varThreshold = p_varThreshold;
}

void myBGSubMog2::setfVarInit(float p_fVarInit)
{
	fVarInit = p_fVarInit;
}

void myBGSubMog2::setfVarMin(float p_fVarMin)
{
	fVarMin = p_fVarMin;
}

void myBGSubMog2::setfVarMax(float p_fVarMax)
{
	fVarMax = p_fVarMax;
}
 
void myBGSubMog2::setfCT(float p_fCT)
{
	fCT = p_fCT;
}
 
void myBGSubMog2::setnShadowDetection(uchar p_shadows)
{
	nShadowDetection = p_shadows;
}

void myBGSubMog2::setfTau(float p_fTau)
{
	fTau = p_fTau;
}
