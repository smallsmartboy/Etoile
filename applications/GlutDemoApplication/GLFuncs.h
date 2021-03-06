#ifndef GLFUNCS_H
#define GLFUNCS_H

#include <Eigen/Eigen>

void drawStringOnScreen(float x, float y, const std::string& s);
void drawArrow3D(const Eigen::Vector3d& pt, const Eigen::Vector3d& dir, const double length, const double thickness, const double arrowThickness=-1);
void drawArrow2D(const Eigen::Vector2d& pt, const Eigen::Vector2d& vec, double thickness);
void drawProgressBar(int currFrame, int totalFrame);
bool screenShot(int w, int h, char *fname, bool _antialias=false);


#endif  //GLFUNCS_H
