//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************

#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY

#define HUE_INTERVALS 12
#define SAT_INTERVALS 4
#define SUBSAMPLE_FACTOR 4

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>


// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage 
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	unsigned long	NumFrames;		// Number of frames
	unsigned long	CurrentFrame;	// Tracks which frame we're on
	char	ImagePath[_MAX_PATH];	// Image location
	char*	Data;					// RGB data of the image
	char*	VideoData;				// Holds all frames
	int		ColorAnalysis[SAT_INTERVALS][HUE_INTERVALS];	// 4 Saturation intervals, 6 Hue intervals
	int		BlackPixelAnalysis;		// Counts the number of black pixels (h=0 and s=0)
	std::vector<double>	AvgHuePerFrame;

public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage( MyImage *otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage & operator= (const MyImage & otherImage);

	// Reader & Writer functions
	void	setWidth(const int w)  { Width = w; }; 
	void	setHeight(const int h) { Height = h; }; 
	void	setNumFrames(const unsigned long nf) { NumFrames = nf; };
	void	setCurrentFrame(const unsigned long cf) { CurrentFrame = cf; };
	void	setImageData( const char *img ) { Data = (char *)img; };
	void	setImagePath( const char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };

	int		getColorAnalysisVal(int sat, int hue) { return ColorAnalysis[sat][hue]; };
	int		getBlackPixelAnalysis() { return BlackPixelAnalysis; };
	std::vector<double> getAvgHuePerFrame() { return AvgHuePerFrame; };

	unsigned long getNumFrames() { return NumFrames; };
	unsigned long getCurrentFrame() { return CurrentFrame; };
	char*	getImageData() { return Data; };
	char*	getVideoData() { return VideoData; };
	char*	getImagePath() { return ImagePath; }

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	void	Analyze();
	bool	Modify();
	void	convertRGBtoHSV(unsigned char r, unsigned char g, unsigned char b, double &h, double &s, double &v);
	void	ColorAnalysisArraySetter(double h, double s);
	void	DoColorAnalysis();

};

#endif //IMAGE_DISPLAY
