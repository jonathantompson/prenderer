#include <stdio.h>  // for printf
#include <thread>
#include <string>
#include <iostream>  // for cout
#include "app/app.h"

#ifndef NULL
#define NULL 0
#endif
#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }

#define LOAD_JBIN_FILES

using std::wruntime_error;
using namespace jtil;
using namespace jtil::data_str;
using namespace jtil::clk;
using namespace jtil::renderer;
using namespace jtil::settings;
using namespace jtil::math;
using namespace jtil::settings;

namespace app {

  App* App::g_app_ = NULL;
#if defined(_WIN32)
  DebugBuf* App::debug_buf = NULL;
  std::streambuf* old_cout_buf = NULL;
#endif

  App::App() {
    app_running_ = false;
    clk_ = NULL;
    rand_norm_ = new NORM_DIST<float>(0.0f, 1.0f);
    rand_uni_ = new UNIF_DIST<float>(-1.0f, 1.0f);

    wrist_bone = NULL;
    robot_face = NULL;
    robot_torso = NULL;
    robot_r_shoulder = NULL;
    robot_l_shoulder = NULL;
    robot_r_forearm = NULL;
    robot_l_forearm = NULL;
  }

  App::~App() {
    SAFE_DELETE(clk_);
    SAFE_DELETE(rand_norm_);
    SAFE_DELETE(rand_uni_);
    Renderer::ShutdownRenderer();
  }

  void App::newApp() {
#if defined(_WIN32)
    // If in windows and using a debug build we should also redirect the std
    // output to the debug window (in visual studio)
    if (IsDebuggerPresent()) {
      debug_buf = new DebugBuf();
      old_cout_buf = std::cout.rdbuf(debug_buf);  // for 'std::cout << x' calls
      std::cout << "WARNING: std::cout redirected to Debug window."
        << " Remember to call '<< std::endl' to flush." << std::endl;
      // TO DO: WORK OUT HOW TO REDIRECT PRINTF
    }
#endif
    g_app_ = new App();
  }

  void App::initApp() {
    g_app_->init();
  }


  void App::init() {
    Renderer::InitRenderer();
    registerNewRenderer();

    clk_ = new Clk();
    frame_time_ = clk_->getTime();
    app_running_ = true;
  }

  void App::killApp() {
    SAFE_DELETE(g_app_);

#if defined(_WIN32)
    // make sure to restore the original cout so we don't get a crash on close!
    std::cout << std::endl;  // Force a flush on exit
    std::cout.rdbuf(old_cout_buf);
    SAFE_DELETE(debug_buf);
#endif
  }

  void App::runApp() {
    g_app_->run();
  }

  void App::registerNewRenderer() {
    Renderer::g_renderer()->registerKeyboardCB(App::keyboardCB);
    Renderer::g_renderer()->registerMousePosCB(App::mousePosCB);
    Renderer::g_renderer()->registerMouseButCB(App::mouseButtonCB);
    Renderer::g_renderer()->registerMouseWheelCB(App::mouseWheelCB);
    Renderer::g_renderer()->registerResetScreenCB(App::resetScreenCB);
    Renderer::g_renderer()->getMousePosition(g_app_->mouse_pos_);
    Renderer::g_renderer()->registerCloseWndCB(App::closeWndCB);
    g_app_->addStuff();
  }

  void App::run() {
    while (app_running_) {
      frame_time_prev_ = frame_time_;
      frame_time_ = clk_->getTime();
      double dt = frame_time_ - frame_time_prev_;

      // Update camera based on real-time inputs
      if (!Renderer::ui()->mouse_over_ui()) {
        moveCamera(dt);
      }
      moveStuff(dt);

      Renderer::g_renderer()->renderFrame();
 
      std::this_thread::yield();  // Give OS the opportunity to deschedule

      bool force_slow_framerate;
      GET_SETTING("force_slow_framerate", bool, force_slow_framerate);
      if (force_slow_framerate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
      }
    }
  }
  
  void App::moveCamera(double dt) {
    renderer::Camera* camera = Renderer::g_renderer()->camera();
    
    // Update the mouse position
    mouse_pos_old_.set(mouse_pos_);
    bool in_screen = Renderer::g_renderer()->getMousePosition(mouse_pos_);
    bool left_mouse_button = Renderer::g_renderer()->getMouseButtonStateLeft();
    
    // Rotate the camera if user wants to
    if (!math::Double2::equal(mouse_pos_, mouse_pos_old_) && 
        in_screen && left_mouse_button) {
      float dx = static_cast<float>(mouse_pos_[0] - mouse_pos_old_[0]);
      float dy = static_cast<float>(mouse_pos_[1] - mouse_pos_old_[1]);
      float camera_speed_rotation;
      GET_SETTING("camera_speed_rotation", float, camera_speed_rotation);
      camera->rotateCamera(dx * camera_speed_rotation,
                           dy * camera_speed_rotation);
    }
    
    // Move the camera if the user wants to
    Float3 cur_dir(0.0f, 0.0f, 0.0f);
    bool W = Renderer::g_renderer()->getKeyState(static_cast<int>('W'));
    bool A = Renderer::g_renderer()->getKeyState(static_cast<int>('A'));
    bool S = Renderer::g_renderer()->getKeyState(static_cast<int>('S'));
    bool D = Renderer::g_renderer()->getKeyState(static_cast<int>('D'));
    bool Q = Renderer::g_renderer()->getKeyState(static_cast<int>('Q'));
    bool E = Renderer::g_renderer()->getKeyState(static_cast<int>('E'));
    bool LShift = Renderer::g_renderer()->getKeyState(KEY_LSHIFT);
    if (W) {
      cur_dir[2] -= 1.0f;
    } 
    if (S) {
      cur_dir[2] += 1.0f;
    }
    if (A) {
      cur_dir[0] -= 1.0f;
    }
    if (D) {
      cur_dir[0] += 1.0f;
    }
    if (Q) {
      cur_dir[1] -= 1.0f;
    }
    if (E) {
      cur_dir[1] += 1.0f;
    }
    if (!(cur_dir[0] == 0.0f && cur_dir[1] == 0.0f && cur_dir[2] == 0.0f)) {
      cur_dir.normalize();
      float camera_speed = 1.0f;
      if (LShift) {
        GET_SETTING("camera_speed_fast", float, camera_speed);
      } else {
        GET_SETTING("camera_speed", float, camera_speed);        
      }
      Float3::scale(cur_dir, camera_speed * static_cast<float>(dt));
      camera->moveCamera(cur_dir);      
    }
  }

  void App::moveStuff(const double dt) {
    // TEMP CODE: (for testing)
    static float t_physics = 0;
    static float t_lights = 0;
    static float t_models = 0;
    bool pause_physics, animate_lights, animate_models;
    GET_SETTING("pause_physics", bool, pause_physics);
    GET_SETTING("animate_lights", bool, animate_lights);
    GET_SETTING("animate_models", bool, animate_models);
    float dtf = (float)dt;
    
    if (!pause_physics) {
      t_physics += dtf;
      Float4x4 mat_prot;
      Float4x4::rotateMatYAxis(mat_prot, 10.0f * (float)(dt));
      Float4x4 mat_nrot(mat_prot);
      mat_nrot.transpose();
      Float4x4 mat_tmp;
      GeometryInstance* g = Renderer::g_renderer()->geometry_manager()->scene_root();
      for (uint32_t i = 0; i < g->numChildren()-2; i++) {
        if (i % 2) {
          Float4x4::multSIMD(mat_tmp, mat_prot, g->getChild(i)->mat());
        } else {
          Float4x4::multSIMD(mat_tmp, mat_nrot, g->getChild(i)->mat());
        }
        g->getChild(i)->mat().set(mat_tmp);
      }
    }
    if (animate_lights) {
      t_lights += dtf;
      uint32_t cur_pt_light = 0;
      Float3 acc;
      const VectorManaged<Light*>& lights = Renderer::g_renderer()->lights();
      for (uint32_t i = 0; i < lights.size(); i++) {
        if (cur_pt_light < NUM_PT_LIGHTS && 
          lights[i]->type() == LightType::LIGHT_POINT) {
          Float3& pos = ((LightPoint*)lights[i])->pos_world();
          Float3& vel = light_vel_[cur_pt_light];
          // Update position
          pos[0] = pos[0] + vel[0] * dtf;
          pos[1] = pos[1] + vel[1] * dtf;
          pos[2] = pos[2] + vel[2] * dtf;
          // Update accelleration
          float orbit_coeff = Float3::dot(pos, pos) * 0.005f;
          acc[0] = (*rand_norm_)(rand_eng_) * 10.0f - pos[0] * orbit_coeff;
          acc[1] = (*rand_norm_)(rand_eng_) * 10.0f - pos[1] * orbit_coeff;
          acc[2] = (*rand_norm_)(rand_eng_) * 10.0f - pos[2] * orbit_coeff;
          // Update velocity
          vel[0] = vel[0] + acc[0] * dtf;
          vel[1] = vel[1] + acc[1] * dtf;
          vel[2] = vel[2] + acc[2] * dtf;
          // Clamp velocity to some max
          float vel_length = vel.length();
          if (abs(vel_length) > PT_LIGHT_MAX_VEL) {
            float vel_scale =  PT_LIGHT_MAX_VEL / abs(vel_length);
            Float3::scale(vel, vel_scale);
          }

          cur_pt_light++;
        }
      }
    }

    if (animate_models) {
      t_models += dtf;
      wrist_bone->mat().set(wrist_bone->bone()->rest_transform);

      float angle1 = 0.5f * (PerlinNoise::Noise(0.0f, t_models));
      float angle2 = 0.35f * (PerlinNoise::Noise(0.0f, 2.0f * t_models));
      wrist_bone->mat().leftMultRotateZAxis(angle1);
      wrist_bone->mat().leftMultRotateXAxis(angle2);

      Float3 tmp1, tmp2, head_pos;
      tmp1.zeros();
      Float3::affineTransformPos(tmp2, robot_face->bone()->bone_offset, tmp1);
      Float3::affineTransformPos(tmp1, robot_face->mat_hierarchy(), tmp2);
      Float3::affineTransformPos(head_pos, 
        *robot_face->bone_root_node()->mat_hierarchy_inv(), tmp1);

      Camera* camera = Renderer::g_renderer()->camera();
      Float3 robot_eye_vec;
      Float3::sub(robot_eye_vec, camera->eye_pos_world(), head_pos);
      robot_eye_vec.normalize();
      Float3 robot_eye_rest(0.0f, 0.0f, -1.0f);
      Float3 rot_axis;
      Float3::cross(rot_axis, robot_eye_vec, robot_eye_rest);
      if (rot_axis.length() > LOOSE_EPSILON) {
        rot_axis.normalize();
        float rot_angle = -acosf(Float3::dot(robot_eye_vec, robot_eye_rest));
        rot_angle = std::max<float>(std::min<float>(rot_angle, (float)M_PI / 8.0f),
          -(float)M_PI / 8.0f);
        rot_angle += 0.1f * (PerlinNoise::Noise(0.0f, 0.5f * t_models));
        Float4x4 rot_mat;
        Float4x4 new_face_mat;
        Float4x4::rotateMatAxisAngle(rot_mat, rot_axis, rot_angle);
        Float4x4::multSIMD(new_face_mat, rot_mat, robot_face->bone()->rest_transform);
        robot_face->mat().set(new_face_mat);
      }
    }
  }

  void App::addStuff() {
    GeometryInstance* model; 
    static_cast<void>(model);

    model = Renderer::g_renderer()->geometry_manager()->makeTorusKnot(lred, 7, 64, 512);
    model->mat().leftMultTranslation(3.0f, 0.0f, 0.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    model = Renderer::g_renderer()->geometry_manager()->makeSphere(25, 25, 1.0f, white);
    model->mat().leftMultTranslation(6.0f, -2.5f, 0.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    model = Renderer::g_renderer()->geometry_manager()->makeSphere(25, 25, 1.0f, white);
    model->mat().leftMultTranslation(-6.0f, 5.0f, 0.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    // Displacement quad test geometry
    model = Renderer::g_renderer()->geometry_manager()->makeDispQuad();
    model->mat().leftMultScale(4.5f, 4.6f, 5.0f);
    model->mat().leftMultTranslation(0.0f, -8.5f, 0.0f);
    model->mtrl().displacement_factor = 1.5f;
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

#ifndef LOAD_JBIN_FILES
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromFile(
      "./models/small_dog/", "small_dog.dae");
    Renderer::g_renderer()->geometry_manager()->saveModelToJBinFile("./models/small_dog/", 
      "small_dog.jbin", model);
#else
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromJBinFile(
      "./models/small_dog/", "small_dog.jbin");
#endif
    model->mat().scaleMat(4, 4, 4);
    model->mat().leftMultTranslation(2.5f, 0, 2.5f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

#ifndef LOAD_JBIN_FILES
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromFile(
      "./models/lib_hand/", "hand_palm_parent_medium_wrist.dae");
    Renderer::g_renderer()->geometry_manager()->saveModelToJBinFile(
      "./models/lib_hand/", "hand_palm_parent_medium_wrist.jbin", model);
#else
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromJBinFile(
      "./models/lib_hand/", "hand_palm_parent_medium_wrist.jbin");
#endif
    model->mat().leftMultScale(1.5f, 1.5f, 1.5f);
    model->mat().leftMultTranslation(0.0f, 5.0f, -3.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    wrist_bone = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/lib_hand/hand_palm_parent_medium_wrist.dae/carpals", model);
    if (!wrist_bone) {
      throw std::wruntime_error("App::addStuff() - ERROR: Couldn't find wrist");
    }

#ifndef LOAD_JBIN_FILES
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromFile( 
      "./models/robot-v2/", "robot-v2.dae", false, true, true);
    Renderer::g_renderer()->geometry_manager()->saveModelToJBinFile(
      "./models/robot-v2/", "robot-v2.jbin", model);
#else
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromJBinFile(
      "./models/robot-v2/", "robot-v2.jbin");
#endif
    math::Float4x4::rotateMatXAxis(model->mat(), (float)M_PI_2);
    model->mat().leftMultRotateYAxis((float)M_PI);
    model->mat().leftMultScale(0.75f, 0.75f, 0.75f);
    model->mat().leftMultTranslation(0.0f, -8.0f, 0.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    robot_face = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/cabeza", model);
    robot_torso = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/pechito", model);
    robot_r_shoulder = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/brazo.R", model);
    robot_l_shoulder = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/brazo.L", model);
    robot_r_forearm = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/antebrazo.R", model);
    robot_l_forearm = renderer::GeometryManager::findGeometryInstanceByName(
      "./models/robot-v2/robot-v2.dae/antebrazo.L", model);
    if (!robot_face || !robot_torso || !robot_r_shoulder || !robot_l_shoulder
      || !robot_r_forearm || !robot_l_forearm) {
      throw std::wruntime_error("App::addStuff() - ERROR: Couldn't find all"
        " the robot bones!");
    }

#ifndef LOAD_JBIN_FILES
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromFile(
      "./models/crytek_sponza/", "sponza.obj", false, true, true);
    Renderer::g_renderer()->geometry_manager()->saveModelToJBinFile(
      "./models/crytek_sponza/", "sponza.jbin", model);
#else
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromJBinFile(
      "./models/crytek_sponza/", "sponza.jbin");
#endif
    math::Float4x4::rotateMatYAxis(model->mat(), (float)M_PI_2);
    model->mat().leftMultScale(0.04f, 0.04f, 0.04f);
    model->mat().leftMultTranslation(0.0f, -10.0f, 0.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

#ifndef LOAD_JBIN_FILES
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromFile(
      "./models/robot_hand/", "robot_hand.dae", false, false, true);
    Renderer::g_renderer()->geometry_manager()->saveModelToJBinFile(
      "./models/robot_hand/", "robot_hand.jbin", model);
#else
    model = Renderer::g_renderer()->geometry_manager()->loadModelFromJBinFile(
      "./models/robot_hand/", "robot_hand.jbin");
#endif
    math::Float4x4::rotateMatXAxis(model->mat(), -(float)M_PI_2);
    model->mat().leftMultScale(1.5f, 1.5f, 1.5f);
    model->mat().leftMultTranslation(0.0f, 8.0f, -3.0f);
    Renderer::g_renderer()->geometry_manager()->scene_root()->addChild(model);

    // Some lighting for testing: this also needs to be in an object manager
    // Spawn a bunch of point lights just above the ground
    LightPoint* light_point;
    for (uint32_t i = 0; i < NUM_PT_LIGHTS; i++) {
      // Assign a random starting velocity64
      light_vel_[i].set((*rand_uni_)(rand_eng_), (*rand_uni_)(rand_eng_),
        (*rand_uni_)(rand_eng_));
      Float3::scale(light_vel_[i], PT_LIGHT_START_VEL);
      light_point = new LightPoint();
      light_point->pos_world().set((*rand_uni_)(rand_eng_), 
        (*rand_uni_)(rand_eng_), (*rand_uni_)(rand_eng_));
      light_point->pos_world()[0] *= PT_LIGHT_ANIM_XDIM;
      light_point->pos_world()[1] *= PT_LIGHT_ANIM_YDIM;
      light_point->pos_world()[2] *= PT_LIGHT_ANIM_ZDIM;
      Float3 col = renderer::colors[(i+(uint32_t)i) % renderer::n_colors];
      light_point->diffuse_color().set(col[0], col[1], col[2]);
      light_point->near_far().set(0.1f, 4.0f);
      light_point->diffuse_intensity() = 1.0f;
      light_point->spec_intensity() = 0.2f;
      Renderer::g_renderer()->addLight(light_point);
    }

    light_point = new LightPoint();
    light_point->pos_world().set(0, 0, 0);
    light_point->pos_world().set(0, 0, 0);
    light_point->diffuse_color().set(1, 1, 1);
    light_point->near_far().set(0.1f, 3.0f);
    light_point->diffuse_intensity() = 1.0f;
    light_point->spec_intensity() = 0.5f;
    Renderer::g_renderer()->addLight(light_point);

    LightDir* light_dir = new LightDir();
    light_dir->diffuse_color().set(1, 1, 1);
    light_dir->diffuse_intensity() = 0.3f;
    light_dir->spec_intensity() = 0.2f;
    light_dir->dir_world().set(-1, 0, 0);
    Renderer::g_renderer()->addLight(light_dir);

    LightSpotCVSM* light_spot_vsm = new LightSpotCVSM(Renderer::g_renderer());
    light_spot_vsm->dir_world().set(0, -1, 0);
    light_spot_vsm->pos_world().set(0, 15, 0);
    light_spot_vsm->near_far().set(0.1f, 30.0f);
    light_spot_vsm->outer_fov_deg() = 35.0f;
    light_spot_vsm->diffuse_intensity() = 1.0f;
    light_spot_vsm->inner_fov_deg() = 30.0f;
    light_spot_vsm->cvsm_count(1);
    Renderer::g_renderer()->addLight(light_spot_vsm);

    ui::UI* ui = Renderer::g_renderer()->ui();
    ui->addHeadingText("Application Settings:");
    ui->addCheckbox("pause_physics", "Pause Physics (spacebar)");
    ui->addCheckbox("animate_lights", "Animate Lights");
    ui->addCheckbox("animate_models", "Animate Models");
    ui->addCheckbox("force_slow_framerate", "Add 20ms frame lag");
  }

  int App::closeWndCB() {
    g_app_->app_running_ = false;
    return 0;
  }

  void App::resetScreenCB() {
    g_app_->registerNewRenderer();
  }

  void App::keyboardCB(int key, int scancode, int action, int mods) {
    switch (key) {
    case KEY_ESC:
      if (action == PRESSED) {
        requestShutdown();
      }
      break;
    case KEY_SPACE:
      if (action == PRESSED) {
        bool pause_physics;
        GET_SETTING("pause_physics", bool, pause_physics);
        SET_SETTING("pause_physics", bool, !pause_physics);
        Renderer::g_renderer()->ui()->setCheckboxVal("pause_physics",
          !pause_physics);
      }
      break;
    default:
      break;
    }
  }
  
  void App::mousePosCB(double x, double y) {

  }
  
  void App::mouseButtonCB(int button, int action, int mods) {
 
  }
  
  void App::mouseWheelCB(double xoffset, double yoffset) {

  }

}  // namespace app
