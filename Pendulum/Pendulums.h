#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct PendulumLike
{
    bool isFreezed = false;
    std::vector<std::pair<float, float>> trailPoints;
};

struct DPendulum : PendulumLike
{
    float theta1, theta2;
    float omega1, omega2;
    float m1, m2;
    float px, py;
    float L1, L2;

    DPendulum(float t1, float t2, float m1_, float m2_, float L1_, float L2_)
        : theta1(t1), theta2(t2), omega1(0.0f), omega2(0.0f),
          m1(m1_), m2(m2_), px(0.0f), py(0.0f),
          L1(L1_), L2(L2_)
    {
    }
};
struct SPendulum : PendulumLike
{
    float theta;
    float omega;
    float m;
    float px, py;
    float L;

    SPendulum(float theta_, float m_, float L_)
        : theta(theta_), omega(0.0f), m(m_), px(0.0f), py(0.0f), L(L_)
    {
    }
};

namespace DPendulums
{

    DPendulum DPendulumUpdate(const DPendulum &s, float damping, float g, float dt);
    DPendulum DPendulumCreate();
    void DPendulumReset(DPendulum &s);
}

namespace SPendulums
{
    SPendulum SPendulumUpdate(const SPendulum &s, float damping, float g, float dt);
    SPendulum SPendulumCreate();
    void SPendulumReset(SPendulum &s);

}

namespace PendulumUtils
{

    void PendulumClearTrail(PendulumLike &s);
    void PendulumAddTrailPoint(PendulumLike &s, float x, float y, int maxTrail);
    float WrapAngle(float theta);
}
