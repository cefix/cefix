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
 
#ifndef _IMAGE_2_PASS_SCALE_HEADER_
#define _IMAGE_2_PASS_SCALE_HEADER_

#include <math.h>
#include <osg/Math>
#include <cefix/ImageResizeFilter.h>

namespace cefix {

#define PS_MAX_DEPTH 4
              
// Based on Eran Yariv's TwoPassScale, modified for speed, bug fixes, and variable depths.

// Does 2 pass scaling of bitmaps
// Template based, can use various methods for interpolation.
// Could certainly be improved.

//    to use:
//    Image2PassScale <BilinearImageFilter> ScaleEngine;
//    ScaleEngine.Scale ((unsigned char *)dibSrc.GetBits(),          // A pointer to the source bitmap bits
//                        depth,                          // The size of a single pixel in bytes (both source and scaled image)
//                        dibSrc.Width(),                  // The width of a line of the source image to scale in pixels
//                        dibSrc.WidthBytes(),              // The width of a single line of the source image in bytes (to allow for padding, etc.)
//                        dibSrc.Height(),                  // The height of the source image to scale in pixels.
//                        (unsigned char *)GetBits(),              // A pointer to a buffer to hold the ecaled image
//                        Width(),                          // The desired width of a line of the scaled image in pixels
//                        WidthBytes(),                      // The width of a single line of the scaled image in bytes (to allow for padding, etc.)
//                        Height());                      // The desired height of the scaled image in pixels.
//    or AllocAndScale()

// Modified 1-17-05 to use more integer math -- much faater. [JRM]





typedef struct
{
   unsigned int*Weights;  // Normalized weights of neighboring pixels
   int Left,Right;   // Bounds of source pixels window
} ContributionType;  // Contirbution information for a single pixel

typedef struct
{
   ContributionType *ContribRow; // Row (or column) of contribution weights
   unsigned int WindowSize,              // Filter window size (of affecting source pixels)
        LineLength;              // Length of line (no. or rows / cols)
} LineContribType;               // Contribution information for an entire line (row or column)



typedef bool (*ProgressAnbAbortCallBack)(unsigned int bPercentComplete);

template <class FilterClass>
class  Image2PassScale
{
public:

    Image2PassScale (ProgressAnbAbortCallBack callback = NULL) :
        m_Callback (callback) {m_byteDepth = 3;}

    virtual ~Image2PassScale() {}

    unsigned char * AllocAndScale (  
        unsigned char   *pOrigImage,
        unsigned int         pixelBytes,
        unsigned int        uOrigWidth,
        unsigned int        uOrigWidthBytes,
        unsigned int        uOrigHeight,
        unsigned int        uNewWidth,
        unsigned int        uNewWidthBytes,
        unsigned int        uNewHeight);

    unsigned char * Scale (  
        unsigned char   *pOrigImage,
        unsigned int         pixelBytes,
        unsigned int        uOrigWidth,
        unsigned int        uOrigWidthBytes,
        unsigned int        uOrigHeight,
        unsigned char   *pDstImage,
        unsigned int        uNewWidth,
        unsigned int        uNewWidthBytes,
        unsigned int        uNewHeight);

        int                            m_byteDepth;

private:

    ProgressAnbAbortCallBack    m_Callback;
    bool                        m_bCanceled;

    LineContribType *AllocContributions (   unsigned int uLineLength,
                                            unsigned int uWindowSize);

    void FreeContributions (LineContribType * p);

    LineContribType *CalcContributions (    unsigned int    uLineSize,
                                            unsigned int    uSrcSize,
                                            double  dScale);

    void ScaleRow ( unsigned char           *pSrc,
                    unsigned int                uSrcWidth,
                    unsigned int                 uSrcWidthBytes,
                    unsigned char           *pRes,
                    unsigned int                uResWidth,
                    unsigned int                 uDstWidthBytes,
                    unsigned int                uRow,
                    LineContribType    *Contrib);

    void HorizScale (   unsigned char           *pSrc,
                        unsigned int                uSrcWidth,
                        unsigned int                 uSrcWidthBytes,
                        unsigned int                uSrcHeight,
                        unsigned char           *pDst,
                        unsigned int                uResWidth,
                        unsigned int                 uResWidthBytes);

    void ScaleCol ( unsigned char           *pSrc,
                    unsigned int                uSrcWidth,
                    unsigned int                 uSrcWidthBytes,
                    unsigned char           *pRes,
                    unsigned int                uResWidth,
                    unsigned int                 uResWidthBytes,
                    unsigned int                uResHeight,
                    unsigned int                uCol,
                    LineContribType    *Contrib);

    void VertScale (    unsigned char           *pSrc,
                        unsigned int                uSrcWidth,
                        unsigned int                 uSrcWidthBytes,
                        unsigned int                uSrcHeight,
                        unsigned char           *pDst,
                        unsigned int                uResHeight);
};

template <class FilterClass>
LineContribType *
Image2PassScale<FilterClass>::
AllocContributions (unsigned int uLineLength, unsigned int uWindowSize)
{
    LineContribType *res = new LineContribType;
        // Init structure header
    res->WindowSize = uWindowSize;
    res->LineLength = uLineLength;
        // Allocate list of contributions
    res->ContribRow = new ContributionType[uLineLength];
    for (unsigned int u = 0 ; u < uLineLength ; u++)
    {
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new unsigned int[uWindowSize];
    }
    return res;
}

template <class FilterClass>
void
Image2PassScale<FilterClass>::
FreeContributions (LineContribType * p)
{
    for (unsigned int u = 0; u < p->LineLength; u++)
    {
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header
}

template <class FilterClass>
LineContribType *
Image2PassScale<FilterClass>::
CalcContributions (unsigned int uLineSize, unsigned int uSrcSize, double dScale)
{
    FilterClass CurFilter;

    double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = CurFilter.GetWidth();

    if (dScale < 1.0)
    {    // Minification
        dWidth = dFilterWidth / dScale;
        dFScale = dScale;
    }
    else
    {    // Magnification
        dWidth= dFilterWidth;
    }

    // Window size is the number of sampled pixels
//    int iWindowSize = 2 * (int)ceil(dWidth) + 1;
    int iWindowSize = 2 * ((int)ceil(dWidth) + 1);     // changed ... causing crash with bi-linear filiter?? [JRM]

    // Allocate a new line contributions strucutre
    LineContribType *res = AllocContributions (uLineSize, iWindowSize);

    double *dWeights = new double[iWindowSize];

    for (unsigned int u = 0; u < uLineSize; u++)
    {   // Scan through line of contributions
        double dCenter = (double)u / dScale;   // Reverse mapping
        // Find the significant edge points that affect the pixel
        int iLeft = osg::maximum (0, (int)floor (dCenter - dWidth));
        int iRight = osg::minimum ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1);

        // Cut edge points to fit in filter window in case of spill-off
        if (iRight - iLeft + 1 > iWindowSize)
        {
            if (iLeft < (int(uSrcSize) - 1 / 2))
            {
                iLeft++;
            }
            else
            {
                iRight--;
            }
        }
        int nFallback = iLeft;    

        bool bNonZeroFound = false;
        double dTotalWeight = 0.0;  // Zero sum of weights
        double dVal;
        for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
        {   // Calculate weights
            dVal = CurFilter.Filter (dFScale * (dCenter - (double)iSrc));
            if (dVal > 0.0)
                dVal *= dFScale;
            else
            {
                dVal = 0.0;
                // zero conribution, trim
                if (!bNonZeroFound)
                {
                    // we are on the left side, trim left
                    iLeft = iSrc+1;
                    continue;
                }
                else
                {
                    // we are on the right side, trim right
                    iRight = iSrc-1;
                    break;
                }
            }
            bNonZeroFound = true;
            dTotalWeight += dVal;
            dWeights[iSrc-iLeft] = dVal;
        }

        if (iLeft > iRight)
        {
            //ASSERT(FALSE);
            iLeft = iRight = nFallback;
            dWeights[0] = 0.0;
        }
        res->ContribRow[u].Left = iLeft;
        res->ContribRow[u].Right = iRight;

        //ASSERT (dTotalWeight >= 0.0);   // An error in the filter function can cause this
        if (dTotalWeight > 0.0)
        {   // Normalize weight of neighbouring points
            for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
            {   // Normalize point
                dWeights[iSrc-iLeft] /= dTotalWeight;
            }
        }
        // scale weights to integers weights for effeciency
        for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
            res->ContribRow[u].Weights[iSrc-iLeft] = (unsigned int)(dWeights[iSrc-iLeft] * 0xffff);
   }
   delete [] dWeights;
   return res;
}


template <class FilterClass>
void
Image2PassScale<FilterClass>::
ScaleRow (  unsigned char           *pSrc,
            unsigned int                uSrcWidth,
            unsigned int                 uSrcWidthBytes,
            unsigned char           *pRes,
            unsigned int                uResWidth,
            unsigned int                 uResWidthBytes,
            unsigned int                uRow,
            LineContribType    *Contrib)
{
    unsigned char * const pSrcRow = &(pSrc[uRow * uSrcWidthBytes]);
    unsigned char * const pDstRow = &(pRes[uRow * uResWidthBytes]);
    unsigned char *pSrcLoc;
    unsigned char *pDstLoc;
    unsigned int vals[PS_MAX_DEPTH];

    for (unsigned int x = 0; x < uResWidth; x++)
    {   // Loop through row
        int v, i;
        for (v= 0; v < m_byteDepth; v++)
            vals[v] = 0;
        int iLeft = Contrib->ContribRow[x].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[x].Right;  // Retrieve right boundries
        pSrcLoc = &pSrcRow[iLeft*m_byteDepth];
        for (i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            
            // Accumulate weighted effect of each neighboring pixel
            for (v= 0; v < m_byteDepth; v++)
                vals[v] += Contrib->ContribRow[x].Weights[i-iLeft] * *pSrcLoc++;
        }
        pDstLoc = &pDstRow[x*m_byteDepth];
        for (v= 0; v < m_byteDepth; v++)
        {
            // copy to destination, and scale back down by unsigned int
            *pDstLoc++ = osg::clampTo<int>(vals[v] >> 16, 0, 0xff); // Place result in destination pixel
        }
    }
}

template <class FilterClass>
void
Image2PassScale<FilterClass>::
HorizScale (    unsigned char           *pSrc,
                unsigned int                uSrcWidth,
                unsigned int                 uSrcWidthBytes,
                unsigned int                uSrcHeight,
                unsigned char           *pDst,
                unsigned int                uResWidth,
                unsigned int                 uResWidthBytes)
{
    // Assumes heights are the same
//    TRACE ("Performing horizontal scaling...\n");
    if (uResWidth == uSrcWidth)
    {   // No scaling required, just copy
        if(uSrcHeight <= 0) return;
        if (uResWidthBytes == uSrcWidthBytes)
        {
            int copy = ((uSrcHeight -1) * uSrcWidthBytes) + uSrcWidth*m_byteDepth; // avoids overrun if starting in middle of image.
               memcpy (pDst, pSrc, copy);
            return;
        }
        else
        {
            for (unsigned int y = 0; y < uSrcHeight; y++)
                memcpy(pDst+uResWidthBytes*y, pSrc+uSrcWidthBytes*y, uSrcWidth*m_byteDepth);
            return;
        }
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResWidth, uSrcWidth, double(uResWidth) / double(uSrcWidth));
    for (unsigned int u = 0; u < uSrcHeight; u++)
    {   // Step through rows
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback ((unsigned int)(double(u) / double (uSrcHeight) * 50.0)))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = true;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
                
        ScaleRow (  pSrc,
                    uSrcWidth,
                    uSrcWidthBytes,
                    pDst,
                    uResWidth,
                    uResWidthBytes,
                    u,
                    Contrib);    // Scale each row
    }
    FreeContributions (Contrib);  // Free contributions structure
}

template <class FilterClass>
void
Image2PassScale<FilterClass>::
ScaleCol (  unsigned char           *pSrc,
            unsigned int                uSrcWidth,
            unsigned int                 uSrcWidthBytes,
            unsigned char           *pRes,
            unsigned int                uResWidth,
            unsigned int                 uResWidthBytes,
            unsigned int                uResHeight,
            unsigned int                uCol,
            LineContribType    *Contrib)
{
    unsigned char *pSrcLoc;
    unsigned char *pDstLoc;
    unsigned int vals[PS_MAX_DEPTH];

    // assumes same height
    for (unsigned int y = 0; y < uResHeight; y++)
    {    // Loop through column
        int v, i;
        for (v= 0; v < m_byteDepth; v++)
            vals[v] = 0;

        int iLeft = Contrib->ContribRow[y].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[y].Right;  // Retrieve right boundries
        pSrcLoc = pSrc + uSrcWidthBytes*iLeft + uCol* m_byteDepth;
        for (i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
           // unsigned char *pCurSrc = pSrc + uSrcWidthBytes*i + uCol* m_byteDepth;
            
			
            for (v= 0; v < m_byteDepth; v++)
                vals[v] += Contrib->ContribRow[y].Weights[i-iLeft] * pSrcLoc[v];
            pSrcLoc += uSrcWidthBytes;
        }
        pDstLoc = pRes + (y * uResWidthBytes) + uCol*m_byteDepth;
        for (v= 0; v < m_byteDepth; v++)
        {
            // scale back
            *pDstLoc++ = osg::clampTo<int>( vals[v] >> 16, 0, 0xff);   // Place result in destination pixel
        }
    }
}


template <class FilterClass>
void
Image2PassScale<FilterClass>::
VertScale ( unsigned char           *pSrc,
            unsigned int                uSrcWidth,
            unsigned int                 uSrcWidthBytes,
            unsigned int                uSrcHeight,
            unsigned char           *pDst,
            unsigned int                uResHeight)
{
//    TRACE ("Performing vertical scaling...");

    // assumes widths are the same!
    if (uSrcHeight == uResHeight)
    {   // No scaling required, just copy
        if (uSrcHeight <= 0) return;
        int copy = ((uSrcHeight -1) * uSrcWidthBytes) + uSrcWidth*m_byteDepth; // avoids overrun if starting in middle of image.
        memcpy (pDst, pSrc, copy);
        return;
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResHeight, uSrcHeight, double(uResHeight) / double(uSrcHeight));
    for (unsigned int u = 0; u < uSrcWidth; u++)
    {   // Step through columns
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback ((unsigned int)(double(u) / double (uSrcWidth) * 50.0) + 50))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = true;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
        ScaleCol (  pSrc,
                    uSrcWidth,
                    uSrcWidthBytes,
                    pDst,
                    uSrcWidth,
                    uSrcWidthBytes,
                    uResHeight,
                    u,
                    Contrib);   // Scale each column
    }
    FreeContributions (Contrib);     // Free contributions structure
}

template <class FilterClass>
unsigned char *
Image2PassScale<FilterClass>::
AllocAndScale (
    unsigned char   *pOrigImage,            // A pointer to the source bitmap bits
    unsigned int         pixelBytes,        // The size of a single pixel in bytes (both source and scaled image)
    unsigned int        uOrigWidth,            // The width of a line of the source image to scale in pixels
    unsigned int        uOrigWidthBytes,    // The width of a single line of the source image in bytes (to allow for padding, etc.)
    unsigned int        uOrigHeight,        // The height of the source image to scale in pixels.
    unsigned int        uNewWidth,            // The desired width of a line of the scaled image in pixels
    unsigned int        uNewWidthBytes,        // The width of a single line of the scaled image in bytes (to allow for padding, etc.)
    unsigned int        uNewHeight)            // The desired height of the scaled image in pixels.
{
    // Scale source image horizontally into temporary image
    m_byteDepth = pixelBytes;
    m_bCanceled = false;
    unsigned char *pTemp = new unsigned char [uNewWidthBytes * uOrigHeight];
    HorizScale (pOrigImage,
                uOrigWidth,
                uOrigWidthBytes,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uNewWidthBytes);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }
    // Scale temporary image vertically into result image    
    unsigned char *pRes = new unsigned char [uNewWidth * uNewHeight *m_byteDepth];
    VertScale ( pTemp,
                uNewWidth,
                uNewWidthBytes,
                uOrigHeight,
                pRes,
                uNewHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        delete [] pRes;
        return NULL;
    }
    delete [] pTemp;
    return pRes;
}

template <class FilterClass>
unsigned char *
Image2PassScale<FilterClass>::
Scale (
    unsigned char   *pOrigImage,            // A pointer to the source bitmap bits
    unsigned int         pixelBytes,        // The size of a single pixel in bytes (both source and scaled image)
    unsigned int        uOrigWidth,            // The width of a line of the source image to scale in pixels
    unsigned int        uOrigWidthBytes,    // The width of a single line of the source image in bytes (to allow for padding, etc.)
    unsigned int        uOrigHeight,        // The height of the source image to scale in pixels.
    unsigned char   *pDstImage,                // A pointer to a buffer to hold the ecaled image
    unsigned int        uNewWidth,            // The desired width of a line of the scaled image in pixels
    unsigned int        uNewWidthBytes,        // The width of a single line of the scaled image in bytes (to allow for padding, etc.)
    unsigned int        uNewHeight)            // The desired height of the scaled image in pixels.
{
    // Scale source image horizontally into temporary image
    //ASSERT(PS_MAX_DEPTH >= pixelBytes);
    m_byteDepth = pixelBytes;
    m_bCanceled = false;
    unsigned char *pTemp = new unsigned char [ uOrigHeight *uNewWidthBytes];
    HorizScale (pOrigImage,
                uOrigWidth,
                uOrigWidthBytes,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uNewWidthBytes);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }

    // Scale temporary image vertically into result image    
    VertScale ( pTemp,
                uNewWidth,
                uNewWidthBytes,
                uOrigHeight,
                pDstImage,
                uNewHeight);
    delete [] pTemp;
    if (m_bCanceled)
    {
        return NULL;
    }
    return pDstImage;
} 

}

#endif
