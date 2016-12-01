#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Grid.hpp"
#include "Camera.hpp"
#include "Character.hpp"

class Scene
{
public:
		Scene();
		~Scene() = default;
		void draw();

		void mousePressEvent(const SDL_MouseButtonEvent &_event);
		void mouseMoveEvent (const SDL_MouseMotionEvent &_event);
		void mouseReleaseEvent(const SDL_MouseButtonEvent &_event);
		void wheelEvent(const SDL_MouseWheelEvent &_event);

private:
		bool m_active = true;
		Camera m_cam;
		Grid m_grid;
		Character m_character;

		bool m_mouse_trans_active;
		bool m_mouse_rot_active;
		float m_mouse_zoom;
		float m_mouse_pan;

		ngl::Vec2 m_mouse_translation;
		float m_mouse_rotation;
		ngl::Vec2 m_mouse_trans_origin;
		float m_mouse_rot_origin;


};

#endif//__SCENE_HPP__
