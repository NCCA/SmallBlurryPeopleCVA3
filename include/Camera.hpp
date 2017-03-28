#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include <ngl/Vec2.h>

#include <vector>

/// @file Camera.hpp
/// @brief The camera class is essentially a wrapper around a load of mat4s. Similar to NGLs default camera class but it represents
/// both the camera and a lookat target. The user can transform both of these. When happy with the transformations they have set up,
/// they can call methods to calculate the view and projection matrices.

class Camera
{
public:
    /// @brief Default camera constructor.
    Camera();
    /// @brief Default camera destructor.
    ~Camera() = default;

    /// @brief Calculates the view matrix from the transformation of the camera about the pivot and the pivot in the world.
    void calculateViewMat();
    /// @brief Calculate the projection matrix. Unless you are changing the FOV or screen aspect, this shouldn't be called much.
    void calculateProjectionMat();

    /// @brief Updates all of the smooth camera values.
    void updateSmoothCamera();

    /// @brief Get view matrix.
    ngl::Mat4 getV() const {return m_V;}
    /// @brief Get projection matrix.
    ngl::Mat4 getP() const {return m_P;}
    /// @brief Get the view-project matrix (this is calculated in calculateViewMat and calculateProjectionMat.
    ngl::Mat4 getVP() const {return m_VP;}

    /// @brief Gets the world space position of the camera. I find this useful for fancy shaders.
    ngl::Vec3 getPos() const {return m_pos;}
    /// @brief Returns the world space position of the pivot.
    ngl::Vec3 getPivot() const {return m_pivot;}

    float getAspect() const {return m_aspect;}

    /// @brief Set aspect ratio (x / y).
    void setAspect(const float _aspect) {m_aspect = _aspect;}
    /// @brief Set horizontal (?) field of view.
    void setFOV(const float _fov) {m_fov = _fov;}

    /// @brief Sets the initial position of the camera, prior to transformation about and of the pivot.
    void setInitPos(const ngl::Vec3 &_pos) {m_initPos = _pos;}
    /// @brief Sets the initial pivot position, it may be transformed later.
    void setInitPivot(const ngl::Vec3 &_pivot) {m_initPivot = _pivot;}
    /// @brief Sets the up vector. [0, 1, 0] is usually pretty appropriate.
    void setUp(const ngl::Vec3 &_up) {m_up = _up;}

    /// @brief Empties the transformation stacks.
    void clearTransforms() {m_cameraTransformationStack.clear(); m_pivotTransformationStack.clear();}

    /// @brief Gets the back vector of the camera. Just ignore the transpose stuff, it works I swear.
    ngl::Vec3 back() {return m_V.transpose().getBackVector();}
    /// @brief Gets the forwards vector of the camera. Just ignore the transpose stuff, it works I swear.
    ngl::Vec3 forwards() {return m_V.getForwardVector();}
    /// @brief Gets the up vector of the camera. Just ignore the transpose stuff, it works I swear.
    ngl::Vec3 up() {return m_V.getUpVector();}
    /// @brief Gets the right vector of the camera. Just ignore the transpose stuff, it works I swear.
    ngl::Vec3 right() {return m_V.transpose().getRightVector();}

    /// @brief Returns the 8 corners of a frustum starting at _start units from the camera, and ending _end units away.
    /// These vertices are in world space.
    std::array<ngl::Vec3, 8> calculateCascade(float _start, float _end);

    /// @brief Moves the camera right, or forwards, relative to its orientation.
    void moveRight(const float _d);
    void moveForward(const float _d);
    void move(const ngl::Vec3 _d);
    void dolly(const float _d);
    void setPos(const ngl::Vec3 _p);
    ngl::Vec3 getTargPos() const {return m_targPos;}

    /// @brief Rotates the camera.
    void rotate(const float _pitch, const float _yaw);

    void setMovementSmoothness(const float _s) {m_movementSmoothness = _s;}
    void setRotationSmoothness(const float _s) {m_rotationSmoothness = _s;}
    void setFocalSmoothness(const float _s) {m_focalSmoothness = _s;}
    void setDollySmoothness(const float _s) {m_dollySmoothness = _s;}

    float getTargetDolly() const {return m_curDolly;}

    float getFocalDepth() const {return m_curFocalDepth;}

    void immediateTransform(const ngl::Mat4 &_mat) {m_V *= _mat; m_VP = m_V * m_P;
                                                        }
private:
    /// @brief Holds the horizontal fov in degrees.
    float m_fov;
    /// @brief Holds screenwidth / screenheight.
    float m_aspect;

    /// @brief The world-space position of the camera.
    ngl::Vec3 m_pos;
    /// @brief The pre-transformation position of the camera.
    ngl::Vec3 m_initPos;
    /// @brief The world-space position of the pivot.
    ngl::Vec3 m_pivot;
    /// @brief The pre-transformation position of the pivot.
    ngl::Vec3 m_initPivot;

    /// @brief Stack of transformations representing the cameras transformation about the pivot.
    std::vector<ngl::Mat4> m_cameraTransformationStack;
    /// @brief During calculation, the camera transformation stack is multiplied down into this single matrix.
    ngl::Mat4 m_cameraTransformation;
    /// @brief Stack of transformations representing the pivots transformation about the origin.
    std::vector<ngl::Mat4> m_pivotTransformationStack;
    /// @brief During calculation, the pivot transformation stack is multiplied down into this single matrix.
    ngl::Mat4 m_pivotTransformation;

    /// @brief The projection matrix.
    ngl::Mat4 m_P;
    /// @brief The view matrix.
    ngl::Mat4 m_V;
    /// @brief The view-project matrix.
    ngl::Mat4 m_VP;

    /// @brief The up direction.
    ngl::Vec3 m_up = ngl::Vec3(0.0f, 1.0f, 0.0f);

    /// @brief Target and current position, for smooth camera
    ngl::Vec3 m_targPos;
    ngl::Vec3 m_curPos;

    /// @brief Target and current rotation, for smooth camera
    ngl::Vec2 m_targRot;
    ngl::Vec2 m_curRot;

    /// @brief Clamp rotation to these values.
    float m_minPitch;
    float m_maxPitch;

    /// @brief Target and current focal depth, for smooth camera
    float m_targFocalDepth;
    float m_curFocalDepth;

    /// @brief Target and current dollying, for smooth camera
    float m_targDolly;
    float m_curDolly;

    /// @brief Controls the smoothness of movement.
    float m_movementSmoothness;

    /// @brief Controls the smoothness of rotation.
    float m_rotationSmoothness;

    /// @brief Controls the smoothness of focal depth change.
    float m_focalSmoothness;

    /// @brief Controls the smoothness of dollying.
    float m_dollySmoothness;

    //These functions are too low-level for general use. I have created public functions like moveRight and moveForwards etc,
    //which call these. Some are used raw for specialised transformations, such as flipping the camera upside down to draw reflections.
    /// @brief Moves the camera (not the pivot).
    void moveCamera(const ngl::Vec3 _translation);
    /// @brief Moves the pivot ( and the camera).
    void movePivot(const ngl::Vec3 _translation);
    /// @brief Rotates the camera around the pivot.
    void rotateCamera(const float _pitch, const float _yaw, const float _roll);
    /// @brief Rotates the pivot and the camera. I don't think I ever really use this function but it's here if you want it.
    void rotatePivot(const float _pitch, const float _yaw, const float _roll);
    /// @brief Transform the camera (allows for customised transforms).
    void transformCamera(const ngl::Mat4 _trans) {m_cameraTransformationStack.push_back(_trans);}
    /// @brief Transform the pivot (allows for customised transforms).
    void transformPivot(const ngl::Mat4 _trans) {m_pivotTransformationStack.push_back(_trans);}
    /// @brief Returns a rotation matrix given pitch, yaw and roll.
    ngl::Mat4 rotationMatrix(float _pitch, float _yaw, float _roll);
    /// @brief Returns a mat4 which will tranlate a given point by _vec.
    ngl::Mat4 translationMatrix(const ngl::Vec3 &_vec);
};


#endif
