//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"


// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	VideoData = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
	CurrentFrame = 0;
	NumFrames = -1;
	for (int i=0 ; i<SAT_INTERVALS ; i++) {
		for (int j=0 ; j<HUE_INTERVALS ; j++) {
			ColorAnalysis[i][j] = 0;
		}
	}
	BlackPixelAnalysis = -1;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;

	if ( VideoData )
		delete VideoData;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	CurrentFrame = otherImage->CurrentFrame;
	NumFrames = otherImage->NumFrames;
	Data   = new char[Width*Height*3];
	VideoData = new char[Width*Height*3*NumFrames];
	strcpy(ImagePath, otherImage->ImagePath);

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}

	for (unsigned int i=0; i<(NumFrames*Height*Width*3); i++ )
	{
		VideoData[i] = otherImage->VideoData[i];
	}

	for (int i=0 ; i<SAT_INTERVALS ; i++) {
		for (int j=0 ; j<HUE_INTERVALS ; j++) {
			ColorAnalysis[i][j] = otherImage->ColorAnalysis[i][j];
		}
	}

	BlackPixelAnalysis = otherImage->BlackPixelAnalysis;

}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	NumFrames = otherImage.NumFrames;
	CurrentFrame = otherImage.CurrentFrame;
	Data   = new char[Width*Height*3];
	VideoData = new char[Width*Height*3*NumFrames];
	strcpy( ImagePath, (char *)otherImage.ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	for ( int i=0; i<(Height*Width*3*NumFrames); i++ )
	{
		VideoData[i]	= otherImage.VideoData[i];
	}

	for (int i=0 ; i<SAT_INTERVALS ; i++) {
		for (int j=0 ; j<HUE_INTERVALS ; j++) {
			ColorAnalysis[i][j] = otherImage.ColorAnalysis[i][j];
		}
	}	

	BlackPixelAnalysis = otherImage.BlackPixelAnalysis;

	return *this;

}

// = operator overload
void MyImage::CopyImage (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	CurrentFrame = otherImage.CurrentFrame;

	if (NumFrames == -1){
		Data   = new char[Width*Height*3];
		VideoData = new char[Width*Height*3*otherImage.NumFrames];
	}
	
	NumFrames = otherImage.NumFrames;

	strcpy( ImagePath, (char *)otherImage.ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	for ( int i=0; i<(Height*Width*3*NumFrames); i++ )
	{
		VideoData[i]	= otherImage.VideoData[i];
	}

	for (int i=0 ; i<SAT_INTERVALS ; i++) {
		for (int j=0 ; j<HUE_INTERVALS ; j++) {
			ColorAnalysis[i][j] = otherImage.ColorAnalysis[i][j];
		}
	}	

	BlackPixelAnalysis = otherImage.BlackPixelAnalysis;

}



// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}
	
	// Count number of frames in the file
	int c;
	unsigned long numPixels = 0;
	do {
		c = fgetc (IN_FILE);
		numPixels++;
    } while (c != EOF);

	this->setNumFrames(numPixels/(3*Height*Width));

	unsigned int pixelsPerFrame = Height*Width;

	numPixels = 0;

	// After done counting frames, start from beginning of file
	rewind(IN_FILE);

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width*this->getNumFrames()]; 
	char *Gbuf = new char[Height*Width*this->getNumFrames()]; 
	char *Bbuf = new char[Height*Width*this->getNumFrames()]; 

	for (int n = 0; n < this->getNumFrames(); ++n){
		for (i = 0; i < Width*Height; i ++)
		{
			numPixels++;
			Rbuf[n*pixelsPerFrame + i] = fgetc(IN_FILE);
		}
		for (i = 0; i < Width*Height; i ++)
		{
			Gbuf[n*pixelsPerFrame + i] = fgetc(IN_FILE);
		}
		for (i = 0; i < Width*Height; i ++)
		{
			Bbuf[n*pixelsPerFrame + i] = fgetc(IN_FILE);
		}
	}
	
	// Allocate Data structure and copy
	Data = new char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1] = Gbuf[i];
		Data[3*i+2] = Rbuf[i];
	}

	// Allocate VideoData structure and copy
	VideoData = new char[Width*Height*3*this->getNumFrames()];
	for (i = 0; i < Height*Width*this->getNumFrames(); i++)
	{
		VideoData[3*i]	= Bbuf[i];
		VideoData[3*i+1] = Gbuf[i];
		VideoData[3*i+2] = Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}



// Conversion function courtesy of
// https://github.com/ratkins/RGBConverter/blob/master/RGBConverter.cpp
void MyImage::convertRGBtoHSV(unsigned char r, unsigned char g, unsigned char b, double &h, double &s, double &v)
{
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double max = max(rd, max(gd, bd));
	double min = min(rd, min(gd, bd));
    v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max == min) { 
        h = 0; // achromatic
    } else {
        if (max == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
}



void MyImage::ColorAnalysisArraySetter(double h, double s)
{
	double hueInterval = HUE_INTERVALS;
	double satInterval = SAT_INTERVALS;
	double hueDegValue = h*360;
	int hueIndex = -1;
	int satIndex = -1;

	if (h == 0 && s == 0)
		BlackPixelAnalysis++;
	else {
		// Determine the saturation index into ColorAnalysis array
		for (int sat = 0; sat < satInterval; sat++){
			if (s == 1){
				satIndex = satInterval-1;
				break;
			}
			if (sat*(1/satInterval) <= s && s < (sat+1)*(1/satInterval)){
				satIndex = sat;
				break;
			}
		}

		for (int hue = 0 ; hue < hueInterval ; hue++)
		{
			if (hueDegValue == 360)
			{
				hueIndex = hueInterval - 1;
				break;
			}
			if (hue*(360/hueInterval) <= hueDegValue && hueDegValue < (hue+1)*(360/hueInterval))
			{
				hueIndex = hue;
				break;
			}
		}
	
		// Determine the hue index into ColorAnalysis array
		/*if ( (hueDegValue >= 0 && hueDegValue < 30) || (hueDegValue >= 330 && hueDegValue <= 360) )
			hueIndex = 0;
		else if (hueDegValue >= 30 && hueDegValue < 90)
			hueIndex = 1;
		else if (hueDegValue >= 90 && hueDegValue < 150)
			hueIndex = 2;
		else if (hueDegValue >= 150 && hueDegValue < 210)
			hueIndex = 3;
		else if (hueDegValue >= 210 && hueDegValue < 270)
			hueIndex = 4;
		else if (hueDegValue >= 270 && hueDegValue < 330)
			hueIndex = 5;*/

		// Increment value of ColorAnalysis array
		ColorAnalysis[satIndex][hueIndex]++;
	}
}



void MyImage::DoColorAnalysis()
{
	int pixelsPerFrame = Height*Width;

	for ( int frame=0 ; frame<NumFrames ; frame++)
	{
		int pixelsProcessed = 0;
		double hueSum = 0;

		for ( int row=0; row<Height; row+=SUBSAMPLE_FACTOR )
		{
			for ( int col = 0; col < Width; col+=SUBSAMPLE_FACTOR )
			{
				double h, s, v;

				unsigned char b = (unsigned char)VideoData[3*(frame*pixelsPerFrame+row*Width+col)]; // BLUE
				unsigned char g = (unsigned char)VideoData[3*(frame*pixelsPerFrame+row*Width+col)+1]; // GREEN
				unsigned char r = (unsigned char)VideoData[3*(frame*pixelsPerFrame+row*Width+col)+2]; // RED

				convertRGBtoHSV(r, g, b, h, s, v);
				ColorAnalysisArraySetter(h, s);
				hueSum += h;
				pixelsProcessed++;
			}
		}

		// TODO: Calculate average hue for each frame
		AvgHuePerFrame.push_back(hueSum / (double)pixelsProcessed);
	}
}



void MyImage::Analyze()
{
	DoColorAnalysis();
}



// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify()
{
	int currentFrame = this->getCurrentFrame();
	int pixelsPerFrame = Height*Width;
	
	for ( int i=0; i<Width*Height; i++ )
	{
		Data[3*i] = VideoData[3*currentFrame*pixelsPerFrame+3*i];
		Data[3*i+1] = VideoData[3*currentFrame*pixelsPerFrame+3*i+1];
		Data[3*i+2] = VideoData[3*currentFrame*pixelsPerFrame+3*i+2];
	}

	//if (currentFrame != (this->getNumFrames()-1)){
	//	this->setCurrentFrame(currentFrame+1);
	//} else {
	//	this->setCurrentFrame(0);
	//}

	return false;
}