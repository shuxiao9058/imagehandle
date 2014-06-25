#pragma once
#include "Image.h"

template <class ImageT> // ImageT represent the image type
class GaussianPyramid
{
private:
	ImageT* ImPyramid;
	int m_nLevels;
	int m_minWidth;
public:
	GaussianPyramid(void);
	~GaussianPyramid(void);

	void setLevels( int nLevels = 4);
	void ConstructPyramid(const ImageT& image,double ratio=0.8,int minWidth=30);
	void displayTop(const char* filename);
	inline int nlevels() const {return m_nLevels;};
	inline ImageT& Image(int index) {return ImPyramid[index];};
};


template <class ImageT>
GaussianPyramid<ImageT>::GaussianPyramid(void):m_nLevels(-1),
	m_minWidth(-1)
{
	ImPyramid=NULL;
}

template <class ImageT>
GaussianPyramid<ImageT>::~GaussianPyramid(void)
{
	if(ImPyramid!=NULL)
		delete []ImPyramid;
}

//---------------------------------------------------------------------------------------
// function to construct the pyramid
// this is the slow way
//---------------------------------------------------------------------------------------
/* template <class ImageT>
void GaussianPyramid<ImageT>::ConstructPyramid(const ImageT &image, double ratio, int minWidth)
{
	// the ratio cannot be arbitrary numbers
	if(ratio>0.98 || ratio<0.4)
		ratio=0.75;
	// first decide how many levels
	if (-1 == m_nLevels) {
		// first decide how many levels
		m_nLevels=log((double)minWidth/image.width())/log(ratio);
	}

	if(ImPyramid!=NULL)
		delete []ImPyramid;
	ImPyramid=new ImageT[m_nLevels];
	ImPyramid[0].copyData(image);
	double baseSigma=(1/ratio-1);
	for(int i=1;i<m_nLevels;i++)
	{
		ImageT foo;
		double sigma=baseSigma*i;
		image.GaussianSmoothing(foo,sigma,sigma*2.5);
		foo.imresize(ImPyramid[i],pow(ratio,i));
	}
}*/

template <class ImageT>
void GaussianPyramid<ImageT>::setLevels(int nLevels /* = 4 */)
{
	m_nLevels = nLevels;
}

//---------------------------------------------------------------------------------------
// function to construct the pyramid
// this is the fast way
//---------------------------------------------------------------------------------------
template <class ImageT>
void GaussianPyramid<ImageT>::ConstructPyramid(const ImageT &image, double ratio, int minWidth)
{
	// the ratio cannot be arbitrary numbers
	if(ratio>0.98 || ratio<0.4)
		ratio=0.75;
	if (-1 == m_nLevels) {
		// first decide how many levels
		m_nLevels=log((double)minWidth/image.width())/log(ratio);
	}
	if(ImPyramid!=NULL)
		delete []ImPyramid;
	ImPyramid=new ImageT[m_nLevels];
	ImPyramid[0].copyData(image);
	double baseSigma=(1/ratio-1);
	int n=log(0.25)/log(ratio);
	double nSigma=baseSigma*n;
	for(int i=1;i<m_nLevels;i++)
	{
		ImageT foo;
		if(i<=n)
		{
			double sigma=baseSigma*i;
			image.GaussianSmoothing(foo,sigma,sigma*3);
			foo.imresize(ImPyramid[i],pow(ratio,i));
		}
		else
		{
			ImPyramid[i-n].GaussianSmoothing(foo,nSigma,nSigma*3);
			double rate=(double)pow(ratio,i)*image.width()/foo.width();
			foo.imresize(ImPyramid[i],rate);
		}
	}
}

template <class ImageT>
void GaussianPyramid<ImageT>::displayTop(const char *filename)
{
	ImPyramid[m_nLevels-1].imwrite(filename);
}
