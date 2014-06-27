#ifndef MYBGSUBMOG_H
#define MYBGSUBMOG_H

#include<opencv2\core\core.hpp>
#include <opencv2\video\background_segm.hpp>

class myBGSubMog2 :
	public cv::BackgroundSubtractorMOG2
{
public:
	myBGSubMog2(void);

	void setHistorySize(int p_historySize);

	/// @brief Maximum allowed number of mixture components. 
	// Actual number is determined dynamically per pixel.
	void setNMixtures(int p_nmixtures);
	/// @details Threshold defining whether the component is significant enough to be included
	/// into the background model ( corresponds to TB=1-cf from the paper??which paper??). 
	/// cf=0.1 => TB=0.9 is default. For alpha=0.001, it means that the mode should exist for approximately 
	// 105 frames before it is considered foreground.
	void setBackgroundRatio(float p_ratio);
	/// @details  
	void setVarThresholdGen(float p_varThreshGen);

	void setVarThreshold(float p_varThreshold);
	/// @details
	void setfVarInit(float p_fVarInit);
	/// @details 
	void setfVarMin(float p_fVarMin);
	/// @details
	void setfVarMax(float p_fVarMax);
	/// @details 
	void setfCT(float p_fCT);
	/// @details 
	void setnShadowDetection(uchar p_shadows);
	/// @details 
	void setfTau(float p_fTau);


	virtual ~myBGSubMog2(void);
};

#endif

