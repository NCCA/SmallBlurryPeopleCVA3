#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "ngl/Vec2.h"
#include "Commands.hpp"
///
/// \brief The XAlignment enum for denoting which side a button "sticks" to
///
enum class XAlignment{
  LEFT,
  CENTER,
  RIGHT
};

///
/// \brief The YAlignment enum for denoting which level a button "sticks" to
///
enum class YAlignment{
  TOP,
  CENTER,
  BOTTOM
};

///
/// \brief The Button class defines a button with a position that can be activated if clicked
///
class Button
{
public:
  ///
  /// \brief Button default constructor, takes alignment information
  /// \param _action functionality of the button
  /// \param _x_align which side the button is bound to, either left, right or center
  /// \param _y_align which level the button is bound to, either up, down or center
  /// \param _window_res window resolution for calculating postion from offset
  /// \param _offset button offset from given alignments
  /// \param _size size of the button
  ///
  Button(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _window_res, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text);
  ///
  /// \brief updatePos update the button's position based on the offset and screen res
  /// \param screen_res resolution of the window
  ///
  void updatePos(ngl::Vec2 _window_res);
  ///
  /// \brief isInside check if given position is inside the button, used for mouse position
  /// \param _pos position to check button position and size against
  /// \return true for inside, false for outside
  ///
  bool isInside(ngl::Vec2 _pos);
  ///
  /// \brief getAction get command from button
  ///
  Action getAction() const;
  ///
  /// \brief getID get the button id
  /// \return id of the button
  ///
  int getID();
  ///
  /// \brief getPos get position of button
  /// \return position of button as vec2
  ///
  ngl::Vec2 getPos();
  ///
  /// \brief getSize get size of button
  /// \return vec2 of x and y size
  ///
  ngl::Vec2 getSize();
  ///
  /// \brief getText get button text
  /// \return text of button, or "\0"
  ///
  const std::string &getText();
  ///
  /// \brief setText change the text on a button
  /// \param _text string to set the text to
  ///
  void setText(const std::string &_text);
  ///
  /// \brief isPassive whether button is clickable
  /// \param character_selected whether there is currently a selected character
  /// \return true if button has no action
  ///
  bool isPassive(bool character_selected);
  ///
  /// \brief resetIdCounter set id counter of buttons to 0, NEED TO WIPE ALL BUTTONS BEFORE DOING THIS
  ///
  static void resetIdCounter();
  ///
  /// \brief move move a button by a given amount
  /// \param _move_vec the vector to move the button by
  ///
  void move(ngl::Vec2 _move_vec);
protected:
  ///
  /// \brief m_id_counter keeps track of number of buttons
  ///
  static int m_id_counter;
  ///
  /// \brief m_id unique button id
  ///
  const int m_id;
  ///
  /// \brief m_offset offset in x and y from the button's alignment
  ///
  ngl::Vec2 m_offset;
  ///
  /// \brief m_pos button absolute position
  ///
  ngl::Vec2 m_pos;
  ///
  /// \brief m_size size of button, in x and y
  ///
  ngl::Vec2 m_size;
  ///
  /// \brief m_x_align alignment of button left/center/right
  ///
  XAlignment m_x_align;
  ///
  /// \brief m_y_align alignment of button top/center/bottom
  ///
  YAlignment m_y_align;
  ///
  /// \brief m_action action that button does upon activation
  ///
  Action m_action;
  ///
  /// \brief m_text button text displayed on button, or used for setting buttons to store key
  ///
  std::string m_text;
};

class NotificationButton : public Button
{
public:
  NotificationButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _window_res, ngl::Vec2 _offset, ngl::Vec2 _size, const std::string &_text, ngl::Vec2 _map_pos);
  void incrementAge();
  int getAge();
  ngl::Vec2 getMapPos();
private:
  ngl::Vec2 m_map_pos;
  int m_age;
};

#endif//__BUTTON_HPP__
