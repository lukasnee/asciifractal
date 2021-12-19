
#include "camera.h"

#include <stdio.h>
#include "screen.h"

double CameraGetCenterX(Camera *pCamera)
{
    return pCamera->cx;
}

void CameraSetCenterX(Camera *pCamera, double cx)
{
    pCamera->cx = cx;
}

double CameraGetCenterY(Camera *pCamera)
{
    return pCamera->cy;
}

void CameraSetCenterY(Camera *pCamera, double cy)
{
    pCamera->cy = cy;
}

double CameraGetWidth(Camera *pCamera)
{
    return pCamera->width;
}

void CameraSetWidth(Camera *pCamera, double width)
{
    pCamera->width = width;
}

double CameraGetHeight(Camera *pCamera)
{
    return pCamera->height;
}

void CameraSetHeight(Camera *pCamera, double height)
{
    pCamera->height = height;
}

double CameraGetAspectRatio(Camera *pCamera)
{
    return ScreenGetRatio();
}

void CameraSetPosition(Camera *pCamera, double cx, double cy, double width)
{
    CameraSetCenterX(pCamera, cx);
    CameraSetCenterY(pCamera, cy);
    CameraSetWidth(pCamera, width);
    CameraSetHeight(pCamera, width / CameraGetAspectRatio(pCamera));
}

double CameraGetXmin(Camera *pCamera)
{
    return CameraGetCenterX(pCamera) - CameraGetWidth(pCamera) / 2.00;
}

double CameraGetXmax(Camera *pCamera)
{
    return CameraGetCenterX(pCamera) + CameraGetWidth(pCamera) / 2.00;
}

double CameraGetYmin(Camera *pCamera)
{
    return CameraGetCenterY(pCamera) - CameraGetHeight(pCamera) / 2.00;
}

double CameraGetYmax(Camera *pCamera)
{
    return CameraGetCenterY(pCamera) + CameraGetHeight(pCamera) / 2.00;
}

void CameraZoom(Camera *pCamera, double scale)
{
    pCamera->width *= scale;
    pCamera->height *= scale;
}

void CameraReset(Camera *pCamera)
{
    pCamera->cx = 0.00;
    pCamera->cy = 0.00;
    pCamera->width = 1.00;
    pCamera->height = 1.00 / CameraGetAspectRatio(pCamera);
}
// TODO
//double CameraGetRotation(Camera * pCamera);
//void CameraSetRotation(Camera * pCamera, double angle);
