//
// Created by Göksu Güvendiren on 03/05/2017.
//

#include <string>
#include <rtk/rtk_fwd.hpp>
#include <rtk/gl/program.hpp>
#include <rtk/gl/shader.hpp>
#include <rtk/geometry/mesh.hpp>
#include <rtk/gl/mesh.hpp>
#include <rtk/camera.hpp>
#include <rtk/window.hpp>
#include <rtk/rtk_init.hpp>
#include <fstream>
#include "Mesh.h"

#include <glm/gtx/transform.hpp>

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

rtk::gl::program load_mesh_shader()
{
    rtk::gl::vertex_shader mesh_vs{read_text_file("/Users/goksu/rtk/shaders/basic.vert").c_str()};
    rtk::gl::fragment_shader mesh_fs{read_text_file("/Users/goksu/rtk/shaders/basic.frag").c_str()};

    rtk::gl::program mesh_shader;
    mesh_shader.attach(mesh_vs);
    mesh_shader.attach(mesh_fs);
    mesh_shader.link();

    return mesh_shader;
}

rtk::gl::program load_ibs_shader()
{
    rtk::gl::vertex_shader mesh_vs{read_text_file("/Users/goksu/rtk/shaders/basic.vert").c_str()};
    rtk::gl::fragment_shader mesh_fs{read_text_file("../ibs.frag").c_str()};

    rtk::gl::program mesh_shader;
    mesh_shader.attach(mesh_vs);
    mesh_shader.attach(mesh_fs);
    mesh_shader.link();

    return mesh_shader;
}

template <class Container>
void draw_point_cloud(const Container& points, const glm::vec3& color, const rtk::camera& cam)
{
    static auto ibs_s = load_ibs_shader();
    static rtk::gl::mesh cube(rtk::geometry::primitive::cube());

    ibs_s.set_variable("projection", cam.GetProjectionMatrix());
    ibs_s.set_variable("view", cam.GetViewMatrix());
    ibs_s.set_variable("color", color);

    for (auto& v : points)
    {
        ibs_s.set_variable("model", glm::translate(v) * glm::scale(glm::vec3{0.05f}));
        cube.draw(ibs_s);
    }
}

int main()
{
    using namespace rtk::literals;
    rtk::rtk_init init_rtk;

    rtk::window win({1024_px, 768_px}, "Geometry");

    Mesh m_chair(1);
    m_chair.LoadMesh("../inputs/pair1/chair1.off");

    rtk::geometry::mesh geomesh_chair;
    geomesh_chair.set_vertices(m_chair.GetVertexData());
    geomesh_chair.set_faces(m_chair.GetFaces());
    rtk::gl::mesh gl_mesh_chair(geomesh_chair);

    Mesh m_table(2);
    m_table.LoadMesh("../inputs/pair1/table1.off");

    rtk::geometry::mesh geomesh_table;
    geomesh_table.set_vertices(m_table.GetVertexData());
    geomesh_table.set_faces(m_table.GetFaces());
    rtk::gl::mesh gl_mesh_table(geomesh_table);

    auto samples_chair = m_chair.SamplePoints(100);
    auto samples_table = m_table.SamplePoints(100);

    rtk::camera cam;

    glm::vec2 last;

    auto control_cam = [&] {
        if (win.get_key_down(GLFW_KEY_W))
            cam.ProcessKeyboard(rtk::Camera_Movement::FORWARD, 1/60.f);

        if (win.get_key_down(GLFW_KEY_S))
            cam.ProcessKeyboard(rtk::Camera_Movement::BACKWARD, 1/60.f);

        if (win.get_key_down(GLFW_KEY_A))
            cam.ProcessKeyboard(rtk::Camera_Movement::LEFT, 1/60.f);

        if (win.get_key_down(GLFW_KEY_D))
            cam.ProcessKeyboard(rtk::Camera_Movement::RIGHT, 1/60.f);

        auto p = win.get_mouse();
        auto diff = p-last;
        last = p;

        cam.ProcessMouseMovement(-diff.x, diff.y);
    };

    auto mesh_shader = load_mesh_shader();
    mesh_shader.set_variable("projection", cam.GetProjectionMatrix());
    mesh_shader.set_variable("model", glm::mat4());

    while (!win.should_close())
    {
        win.begin_draw();
        control_cam();
        mesh_shader.set_variable("view", cam.GetViewMatrix());
        gl_mesh_chair.draw(mesh_shader);
        draw_point_cloud(samples_chair, glm::vec3(1, 0, 0), cam);
        win.end_draw();
    }

    return 0;
}