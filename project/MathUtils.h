#pragma once
#include <wrl.h>
#include <cmath>

namespace MyMath
{
    struct Vector2 {
        float x;
        float y;
    };

    struct Vector3 {
        float x;
        float y;
        float z;
    };

    struct Vector4 {
        float x;
        float y;
        float z;
        float w;
    };

    struct Matrix4x4 {
        float m[4][4];
    };

    struct Transform {
        Vector3 scale;
        Vector3 rotate;
        Vector3 translate;
    };

    // 単位行列
    Matrix4x4 MakeIdentity4x4();

    // X軸で回転
    Matrix4x4 MakeRotateXMatrix(float radian);

    // Y軸で回転
    Matrix4x4 MakeRotateYMatrix(float radian);

    // Z軸で回転
    Matrix4x4 MakeRotateZMatrix(float radian);

    // Affine変換
    Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

    Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

    // 4x4の掛け算
    Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
}