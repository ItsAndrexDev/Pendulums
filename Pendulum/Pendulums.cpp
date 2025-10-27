#include "Pendulums.h"

DPendulum DPendulums::DPendulumUpdate(const DPendulum& s, float damping, float g, float dt)
{
    if (s.isFreezed) return s;
    float delta = s.theta2 - s.theta1;
    float den1 = (s.m1 + s.m2) * s.L1 - s.m2 * s.L1 * cos(delta) * cos(delta);
    float den2 = (s.L2 / s.L1) * den1;

    float a1 = (s.m2 * s.L1 * s.omega1 * s.omega1 * sin(delta) * cos(delta) +
        s.m2 * g * sin(s.theta2) * cos(delta) +
        s.m2 * s.L2 * s.omega2 * s.omega2 * sin(delta) -
        (s.m1 + s.m2) * g * sin(s.theta1)) /
        den1;

    float a2 = (-s.m2 * s.L2 * s.omega2 * s.omega2 * sin(delta) * cos(delta) +
        (s.m1 + s.m2) * (g * sin(s.theta1) * cos(delta) -
            s.L1 * s.omega1 * s.omega1 * sin(delta) -
            g * sin(s.theta2))) /
        den2;

    // Linear damping
    a1 -= damping * s.omega1;
    a2 -= damping * s.omega2;

    // Euler integration
    DPendulum next = s;
    next.omega1 += a1 * dt;
    next.omega2 += a2 * dt;
    next.theta1 += next.omega1 * dt;
    next.theta2 += next.omega2 * dt;
    return next;
}

DPendulum DPendulums::DPendulumCreate() {
    DPendulum pend{ 0, 0, 0.0, 0.0, 1, 1, 0.0f, 0.0f, 0.5f, 0.5f, false };
	return pend;

}

void DPendulums::DPendulumReset(DPendulum& s) {
    s.theta1 = 0.0f;
    s.theta2 = 0.0f;
    s.omega1 = 0.0f;
    s.omega2 = 0.0f;
}

void DPendulums::DpendulumClearTrail(DPendulum& s) {
    s.trailPoints.clear();
}

void DPendulums::DPendulumAddTrailPoint(DPendulum& s, float x, float y, int maxTrail) {
	s.trailPoints.emplace_back(x, y);
    if (s.trailPoints.size() > (size_t)maxTrail) {
        size_t excess = s.trailPoints.size() - (size_t)maxTrail;
        s.trailPoints.erase(s.trailPoints.begin(), s.trailPoints.begin() + excess);
    }
}