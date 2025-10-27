# 🌀 Double & Single Pendulum Simulation

A high-performance, **real-time double && single pendulum simulator** written in **C++17**, powered by **OpenGL**, **GLFW**, **GLEW**, and **Dear ImGui**.  
This program visually demonstrates the **chaotic nature** of the double pendulum — one of the most fascinating examples of deterministic chaos in classical mechanics.

![preview](docs/preview.gif)

---

## 🌍 Overview

This simulator numerically integrates the equations of motion for a **double pendulum system** and renders it in real time using OpenGL.  
It features a full **ImGui-based control interface**, enabling you to modify all physical parameters while the simulation runs.

Each pendulum consists of two masses (`m₁`, `m₂`) connected by rigid rods of lengths (`L₁`, `L₂`).  
The system evolves according to Newtonian mechanics with gravitational acceleration `g = 9.81 m/s²`.

---

## ⚙️ Key Features

- 🧮 **Accurate physics simulation**
  - Solved using the **Euler integration method**
  - Adjustable parameters: masses, lengths, angles, angular velocities, and pivot point
- 🧰 **Fully interactive GUI**
  - Powered by **Dear ImGui**
  - Dynamic sliders for every physical property
- 🖼️ **Multi-pendulum support**
  - Add as many pendulums as your GPU can handle
  - Each pendulum runs independently with its own settings
- 🌈 **Customizable trail rendering**
  - Adjustable trail length up to 50,000 points
  - Smooth motion path visualization
- ⚡ **Optimized rendering**
  - Real-time OpenGL 2D visualization
  - Consistent 60+ FPS even with multiple pendulums
- 💻 **Cross-platform**
  - Runs on Windows and Linux

---

## 🧪 Physics Explanation

The motion of a double pendulum is governed by two coupled second-order nonlinear differential equations derived from Newtonian mechanics.

Let:
- `θ₁`, `θ₂` = angles of pendulum arms  
- `ω₁`, `ω₂` = angular velocities  
- `m₁`, `m₂` = masses  
- `L₁`, `L₂` = lengths  
- `g` = gravitational acceleration  

Then the angular accelerations are given by:

\[
\begin{aligned}
\alpha_1 &= \frac{
  m_2 L_1 \omega_1^2 \sin(\delta)\cos(\delta)
  + m_2 g \sin(\theta_2)\cos(\delta)
  + m_2 L_2 \omega_2^2 \sin(\delta)
  - (m_1 + m_2) g \sin(\theta_1)
}{
  (m_1 + m_2)L_1 - m_2 L_1 \cos^2(\delta)
} \\
\alpha_2 &= \frac{
  -m_2 L_2 \omega_2^2 \sin(\delta)\cos(\delta)
  + (m_1 + m_2)\left[g \sin(\theta_1)\cos(\delta)
  - L_1 \omega_1^2 \sin(\delta)
  - g \sin(\theta_2)\right]
}{
  \frac{L_2}{L_1}\left[(m_1 + m_2)L_1 - m_2 L_1 \cos^2(\delta)\right]
}
\end{aligned}
\]

where `δ = θ₂ - θ₁`.

This simulation uses **Euler integration** to approximate motion:
ω₁ += α₁ * Δt
ω₂ += α₂ * Δt
θ₁ += ω₁ * Δt
θ₂ += ω₂ * Δt

---

## 🖥️ User Interface

Each pendulum has its own control window:

| Control | Description |
|----------|--------------|
| **Pivot X/Y** | Move the attachment point on the screen |
| **Length 1/2** | Adjust arm lengths |
| **Mass 1/2** | Set individual mass values |
| **Theta 1/2** | Change starting angles (radians) |
| **Omega 1/2** | Modify angular velocities |
| **Max Trail** | Control trail persistence |
| **Add Pendulum** | Create a new system |
| **Delete All Pendulums** | Clear all data instantly |

All changes are **immediate** — the simulation updates live as you adjust sliders.

---

## 🔧 Dependencies

| Library | Purpose |
|----------|----------|
| [GLFW](https://www.glfw.org/) | Window creation and input handling |
| [GLEW](http://glew.sourceforge.net/) | OpenGL extension loader |
| [Dear ImGui](https://github.com/ocornut/imgui) | UI rendering |
| OpenGL (≥3.3) | Graphics API |

---
