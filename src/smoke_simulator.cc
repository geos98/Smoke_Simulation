#include "smoke_simulator.hh"

SmokeSimulator::SmokeSimulator(std::string shader_dir, nanogui::Screen *screen) {
    shader = std::make_shared<nanogui::GLShader>();
    shader->initFromFiles("Default",
                          shader_dir + "/point.vert",
                          shader_dir + "/point.frag");
    
    for (int i = 0; i < 100; ++i) {
        particles.emplace_back(Particle());
    }
}

void SmokeSimulator::draw() {
    for (auto& p : particles) {
        p.update(1.0);
    }

    nanogui::MatrixXf positions(4, particles.size());

    shader->bind();
    for (int i = 0; i < 100; ++i) {
        positions.col(i) << particles[i].pos, 1.0;
    }
    shader->uploadAttrib("in_position", positions, false);
    shader->drawArray(GL_POINTS, 0, particles.size());
}