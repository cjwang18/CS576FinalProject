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
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}

	for (unsigned int i=0; i<(NumFrames*Height*Width*3); i++ )
	{
		VideoData[i] = otherImage->VideoData[i];
	}

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
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	for ( int i=0; i<(Height*Width*3*NumFrames); i++ )
	{
		VideoData[i]	= otherImage.VideoData[i];
	}
	

	return *this;

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

	if (currentFrame != (this->getNumFrames()-1)){
		this->setCurrentFrame(currentFrame+1);
	} else {
		this->setCurrentFrame(0);
	}

	return false;
}