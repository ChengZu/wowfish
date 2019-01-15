/*
 * Roobot.h
 *
 *  Created on: 2018-06-15
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef ROBOT_H_
#define  ROBOT_H_
#include <windows.h>
#include <cmath>

typedef struct _Color
{
    _Color();
    _Color(unsigned char R1, unsigned char G1, unsigned char B1);
    _Color(unsigned char R1, unsigned char G1, unsigned char B1, unsigned char A1);
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
} Color;

typedef struct _HSV
{
    int H;
    float S;
    float V;

} HSV;

class Robot
{
public:
    Robot();
    VOID captureScreen();
    VOID delay(int ms);
    Color getPixelColor(int x, int y);
    POINT findColor(Color color, float similar);
    POINT findColor(Color color, float similar, POINT point);
    VOID keyPress(int keycode);
    VOID keyRelease(int keycode);
    VOID keyDownUp(int keycode);
    VOID mouseMove(int x, int y);
    POINT getCursorPos();
    INT getCursorShap();
    VOID mousePress(DWORD buttons);
    VOID mouseRelease(DWORD buttons);
    VOID mouseLeftClick();
    VOID mouseRightClick();
    VOID mouseWheel(DWORD wheelAmt);
    INT getImageW();
    INT getImageH();
    virtual ~Robot();
private:
    unsigned int imageW, imageH;
    unsigned char *imageBuff;
    bool lock;
    bool onCaptureScreen;
    HCURSOR oldCur;
    int oldCurNum;
};


inline void RGB2HSV(Color &color, HSV &hsv)
{
    UCHAR b = color.B, g = color.G, r = color.R;
    UCHAR max, min, tmp;
    float h = 0, s = 0, v = 0, f_tmp = 0;
    tmp = b > g ? b : g;
    max = tmp > r ? tmp : r; //取得最大值

    if(max == 0)
    {
        hsv.H = h;
        hsv.S = s;
        hsv.V = v;
        return;
    }
    tmp = b > g ? g : b;
    min = tmp > r ? r : tmp; //取得最小值


    if(max != min)
    {

        h = r - g;
        s = r - b;
        v = g - b;
        f_tmp = acos((h + s) / 2 / sqrt(h * h + s * v)) * 180 / 3.1415926;
        if(b <= g)
            h = f_tmp;
        else h = 360 - f_tmp;

    }

    s = (max - min) / static_cast<float>(max); //s
    v = max / 255.0; //v
    hsv.H = h;
    hsv.S = s;
    hsv.V = v;
}

inline bool isColorSimilar(Color color1, Color color2, float similar)
{

    float R = 100;
    float angle = 30;
    float h = R * cos(angle / 180 * M_PI);
    float r = R * sin(angle / 180 * M_PI);

    HSV hsv1, hsv2 ;
    RGB2HSV(color1, hsv1);
    RGB2HSV(color2, hsv2);
    int hsv1h = hsv1.H / 30;
    int hsv2h = hsv2.H / 30;
    if(hsv1h != hsv2h) return false;
    float x1 = r * hsv1.V * hsv1.S * cos(hsv1.H / 180 * M_PI);
    float y1 = r * hsv1.V * hsv1.S * sin(hsv1.H / 180 * M_PI);
    float z1 = h * (1 - hsv1.V);
    float x2 = r * hsv2.V * hsv2.S * cos(hsv2.H / 180 * M_PI);
    float y2 = r * hsv2.V * hsv2.S * sin(hsv2.H / 180 * M_PI);
    float z2 = h * (1 - hsv2.V);
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dz = z1 - z2;
    float ds = sqrt(dx * dx + dy * dy + dz * dz);
    float sm = 90 * (1.0f - similar);
    return ds <= sm;


    /*
    int dsR = color1.R - color2.R;
    int dsG = color1.G - color2.G;
    int dsB = color1.B - color2.B;
    float sml = sqrt(dsR * dsR + dsG * dsG + dsB * dsB) / 441.6729;
    return (1.0f-sml) > similar;
    */


}

#endif /*  ROBOT_H_ */
