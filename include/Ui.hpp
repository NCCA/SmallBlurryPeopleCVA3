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
  /// \brief init initialises or resets Gui
  ///
  void init();
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

  void createTestButtons();
  ///
  /// \brief addButton creates a button in the Gui
  /// \param _x x position
  /// \param _y y position
  /// \param _w width
  /// \param _h height
  ///
  void addButton(double _x, double _y, double _w, double _h);
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
};

#endif//__GUI_HPP__
