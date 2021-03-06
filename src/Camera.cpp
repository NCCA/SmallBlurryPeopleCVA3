#include <algorithm>
#include <iostream>
#include <ngl/NGLStream.h>

#include <ngl/Util.h>
#include "Camera.hpp"
#include "Utility.hpp"

Camera::Camera()
    :
      m_ipos(ngl::Vec3(0.0f, 0.0f, 0.0f), ngl::Vec3(0.0f, 0.0f, 0.0f), 0.08f),
      m_irot(ngl::Vec2(1.0f, 0.0f), ngl::Vec2(1.0f, 0.0f), 0.14f),
      m_iFocalDepth(0.0f, 0.0f, 0.2f),
      m_iDolly(10.0f, 10.0f, 0.08f)
{
    m_up = ngl::Vec3(0.0f, 1.0f, 0.0f);
    m_fov = 60.0f;

    m_maxPitch = 60.0f;
    m_minPitch = 0.0f;

    m_minDolly = 5.0f;
    m_maxDolly = 35.0f;
}

void Camera::calculateViewMat()
{
    setInitPos(ngl::Vec3(0.0f, 0.0f, m_iDolly.get()));
    movePivot(m_ipos.get());
    rotateCamera(m_irot.get().m_x, 0.0f, 0.0f);
    rotateCamera(0.0f, m_irot.get().m_y, 0.0f);

    m_cameraTransformation = ngl::Mat4();
    m_pivotTransformation = ngl::Mat4();

    for(auto &i : m_cameraTransformationStack)
        m_cameraTransformation *= i;
    for(auto &i : m_pivotTransformationStack)
        m_pivotTransformation *= i;

    m_V = ngl::lookAt(
                m_initPos,
                m_initPivot,
                m_up
                );

    m_V *= m_cameraTransformation;
    m_V *= m_pivotTransformation;

    ngl::Mat4 VI = m_V.inverse();
    ngl::Mat4 wti = m_pivotTransformation.inverse();
    m_pos = ngl::Vec3(VI.m_30, VI.m_31, VI.m_32);
    m_pivot = ngl::Vec3(wti.m_30, wti.m_31, wti.m_32);

    m_VP = m_V * m_P;
}

void Camera::updateSmoothCamera()
{
    m_ipos.update();
    m_irot.update();
    m_iFocalDepth.update();
    m_iDolly.update();
    //std::cout << "cur pos " << m_curPos << ", cur rot " << m_curRot << ", cur focal depth " << m_curFocalDepth << " cur dolly " << m_curDolly << '\n';
}

void Camera::calculateProjectionMat()
{
    m_P = ngl::perspective(
                m_fov,
                m_aspect,
                0.1f,
                512.0f
                );
}

void Camera::rotateCamera(const float _pitch, const float _yaw, const float _roll)
{
    m_cameraTransformationStack.push_back( rotationMatrix(_pitch, _yaw, _roll) );
}

void Camera::rotatePivot(const float _pitch, const float _yaw, const float _roll)
{
    m_pivotTransformationStack.push_back( rotationMatrix(_pitch, _yaw, _roll) );
}

void Camera::moveCamera(const ngl::Vec3 _translation)
{
    m_cameraTransformationStack.push_back( translationMatrix(_translation) );
}

void Camera::movePivot(const ngl::Vec3 _translation)
{
    m_pivotTransformationStack.push_back( translationMatrix(_translation) );
}

ngl::Mat4 Camera::rotationMatrix(float _pitch, float _yaw, float _roll)
{
    ngl::Mat4 p;
    ngl::Mat4 y;
    ngl::Mat4 r;

    p.rotateX( _pitch );
    y.rotateY( _yaw );
    r.rotateZ( _roll );

    ngl::Mat4 ret = p * y * r;
    return ret;
}

ngl::Mat4 Camera::translationMatrix(const ngl::Vec3 &_vec)
{
    ngl::Mat4 trans;
    trans.translate( _vec.m_x, _vec.m_y, _vec.m_z );
    return trans;
}

void Camera::moveRight(const float _d)
{
    m_ipos.incrEnd( right() * _d );
}

void Camera::moveForward(const float _d)
{
    ngl::Vec3 diff = forwards() * _d;
    diff.m_y = 0.0f;
    m_ipos.incrEnd( diff );
}

void Camera::move(const ngl::Vec3 _d)
{
    m_ipos.incrEnd( _d );
}

void Camera::moveScreenSpace(const ngl::Vec3 _d)
{
  moveRight(_d.m_x);

  move(ngl::Vec3(0, _d.m_y, 0));

  moveForward(_d.m_z);
}

void Camera::rotate(const float _pitch, const float _yaw)
{
    ngl::Vec2 targ = m_irot.getEnd();
    targ.m_x = std::min( std::max( targ.m_x + _pitch, m_minPitch ), m_maxPitch );

    m_irot.setEnd( ngl::Vec2(targ.m_x, targ.m_y + _yaw) );
}

void Camera::dolly(const float _d)
{
    float dolly = m_iDolly.getEnd();
    dolly += _d;
    dolly = std::max( std::min( dolly, m_maxDolly ), m_minDolly );
    m_iDolly.setEnd( dolly );
}

void Camera::setPos(const ngl::Vec3 _p)
{
    m_ipos.setEnd( _p );
}

std::array<ngl::Vec3, 8> Camera::calculateCascade(float _start, float _end)
{
    std::array<ngl::Vec3, 8> cascade;

    //Distance from the look vector, on the right vector
    float horizontal = tanf( Utility::radians(m_fov) / 1.0f );

    //float verticalFOV = m_fov / m_aspect;

    float verticalFOV = 2.0f * atan( tanf(Utility::radians(m_fov) / 2.0f) / m_aspect );

    float vertical = tanf(verticalFOV);

    _start = -_start;
    _end = -_end;

    float sh = _start * horizontal;
    float sv = _start * vertical;
    float eh = _end * horizontal;
    float ev = _end * vertical;

    //Near plane
    cascade[0] = ngl::Vec3( -sh, sv, _start ); //Top left
    cascade[1] = ngl::Vec3( sh, sv, _start ); //Top right
    cascade[2] = ngl::Vec3( -sh, -sv, _start ); //Bottom left
    cascade[3] = ngl::Vec3( sh, -sv, _start ); //Bottom right

    //Far plane
    cascade[4] = ngl::Vec3( -eh, ev, _end ); //Top left
    cascade[5] = ngl::Vec3( eh, ev, _end ); //Top right
    cascade[6] = ngl::Vec3( -eh, -ev, _end ); //Bottom left.
    cascade[7] = ngl::Vec3( eh, -ev, _end ); //Bottom right

    //Project from view space to world space
    ngl::Mat4 iv = m_V;
    //iv = iv.transpose();
    iv = iv.inverse();

    for(auto &i : cascade)
    {
        ngl::Vec4 i4 = ngl::Vec4( i.m_x, i.m_y, i.m_z, 1.0f );
        i4 = i4 * iv;
        i = ngl::Vec3( i4.m_x, i4.m_y, i4.m_z );
    }

    //Return cascade in world space.
    return cascade;
}
