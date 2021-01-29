/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: a 3D camera class
Language: MSVC c++17
Platform: win32 compiled in Visual Studio 2019
Project: jordan.h_CS300_1
Author: Jordan Hoffmann, jordan.h
Creation date: 9/28/2020
End Header --------------------------------------------------------*/

#ifndef CS250_CAMERA_H
#define CS250_CAMERA_H

#include <glm/glm.hpp>

class Camera {
  public:
    Camera(void);
    Camera(const glm::vec4 &E, const glm::vec4 &look, const glm::vec4 &rel,
           float fov, float aspect, float N, float F);
    [[nodiscard]] glm::vec4 eye(void) const               { return eye_point; }
    [[nodiscard]] glm::vec4 right(void) const             { return right_vector; }
    [[nodiscard]] glm::vec4 up(void) const                { return up_vector; }
    [[nodiscard]] glm::vec4 back(void) const              { return back_vector; }
    [[nodiscard]] glm::vec4 viewport(void) const;
    [[nodiscard]] float near(void) const                  { return near_distance; }
    [[nodiscard]] float far(void) const                   { return far_distance; }
    Camera& zoom(float factor);
    Camera& forward(float distance);
    Camera& yaw(float angle);
    Camera& pitch(float angle);
    Camera& roll(float angle);

    void UpdateGPUCamera() const;
  private:
    glm::vec4 eye_point;
    glm::vec4 right_vector, up_vector, back_vector;
    float vp_width, vp_height, vp_distance,
          near_distance, far_distance;
};

// The next three functions will be implemented in a later assignment.
// DO NOT implement them in assignment #3.
glm::mat4 model(const Camera& cam);
glm::mat4 view(const Camera& cam);
glm::mat4 perspective(const Camera& cam);


#endif

