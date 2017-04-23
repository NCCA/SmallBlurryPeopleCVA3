#include "Gui.hpp"
#include "ngl/ShaderLib.h"
#include "ngl/Vec2.h"
#include "ngl/NGLStream.h"
#include "AssetStore.hpp"

constexpr char TEXT_PLAY[2]   = {5, 0};
constexpr char TEXT_PAUSE[2]  = {6, 0};
constexpr char TEXT_SMILEY[2] = {29,0};
//constexpr char TEXT_CROSS[2] = {32,0}; // exception for cross

constexpr float FONT_SIZE = 20;
constexpr float FONT_SPACE = 0.5;

constexpr int MAX_AGE = 300;
constexpr int DOUBLE_MAX_NOTES = 10;

Gui::Gui()
{

}

void Gui::init(Scene *_scene, ngl::Vec2 _res, const std::string &_shader_name)
{
  m_scene = _scene;
  m_shader_name = _shader_name;
  setResolution(_res);
  initGL();
  createStartMenuButtons();
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
  if(m_selected_button)
  {
    std::cout << "clicked button " << m_selected_button->getID() << std::endl;
    executeAction(m_selected_button->getAction());
  }
}

std::shared_ptr<Command> Gui::generateCommand(Action _action)
{
  Prefs *prefs = Prefs::instance();
  std::shared_ptr<Command> command(nullptr);

  std::string button_text("");
  if(m_selected_button)
  {
    button_text = m_selected_button->getText();
  }
  switch (_action)
  {
  case Action::PASSIVE:
  case Action::PASSIVE_CHARACTER:
  case Action::PREFS_VALUE:
  case Action::CHAR_STATE:
  case Action::STAMINA_BAR:
  case Action::HEALTH_BAR:
    command.reset(new PassiveCommand);
    break;
  case Action::QUIT:
    command.reset(new QuitCommand(m_scene));
    break;
  case Action::BUILDHOUSE:
		command.reset(new BuildCommand(m_scene->getActiveCharacter(), TileType::HOUSE));
    break;
  case Action::BUILDSTORE:
		command.reset(new BuildCommand(m_scene->getActiveCharacter(), TileType::STOREHOUSE));
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
		command.reset(new MoveCamCommand(m_scene, Direction::LEFTWARDS, false));
    break;
  case Action::MOVERIGHT:
		command.reset(new MoveCamCommand(m_scene, Direction::RIGHTWARDS, false));
    break;
  case Action::STOPFORWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::FORWARDS, true));
    break;
  case Action::STOPBACKWARD:
    command.reset(new MoveCamCommand(m_scene, Direction::BACKWARDS, true));
    break;
  case Action::STOPLEFT:
		command.reset(new MoveCamCommand(m_scene, Direction::LEFTWARDS, true));
    break;
  case Action::STOPRIGHT:
		command.reset(new MoveCamCommand(m_scene, Direction::RIGHTWARDS, true));
    break;
  case Action::PREFERENCES:
    command.reset(new PrefsCommand(m_scene));
    break;
  case Action::TOGGLEBOOLPREF:
    command.reset(new SetPrefsCommand<bool>(button_text, !prefs->getBoolPref(button_text)));
    m_text_outdated = true;
    break;
  case Action::FORAGE:
    command.reset(new ForageCommand(m_scene->getActiveCharacter()));
    break;
  case Action::NOTIFY:
    if(m_selected_button)
    {
      command.reset(new CentreNotificationCommand(m_scene, ((NotificationButton *)m_selected_button)->getMapPos()));
    }
    break;
  case Action::INCR_PREFS:
    switch(prefs->getTypeOfPref(button_text))
    {
    case PrefType::BOOL:
      command.reset(new SetPrefsCommand<bool>(button_text, !prefs->getBoolPref(button_text)));
      break;
    case PrefType::INT:
      command.reset(new SetPrefsCommand<int>(button_text, prefs->getIntPref(button_text) + prefs->getIntIncValue(button_text)));
      break;
    case PrefType::FLOAT:
      command.reset(new SetPrefsCommand<float>(button_text, prefs->getFloatPref(button_text) + prefs->getFloatIncValue(button_text)));
      break;
    case PrefType::ERROR:
      break;
    case PrefType::STRING:
      break;
    }
    m_text_outdated = true;
    break;
  case Action::DECR_PREFS:
    switch(prefs->getTypeOfPref(button_text))
    {
    case PrefType::BOOL:
      command.reset(new SetPrefsCommand<bool>(button_text, !prefs->getBoolPref(button_text)));
      break;
    case PrefType::INT:
      command.reset(new SetPrefsCommand<int>(button_text, prefs->getIntPref(button_text) - prefs->getIntDecValue(button_text)));
      break;
    case PrefType::FLOAT:
      command.reset(new SetPrefsCommand<float>(button_text, prefs->getFloatPref(button_text) - prefs->getFloatDecValue(button_text)));
      break;
    case PrefType::ERROR:
      break;
    case PrefType::STRING:
      break;
    }
    m_text_outdated = true;
    break;
  case Action::SAVE_PREFERENCES:
    command.reset(new SavePreferencesCommand);
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
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    if(button.get()->isInside(_pos))
    {
      m_selected_button = button.get();
      button_selected = true;
    }
  }
  if(!button_selected || (m_selected_button && m_selected_button->isPassive(m_scene->getActiveCharacter())))
  {
    m_selected_button = nullptr;
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

void Gui::createStartMenuButtons()
{
  wipeButtons();
  addButton(Action::PASSIVE, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0,-125), ngl::Vec2(130,40), "GAME TITLE");
  addButton(Action::ESCAPE, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0,-25), ngl::Vec2(130,40), TEXT_PLAY);
  addButton(Action::PREFERENCES, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 25), ngl::Vec2(130,40), "PREFERENCES");
  addButton(Action::QUIT, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 75), ngl::Vec2(130, 40), "QUIT");

  updateButtonArrays();
}

void Gui::createSceneButtons()
{
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::RIGHT, YAlignment::TOP, ngl::Vec2(10, 10), ngl::Vec2(40, 40), TEXT_PAUSE);
  addButton(Action::BUILDHOUSE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 10), ngl::Vec2(40, 40), "BH");
  addButton(Action::BUILDSTORE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(60, 10), ngl::Vec2(40, 40), "BS");
  addButton(Action::FORAGE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(110, 10), ngl::Vec2(40, 40), "F");
  addButton(Action::PASSIVE_CHARACTER, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 110), ngl::Vec2(140, 40), m_scene->getActiveCharacterName());
  addButton(Action::CHAR_STATE, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 60), ngl::Vec2(140, 40), "");
  addButton(Action::CENTRECAMERA, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(160, 110), ngl::Vec2(40, 40), TEXT_SMILEY);

  addButton(Action::STAMINA_BAR, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 190), ngl::Vec2(190, 20), "");
  addButton(Action::HEALTH_BAR, XAlignment::LEFT, YAlignment::BOTTOM, ngl::Vec2(10, 160), ngl::Vec2(190, 20), "");
  updateButtonArrays();
}

void Gui::createPauseButtons()
{
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, -50), ngl::Vec2(130, 40), TEXT_PLAY);
  addButton(Action::PREFERENCES, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 0), ngl::Vec2(130, 40), "PREFERENCES");
  addButton(Action::QUIT, XAlignment::CENTER, YAlignment::CENTER, ngl::Vec2(0, 50), ngl::Vec2(130, 40), "QUIT");
  updateButtonArrays();
}

void Gui::createPrefsButtons()
{
  Prefs *prefs = Prefs::instance();
  float y0 = 10;
  float y_pos = 10;
  float x_pos = 10;
  int num_prefs = prefs->getNumChangeablePrefs();
  std::string name = "";
  wipeButtons();
  addButton(Action::ESCAPE, XAlignment::RIGHT, YAlignment::TOP, ngl::Vec2(10, 10), ngl::Vec2(40, 40), "X");
  for(auto &p : prefs->getIntMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::PREFS_VALUE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(100,40), name);
    y_pos += 50;
  }
  x_pos += 320;
  y_pos = y0;
  for(auto &p : prefs->getBoolMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::TOGGLEBOOLPREF, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(40,40), name);
    y_pos += 50;
  }
  x_pos += 260;
  y_pos = y0;
  for(auto &p : prefs->getFloatMap())
  {
    name = p.first;
    addButton(Action::PASSIVE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos, y_pos), ngl::Vec2(200,40), name);
    addButton(Action::PREFS_VALUE, XAlignment::LEFT, YAlignment::TOP, ngl::Vec2(x_pos + 210, y_pos), ngl::Vec2(100,40), name);
    y_pos += 50;
  }
  std::string save_prefs("SAVE PREFERENCES");
  addButton(Action::SAVE_PREFERENCES, XAlignment::CENTER, YAlignment::BOTTOM, ngl::Vec2(0, 60), ngl::Vec2(getButtonLength(save_prefs), 40), save_prefs);
  std::string prefs_warning("You will need to SAVE PREFERENCES and restart the game for some settings to take effect");
  addButton(Action::PASSIVE, XAlignment::CENTER, YAlignment::BOTTOM, ngl::Vec2(0,10), ngl::Vec2(getButtonLength(prefs_warning), 40), prefs_warning);

  updateButtonArrays();
}

void Gui::addButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text)
{
  m_buttons.push_back(std::shared_ptr<Button>(new Button(_action, _x_align, _y_align, ngl::Vec2(m_win_w, m_win_h), _offset, _size, _text)));
}

void Gui::addNotification(const std::string &_text, ngl::Vec2 _map_pos)
{
  ngl::Vec2 size(0,40);
  size.m_x = getButtonLength(_text);
  moveNotifications(ngl::Vec2(0, 50));
  m_buttons.push_back(std::shared_ptr<NotificationButton>(new NotificationButton(Action::NOTIFY, XAlignment::RIGHT, YAlignment::BOTTOM, ngl::Vec2(m_win_w, m_win_h), ngl::Vec2(10,10), size, _text, _map_pos)));
}

void Gui::removeButton(std::shared_ptr<Button> button)
{
  auto it = std::find(m_buttons.begin(), m_buttons.end(), button);
  if(it != m_buttons.end())
  {
    m_buttons.erase(it);
  }
  updateButtonArrays();
}

void Gui::updateButtonArrays()
{
  ngl::Vec2 res(m_win_w, m_win_h);
  std::vector<ngl::Vec2> positions;
  std::vector<ngl::Vec2> sizes;
  std::vector<int> shader_ids;
  std::vector<Action> actions;
  int shader_id = 0;

  std::array<int, DOUBLE_MAX_NOTES> notification_ages{0};
  size_t notification_ages_index = 0;
  bool notification_uniform_needs_updating = false;
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    Button *b = button.get();
    if(b)
    {
      b->updatePos(res);
      positions.push_back(b->getPos());
      sizes.push_back(b->getSize());
      shader_ids.push_back(shader_id);
      actions.push_back(b->getAction());
      if(b->getAction() == Action::NOTIFY)
      {
        notification_uniform_needs_updating = true;
        if(notification_ages_index < notification_ages.size())
        {
          notification_ages[notification_ages_index] = shader_id;
          notification_ages[notification_ages_index+1] = ((NotificationButton *)b)->getAge();
          notification_ages_index+=2;
        }
      }
    }
    shader_id++;
  }

  if(notification_uniform_needs_updating)
  {
    glUniform1iv(glGetUniformLocation(ngl::ShaderLib::instance()->getProgramID(m_shader_name), "notification_ages"), DOUBLE_MAX_NOTES, &(notification_ages[0]));
    glUniform1i(glGetUniformLocation(ngl::ShaderLib::instance()->getProgramID(m_shader_name), "max_notification_age"), MAX_AGE);
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_INT) * shader_ids.size(), &(shader_ids[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 2
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_INT) * actions.size(), &(actions[0]), GL_DYNAMIC_DRAW);
  // now fix this to the attribute buffer 2
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray(3);

  glBindVertexArray(0);
  m_text_outdated = true;
}

void Gui::updateNotifications()
{
  // flag for whether there are any notifications to updated
  bool uniform_needs_updating = false;
  std::array<int, DOUBLE_MAX_NOTES> ages{0};
  size_t i = 0;
  int shader_id = 0;
  std::vector< std::shared_ptr<Button> > buttons_to_remove;
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    Button *b = button.get();
    if(b->getAction() == Action::NOTIFY)
    {
      uniform_needs_updating = true;
      ((NotificationButton *)b)->incrementAge();
      if(((NotificationButton *)b)->getAge() > MAX_AGE)
      {
        buttons_to_remove.push_back(button);
      }
      else if(i < ages.size())
      {
        ages[i] = shader_id;
        i++;
        ages[i] = ((NotificationButton *)b)->getAge();
        i++;
      }
    }
    shader_id++;
  }

  for(std::shared_ptr<Button> &button : buttons_to_remove)
  {
    removeButton(button);
  }

  if(uniform_needs_updating)
  {
    glUniform1iv(glGetUniformLocation(ngl::ShaderLib::instance()->getProgramID(m_shader_name), "notification_ages"), DOUBLE_MAX_NOTES, &(ages[0]));
    glUniform1i(glGetUniformLocation(ngl::ShaderLib::instance()->getProgramID(m_shader_name), "max_notification_age"), MAX_AGE);
  }
}

void Gui::drawButtons()
{
  ngl::ShaderLib *slib = ngl::ShaderLib::instance();
  AssetStore *store = AssetStore::instance();

  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  slib->use(m_shader_name);
  updateNotifications();
  if(m_text_outdated)
  {
    updateText();
    // need to update notification set if text has changed
    updateNotifications();
  }
  slib->setRegisteredUniform("game_state", m_scene->getState());
  slib->setRegisteredUniform("FONT_SIZE", FONT_SIZE);
  slib->setRegisteredUniform("FONT_SPACE", FONT_SPACE);

  bindTextureToShader(store->getTexture("icons"), "icons", 0);
  bindTextureToShader(store->getTexture("font"), "font", 1);

  Character *character = m_scene->getActiveCharacter();
  if(character)
  {
    slib->setRegisteredUniform("character_selected", true);
    slib->setRegisteredUniform("character_state", (int)character->getState());
    slib->setRegisteredUniform("character_stamina", character->getStamina());
    slib->setRegisteredUniform("character_health", character->getHealth());
    slib->setRegisteredUniform("character_color", character->getColour());
  }
  else
  {
    slib->setRegisteredUniform("character_selected", false);
    slib->setRegisteredUniform("character_state", -1);
    slib->setRegisteredUniform("character_stamina", 1.0f);
    slib->setRegisteredUniform("character_health", 1.0f);
    slib->setRegisteredUniform("character_color", ngl::Vec3(0.7, 0.1, 0.0));
  }
  if(m_mouse_down)
  {
    slib->setRegisteredUniform("mouseOver", -1);
  }
  else
  {
    if(m_selected_button)
    {
      int id = m_selected_button->getID();
      int index = 0;
      while(m_buttons[index]->getID() != id)
      {
        index++;
      }
      slib->setRegisteredUniform("mouseOver", index);
    }
    else
    {
      slib->setRegisteredUniform("mouseOver", -1);
    }
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
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    Button *b = button.get();
    // get its text
    std::string text = "";
    switch(b->getAction())
    {
    case Action::TOGGLEBOOLPREF:
    case Action::PREFS_VALUE:
      text = prefs->getPrefValueString(b->getText());
      break;
    default:
      text = b->getText();
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
  std::cout << "button text size: " << button_text.size() << std::endl;
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
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    Button *b = button.get();
    switch(b->getAction())
    {
    case Action::PASSIVE_CHARACTER:
      if(b->getText() != char_name)
      {
        b->setText(char_name);
        m_text_outdated = true;
      }
      break;
    default:
      break;
    }
  }
}

void Gui::notify(const std::string &_text, ngl::Vec2 _pos)
{
  std::vector< std::shared_ptr<Button> > buttons_to_remove;
  int num_notifications = 1;
  for(int i = m_buttons.size()-1 ; i>=0; i--)
  {
    std::shared_ptr<Button> button(m_buttons[i]);
    if(button.get()->getAction() == Action::NOTIFY)
    {
      if(num_notifications*2 >= DOUBLE_MAX_NOTES)
      {
        buttons_to_remove.push_back(button);
      }
      else
      {
        num_notifications++;
      }
    }
  }
  for(std::shared_ptr<Button> &button : buttons_to_remove)
  {
    removeButton(button);
  }
  addNotification(_text, _pos);
  updateButtonArrays();
}

void Gui::moveNotifications(ngl::Vec2 _move_vec)
{
  for(std::shared_ptr<Button> &button : m_buttons)
  {
    Button *b = button.get();
    if(b->getAction() == Action::NOTIFY)
    {
      b->move(_move_vec);
    }
  }
}

int Gui::getButtonLength(const std::string &_text)
{
  return std::max((_text.length()+2) * (FONT_SIZE) * FONT_SPACE, 40.0f);
}

void Gui::scrollButton(int _dir)
{
  if(m_selected_button)
  {
    switch(m_selected_button->getAction())
    {
    case Action::PREFS_VALUE:
    case Action::TOGGLEBOOLPREF:
      if(_dir>0)
      {
        executeAction(Action::INCR_PREFS);
      }
      else
      {
        executeAction(Action::DECR_PREFS);
      }
      break;
    default:
      break;
    }
  }
}
