#include "Gui.hpp"
#include "ngl/ShaderLib.h"
#include "ngl/Vec2.h"
#include "ngl/NGLStream.h"

Gui::Gui()
{

}

void Gui::init(ngl::Vec2 _res, const std::string &_shader_name)
{
  init(_shader_name);
  setResolution(_res);
}

void Gui::init(const std::string &_shader_name)
{
  m_shader_name = _shader_name;
  wipeButtons();
  createTestButtons();
  initGL();
}

void Gui::setResolution(ngl::Vec2 _res)
{
  m_win_w = _res.m_x;
  m_win_h = _res.m_y;
  ngl::ShaderLib::instance()->use(m_shader_name);
  ngl::ShaderLib::instance()->setRegisteredUniform("vResolution", ngl::Vec2(m_win_w, m_win_h));
}

void Gui::initGL()
{
  glGenVertexArrays(1, &m_vao_id);
  glGenBuffers(2, m_vbo_ids);
  updateButtonArrays();
}

void Gui::click()
{
  if(m_selected_button_id >= 0 && (size_t)m_selected_button_id < m_buttons.size())
  {
    m_buttons[m_selected_button_id].activate();
  }
}

bool Gui::mousePos(ngl::Vec2 _pos)
{
  bool button_selected = false;
  for(Button &button : m_buttons)
  {
    if(button.isInside(_pos))
    {
      m_selected_button_id = button.getID();
      button_selected = true;
    }
  }
  if(!button_selected)
  {
    m_selected_button_id = -1;
  }
  return button_selected;
}

void Gui::wipeButtons()
{
  m_buttons.clear();
  Button::resetIdCounter();
}

void Gui::createTestButtons()
{
  addButton(XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(100, 100), ngl::Vec2(500, 500));
}

void Gui::addButton(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size)
{
  m_buttons.push_back(Button(_x_align, _y_align, ngl::Vec2(m_win_w, m_win_h), _offset, _size));
}

void Gui::updateButtonArrays()
{
  std::vector<ngl::Vec2> positions;
  std::vector<ngl::Vec2> sizes;
  for(Button &button : m_buttons)
  {
    positions.push_back(button.getPos());
    sizes.push_back(button.getSize());
  }
  glBindVertexArray(m_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ngl::Vec2) * positions.size(), &positions[0], GL_STATIC_DRAW);
  // now fix this to the attribute buffer 0
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  // enable this attribute (will be inPosition in the shader)
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ngl::Vec2) * sizes.size(), &sizes[0], GL_STATIC_DRAW);
  // now fix this to the attribute buffer 1
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  // enable this attribute (will be inPosition in the shader)
  glEnableVertexAttribArray(1);


  glBindVertexArray(0);
}

void Gui::drawButtons()
{
  ngl::ShaderLib::instance()->use(m_shader_name);
  glBindVertexArray(m_vao_id);
  glDrawArrays(GL_POINTS, 0, m_buttons.size());
	//std::cout << m_buttons.size() << std::endl;
	//std::cout << "button1:" << m_buttons[0].getPos()[0]/m_win_w << std::endl;
}
