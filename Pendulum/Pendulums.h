#pragma once
#include <cmath>
#include <vector>
struct DPendulum
{
    float theta1, theta2;
    float omega1, omega2;
    float m1, m2;
    float px, py;
    float L1, L2;
    bool isFreezed = false;
	std::vector<std::pair<float, float>> trailPoints;
};
struct PendulumState
{
    float theta;
    float omega;
    float m;
    float px, py;
    float L;
    bool isFreezed = false;
};

namespace DPendulums {
    
    DPendulum DPendulumUpdate(const DPendulum& s, float damping, float g, float dt);
	DPendulum DPendulumCreate();
	void DPendulumReset(DPendulum& s);
	void DpendulumClearTrail(DPendulum& s);
	void DPendulumAddTrailPoint(DPendulum& s, float x, float y, int maxTrail);
}

namespace Pendulums {
    PendulumState PendulumUpdate(const PendulumState& s, float damping, float g, float dt);
    PendulumState PendulumCreate();
    void PendulumReset(PendulumState& s);
}