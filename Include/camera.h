#pragma once

//todo add rotation
typedef struct Camera_
{
    double cx, cy, width, height; // whatcha lookin at ?!

} Camera;

extern const Camera CameraDefault;

double CameraGetCenterX(Camera *pCamera);
void CameraSetCenterX(Camera *pCamera, double cx);
double CameraGetCenterY(Camera *pCamera);
void CameraSetCenterY(Camera *pCamera, double cy);
double CameraGetWidth(Camera *pCamera);
void CameraSetWidth(Camera *pCamera, double width);
double CameraGetHeight(Camera *pCamera);
void CameraSetHeight(Camera *pCamera, double height);
double CameraGetAspectRatio(Camera *pCamera);
void CameraSetPosition(Camera *pCamera, double cx, double cy, double width);

double CameraGetXmin(Camera *pCamera);
double CameraGetXmax(Camera *pCamera);
double CameraGetYmin(Camera *pCamera);
double CameraGetYmax(Camera *pCamera);

void CameraZoom(Camera *pCamera, double scale);
void CameraReset(Camera *pCamera);
//double CameraGetRotation(Camera * pCamera);
//void CameraSetRotation(Camera * pCamera, double angle);
