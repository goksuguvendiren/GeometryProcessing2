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
#include <voro++/voro++.hh>

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
        ibs_s.set_variable("model", glm::translate(v) * glm::scale(glm::vec3{0.5f}));
        cube.draw(ibs_s);
    }
}

void Camera_Loop(const Mesh& m_chair, const Mesh& m_table,
                 const std::vector<glm::vec3>& samples_chair, const std::vector<glm::vec3>& samples_table,
                 const std::vector<Triangle>& IBS)
{
    using namespace rtk::literals;
    rtk::rtk_init init_rtk;

    rtk::window win({1024_px, 768_px}, "Geometry");

    rtk::geometry::mesh geomesh_chair;
    geomesh_chair.set_vertices(m_chair.GetVertexData());
    geomesh_chair.set_faces(m_chair.GetFaces());
    rtk::gl::mesh gl_mesh_chair(geomesh_chair);

    rtk::geometry::mesh geomesh_table;
    geomesh_table.set_vertices(m_table.GetVertexData());
    geomesh_table.set_faces(m_table.GetFaces());
    rtk::gl::mesh gl_mesh_table(geomesh_table);

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

    std::vector<glm::vec3> IBS_cloud;
    IBS_cloud.reserve(IBS.size() * 3);

    std::vector<std::uint32_t> faceData;

    unsigned int j = 0;
    for (int i = 0; i < IBS.size(); i++)
    {
        IBS_cloud.push_back(IBS[i].PointA().Data());
        IBS_cloud.push_back(IBS[i].PointB().Data());
        IBS_cloud.push_back(IBS[i].PointC().Data());

        faceData.push_back(j++);
        faceData.push_back(j++);
        faceData.push_back(j++);
    }

    rtk::geometry::mesh geomesh_IBS;
    geomesh_IBS.set_vertices(IBS_cloud);
    geomesh_IBS.set_faces(faceData);
    rtk::gl::mesh gl_mesh_IBS(geomesh_IBS);

    win.lock_cursor(true);

    while (!win.should_close())
    {
        win.begin_draw();
        control_cam();
        mesh_shader.set_variable("view", cam.GetViewMatrix());

        gl_mesh_chair.draw(mesh_shader);
        gl_mesh_table.draw(mesh_shader);
        gl_mesh_IBS.draw(mesh_shader);

        draw_point_cloud(samples_chair, glm::vec3(1, 0, 0), cam);
        draw_point_cloud(samples_table, glm::vec3(1, 0, 0), cam);
        draw_point_cloud(IBS_cloud,     glm::vec3(0, 0, 1), cam);
        win.end_draw();
    }
}

void createIBS(std::vector<Triangle>& IBS, const std::vector<int> &f_vert, const std::vector<double> &v, int j)
{
    std::array<glm::vec3, 10> s;
//    static char s[6][128];
    int n = f_vert[j];

    for(int k = 0; k < n; k++)
    {
        int l = 3 * f_vert[j + k + 1];
        s[k] = {v[l], v[l + 1], v[l + 2]};
    }

    // Triangulate polygons.
    for(int k = 2; k < n; k++)
    {
        Vertex a = Vertex(s[0], 0);
        Vertex b = Vertex(s[k - 1], 0);
        Vertex c = Vertex(s[k], 0);

        IBS.push_back(Triangle(a, b, c, 1));
    }

}

int main()
{
    Mesh m_chair(1);
    m_chair.LoadMesh("../inputs/pair1/chair1.off");
    m_chair.Translate({0, 60, -10});

    Mesh m_table(2);
    m_table.LoadMesh("../inputs/pair1/table1.off");

    auto samples_chair = m_chair.SamplePoints(100);
    auto samples_table = m_table.SamplePoints(100);

    m_chair.CreateBoundingBox();
    m_table.CreateBoundingBox();
    BoundingBox box(glm::min(m_chair.Box().mins, m_table.Box().mins), glm::max(m_chair.Box().maxs, m_table.Box().maxs));

    voro::pre_container preContainer(box.mins.x, box.maxs.x,
                                     box.mins.y, box.maxs.y,
                                     box.mins.z, box.maxs.z,
                                     false, false, false);

    int i = 0;
    for (auto& sample : samples_chair)
    {
        preContainer.put(i++, sample.x, sample.y, sample.z);
    }

    i = 400;
    for (auto& sample : samples_table)
    {
        preContainer.put(i++, sample.x, sample.y, sample.z);
    }

    int nx, ny, nz;
    preContainer.guess_optimal(nx, ny, nz);


    voro::container container(box.mins.x, box.maxs.x,
                              box.mins.y, box.maxs.y,
                              box.mins.z, box.maxs.z,
                              nx, ny, nz, false, false, false, 10);

    preContainer.setup(container);

    double x, y, z;
    std::vector<int> neigh, f_vert;
    std::vector<double> v;
    voro::voronoicell_neighbor c;

    std::vector<Triangle> IBS;

    voro::c_loop_all cl(container);
    if(cl.start())
    {
        do {
            if (container.compute_cell(c, cl)) {
                cl.pos(x, y, z);
                int id = cl.pid();

                // Gather information about the computed Voronoi cell
                c.neighbors(neigh);
                c.face_vertices(f_vert);
                c.vertices(x, y, z, v);

                // Loop over all faces of the Voronoi cell
                int j = 0;
                for (int i = 0; i < neigh.size(); i++)
                {
                    if (id < 400 && neigh[i] >= 400 && neigh[i] < 500)
                    {
                        if (f_vert[j] <= 10)
                        {
                            createIBS(IBS, f_vert, v, j);
                        }
                    }
                    j += f_vert[j] + 1;
                }
            }
        } while (cl.inc());
    }

    Camera_Loop(m_chair, m_table, samples_chair, samples_table, IBS);

    return 0;
}