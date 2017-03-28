#include "Gui.hpp"
#include "ngl/ShaderLib.h"
#include "ngl/Vec2.h"
#include "ngl/NGLStream.h"
#include "AssetStore.hpp"

constexpr char TEXT_PLAY[2]   = {5, 0};
constexpr char TEXT_PAUSE[2]  = {6, 0};
constexpr char TEXT_SMILEY[2] = {29,0};

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
  glGenBuffers(4, m_vbo_ids);
  AssetStore::instance()->loadTexture("icons", "buttons/icons.png");
  AssetStore::instance()->loadTexture("font", "buttons/font.png");
}

void Gui::click()
{
  if(m_selected_button_id >= 0 && (size_t)m_selected_button_id < m_buttons.size())
  {
    std::cout << "clicked button " << m_selected_button_id << std::endl;
    executeAction(getCurrentButton()->getAction());
  }
}

std::shared_ptr<Command> Gui::generateCommand(Action _action)
{
  Prefs *prefs = Prefs::instance();
  std::shared_ptr<Command> command(nullptr);
  switch (_action)
  {
  case Action::PASSIVE:
  case Action::PASSIVE_CHARACTER:
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
  case Action::ESCAPE:
    command.reset(new EscapeCommand(m_scene));
    break;
  case Action::ZOOMIN:
    command.reset(new ZoomCommand(m_scene, 1));
    break;
  case Action::ZOOMOUT:
    command.reset(new ZoomCommand(m_scene, -1));
    break;
  case Action::MOVEFORWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::FORWARDS, false));
    break;
  case Action::MOVEBACKWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::BACKWARDS, false));
    break;
  case Action::MOVELEFT:
    command.reset(new MoveCamCommand(m_scene, Direction::LEFT, false));
    break;
  case Action::MOVERIGHT:
    command.reset(new MoveCamCommand(m_scene, Direction::RIGHT, false));
    break;
  case Action::STOPFORWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::FORWARDS, true));
    break;
  case Action::STOPBACKWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::BACKWARDS, true));
    break;
  case Action::STOPLEFT:
    command.reset(new MoveCamCommand(m_scene, Direction::LEFT, true));
    break;
  case Action::STOPRIGHT:
    command.reset(new MoveCamCommand(m_scene, Direction::RIGHT, true));
    break;
  case Action::PREFERENCES:
    command.reset(new PrefsCommand(m_scene));
    break;
  case Action::SETBOOLPREF:
    command.reset(new SetPrefsCommand<bool>(getCurrentButton()->getText(), !prefs->getBoolPref(getCurrentButton()->getText())));
    m_text_outdated = true;
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
  if(!button_selected || getCurrentButton()->isPassive(m_scene->getActiveCharacter()))
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

void Gui::pause()
{
  createPauseButtons();
}

void Gui::unpause()
{
  createSceneButtons();
}

void Gui::createSceneButtons()
{
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::RIGHT, YAlignment::TOP, ngl::Vec2(10, 10), ngl::Vec2(40, 40), TEXT_PAUSE);
  addButton(Action::BUILDHOUSE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 10), ngl::Vec2(40, 40), "BH");
  addButton(Action::BUILDSTORE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(60, 10), ngl::Vec2(40, 40), "BS");
  addButton(Action::PASSIVE_CHARACTER, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 100), ngl::Vec2(130, 40), m_scene->getActiveCharacterName());
  addButton(Action::CENTRECAMERA, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(150, 100), ngl::Vec2(40, 40), TEXT_SMILEY);
  updateButtonArrays();
  m_text_outdated = true;
}

void Gui::createPauseButtons()
{
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, -50), ngl::Vec2(130, 40), TEXT_PLAY);
  addButton(Action::PREFERENCES, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 0), ngl::Vec2(130, 40), "PREFERENCES");
  addButton(Action::QUIT, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 50), ngl::Vec2(130, 40), "QUIT");
  updateButtonArrays();
  m_text_outdated = true;
}

void Gui::createPrefsButtons()
{
  Prefs *prefs = Prefs::instance();
  float y0 = 10;
  float y_pos = 10;
  float x_pos = 10;
  std::string name = "";
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::RIGHT, YAlignment::TOP, ngl::Vec2(10, 10), ngl::Vec2(40, 40), "X");
  for(auto &p : prefs->getIntMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::SETBOOLPREF, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(100,40), name);
    y_pos += 50;
  }
  x_pos += 320;
  y_pos = y0;
  for(auto &p : prefs->getBoolMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::SETBOOLPREF, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(40,40), name);
    y_pos += 50;
  }
  x_pos += 260;
  y_pos = y0;
  for(auto &p : prefs->getFloatMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::SETBOOLPREF, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(100,40), name);
    y_pos += 50;
  }
  x_pos += 320;
  y_pos = y0;
  for(auto &p : prefs->getStrMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::SETBOOLPREF, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(100,40), name);
    y_pos += 50;
  }

  updateButtonArrays();
  m_text_outdated = true;
}

void Gui::addButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text)
{
  m_buttons.push_back(Button(_action, _x_align, _y_align, ngl::Vec2(m_win_w, m_win_h), _offset, _size, _text));
}

void Gui::updateButtonArrays()
{
  ngl::Vec2 res(m_win_w, m_win_h);
  std::vector<ngl::Vec2> positions;
  std::vector<ngl::Vec2> sizes;
  std::vector<int> ids;
  std::vector<Action> actions;
  for(Button &button : m_buttons)
  {
    button.updatePos(res);
    positions.push_back(button.getPos());
    sizes.push_back(button.getSize());
    ids.push_back(button.getID());
    actions.push_back(button.getAction());
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


  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_INT) * actions.size(), &(actions[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 2
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
}

void Gui::drawButtons()
{
  ngl::ShaderLib *slib = ngl::ShaderLib::instance();
  AssetStore *store = AssetStore::instance();

  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  slib->use(m_shader_name);
  if(m_text_outdated)
  {
    updateText();
  }
  ngl::ShaderLib::instance()->setRegisteredUniform("game_state", m_scene->getState());
  bindTextureToShader(store->getTexture("icons"), "icons", 0);
  bindTextureToShader(store->getTexture("font"), "font", 1);
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
        //std::cerr << "Uh oh! Invalid uniform location in Gui::bindTextureToShader!! " << _uniform << std::endl;
        //std::cerr << "shader name: " << m_shader_name << std::endl;
        //std::cerr << "program id" << id << std::endl;
        // don't exit, because texture is optimised out when shader is compiled
        //exit(EXIT_FAILURE);
    }
    glUniform1i(loc, _target);

    glActiveTexture(GL_TEXTURE0 + _target);
    glBindTexture(GL_TEXTURE_2D, _tex);
}

void Gui::updateText()
{
  Prefs *prefs = Prefs::instance();
  std::vector<uint> button_text;
  // for each button
  for(Button &b : m_buttons)
  {
    // get its text
    std::string text = "";
    switch(b.getAction())
    {
    case Action::SETBOOLPREF:
      text = prefs->getPrefValueString(b.getText());
      break;
    default:
      text = b.getText();
      break;
    }

    // add it to the text vector as uints
    for(char c : text)
    {
      button_text.push_back((uint)c);
    }
    // add a 0 value for break
    button_text.push_back(0);
  }
  if(button_text.size() > BUTTON_TEXT_LENGTH)
  {
    std::cerr << "button text of size " << button_text.size() << " too long for current limit of " << BUTTON_TEXT_LENGTH << ", recommended to increase limit" << std::endl;
  }
  glUniform1uiv(glGetUniformLocation(ngl::ShaderLib::instance()->getProgramID(m_shader_name), "button_text"), std::min((uint)button_text.size(), BUTTON_TEXT_LENGTH), (uint *)&(button_text[0]));
  m_text_outdated = false;
}

void Gui::updateActiveCharacter()
{
  std::string char_name = m_scene->getActiveCharacterName();
  for(Button &b : m_buttons)
  {
    switch(b.getAction())
    {
    case Action::PASSIVE_CHARACTER:
      if(b.getText() != char_name)
      {
        b.setText(char_name);
        m_text_outdated = true;
      }
      break;
    default:
      break;
    }
  }
}

Button *Gui::getCurrentButton()
{
  return &(m_buttons[m_selected_button_id]);
}
