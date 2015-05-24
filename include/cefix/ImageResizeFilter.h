/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef IMAGE_FILTER_HEADER_
#define IMAGE_FILTER_HEADER_

#include <osg/Math>
#include <cefix/Export.h>
namespace cefix {

	class CEFIX_EXPORT  ImageFilterBase
{
public:
    
    ImageFilterBase (double dWidth) : m_dWidth (dWidth) {}
    virtual ~ImageFilterBase() {}

    double GetWidth()                   { return m_dWidth; }
    void   SetWidth (double dWidth)     { m_dWidth = dWidth; }

    virtual double Filter (double dVal) = 0;

protected:

    #define FILTER_PI  double (3.1415926535897932384626433832795)
    #define FILTER_2PI double (2.0 * 3.1415926535897932384626433832795)
    #define FILTER_4PI double (4.0 * 3.1415926535897932384626433832795)

    double  m_dWidth;
};

class BoxImageFilter : public ImageFilterBase
{
public:

    BoxImageFilter (double dWidth = double(0.5)) : ImageFilterBase(dWidth) {}
    virtual ~BoxImageFilter() {}

    double Filter (double dVal) { return (fabs(dVal) <= m_dWidth ? 1.0 : 0.0); }
};

class BilinearImageFilter : public ImageFilterBase
{
public:

    BilinearImageFilter (double dWidth = double(1.0)) : ImageFilterBase(dWidth) {}
    virtual ~BilinearImageFilter() {}

    double Filter (double dVal)
        {
            dVal = fabs(dVal);
            return (dVal < m_dWidth ? m_dWidth - dVal : 0.0);
        }
};

class GaussianImageFilter : public ImageFilterBase
{
public:

    GaussianImageFilter (double dWidth = double(3.0)) : ImageFilterBase(dWidth) {}
    virtual ~GaussianImageFilter() {}

    double Filter (double dVal)
        {
            if (fabs (dVal) > m_dWidth)
            {
                return 0.0;
            }
            return exp (-dVal * dVal / 2.0) / sqrt (FILTER_2PI);
        }
};

class HammingImageFilter : public ImageFilterBase
{
public:

    HammingImageFilter (double dWidth = double(0.5)) : ImageFilterBase(dWidth) {}
    virtual ~HammingImageFilter() {}

    double Filter (double dVal)
        {
            if (fabs (dVal) > m_dWidth)
            {
                return 0.0;
            }
            double dWindow = 0.54 + 0.46 * cos (FILTER_2PI * dVal);
            double dSinc = (dVal == 0) ? 1.0 : sin (FILTER_PI * dVal) / (FILTER_PI * dVal);
            return dWindow * dSinc;
        }
};

class BlackmanImageFilter : public ImageFilterBase
{
public:

    BlackmanImageFilter (double dWidth = double(0.5)) : ImageFilterBase(dWidth) {}
    virtual ~BlackmanImageFilter() {}

    double Filter (double dVal)
        {
            if (fabs (dVal) > m_dWidth)
            {
                return 0.0;
            }
            double dN = 2.0 * m_dWidth + 1.0;
            return 0.42 + 0.5 * cos (FILTER_2PI * dVal / ( dN - 1.0 )) +
                   0.08 * cos (FILTER_4PI * dVal / ( dN - 1.0 ));
        }
};


}
#endif
