#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <vector>
#include <memory>
#include "ngl/Singleton.h"
#include "ngl/Vec2.h"
#include "Button.hpp"
#include "Commands.hpp"
#include "Scene.hpp"

///
/// \file Gui.hpp
/// \brief The Gui is used for user interaction, and managing and drawing the buttons
///

///
/// \brief BUTTON_TEXT_LENGTH
///
constexpr unsigned int BUTTON_TEXT_LENGTH = 1024;

///
/// \brief The Gui class contains button positions and managing their use
///
class Gui : public ngl::Singleton<Gui>
{
  friend class Singleton<Gui>;
public :
  ///
  /// \brief init initialise or reset Gui with specific resolution
  /// \param _res resolution to initialise with
  /// \param _shader_name name of gui shader
  ///
  void init(Scene *_scene, ngl::Vec2 _res, const std::string &_shader_name);
  ///
  /// \brief setResolution set the window width and window height
  ///
  void setResolution(ngl::Vec2 _res);
  ///
  /// \brief initGL initialise gl values
  ///
  void initGL();
  ///
  /// \brief click check if user has clicked inside any button area
  /// \param pos mouse position to compare buttons against
  ///
  void click();
  ///
  /// \brief generateCommand create a command to be executed
  /// \return command to be executed
  ///
  std::shared_ptr<Command> generateCommand(Action _action);
  ///
  /// \brief executeAction generate a command and execute it
  /// \param _action action to execute
  /// \return 0 for no errors, 1 for nothing executed
  ///
  int executeAction(Action _action);
  ///
  /// \brief mousePos update mouse position
  /// \param _pos current mouse position
  ///
  bool mousePos(ngl::Vec2 _pos);
  ///
  /// \brief wipeButtons used to clear buttons, and button id counter is reset to 0
  ///
  void wipeButtons();
  ///
  /// \brief pause create pause menu and set pause uniform
  ///
  void pause();
  ///
  /// \brief unpause leave pause menu and set pause uniform
  ///
  void unpause();
  ///
  /// \brief createStartMenuButtons creates the set of buttons for start menu
  ///
  void createStartMenuButtons();
  ///
  /// \brief createSceneButtons creates the set of default buttons for the scene
  ///
  void createSceneButtons();
  ///
  /// \brief createPauseButtons create the set of buttons for the pause menu
  ///
  void createPauseButtons();
  ///
  /// \brief createPrefsButtons create the set of buttons for the preferences menu
  ///
  void createPrefsButtons();
  ///
  /// \brief createEndGameButtons create set of buttons for the game over screen
  ///
  void createEndGameButtons();
  ///
  /// \brief addButton add a button to the button vector
  /// \param _action button's action when clicked
  /// \param _x_align horizonal alignment (to left/right/center of screen)
  /// \param _y_align vertical alignment (to top/bottom/center of screen)
  /// \param _offset offset from edge/center
  /// \param _size size of button
  /// \param _text button's text
  ///
  void addButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text);
  ///
  /// \brief addNotification add a notificaion to the button vector
  /// \param _text text of notification
  /// \param _map_pos position notification comes from
  ///
  void addNotification(const std::string &_text, ngl::Vec2 _map_pos);
  ///
  /// \brief removeButton delete a button from the vector
  /// \param button button to remove
  ///
  void removeButton(std::shared_ptr<Button> button);
  ///
  /// \brief updateButtonArrays updates positions and passes them to openGL, useful for changing buttons or resizing screen
  ///
  void updateButtonArrays();
  ///
  /// \brief updateNotifications update age of notifications and delete any if necessary
  ///
  void updateNotifications();
  ///
  /// \brief drawButtons draw buttons to screen
  ///
  void drawButtons();
  ///
  /// \brief mouseDown register mouse as clicked
  ///
  void mouseDown();
  ///
  /// \brief mouseUp register mouse as not clicked
  ///
  void mouseUp();
  ///
  /// \brief bindTextureToShader bind the given texture to the button shader
  /// \param _tex texture id to use
  /// \param _uniform name of texture in shader
  /// \param _target which number texture unit to use
  ///
  void bindTextureToShader(const GLuint _tex, const char *_uniform, int _target);
  ///
  /// \brief updateText send m_button_text to shader
  ///
  void updateText();
  ///
  /// \brief updateActiveCharacter if active character changes, this function updates button text
  ///
  void updateActiveCharacter();
  ///
  /// \brief notify create a notification command
  /// \param _text text for notification
  /// \param _pos position on map that notification comes from
  ///
  void notify(const std::string &_text, ngl::Vec2 _pos);
  ///
  /// \brief moveNotifications move all notifications eg up or down
  /// \param _move_vec
  ///
  void moveNotifications(ngl::Vec2 _move_vec);
  ///
  /// \brief getButtonLength find out how long a button should be to fit a string
  /// \param _text text on button
  /// \return length of button needed
  ///
  int getButtonLength(const std::string &_text);
  ///
  /// \brief scrollButton send an increment or decrement command
  /// \param _dir positive for increment, negative for decrement
  ///
  void scrollButton(int _dir);
private :
  ///
  /// \brief Gui private default constructor due to singleton pattern
  ///
  Gui();
  ///
  /// \brief m_scene pointer to scene
  ///
  Scene *m_scene;
  ///
  /// \brief m_buttons vector of buttons contained in the Gui
  ///
  std::vector< std::shared_ptr<Button> > m_buttons;
  ///
  /// \brief m_selected_button_id current button that mouse is over
  ///
  Button *m_selected_button;
  ///
  /// \brief m_win_w width of window
  ///
  int m_win_w;
  ///
  /// \brief m_win_h height of window
  ///
  int m_win_h;
  ///
  /// \brief m_shader_name string denoting shader that buttons use
  ///
  std::string m_shader_name;
  ///
  /// \brief m_vao_id geometry VAO for button points
  ///
  GLuint m_vao_id;
  ///
  /// \brief m_vbo_ids VBOs for various button attributes
  ///
  GLuint m_vbo_ids[4];
  ///
  /// \brief m_mouse_down whether mouse is pressed down or not
  ///
  bool m_mouse_down;
  ///
  /// \brief m_active_character pointer to currently selected character
  ///
  Character *m_active_character;
  ///
  /// \brief m_text_outdated flag. if true, text will be updated on next draw call
  ///
  bool m_text_outdated;
};

#endif//__GUI_HPP__
