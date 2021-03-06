#ifndef RENDERER_CAMERA_H
#define RENDERER_CAMERA_H

#include "Eigen/Core"

#define DEFAULT_NEAR_PLANE 0.001
#define DEFAULT_FAR_PLANE 15

enum AXIS {
	AXIS_X,
	AXIS_Y,
	AXIS_Z
};

class Camera {
public:
	Camera() {}
	virtual ~Camera() {};
	virtual void set(const Eigen::Vector3d& _eye, const Eigen::Vector3d& _look, const Eigen::Vector3d& _up);
	virtual void slide(double _delX, double _delY, double _delZ, bool _bLocal = false);
	virtual void setFrustum(float _vAng, float _asp, float _nearD, float _farD);
	virtual void setOrtho(float _width, float _height, float _nearD, float _farD);


	virtual void roll(float _angle);
	virtual void pitch(float _angle);
	virtual void yaw(float _angle);
	virtual void localRotate(float _angle, AXIS _axis);
	virtual void globalRotate(float _angle, AXIS _axis);

	virtual Eigen::Vector3d getEye(void) const {
		return Eigen::Vector3d(0.0f, 0.0f, 1.0f);
	}
	virtual Eigen::Vector3d getLookAtDir(void) const {
		return Eigen::Vector3d(0.0f, 0.0f, 0.0f);
	}
	virtual Eigen::Vector3d getUpDir(void) const {
		return Eigen::Vector3d(0.0f, 1.0f, 0.0f);

	}
	virtual bool isOrthogonal(void) const {
		return mIsOrthognal;
	}

	virtual float getVerticalViewAngle(void) const {
		return mViewAngle;
	}
	virtual float getNearDistance() const {
		return mNearDist;
	}
	virtual float getFarDistance() const {
		return mFarDist;
	}
	virtual float getWidth() const {
		return mWidth;
	}
	virtual float getHeight() const {
		return mHeight;
	}

protected:
	float mViewAngle, mAspect, mNearDist, mFarDist, mWidth, mHeight;
	bool mIsOrthognal;
};


#endif // #ifndef RENDERER_CAMERA_H
