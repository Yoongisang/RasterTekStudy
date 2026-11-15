#pragma once

#include <directxmath.h>

using namespace DirectX;

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass& other);
    ~LightClass();

    void SetAmbientColor(float red, float green, float blue, float alpha);
    void SetDiffuseColor(float red, float green, float blue, float alpha);
    void SetDirection(float x, float y, float z);
    void SetSpecularColor(float red, float green, float blue, float alpha);
    void SetSpecularPower(float power);  
    void SetPosition(float x, float y, float z);
    
    XMFLOAT4 GetAmbientColor(); 
    XMFLOAT4 GetDiffuseColor();
    XMFLOAT3 GetDirection();
    XMFLOAT4 GetSpecularColor();
    XMFLOAT4 GetPosition();
    float GetSpecularPower();
private:
    XMFLOAT4 m_ambientColor; 
    XMFLOAT4 m_diffuseColor;
    XMFLOAT3 m_direction;
    XMFLOAT4 m_specularColor;
    XMFLOAT4 m_position; 
    float m_specularPower;  
};
