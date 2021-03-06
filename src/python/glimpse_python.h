/*
 * Copyright (C) 2017 Glimp IP Ltd
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "glimpse.h"

namespace Glimpse
{
  class DepthImage {
    friend class Forest;
    friend class JointMap;
    friend DepthImage* DepthImageFromPointCloud(float* aPointCloud,
                                                int    aNPoints,
                                                int    aNDims,
                                                int    aHeight,
                                                int    aWidth,
                                                float  aVFOV,
                                                float  aBackground);

    private:
      bool              mValid;
      half_float::half* mDepthImage;
      uint32_t          mWidth;
      uint32_t          mHeight;

      DepthImage(half_float::half* aDepthImage,
                 uint32_t          aWidth,
                 uint32_t          aHeight);

    public:
      DepthImage(const char* aFileName);
      DepthImage(float*      aDepthImage,
                 int         aHeight,
                 int         aWidth);
      ~DepthImage();

      void writeEXR(const char* aFileName);
      void asArray(float** aDepth, int* aOutHeight, int* aOutWidth);
      void asPointCloud(float aVFOV, float aThreshold, float** aCloud,
                        int* aOutNPoints, int* aOutNDims);
  };

  DepthImage* DepthImageFromPointCloud(float* aPointCloud,
                                       int    aNPoints,
                                       int    aNDims,
                                       int    aHeight,
                                       int    aWidth,
                                       float  aVFOV,
                                       float  aBackground = 0.f);

  class Forest {
    friend class JointMap;

    private:
      RDTree**     mForest;
      unsigned int mNTrees;

    public:
      Forest(const char** aFiles,
             unsigned int aNFiles);
      ~Forest();

      void inferLabels(DepthImage* aDepthImage,
                       float**     aLabelPr,
                       int*        aOutHeight,
                       int*        aOutWidth,
                       int*        aNLabels);
  };

  class JointMap {
    private:
      bool mValid;
      JIParams* mParams;
      JSON_Value* mJointMap;

    public:
      JointMap(char* aJointMap, char* aJointInferenceParams);
      ~JointMap();

      void inferJoints(Forest*     aForest,
                       DepthImage* aDepthImage,
                       float**     aJoints,
                       int*        aOutNJoints,
                       int*        aOutNDims);
  };
}
