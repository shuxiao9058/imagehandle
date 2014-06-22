#ifndef _ImageIO_h
#define _ImageIO_h

#ifdef _OPENCV
#include "opencv/cv.h"
#include "opencv/highgui.h"

using namespace cv;
#else
#include <setjmp.h>
#include "jpeg/jerror.h"
#include "jpeg/jpeglib.h"
#include "png/png.h"
#endif

#include "png/png.h"
#include "jpeg/jpeglib.h"
#include "EasyBMP/EasyBMP.h"

class ImageIO
{
public:
	enum ImageType{standard, derivative, normalized};
	ImageIO(void);
	~ImageIO(void);
public:
	template <class T>
	static bool loadImage(const char* filename,T*& pImagePlane,int& width,int& height, int& nchannels);
	template <class T>
	static bool saveImage(const char* filename,const T* pImagePlane,int width,int height, int nchannels,ImageType imtype = standard);
private:
	// template <class T>
	static bool loadjpg(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels);
	// template <class T>
	static bool loadpng(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels);
	// template <class T>
	static bool loadbmp(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels);
	
	// template <class T>
	static bool savejpg(const char* filename,const unsigned char *pData,int width,int height, int nchannels);
	// template <class T>
	static bool savebmp(const char* filename,const unsigned char *pData,int width,int height, int nchannels);
};

// template <class T>
bool ImageIO::loadjpg(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	width = cinfo.output_width;
	height = cinfo.output_height;
	nchannels = cinfo.output_components;
	pData = new unsigned char[width * height * nchannels];
	unsigned char *p = pData;

	while (cinfo.output_scanline < cinfo.output_height) {

		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		for (int i = 0; i < width; ++i)
		{
			switch(nchannels) {
			case 3:
				*p++ = *((JSAMPLE *)buffer[0] + (i * nchannels + 2));
				*p++ = *((JSAMPLE *)buffer[0] + (i * nchannels + 1));
				*p++ = *((JSAMPLE *)buffer[0] + (i * nchannels + 0));
				break;
			case 1:
				*p++ = *((JSAMPLE *)buffer[0] + i);
				break;
			default:
				break;
			}
		}

	}
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return true;
}

// template <class T>
bool ImageIO::loadpng(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels)
{
	FILE *infile;
	if ((infile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return false;
	}
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;

	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, infile);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	nchannels = 3; // = png_get_channels(png_ptr, info_ptr); 
	int png_chn = png_get_channels(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	pData = new unsigned char[width * height * nchannels];
	unsigned char *p = pData;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < png_chn * width; j += png_chn)
		{
			*p++ = row_pointers[i][j + 2]; // B
			*p++ = row_pointers[i][j + 1]; // G
			*p++ = row_pointers[i][j];  // R
			// 			if (4 == nchannels)
			// 				*p++ = row_pointers[i][j + 3]; // A
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(infile);
	return true;
}

// template <class T>
bool ImageIO::loadbmp(const char* filename,unsigned char*& pData,int& width,int& height, int& nchannels)
{
	BMP inBmp;
	if (!inBmp.ReadFromFile(filename))
		return false;
	width = inBmp.TellWidth();
	height = inBmp.TellHeight();
	nchannels = inBmp.TellBitDepth()/8;
	pData = new unsigned char[width * height * nchannels];
	unsigned char *p = pData;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			RGBApixel pix = inBmp.GetPixel(i, j);
			if (1 == nchannels)
			{
				*p++ = pix.Blue;
			}
			else {
				*p++ = pix.Blue;
				*p++ = pix.Green;
				*p++ = pix.Red;
			}
		}
	}

	return true;
}

// template <class T>
bool ImageIO::savejpg(const char* filename,const unsigned char* pData,int width,int height, int nchannels)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE * outfile;		
	JSAMPROW row_pointer[1];	
	int row_stride;		

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return false;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = nchannels;
	JSAMPLE *pBits = new JSAMPLE[width * height * nchannels];
	JSAMPLE *p = pBits;
	switch (nchannels) {
	case 1:
		cinfo.in_color_space = JCS_GRAYSCALE;
		for (int i = 0; i < height * width; ++i)
		{
			*p++ = pData[i];
		}
		break;
	case 3:
		cinfo.in_color_space = JCS_RGB;
		for (int i = 0; i < height * width * nchannels; i += nchannels)
		{

			*p++ = pData[i+2];
			*p++ = pData[i+1];
			*p++ = pData[i];
		}
		break;
	default:
		fprintf(stderr, "unsupport channel: %d!\n", nchannels);
		return false;
	}

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 90, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = width * nchannels;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = (JSAMPROW)& pBits[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);

	if (pBits)
		delete[] pBits;

	return true;
}

// template <class T>
bool ImageIO::savebmp(const char* filename,const unsigned char *pData,int width,int height, int nchannels)
{
	BMP outBmp;
	outBmp.SetBitDepth(nchannels * 8);
	outBmp.SetSize(width, height);
	RGBApixel pix;
	unsigned char *p = (unsigned char *)pData;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			if (1 == nchannels)
			{
				pix.Blue = *p++;
				pix.Green = pix.Blue;
				pix.Red = pix.Blue;
				pix.Alpha = 0;
			}
			else {
				pix.Blue = *p++;
				pix.Green = *p++;
				pix.Red = *p++;
				pix.Alpha = 0;
			}
			outBmp.SetPixel(i, j, pix);
		}
	}
	if (1 == nchannels)
		CreateGrayscaleColorTable(outBmp);

	if (outBmp.WriteToFile(filename))
		return true;
	 
	return false;
}

template <class T>
bool ImageIO::loadImage(const char *filename, T *&pImagePlane, int &width, int &height, int &nchannels)
{
#ifdef _OPENCV
	Mat im = imread(filename);
	if(im.data == NULL) // if allocation fails
		return false;
	if(im.type()!=CV_8UC1 && im.type()!=CV_8UC3 && im.type()!=CV_8UC4) // we only support three types of image information for now
		return false;
	width = im.size().width;
	height = im.size().height;
	nchannels = im.channels();
	pImagePlane = new T[width*height*nchannels];

	if(typeid(T) == typeid(unsigned char))
	{
		for(int i = 0;i<height;i++)
			memcpy(pImagePlane+i*im.step,im.data+i*im.step,width*nchannels);
		return true;
	}

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	for(int i =0;i<height;i++)
	{
		int offset1 = i*width*nchannels;
		int offset2 = i*im.step;
		for(int j=0;j<im.step;j++)
		{
			if(IsFloat)
				pImagePlane[offset1+j] = (T)im.data[offset2+j]/255;
			else
				pImagePlane[offset1+j] = im.data[offset2+j];
		}
	}
	return true;
#else // impl with jpeg library
	FILE *fp = NULL;
	unsigned char *pData;
	bool ret = false;

	if ((fp = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return false;
	}
	int flags;
	fread(&flags, 4, 1, fp);
	// std::cout << std::hex << flags << std::endl;
	if (0xffd8ff == (flags & 0xffffff)) // jpg
	{
		ret = loadjpg(filename, pData, width, height, nchannels);
	}
	else if (0x4D42 == (flags & 0xffff)) // bmp
	{
		ret = loadbmp(filename, pData, width, height, nchannels);
	}
	else if(flags == 0x474e5089) // png
	{
		ret = loadpng(filename, pData, width, height, nchannels);
	}
	else
	{
		fprintf(stderr, "unsupport file format, flags = 0x%x,only support with bmp/jpg/png image files.", flags);
		ret = false;
	}

	if (!ret)
	{
		if(pData)
			delete[] pData;
		return false;
	}
	pImagePlane = new T[width * height * nchannels];
	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	unsigned char *p1 = pData;
	T *p2 = pImagePlane;
	
	for (int i = 0; i < height * width * nchannels; i++)
	{
		if (IsFloat)
			*p2++ = *p1++/255.0;
		else
			*p2++ = *p1++;
	}
	delete[] pData;

	return true;
#endif
}

template <class T>
bool ImageIO::saveImage(const char* filename,const T* pImagePlane,int width,int height, int nchannels,ImageType imtype)
{

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	T Max,Min;
	int nElements = width*height*nchannels;
	switch(imtype){
	case standard:
		break;
	case derivative:
		// find the max of the absolute value
		Max = pImagePlane[0];
		if(!IsFloat)
			for(int i = 0;i<nElements;i++)
				Max = __max(Max,abs(pImagePlane[i]));
		else
			for(int i=0;i<nElements;i++)
				Max = __max(Max,fabs((double)pImagePlane[i]));
		Max*=2;
		break;
	case normalized:
		Max = Min = pImagePlane[0];
		for(int i = 0;i<nElements;i++)
		{
			Max = __max(Max,pImagePlane[i]);
			Min = __min(Min,pImagePlane[i]);
		}
		break;
	}
#ifdef _OPENCV
	Mat im;
	switch(nchannels){
	case 1:
		im.create(height,width,CV_8UC1);
		break;
	case 3:
		im.create(height,width,CV_8UC3);
		break;
	default:
		return -1;
	}
	if(typeid(T) == typeid(unsigned char) && imtype == standard)
	{
		for(int i = 0;i<height;i++)
			memcpy(im.data+i*im.step,pImagePlane+i*im.step,width*nchannels);
	}
	else
	{
		for(int i =0;i<height;i++)
		{
			int offset1 = i*width*nchannels;
			int offset2 = i*im.step;
			for(int j=0;j<im.step;j++)
			{
				switch(imtype){
				case standard:
					if(IsFloat)
						im.data[offset2+j] = pImagePlane[offset1+j]*255;
					else
						im.data[offset2+j] = pImagePlane[offset1+j];
					break;
				case derivative:
					if(IsFloat)
						im.data[offset2+j] = (double)(pImagePlane[offset1+j]/Max+0.5)*255;
					else
						im.data[offset2+j] = ((double)pImagePlane[offset1+j]/Max+0.5)*255;
					break;
				case normalized:
					im.data[offset2+j] = (double)(pImagePlane[offset1+j]-Min)/(Max-Min)*255;
					break;
				}
			}
		}
	}
	return imwrite(filename,im);
#else
	char fn[_MAX_PATH];
	strcpy(fn, filename);
	char *p = strrchr(fn, '.');
	char ext[_MAX_EXT];
	strcpy(ext, p);

	unsigned char *pData = new unsigned char[width * height * nchannels];
	bool ret = false;
	unsigned char *p1 = pData;
	T *p2 = (T *)pImagePlane;

	for(int i = 0;i < height * width * nchannels; i++)
	{
		switch(imtype){
		case standard:
			if(IsFloat)
				*p1++ = *p2++*255;
			else
				*p1++ = *p2++;
			break;
		case derivative:
				*p1++ = (double(*p2++)/Max+0.5)*255;
			break;
		case normalized:
			*p1++ = (double)(*p2++ - Min)/(Max-Min)*255;
			break;
		}
	}

	if (0 == strcmp(strupr(ext), ".JPG"))
	{
		ret = savejpg(filename, pData, width, height, nchannels);
	}
	else if (0 == strcmp(strupr(ext), ".BMP"))
	{
		ret = savebmp(filename, pData, width, height, nchannels);
	}
	else {
		fprintf(stderr, "save image: only support with jpg/bmp files.");
		ret = false;
	}
	if (pData)
		delete[] pData;

	return ret;
#endif
}

/*
#include <QVector>
#include <QImage>
#include <QString>
#include "math.h"
//-----------------------------------------------------------------------------------------
// this class is a wrapper to use QImage to load image into image planes
//-----------------------------------------------------------------------------------------

class ImageIO
{
public:
	enum ImageType{standard, derivative, normalized};
	ImageIO(void);
	~ImageIO(void);
public:
	template <class T>
	static void loadImage(const QImage& image,T*& pImagePlane,int& width,int& height,int& nchannels);
	template <class T>
	static bool loadImage(const QString& filename,T*& pImagePlane,int& width,int& height,int& nchannels);

	template <class T>
	static unsigned char convertPixel(const T& value,bool IsFloat,ImageType type,T& _Max,T& _Min);

	template <class T>
	static bool writeImage(const QString& filename, const T*& pImagePlane,int width,int height,int nchannels,ImageType type=standard,int quality=-1);

	template <class T>
	static bool writeImage(const QString& filename,const T* pImagePlane,int width,int height,int nchannels,T min, T max,int quality=-1);

};

template <class T>
void ImageIO::loadImage(const QImage& image, T*& pImagePlane,int& width,int& height,int& nchannels)
{
	// get the image information
	width=image.width();
	height=image.height();
	nchannels=3;
	pImagePlane=new T[width*height*nchannels];

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	const unsigned char* plinebuffer;
	for(int i=0;i<height;i++)
	{
		plinebuffer=image.scanLine(i);
		for(int j=0;j<width;j++)
		{
			if(IsFloat)
			{
				pImagePlane[(i*width+j)*3]=(T)plinebuffer[j*4]/255;
				pImagePlane[(i*width+j)*3+1]=(T)plinebuffer[j*4+1]/255;
				pImagePlane[(i*width+j)*3+2]=(T)plinebuffer[j*4+2]/255;
			}
			else
			{
				pImagePlane[(i*width+j)*3]=plinebuffer[j*4];
				pImagePlane[(i*width+j)*3+1]=plinebuffer[j*4+1];
				pImagePlane[(i*width+j)*3+2]=plinebuffer[j*4+2];
			}
		}
	}
}

template <class T>
bool ImageIO::loadImage(const QString&filename, T*& pImagePlane,int& width,int& height,int& nchannels)
{
	QImage image;
	if(image.load(filename)==false)
		return false;
	if(image.format()!=QImage::Format_RGB32)
	{
		QImage temp=image.convertToFormat(QImage::Format_RGB32);
		image=temp;
	}
	loadImage(image,pImagePlane,width,height,nchannels);
	return true;
}

template <class T>
bool ImageIO::writeImage(const QString& filename, const T*& pImagePlane,int width,int height,int nchannels,ImageType type,int quality)
{
	int nPixels=width*height,nElements;
	nElements=nPixels*nchannels;
	unsigned char* pTempBuffer;
	pTempBuffer=new unsigned char[nPixels*4];
	memset(pTempBuffer,0,nPixels*4);

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	T _Max=0,_Min=0;
	switch(type){
		case standard:
			break;
		case derivative:
			_Max=0;
			for(int i=0;i<nPixels;i++)
			{
				if(IsFloat)
					_Max=__max(_Max,fabs((double)pImagePlane[i]));
				else
					_Max=__max(_Max,abs(pImagePlane[i]));
			}
			break;
		case normalized:
			_Min=_Max=pImagePlane[0];
			for(int i=1;i<nElements;i++)
			{
				_Min=__min(_Min,pImagePlane[i]);
				_Max=__max(_Max,pImagePlane[i]);
			}
			break;
	}

	for(int i=0;i<nPixels;i++)
	{
		if(nchannels>=3)
		{
			pTempBuffer[i*4]=convertPixel(pImagePlane[i*nchannels],IsFloat,type,_Max,_Min);
			pTempBuffer[i*4+1]=convertPixel(pImagePlane[i*nchannels+1],IsFloat,type,_Max,_Min);
			pTempBuffer[i*4+2]=convertPixel(pImagePlane[i*nchannels+2],IsFloat,type,_Max,_Min);
		}
		else 
			for (int j=0;j<3;j++)
				pTempBuffer[i*4+j]=convertPixel(pImagePlane[i*nchannels],IsFloat,type,_Max,_Min);
		pTempBuffer[i*4+3]=255;
	}
	QImage *pQImage=new QImage(pTempBuffer,width,height,QImage::Format_RGB32);
	bool result= pQImage->save(filename,0,quality);
	delete pQImage;
	delete pTempBuffer;
	return result;
}

template <class T>
bool ImageIO::writeImage(const QString& filename, const T* pImagePlane,int width,int height,int nchannels,T min,T max,int quality)
{
	int nPixels=width*height,nElements;
	nElements=nPixels*nchannels;
	unsigned char* pTempBuffer;
	pTempBuffer=new unsigned char[nPixels*4];
	memset(pTempBuffer,0,nPixels*4);

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	T _Max=max,_Min=min;

	for(int i=0;i<nPixels;i++)
	{
		if(nchannels>=3)
		{
			pTempBuffer[i*4]=convertPixel(pImagePlane[i*nchannels],IsFloat,normalized,_Max,_Min);
			pTempBuffer[i*4+1]=convertPixel(pImagePlane[i*nchannels+1],IsFloat,normalized,_Max,_Min);
			pTempBuffer[i*4+2]=convertPixel(pImagePlane[i*nchannels+2],IsFloat,normalized,_Max,_Min);
		}
		else 
			for (int j=0;j<3;j++)
				pTempBuffer[i*4+j]=convertPixel(pImagePlane[i*nchannels],IsFloat,normalized,_Max,_Min);
		pTempBuffer[i*4+3]=255;
	}
	QImage *pQImage=new QImage(pTempBuffer,width,height,QImage::Format_RGB32);
	bool result= pQImage->save(filename,0,quality);
	delete pQImage;
	delete pTempBuffer;
	return result;
}

template <class T>
unsigned char ImageIO::convertPixel(const T& value,bool IsFloat,ImageType type,T& _Max,T& _Min)
{
	switch(type){
		case standard:
			if(IsFloat)
				return __max(__min(value*255,255),0);
			else
				return __max(__min(value,255),0);
			break;
		case derivative:
			return (double)((double)value/_Max+1)/2*255;
			break;
		case normalized:
			return (double)(value-_Min)/(_Max-_Min)*255;
			break;
	}
	return 0;
}
//*/
#endif