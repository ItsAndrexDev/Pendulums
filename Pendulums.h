#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include "Renderer.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum PendulumTypes
{
    UNDECLARED = 0, SPend = 1, DPend = 2
};

struct PendulumLike
{
    void setMaxTrail(int maxTrail) { this->maxTrail = maxTrail; }
    virtual PendulumTypes getType() const { return UNDECLARED; };
    virtual void update(float damping, float g, float dt) = 0;
    virtual void reset() = 0;
    void clearTrail();
    virtual void AddTrailPoint() = 0;
    virtual void render() = 0;
    virtual void drawUI(size_t index, std::vector<std::shared_ptr<PendulumLike>>& PendVec) = 0;
    
    
    bool isFreezed = false;
    std::vector<std::pair<float, float>> trailPoints;


protected:
	int maxTrail = 300;
    float WrapAngle(float theta);
};

struct DPendulum : PendulumLike
{
	PendulumTypes getType() const override { return DPend; }
    void update(float damping, float g, float dt) override;
	void reset() override;
    void AddTrailPoint() override;
    void render() override;
	void drawUI(size_t index, std::vector<std::shared_ptr<PendulumLike>>& PendVec) override;
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
    PendulumTypes getType() const override { return SPend; }
    void reset() override;
    void update(float damping, float g, float dt) override;
    void AddTrailPoint() override;
    void render() override;
    void drawUI(size_t index, std::vector<std::shared_ptr<PendulumLike>>& PendVec) override;
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
