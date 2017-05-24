cbuffer AppCB : register(b0)
{
    float4 m_ScreenResolution;
};

cbuffer FrameCB : register(b1)
{
    float4 m_Light0Direction;
    float4 m_CameraPosition;
    float m_Time;
};

cbuffer ObjectCB : register(b2)
{
    matrix m_MVP;
    matrix m_M;
    matrix m_IT_M;
};