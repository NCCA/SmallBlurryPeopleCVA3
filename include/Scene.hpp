#ifndef __SCENE_HPP__
#define __SCENE_HPP__

class Scene
{
	public:
		Scene() = default;
		~Scene() = default;
		bool isActive();
		void update();
		void draw();
	private:
		bool m_active = true;

};

#endif//__SCENE_HPP__
