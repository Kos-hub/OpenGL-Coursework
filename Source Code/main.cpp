#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace std::chrono;
using namespace graphics_framework;
using namespace glm;

//Assign variables for fire
const unsigned int MAX_PARTICLES = 4096;
vec4 part_positions[MAX_PARTICLES];
vec4 velocitys[MAX_PARTICLES];
GLuint G_Position_buffer, G_Velocity_buffer;
GLuint vao;

// Map for meshes
map<string, mesh> meshes;

// Effects
effect eff;
effect shadow_eff;
effect grain;
effect grey;
effect contrast;
effect fire;
effect comp_fire;

// Textures
texture tex;
texture ftex;

// Camera
target_camera cam;

// Spot Light
spot_light spot;

// Frame buffers and screen quad
frame_buffer frame;
frame_buffer grey_frame;
frame_buffer contrast_frame;
geometry screen_quad;

// Shadow map
shadow_map shadow;

// Variable for controlling post proc toggling
bool enablePostProc = false;

bool load_content() {

     // Creating frame buffer using screen width and height
    grey_frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
    contrast_frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
    frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

    // Create screen quad
    vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
    vec3(1.0f, 1.0f, 0.0f) };
    vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };

    screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
    screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
    screen_quad.set_type(GL_TRIANGLE_STRIP);

    screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
    screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);

    // Creating shadowmap using screen size
    shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

    // Create plane mesh
    meshes["plane"] = mesh(geometry_builder::create_plane());

    // Create scene
    meshes["box"] = mesh(geometry_builder::create_box());
    meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
    meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
    meshes["disk"] = mesh(geometry_builder::create_disk(20));
    meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
    meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
    meshes["balloon"] = mesh(geometry("res/models/balloon.obj"));

    // Transform objects
    meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
    meshes["box"].get_transform().translate(vec3(10.0f, 2.5f, -30.0f));

    meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
    meshes["tetra"].get_transform().translate(vec3(30.0f, 10.0f, -10.0f));

    meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
    meshes["pyramid"].get_transform().translate(vec3(10.0f, 7.5f, -30.0f));

    meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
    meshes["disk"].get_transform().translate(vec3(10.0f, 11.5f, -30.0f));
    meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

    meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
    meshes["cylinder"].get_transform().translate(vec3(-10.0f, 2.5f, -25.0f));

    meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
    meshes["sphere"].get_transform().translate(vec3(-10.0f, 10.0f, -25.0f));

    meshes["balloon"].get_transform().translate(vec3(0.0f, 15.0f, 0.0f));
    meshes["balloon"].get_transform().scale = vec3(0.04f, 0.04f, 0.04f);

    // Set materials
    // Red box
    meshes["box"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["box"].get_material().set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    meshes["box"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["box"].get_material().set_shininess(25.0f);
    // Green tetra
    meshes["tetra"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["tetra"].get_material().set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
    meshes["tetra"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["tetra"].get_material().set_shininess(25.0f);
    // Blue pyramid
    meshes["pyramid"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["pyramid"].get_material().set_diffuse(vec4(0.0f, 0.0f, 1.0f, 1.0f));
    meshes["pyramid"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["pyramid"].get_material().set_shininess(25.0f);
    // Yellow disk
    meshes["disk"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["disk"].get_material().set_diffuse(vec4(1.0f, 1.0f, 0.0f, 1.0f));
    meshes["disk"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["disk"].get_material().set_shininess(25.0f);
    // Magenta cylinder
    meshes["cylinder"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["cylinder"].get_material().set_diffuse(vec4(1.0f, 0.0f, 1.0f, 1.0f));
    meshes["cylinder"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["cylinder"].get_material().set_shininess(25.0f);
    // Cyan sphere
    meshes["sphere"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["sphere"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
    meshes["sphere"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["sphere"].get_material().set_shininess(25.0f);
    // Red Balloon
    meshes["balloon"].get_material().set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
    meshes["balloon"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
    meshes["balloon"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes["balloon"].get_material().set_shininess(25.0f);

    // Load texture
    tex = texture("res/textures/check_1.png");

    // Set lighting values
    spot.set_position(vec3(50.0f, 50.0f, 0.0f));
    spot.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    spot.set_direction(normalize(-spot.get_position()));
    spot.set_range(1000.0f);
    spot.set_power(20.0f);

    //Main Shader
    eff.add_shader("res/shaders/shadow.vert", GL_VERTEX_SHADER);
    vector<string> frag_shaders{ "res/shaders/shadow.frag",
        "res/shaders/part_spot.frag",
        "res/shaders/part_shadow.frag" };
    eff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);
    eff.build();

    //Shadow Shader
    shadow_eff.add_shader("res/shaders/spot.vert", GL_VERTEX_SHADER);
    shadow_eff.add_shader("res/shaders/spot.frag", GL_FRAGMENT_SHADER);
    shadow_eff.build();

    //Grain Shader
    grain.add_shader("res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
    grain.add_shader("res/shaders/grain.frag", GL_FRAGMENT_SHADER);
    grain.build();

    //Grey Shader
    grey.add_shader("res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
    grey.add_shader("res/shaders/greyscale.frag", GL_FRAGMENT_SHADER);
    grey.build();

    //contrast Shader
    contrast.add_shader("res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
    contrast.add_shader("res/shaders/contrast.frag", GL_FRAGMENT_SHADER);
    contrast.build();

    //Fire Shader
    fire.add_shader("res/shaders/smoke.vert", GL_VERTEX_SHADER);
    fire.add_shader("res/shaders/smoke.frag", GL_FRAGMENT_SHADER);
    fire.add_shader("res/shaders/smoke.geom", GL_GEOMETRY_SHADER);
    comp_fire.add_shader("res/shaders/particle.comp", GL_COMPUTE_SHADER);     
    fire.build();
    comp_fire.build();

    //Variables for random numbers for the particle system
    default_random_engine rand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    uniform_real_distribution<float> dist;

    //Bind fire texture
    ftex = texture("res/textures/smoke.png"); 
    // Initilise particles
    for (unsigned int i = 0; i < MAX_PARTICLES; ++i) {
        part_positions[i] = vec4(((10.0f * dist(rand)) - 1.0f) / 20.0f, 5.0 * dist(rand), 0.0f, 0.0f);
        velocitys[i] = vec4(0.0f, 0.1f + dist(rand), 0.0f, 0.0f);
    }

    // a useless vao, but we need it bound or we get errors.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
     //Generate Position Data buffer
    glGenBuffers(1, &G_Position_buffer);
    // Bind as GL_SHADER_STORAGE_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, G_Position_buffer);
    // Send Data to GPU, use GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* MAX_PARTICLES, part_positions, GL_DYNAMIC_DRAW);

    // Generate Velocity Data buffer
    glGenBuffers(1, &G_Velocity_buffer);
    // Bind as GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, G_Velocity_buffer);
    // Send Data to GPU, use GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* MAX_PARTICLES, velocitys, GL_DYNAMIC_DRAW);
     //Unbind
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Set camera properties
    cam.set_position(vec3(0.0f, 50.0f, -75.0f));
    cam.set_target(vec3(0.0f, 0.0f, 0.0f));
    cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

    return true;
}

bool update(float delta_time) {

    //Handling camera positions
    if (glfwGetKey(renderer::get_window(), '1')) {
        cam.set_position(vec3(50, 10, 50));
    }
    if (glfwGetKey(renderer::get_window(), '2')) {
        cam.set_position(vec3(-50, 10, 50));
    }
    if (glfwGetKey(renderer::get_window(), '3')) {
        cam.set_position(vec3(-50, 10, -50));
    }
    if (glfwGetKey(renderer::get_window(), '4')) {
        cam.set_position(vec3(50, 10, -50));
    }

    //USE THIS TO FOCUS ON THE FIRE
    if (glfwGetKey(renderer::get_window(), '5')) {
        cam.set_position(vec3(0.0f, 2.5f, 10.0f));
        cam.set_target(vec3(0.0f, 0, 0.0f));
        cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
    }

    //Handling post processing effects
    if (glfwGetKey(renderer::get_window(), 'R')) {
        enablePostProc = true;
    }
    if (glfwGetKey(renderer::get_window(), 'T')) {
        enablePostProc = false;
    }
    //FPS Counter
    cout << "FPS: " << 1.0f / delta_time << endl;
    //Take position for shadows from spot light
    shadow.light_position = spot.get_position();
    //Take shadow direction for shadows from spot light
    shadow.light_dir = spot.get_direction();

    //Bind computing effect
    renderer::bind(comp_fire);
    //Get uniforms for computing effect from update
    glUniform3fv(comp_fire.get_uniform_location("max_dims"), 1, &(vec3(3.0f, 5.0f, 5.0f))[0]);
    glUniform1f(comp_fire.get_uniform_location("delta_time"), delta_time);
    // Rotate the box
    meshes["box"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

    // Rotate and translate sphere
    meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);
    meshes["sphere"].get_transform().translate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

    // Translate balloon
    meshes["balloon"].get_transform().translate(vec3(0.0f, 1.0f, 0.0f) * delta_time);
    //Update the camera
    cam.update(delta_time);
    return true;
}

void greyScale() 
{
    // Bind grey effect
    renderer::bind(grey);

    // Set MVP as identity matrix and set uniform
    auto MVP = mat4(1.0f);
    glUniformMatrix4fv(grey.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

    // Get texture from frame buffer and set uniform
    renderer::bind(frame.get_frame(), 0);
    glUniform1i(grey.get_uniform_location("tex"), 0);
    // Render the screen quad
    renderer::render(screen_quad);
}

void contrastEff()
{
    // Bind contrast effect
    renderer::bind(contrast);

    // Set MVP as identity matrix and set uniform
    auto MVP = mat4(1.0f);
    glUniformMatrix4fv(contrast.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

    // Get texture from frame buffer and set uniform
    renderer::bind(frame.get_frame(), 0);
    glUniform1i(contrast.get_uniform_location("tex"), 0);

    // Render screen quad
    renderer::render(screen_quad);
}

void grainEff() {
    // Bind grain effect
    renderer::bind(grain);

    // Set MVP as identity matrix and set uniform
    auto MVP = mat4(1.0f);
    glUniformMatrix4fv(grain.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
    //Set the screen resolution uniform casting the returning variables to float
    glUniform2f(grain.get_uniform_location("resolution"), (float)renderer::get_screen_width(), (float)renderer::get_screen_height());

    // Get texture from frame buffer and set uniform
    renderer::bind(frame.get_frame(), 0);
    glUniform1i(grain.get_uniform_location("tex"), 0);

    // Set time uniform
    glUniform1f(grain.get_uniform_location("time"), (float)glfwGetTime());

    // Render screen quad
    renderer::render(screen_quad);
}
bool render() {

    // We could just use the Camera's projection, 
    // but that has a narrower FoV than the cone of the spot light, so we would get clipping.
    // so we have to create a new Proj Mat with a field of view of 90.
    mat4 LightProjMat = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.f);

    // Setting render target to frame and clearing
    renderer::set_render_target(frame);
    renderer::clear();
    // Binding main effect
    renderer::bind(eff);
    for (auto& e : meshes)
    {
        auto m = e.second;
        // Create MVP matrix
        auto M = m.get_transform().get_transform_matrix();
        auto V = cam.get_view();
        auto P = cam.get_projection();
        auto MVP = P * V * M;

        // Assign uniform values for MVP, M and N 
        glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
        glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
        glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

        // MVP matrix based on shadow projection and view
        auto lightMVP = LightProjMat * shadow.get_view() * M;
        glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));
        //Binding material and light
        renderer::bind(m.get_material(), "mat");
        renderer::bind(spot, "spot");
        //Binding texture and assign uniform
        renderer::bind(tex, 0);
        glUniform1i(eff.get_uniform_location("tex"), 0);
        //Assign uniform for eye position
        glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
        //Binding and assigning shadow map
        renderer::bind(shadow.buffer->get_depth(), 1);
        glUniform1i(eff.get_uniform_location("shadow_map"), 1);
        renderer::render(m);
    }
    // Binding computing effect
    renderer::bind(comp_fire);
    // Bind data as SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, G_Position_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, G_Velocity_buffer);
    // Dispatch
    glDispatchCompute(MAX_PARTICLES / 128, 1, 1);
    // Sync, wait for completion
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Bind render effect
    renderer::bind(fire);
    // Create MV matrix
    mat4 M(1.0f);
    auto V = cam.get_view();
    auto MV = M * V;
    // Set the colour uniform
    glUniform4fv(fire.get_uniform_location("colour"), 1, value_ptr(vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    // Set MV, and P matrix uniforms seperatly
    glUniformMatrix4fv(fire.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));
    glUniformMatrix4fv(fire.get_uniform_location("P"), 1, GL_FALSE, value_ptr(cam.get_projection()));
    // Set point_size size uniform to .1f
    glUniform1f(fire.get_uniform_location("point_size"), 0.6f);
    // Bind particle texture
    renderer::bind(ftex, 0);
    glUniform1i(fire.get_uniform_location("tex"), 0);

    // Bind position buffer as GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, G_Position_buffer);
    // Setup vertex format
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Enable Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Disable Depth Mask
    glDepthMask(GL_FALSE);
    // Render
    glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
    // Tidy up, enable depth mask
    glDepthMask(GL_TRUE);
    // Disable Blend
    glDisable(GL_BLEND);
    // Unbind all arrays
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    // Set target to shadowmap
    renderer::set_render_target(shadow);
    // Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    // Set face cull to front
    glCullFace(GL_FRONT);

    //Binding shadow effect
    renderer::bind(shadow_eff);
    for (auto& e : meshes) {
        auto m = e.second;
        auto M = m.get_transform().get_transform_matrix();
        auto V = shadow.get_view();
        auto MVP = LightProjMat * V * M;
        glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
        renderer::render(m);
    }

    glCullFace(GL_BACK);  

    //Function used to toggle on and off contrast and greyScale
    if (enablePostProc) {
        grey_frame = frame;
        renderer::set_render_target(grey_frame);
        greyScale();
    }
    else {
        contrast_frame = grey_frame;
        renderer::set_render_target(contrast_frame);
        contrastEff();
    }

    // Set render back to screen and use grain effect as a last touch
    renderer::set_render_target();
    grainEff();
    return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}