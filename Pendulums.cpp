#include "Pendulums.h"

float PendulumLike::WrapAngle(float theta)
{
    const float TWO_PI = 2.0f * M_PI;

    theta = std::fmod(theta, TWO_PI);

    if (theta > M_PI)
        theta -= TWO_PI;
    else if (theta < -M_PI)
        theta += TWO_PI;

    return theta;
}

void PendulumLike::clearTrail()
{
    this->trailPoints.clear();
}

void DPendulum::AddTrailPoint()
{
    float x2 = this->px + this->L1 * sin(this->theta1) + this->L2 * sin(this->theta2);
    float y2 = this->py - this->L1 * cos(this->theta1) - this->L2 * cos(this->theta2);
    if(!this->isFreezed)
        trailPoints.emplace_back(x2, y2);
    if (trailPoints.size() > (size_t)maxTrail)
    {
        size_t excess = trailPoints.size() - (size_t)maxTrail;
        trailPoints.erase(trailPoints.begin(), trailPoints.begin() + excess);
    }
}
void DPendulum::update( float damping, float g, float dt)
{
    if (this->isFreezed)
        return;
    float delta = this->theta2 - this->theta1;
    float den1 = (this->m1 + this->m2) * this->L1 - this->m2 * this->L1 * cos(delta) * cos(delta);
    float den2 = (this->L2 / this->L1) * den1;

    float a1 = (this->m2 * this->L1 * this->omega1 * this->omega1 * sin(delta) * cos(delta) +
                this->m2 * g * sin(this->theta2) * cos(delta) +
                this->m2 * this->L2 * this->omega2 * this->omega2 * sin(delta) -
                (this->m1 + this->m2) * g * sin(this->theta1)) /
               den1;

    float a2 = (-this->m2 * this->L2 * this->omega2 * this->omega2 * sin(delta) * cos(delta) +
                (this->m1 + this->m2) * (g * sin(this->theta1) * cos(delta) -
                                 this->L1 * this->omega1 * this->omega1 * sin(delta) -
                                 g * sin(this->theta2))) /
               den2;

    // Linear damping
    a1 -= damping * this->omega1;
    a2 -= damping * this->omega2;

    this->omega1 += a1 * dt;
    this->omega2 += a2 * dt;
    this->theta1 += this->omega1 * dt;
    this->theta2 += this->omega2 * dt;

    this->theta1 = WrapAngle(this->theta1);
    this->theta2 = WrapAngle(this->theta2);
}


void DPendulum::render()
{
    float x1 = this->px + this->L1 * sin(this->theta1);
    float y1 = this->py - this->L1 * cos(this->theta1);
    float x2 = x1 + this->L2 * sin(this->theta2);
    float y2 = y1 - this->L2 * cos(this->theta2);

    glColor3f(1.0f, 1.0f, 1.0f);
    Renderer::drawLine(this->px, this->py, x1, y1);
    Renderer::drawLine(x1, y1, x2, y2);

    glColor3f(1.0f, 0.3f, 0.3f);
    Renderer::drawCircle(x1, y1, 0.03f);
    Renderer::drawCircle(x2, y2, 0.03f);
    glColor3f(0.2f, 0.7f, 0.2f);
    for (std::pair<float, float>& p : this->trailPoints)
        Renderer::drawCircle(p.first, p.second, 0.01f);
}




void DPendulum::reset()
{
    this->theta1 = 0.0f;
    this->theta2 = 0.0f;
    this->omega1 = 0.0f;
    this->omega2 = 0.0f;
}


void SPendulum::reset()
{
    this->theta = 0.0f;
    this->omega = 0.0f;
}
void SPendulum::update(float damping, float g, float dt)
{
    if (this->isFreezed)
        return;
    float a = -(g / this->L) * sin(this->theta);
    // Linear damping
    a -= damping * this->omega;
    this->omega += a * dt;
    this->theta += this->omega * dt;
	this->theta = WrapAngle(this->theta);
}



void SPendulum::AddTrailPoint()
{
    float x = this->px + this->L * sin(this->theta);
    float y = this->py - this->L * cos(this->theta);
	if (!this->isFreezed)
        trailPoints.emplace_back(x, y);
    if (trailPoints.size() > (size_t)maxTrail)
    {
        size_t excess = trailPoints.size() - (size_t)maxTrail;
        trailPoints.erase(trailPoints.begin(), trailPoints.begin() + excess);
    }
}


void SPendulum::render()
{
    float x = this->px + this->L * sin(this->theta);
    float y = this->py - this->L * cos(this->theta);
    glColor3f(1.0f, 1.0f, 1.0f);
    Renderer::drawLine(this->px, this->py, x, y);
    glColor3f(0.3f, 0.3f, 1.0f);
    Renderer::drawCircle(x, y, 0.03f);
    glColor3f(0.2f, 0.7f, 0.2f);
    for (std::pair<float, float>& p : this->trailPoints)
        Renderer::drawCircle(p.first, p.second, 0.01f);
}

void SPendulum::drawUI(size_t index, std::vector<std::shared_ptr<PendulumLike>> &PendVec) {

    ImGui::Begin(("Single Pendulum " + std::to_string(index + 1)).c_str());
    ImGui::Checkbox(("Freeze Pendulum " + std::to_string(index + 1)).c_str(), &this->isFreezed);
    ImGui::Text("Pendulum %d Controls", (int)(index + 1));
    ImGui::Text("Pivoting");
    ImGui::SliderFloat("X Pivot", &this->px, -2.0f, 2.0f);
    ImGui::SliderFloat("Y Pivot", &this->py, -1.5f, 1.5f);
    ImGui::Text("Length");
    ImGui::SliderFloat("Length", &this->L, 0.1, 2.0);
    ImGui::Text("Mass");
    ImGui::SliderFloat("Mass", &this->m, 0.1, 1000.0);
    ImGui::Text("Theta Angle (radians)");
    ImGui::SliderFloat("Theta", &this->theta, -M_PI, M_PI);
    ImGui::Text("Angular Velocity (rad/s)");
    ImGui::SliderFloat("Omega", &this->omega, -10.0f, 10.0f);
    if (ImGui::Button(("Delete Pendulum " + std::to_string(index + 1)).c_str()))
    {
        clearTrail();
		PendVec.erase(PendVec.begin() + index);
    }
    if (ImGui::Button(("Reset Pendulum " + std::to_string(index + 1)).c_str()))
    {
		reset();
		clearTrail();
    }
    ImGui::End();
}


void DPendulum::drawUI(size_t index, std::vector<std::shared_ptr<PendulumLike>>& PendVec) {
    ImGui::Begin(("Double Pendulum " + std::to_string(index + 1)).c_str());
    ImGui::Checkbox(("Freeze Pendulum " + std::to_string(index + 1)).c_str(), &this->isFreezed);
    ImGui::Text("Pendulum %d Controls", (int)(index + 1));
    ImGui::Text("Pivoting");
    ImGui::SliderFloat("X Pivot", &this->px, -2.0f, 2.0f);
    ImGui::SliderFloat("Y Pivot", &this->py, -1.5f, 1.5f);
    ImGui::Text("Lengths");
    ImGui::SliderFloat("Length 1", &this->L1, 0.1, 2.0);
    ImGui::SliderFloat("Length 2", &this->L2, 0.1, 2.0);
    ImGui::Text("Masses");
    ImGui::SliderFloat("Mass 1", &this->m1, 0.1, 1000.0);
    ImGui::SliderFloat("Mass 2", &this->m2, 0.1, 1000.0);
    ImGui::Text("Theta Angles (radians)");
    ImGui::SliderFloat("Theta 1", &this->theta1, -M_PI, M_PI);
    ImGui::SliderFloat("Theta 2", &this->theta2, -M_PI, M_PI);
    ImGui::Text("Angular Velocities (rad/s)");
    ImGui::SliderFloat("Omega 1", &this->omega1, -10.0f, 10.0f);
    ImGui::SliderFloat("Omega 2", &this->omega2, -10.0f, 10.0f);
    if (ImGui::Button(("Delete Pendulum " + std::to_string(index + 1)).c_str()))
    {
        clearTrail();
        PendVec.erase(PendVec.begin() + index);
    }
    if (ImGui::Button(("Reset Pendulum " + std::to_string(index + 1)).c_str()))
    {
        reset();
        clearTrail();
    }
    ImGui::End();
}