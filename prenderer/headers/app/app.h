//
//  app.h
//
//  Created by Jonathan Tompson on 5/1/12.
//

#pragma once

#include <thread>
#include <string>
#include <random>
#include "jtil/jtil.h"

#define NUM_PT_LIGHTS 128
#define PT_LIGHT_ANIM_XDIM 15.0f
#define PT_LIGHT_ANIM_YDIM 15.0f
#define PT_LIGHT_ANIM_ZDIM 10.0f
#define PT_LIGHT_START_VEL 10.0f
#define PT_LIGHT_MAX_VEL 20.0f

#if defined(_WIN32)
class DebugBuf;
#endif

namespace app {

  class App {
  public:
    App();
    ~App();

    // Static methods for interfacing with the global singleton
    static void newApp();  // Creates the g_app singleton
    static void initApp();  // Init routines
    static void runApp();  // Triggers main event loop
    static void killApp();  // Call once when you really want to shut down

    // Getter methods
    static inline App* app() { return g_app_; }
    
    static inline void requestShutdown() { g_app_->app_running_ = false; }
    static inline bool appRunning() { return g_app_->app_running_; }
    static void keyboardCB(int key, int scancode, int action, int mods);
    static void mousePosCB(double x, double y);
    static void mouseButtonCB(int button, int action, int mods);
    static void mouseWheelCB(double xoffset, double yoffset);

  private:
    static App* g_app_;  // Global singleton

#if defined(_WIN32)
    static DebugBuf* debug_buf;
#endif
    bool app_running_;

    jtil::clk::Clk* clk_;
    jtil::math::Double2 mouse_pos_;
    jtil::math::Double2 mouse_pos_old_;
    double frame_time_;
    double frame_time_prev_;

    // Light animation data
    jtil::math::Float3 light_vel_[NUM_PT_LIGHTS];
    RAND_ENGINE rand_eng_;
    NORM_DIST<float>* rand_norm_;
    UNIF_DIST<float>* rand_uni_;

    jtil::renderer::GeometryInstance* wrist_bone;

    jtil::renderer::GeometryInstance* robot_face;
    jtil::renderer::GeometryInstance* robot_torso;
    jtil::renderer::GeometryInstance* robot_r_shoulder;
    jtil::renderer::GeometryInstance* robot_l_shoulder;
    jtil::renderer::GeometryInstance* robot_r_forearm;
    jtil::renderer::GeometryInstance* robot_l_forearm;

    void run();
    void init();
    static void resetScreenCB();
    static int closeWndCB();
    void moveCamera(const double dt);
    void moveStuff(const double dt);  // Temporary: just to play with renderer
    void addStuff();
    void registerNewRenderer();

    // Non-copyable, non-assignable.
    App(App&);
    App& operator=(const App&);
  };

};  // namespace app
