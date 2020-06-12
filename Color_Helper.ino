/*! \brief Convert RGB to HSV color space

  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0,
  1], respectively.

  \param fR Red component, used as input, range: [0, 1]
  \param fG Green component, used as input, range: [0, 1]
  \param fB Blue component, used as input, range: [0, 1]
  \param fH Hue component, used as output, range: [0, 360]
  \param fS Hue component, used as output, range: [0, 1]
  \param fV Hue component, used as output, range: [0, 1]

*/
void RGBtoHSV(float& fR, float& fG, float fB, float& fH, float& fS, float& fV) {
  float fCMax = max(max(fR, fG), fB);
  float fCMin = min(min(fR, fG), fB);
  float fDelta = fCMax - fCMin;

  if (fDelta > 0) {
    if (fCMax == fR) {
      fH = 60 * (fmod(((fG - fB) / fDelta), 6));
    } else if (fCMax == fG) {
      fH = 60 * (((fB - fR) / fDelta) + 2);
    } else if (fCMax == fB) {
      fH = 60 * (((fR - fG) / fDelta) + 4);
    }

    if (fCMax > 0) {
      fS = fDelta / fCMax;
    } else {
      fS = 0;
    }

    fV = fCMax;
  } else {
    fH = 0;
    fS = 0;
    fV = fCMax;
  }

  if (fH < 0) {
    fH = 360 + fH;
  }
}


/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param fR Red component, used as output, range: [0, 1]
  \param fG Green component, used as output, range: [0, 1]
  \param fB Blue component, used as output, range: [0, 1]
  \param fH Hue component, used as input, range: [0, 360]
  \param fS Hue component, used as input, range: [0, 1]
  \param fV Hue component, used as input, range: [0, 1]

*/
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
  float fC = fV * fS; // Chroma
  float fHPrime = fmod(fH / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fV - fC;

  if (0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if (1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if (2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if (3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if (4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if (5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }

  fR += fM;
  fG += fM;
  fB += fM;
}

float g(int x, float t_x_v, uint16_t n) {
  return ((int)(t_x_v - x)) % (n);
}

float f(int x, float t_x_v, float a, float b, uint16_t n) {
  if (g(x, t_x_v, n) < n / 2) {
    return a + (b - a) * g(x, t_x_v, n) / (n / 2);
  } else {
    return b + (a - b) * g(x - (n / 2), t_x_v, n) / (n / 2);
  }
}
