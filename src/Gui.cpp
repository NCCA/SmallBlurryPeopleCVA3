#include "Gui.hpp"
#include "ngl/ShaderLib.h"
#include "ngl/Vec2.h"
#include "ngl/NGLStream.h"
#include "AssetStore.hpp"
Gui::Gui()
{

}

void Gui::init(Scene *_scene, ngl::Vec2 _res, const std::string &_shader_name)
{
  m_scene = _scene;
  m_shader_name = _shader_name;
  setResolution(_res);
  initGL();
  createSceneButtons();
  m_mouse_down = false;
}

void Gui::setResolution(ngl::Vec2 _res)
{
  m_win_w = _res.m_x;
  m_win_h = _res.m_y;
  ngl::ShaderLib::instance()->use(m_shader_name);
  ngl::ShaderLib::instance()->setRegisteredUniform("vResolution", ngl::Vec2(m_win_w, m_win_h));
  ngl::ShaderLib::instance()->setRegisteredUniform("fResolution", ngl::Vec2(m_win_w, m_win_h));
  if(!m_buttons.empty())
  {
    updateButtonArrays();
  }
}

void Gui::initGL()
{
  glGenVertexArrays(1, &m_vao_id);
  glGenBuffers(3, m_vbo_ids);
  AssetStore::instance()->loadTexture("icons", "buttons/icons.png");
}

void Gui::click()
{
  if(m_selected_button_id >= 0 && (size_t)m_selected_button_id < m_buttons.size())
  {
    std::cout << "clicked button " << m_selected_button_id << std::endl;
    executeAction(m_buttons[m_selected_button_id].getAction());
  }
}

std::shared_ptr<Command> Gui::generateCommand(Action _action)
{
  std::shared_ptr<Command> command(nullptr);
  switch (_action)
  {
  case Action::PASSIVE:
    command.reset(new PassiveCommand);
    break;
  case Action::QUIT:
    command.reset(new QuitCommand(m_scene));
    break;
  case Action::BUILDHOUSE:
    command.reset(new BuildCommand(m_scene->getActiveCharacter(), BuildingType::HOUSE));
    break;
  case Action::BUILDSTORE:
    command.reset(new BuildCommand(m_scene->getActiveCharacter(), BuildingType::STOREHOUSE));
    break;
  case Action::CENTRECAMERA:
    command.reset(new CentreCameraCommand(m_scene));
    break;
  case Action::PAUSE:
    command.reset(new PauseCommand(m_scene));
    break;
  case Action::ZOOMIN:
    command.reset(new ZoomCommand(m_scene, 1));
    break;
  case Action::ZOOMOUT:
    command.reset(new ZoomCommand(m_scene, -1));
    break;
  default:
    break;
  }
  return command;
}

int Gui::executeAction(Action _action)
{
  std::shared_ptr<Command> command(generateCommand(_action));
  if(command.get())
  {
    // command generation successful, so execute command

    command.get()->execute();
    return 0;
  }
  else
  {
    // no command corresponds to given action, so don't execute anything

    return 1;
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
  if(!button_selected || m_buttons[m_selected_button_id].getAction() == Action::PASSIVE)
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

void Gui::createSceneButtons()
{
  wipeButtons();

  addButton(Action::PAUSE, XAlignment::RIGHT, YAlignment::TOP, ngl::Vec2(10, 10), ngl::Vec2(40, 40));
  updateButtonArrays();
}

void Gui::createPauseButtons()
{
  wipeButtons();
  addButton(Action::PAUSE, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, -25), ngl::Vec2(120, 40));
  addButton(Action::QUIT, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 25), ngl::Vec2(120, 40));
  updateButtonArrays();
}

void Gui::addButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size)
{
  m_buttons.push_back(Button(_action, _x_align, _y_align, ngl::Vec2(m_win_w, m_win_h), _offset, _size));
}

void Gui::updateButtonArrays()
{
  ngl::Vec2 res(m_win_w, m_win_h);
  std::vector<ngl::Vec2> positions;
  std::vector<ngl::Vec2> sizes;
  std::vector<int> ids;
  for(Button &button : m_buttons)
  {
    button.updatePos(res);
    positions.push_back(button.getPos());
    sizes.push_back(button.getSize());
    ids.push_back(button.getID());
  }
  glBindVertexArray(m_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ngl::Vec2) * positions.size(), &(positions[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 0
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ngl::Vec2) * sizes.size(), &(sizes[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 1
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_INT) * ids.size(), &(ids[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 2
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Gui::drawButtons()
{
  ngl::ShaderLib *slib = ngl::ShaderLib::instance();
  AssetStore *store = AssetStore::instance();

  slib->use(m_shader_name);
  bindTextureToShader(store->getTexture("icons"), "icons", 0);

  if(m_mouse_down)
  {
    slib->setRegisteredUniform("mouseOver", -1);
  }
  else
  {
    slib->setRegisteredUniform("mouseOver", m_selected_button_id);
  }
  glBindVertexArray(m_vao_id);
  glDrawArrays(GL_POINTS, 0, m_buttons.size());
}

void Gui::mouseDown()
{
  m_mouse_down = true;
}

void Gui::mouseUp()
{
  m_mouse_down = false;
}

void Gui::bindTextureToShader(const GLuint _tex, const char *_uniform, int _target)
{
    ngl::ShaderLib * slib = ngl::ShaderLib::instance();
    GLint id = slib->getProgramID("button");
    GLint loc = glGetUniformLocation(id, _uniform);

    if(loc == -1)
    {
        std::cerr << "Uh oh! Invalid uniform location in Gui::bindTextureToShader!! " << _uniform << std::endl;
        std::cerr << m_shader_name << std::endl;
        std::cerr << id << std::endl;
        exit(EXIT_FAILURE);
    }
    glUniform1i(loc, _target);

    glActiveTexture(GL_TEXTURE0 + _target);
    glBindTexture(GL_TEXTURE_2D, _tex);
}
