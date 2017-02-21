#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <vector>
#include "ngl/Singleton.h"
#include "ngl/Vec2.h"
#include "Button.hpp"

class Gui : public ngl::Singleton<Gui>
{
  friend class Singleton<Gui>;
public :
  ///
  /// \brief init initialise or reset Gui with specific resolution
  /// \param _res resolution to initialise with
  ///
  void init(ngl::Vec2 _res);
  ///
  /// \brief init initialises or resets Gui without changing/setting resolution
  ///
  void init();
  ///
  /// \brief setResolution set the window width and window height
  ///
  void setResolution(ngl::Vec2 _res);
  ///
  /// \brief click check if user has clicked inside any button area
  /// \param pos mouse position to compare buttons against
  ///
  void click();
  ///
  /// \brief mousePos update mouse position
  /// \param _pos current mouse position
  ///
  void mousePos(ngl::Vec2 _pos);
  ///
  /// \brief wipeButtons used to clear buttons, and button id counter is reset to 0
  ///
  void wipeButtons();
  ///
  /// \brief createTestButtons create a set of buttons for debugging
  ///
  void createTestButtons();




  void addButton(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size);




private :
  ///
  /// \brief Gui private default constructor due to singleton pattern
  ///
  Gui();
  ///
  /// \brief m_buttons vector of buttons contained in the Gui
  ///
  std::vector<Button> m_buttons;
  ///
  /// \brief m_selected_button_id current button that mouse is over
  ///
  int m_selected_button_id;
  ///
  /// \brief m_win_w width of window
  ///
  int m_win_w;
  ///
  /// \brief m_win_h height of window
  ///
  int m_win_h;
};

#endif//__GUI_HPP__
