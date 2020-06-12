#ifndef _Color_Helper
#define _Color_Helper
void RGBtoHSV(float& fR, float& fG, float fB, float& fH, float& fS, float& fV);
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV);
float g(int x, float t_x_v, uint16_t n);
float f(int x, float t_x_v, float a, float b, uint16_t n);
#endif // _Color_Helper
