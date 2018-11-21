#pragma once

#include "D3D12Util.h"

class Camera
{
public:
	Camera();
	~Camera();

	//position method
	XMVECTOR GetPosition() const;
	XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	//basic vectors
	XMVECTOR GetRight() const;
	XMFLOAT3 GetRight3f() const;
	XMVECTOR GetUp()const;
	XMFLOAT3 GetUp3f()const;
	XMVECTOR GetLook()const;
	XMFLOAT3 GetLook3f()const;

	//get frustum 
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovX()const;
	float GetFovY()const;

	//set frustum
	void SetLens(float fovY, float aspect, float zn, float zf);

	//get near and far plane demensions
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& worldUp);


	// Get View/Proj matrices.
	XMMATRIX GetView()const;
	XMMATRIX GetProj()const;

	XMFLOAT4X4 GetView4x4f()const;
	XMFLOAT4X4 GetProj4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

private:
	XMFLOAT3 mPosition = { 0.0f,0.0f,0.0f };
	XMFLOAT3 mRight = { 1.0f,0.0f,0.0f };
	XMFLOAT3 mUp = { 0.0f,1.0f,0.0f };
	XMFLOAT3 mLook = { 0.0f,0.0f,1.0f };

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	//is need normalize
	bool mViewDirty = true;

	// Cache View/Proj matrices.
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};