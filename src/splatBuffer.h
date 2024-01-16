#ifndef SPLATBUFFER_H
#define SPLATBUFFER_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <cmath>

 class SplatBuffer {
 public:
  using vec3 = glm::vec3;
  int CenterComponentCount = 3;
  int ScaleComponentCount = 3;
  int RotationComponentCount = 4;
  int ColorComponentCount = 4;

  static const struct CompressionLevels {
    int BytesPerCenter;
    int BytesPerScale;
    int BytesPerColor;
    int BytesPerRotation;
    int ScaleRange;
  };
  inline static const std::vector<CompressionLevels> levels = {
      {12, 12, 4, 16, 1},
      {6, 6, 4, 8, 32767}};

  static const int CovarianceSizeFloats = 6;
  static const int CovarianceSizeBytes = 24;
  static const int HeaderSizeBytes = 1024;

  int bytesPerCenter, bytesPerScale, bytesPerColor, bytesPerRotation,
      bytesPerSplat;
  uint8_t compressionLevel, versionMajor, versionMinor, headerExtraK;
  uint32_t splatCount, bucketSize, bucketCount, bucketBlockSize, bytesPerBucket,
      compressionScaleRange;
  float halfBucketBlockSize, compressionScaleFactor;
  uint8_t* headerBufferData;
  uint8_t* headerArrayUint8;
  uint32_t* headerArrayUint32;
  float* headerArrayFloat32;
  std::vector<uint8_t> splatbuffer;
  char* splatBufferData;
  std::vector<uint16_t> centerArray;
  std::vector<uint16_t> rotationArray;
  std::vector<uint16_t> scaleArray;
  std::vector<uint8_t> colorArray;
  int bucketsBase;

  template <typename T>
  SplatBuffer(std::vector<T> bufferData) {
    std::vector<uint8_t> headerBuffer(HeaderSizeBytes);
    headerBufferData = headerBuffer.data();
    headerArrayUint8 = reinterpret_cast<uint8_t*>(headerBufferData);
    headerArrayUint32 = reinterpret_cast<uint32_t*>(headerBufferData);
    headerArrayFloat32 = reinterpret_cast<float*>(headerBufferData);
    memcpy(headerArrayUint8, bufferData.data(),
           sizeof(uint8_t) * HeaderSizeBytes);
    this->versionMajor = headerArrayUint8[0];
    this->versionMinor = headerArrayUint8[1];
    this->headerExtraK = headerArrayUint8[2];
    this->compressionLevel = headerArrayUint8[3];
    this->splatCount = headerArrayUint32[1];
    this->bucketSize = headerArrayUint32[2];
    this->bucketCount = headerArrayUint32[3];
    this->bucketBlockSize = headerArrayFloat32[4];
    this->halfBucketBlockSize = (float)bucketBlockSize / 2.0;
    this->bytesPerBucket = headerArrayUint32[5];
    this->compressionScaleRange =
        headerArrayUint32[6] == 0 ? headerArrayUint32[6]
                                  : levels[this->compressionLevel].ScaleRange;
    this->compressionScaleFactor =
        (float)halfBucketBlockSize / (float)compressionScaleRange;

    const long dataBufferSizeBytes = bufferData.size() - HeaderSizeBytes;
    this->splatBufferData = new char[dataBufferSizeBytes];
    std::memcpy(this->splatBufferData,
                bufferData.data() + SplatBuffer::HeaderSizeBytes,
                dataBufferSizeBytes);

    bytesPerCenter = levels[this->compressionLevel].BytesPerCenter;
    bytesPerScale = levels[this->compressionLevel].BytesPerScale;
    bytesPerColor = levels[this->compressionLevel].BytesPerColor;
    bytesPerRotation = levels[this->compressionLevel].BytesPerRotation;

    bytesPerSplat =
        bytesPerCenter + bytesPerScale + bytesPerColor + bytesPerRotation;

    linkBufferArrays();
  }

  void linkBufferArrays() {
    if (compressionLevel == 0) {
      std::vector<float> centerBuffer(splatCount * CenterComponentCount);
      memcpy(centerBuffer.data(), splatBufferData,
             splatCount * CenterComponentCount);
      // centerArray = centerBuffer.data();

      std::vector<float> scaleBuffer(splatCount * CenterComponentCount);
      memcpy(scaleBuffer.data(), splatBufferData + bytesPerCenter * splatCount,
             splatCount * CenterComponentCount);
      // scaleArray = scaleBuffer.data();

      std::vector<uint8_t> colorBuffer(splatCount * CenterComponentCount);
      memcpy(colorBuffer.data(),
             splatBufferData + (bytesPerCenter + bytesPerScale) * splatCount,
             splatCount * CenterComponentCount);
      // colorArray = colorBuffer.data();

      std::vector<float> rotationBuffer(splatCount * CenterComponentCount);
      memcpy(rotationBuffer.data(),
             splatBufferData +
                 (bytesPerCenter + bytesPerScale + bytesPerColor) * splatCount,
             splatCount * CenterComponentCount);
      // rotationArray = rotationBuffer.data();

      bucketsBase = splatCount * bytesPerSplat;
    } else {
      centerArray = std::vector<uint16_t>(this->splatCount *
                                          SplatBuffer::CenterComponentCount);
      memcpy(centerArray.data(), splatBufferData,
             this->splatCount * SplatBuffer::CenterComponentCount);

      scaleArray = std::vector<uint16_t>(this->splatCount *
                                         SplatBuffer::ScaleComponentCount);
      memcpy(scaleArray.data(),
             splatBufferData + this->bytesPerCenter * this->splatCount,
             this->splatCount * SplatBuffer::ScaleComponentCount);

      colorArray = std::vector<uint8_t>(this->splatCount *
                                        SplatBuffer::ColorComponentCount);
      memcpy(colorArray.data(),
             splatBufferData + (this->bytesPerCenter + this->bytesPerScale) *
                                   this->splatCount,
             this->splatCount * SplatBuffer::ColorComponentCount);

      rotationArray = std::vector<uint16_t>(
          this->splatCount * SplatBuffer::RotationComponentCount);
      memcpy(rotationArray.data(),
             splatBufferData + (this->bytesPerCenter + this->bytesPerScale +
                                this->bytesPerColor) *
                                   this->splatCount,
             this->splatCount * SplatBuffer::RotationComponentCount);

      bucketsBase = splatCount * bytesPerSplat;
    }
  }
  void float32(float* __restrict out, const uint16_t in) {
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;

    t1 = in & 0x7fffu;  // Non-sign bits
    t2 = in & 0x8000u;  // Sign bit
    t3 = in & 0x7c00u;  // Exponent

    t1 <<= 13u;  // Align mantissa on MSB
    t2 <<= 16u;  // Shift sign bit into position

    t1 += 0x38000000;  // Adjust bias

    t1 = (t3 == 0 ? 0 : t1);  // Denormals-as-zero

    t1 |= t2;  // Re-insert sign bit

    *((uint32_t*)out) = t1;
  };
  // 16bit to 32bit
  union Fp32 {
    uint32_t u;
    float f;
  };
  inline float fbf(uint16_t value) {
    if (this->compressionLevel == 0) {
      return value;
    } else {
      const Fp32 magic = {(254U - 15U) << 23};
      const Fp32 was_infnan = {(127U + 16U) << 23};
      Fp32 out;

      out.u = (value & 0x7FFFU) << 13; /* exponent/mantissa bits */
      out.f *= magic.f;                /* exponent adjust */
      if (out.f >= was_infnan.f)       /* make sure Inf/NaN survive */
      {
        out.u |= 255U << 23;
      }
      out.u |= (value & 0x8000U) << 16; /* sign bit */
      return out.f;
    }
  };

  uint8_t* getHeaderBufferData() { return this->headerBufferData; }

  char* getSplatBufferData() { return this->splatBufferData; }

  uint32_t getSplatCount() { return this->splatCount; }

  void fillSplatCenterArray(std::vector<float>& outCenterArray,
                            int destOffset) {
    int splatCount = this->splatCount;
    float bucket[3] = {0, 0, 0};
    float center[3] = {0, 0, 0};
    for (int i = 0; i < splatCount; i++) {
      int centerSrcBase = i * SplatBuffer::CenterComponentCount;
      int centerDestBase = (i + destOffset) * SplatBuffer::CenterComponentCount;
      if (this->compressionLevel > 0) {
        int bucketIndex = floor(i / this->bucketSize);
        float* bucket = new float[3];
        memcpy(bucket,
               this->splatBufferData + this->bucketsBase +
                   bucketIndex * this->bytesPerBucket,
               3 * sizeof(float));

        float sf = this->compressionScaleFactor;
        float sr = this->compressionScaleRange;
        center[0] = (this->centerArray[centerSrcBase] - sr) * sf + bucket[0];
        center[1] =
            (this->centerArray[centerSrcBase + 1] - sr) * sf + bucket[1];
        center[2] =
            (this->centerArray[centerSrcBase + 2] - sr) * sf + bucket[2];
      } else {
        center[0] = this->centerArray[centerSrcBase];
        center[1] = this->centerArray[centerSrcBase + 1];
        center[2] = this->centerArray[centerSrcBase + 2];
      }
      // if (transform) {
      //     center.applyMatrix4(transform);
      // }
      outCenterArray[centerDestBase] = center[0];
      outCenterArray[centerDestBase + 1] = center[1];
      outCenterArray[centerDestBase + 2] = center[2];
    }
  }

  void fillSplatScaleAndRotationArray(std::vector<float>& scaleArray,
                                      std::vector<float>& rotationArray,
                                      int destOffse) {
    const int splatCount = this->splatCount;

    float scale[3] = {0, 0, 0};
    glm::mat3 rotationMatrix;
    glm::mat3 covarianceMatrix;
    glm::mat3 transformedCovariance;
    glm::mat3 transform3x3;
    glm::mat3 transform3x3Transpose;
    glm::mat4 tempMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
                                      0, 1);

    for (int i = 0; i < splatCount; i++) {
      const int scaleBase = i * SplatBuffer::ScaleComponentCount;
      scaleArray[i * 3] = fbf(this->scaleArray[scaleBase]);
      scaleArray[i * 3 + 1] = fbf(this->scaleArray[scaleBase + 1]);
      scaleArray[i * 3 + 2] = fbf(this->scaleArray[scaleBase + 2]);
   
      const int rotationBase = i * SplatBuffer::RotationComponentCount;
      rotationArray[i * 4] = fbf(this->rotationArray[rotationBase]);
      rotationArray[i * 4 + 1] = fbf(this->rotationArray[rotationBase + 1]);
      rotationArray[i * 4 + 2] = fbf(this->rotationArray[rotationBase + 2]);
      rotationArray[i * 4 + 3] = fbf(this->rotationArray[rotationBase + 3]);
    }
  }

  void fillSplatCovarianceArray(std::vector<float>& covarianceArray,
                                int destOffset) {
    const int splatCount = this->splatCount;
    float scale[3] = {0, 0, 0};
    glm::mat3 rotationMatrix;
    glm::mat3 covarianceMatrix;
    glm::mat3 transformedCovariance;
    glm::mat3 transform3x3;
    glm::mat3 transform3x3Transpose;
    glm::mat4 tempMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

    for (int i = 0; i < splatCount; i++) {
      const int scaleBase = i * SplatBuffer::ScaleComponentCount;
      scale[0] = fbf(this->scaleArray[scaleBase]);
      scale[1] = fbf(this->scaleArray[scaleBase + 1]);
      scale[2] = fbf(this->scaleArray[scaleBase + 2]);
      // TODO
    }
  }

  inline void makeRotationFromQuaternion(glm::quat& quaternion,
                                         glm::mat4& matrix) {
    // const te = this.elements;

    const float x = quaternion[0];
    const float y = quaternion.y;
    const float z = quaternion.z;
    const float w = quaternion.w;
    const float x2 = x + x, y2 = y + y, z2 = z + z;
    const float xx = x * x2, xy = x * y2, xz = x * z2;
    const float yy = y * y2, yz = y * z2, zz = z * z2;
    const float wx = w * x2, wy = w * y2, wz = w * z2;

    const float sx = 1.0, sy = 1.0, sz = 1.0;

    (&matrix[0][0])[0] = (1 - (yy + zz)) * sx;
    (&matrix[0][0])[1] = (xy + wz) * sx;
    (&matrix[0][0])[2] = (xz - wy) * sx;
    (&matrix[0][0])[3] = 0;

    (&matrix[0][0])[4] = (xy - wz) * sy;
    (&matrix[0][0])[5] = (1 - (xx + zz)) * sy;
    (&matrix[0][0])[6] = (yz + wx) * sy;
    (&matrix[0][0])[7] = 0;

    (&matrix[0][0])[8] = (xz + wy) * sz;
    (&matrix[0][0])[9] = (yz - wx) * sz;
    (&matrix[0][0])[10] = (1 - (xx + yy)) * sz;
    (&matrix[0][0])[11] = 0;

    (&matrix[0][0])[12] = 0;
    (&matrix[0][0])[13] = 0;
    (&matrix[0][0])[14] = 0;
    (&matrix[0][0])[15] = 1;
  }
  void fillSplatColorArray(std::vector<uint8_t>& outColorArray,
                           int destOffset) {
    const int splatCount = this->splatCount;
    for (int i = 0; i < splatCount; i++) {
      const int colorSrcBase = i * SplatBuffer::ColorComponentCount;
      const int colorDestBase =
          (i + destOffset) * SplatBuffer::ColorComponentCount;
      outColorArray[colorDestBase] = this->colorArray[colorSrcBase];
      outColorArray[colorDestBase + 1] = this->colorArray[colorSrcBase + 1];
      outColorArray[colorDestBase + 2] = this->colorArray[colorSrcBase + 2];
      outColorArray[colorDestBase + 3] = this->colorArray[colorSrcBase + 3];
      // TODO: implement application of transform for spherical harmonics
    }
  }
};
#endif