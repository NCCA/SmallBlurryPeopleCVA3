#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "Character.hpp"
#include "Scene.hpp"

///
/// \brief The Action enum for what the buttons does, ints are commented in to match shader consts.
/// Includes:
/// PASSIVE - text button with no action
/// QUIT - button quits the game
/// BUILDHOUSE - tell character to build a house
/// BUILDSTORE - tell character to build a storehouse
/// CENTRECAMERA - center the camera on the current character
/// ESCAPE escape the current menu
/// ZOOMIN zoom camera in
/// ZOOMOUT zoom camera out
/// MOVEFORWARD move camera forward
/// MOVEBACKWARD move camera backward
/// MOVELEFT move camera left
/// MOVERIGHT move camera right
/// STOPFORWARD stop camera move forward
/// STOPBACKWARD stop camera move backward
/// STOPLEFT stop camera move left
/// STOPRIGHT stop camera move right
/// PREFERENCES open preferences menu
/// PASSIVE_CHARACTER display character name
/// TOGGLEBOOLPREF toggle a boolean preference
/// FORAGE tell character to forage for berries
/// NOTIFY display a notification
/// PREFS_VALUE display a preferences value
/// INCR_PREFS increase a preferences value
/// DECR_PREFS decrease a preferences value
/// SAVE_PREFERENCES save preferences to preferences.conf
/// CHAR_STATE display characters's state
/// STAMINA_BAR disaplay stamina of character
/// HEALTH_BAR display health of character
/// HUNGER_BAR display hunger of character
/// POPULATION display population
/// CHAR_EAT_BERRIES tell character to eat berries
/// CHAR_EAT_FISH tell character to eat fish
/// INV_WOOD display amount of wood in inventory
/// INV_BERRIES display amount of berries in inventory
/// INV_FISH display amount of fish in inventory
/// NEXT_MAP go to the next map
/// PREV_MAP go to the previous map
/// MAP_NAME display name of current map
/// PLUS_WIDTH add to the map width
/// SUB_WIDTH subtract from the map width
/// PLUS_HEIGHT add to the map height
/// SUB_HEIGHT subtract from the map height
/// PLUS_SEED add to the map seed
/// SUB_SEED subtract from the map seed
/// MAP_WIDTH display map width
/// MAP_HEIGHT display map height
/// MAP_SEED display map seed
/// END_GAME go to the main menu
///
enum class Action
{
  PASSIVE,           //  0
  QUIT,              //  1
  BUILDHOUSE,        //  2
  BUILDSTORE,        //  3
  CENTRECAMERA,      //  4
  ESCAPE,            //  5
  ZOOMIN,            //  6
  ZOOMOUT,           //  7
  MOVEFORWARD,       //  8
  MOVEBACKWARD,      //  9
  MOVELEFT,          // 10
  MOVERIGHT,         // 11
  STOPFORWARD,       // 12
  STOPBACKWARD,      // 13
  STOPLEFT,          // 14
  STOPRIGHT,         // 15
  PREFERENCES,       // 16
  PASSIVE_CHARACTER, // 17
  TOGGLEBOOLPREF,    // 18
  FORAGE,            // 19
  NOTIFY,            // 20
  PREFS_VALUE,       // 21
  INCR_PREFS,        // 22
  DECR_PREFS,        // 23
  SAVE_PREFERENCES,  // 24
  CHAR_STATE,        // 25
  STAMINA_BAR,       // 26
  HEALTH_BAR,        // 27
  HUNGER_BAR,        // 28
  POPULATION,        // 29
  CHAR_EAT_BERRIES,  // 30
  CHAR_EAT_FISH,     // 31
  INV_WOOD,          // 32
  INV_BERRIES,       // 33
  INV_FISH,          // 34
  NEXT_MAP,          // 35
  PREV_MAP,          // 36
  MAP_NAME,          // 37
  PLUS_WIDTH,        // 38
  SUB_WIDTH,         // 39
  PLUS_HEIGHT,       // 40
  SUB_HEIGHT,        // 41
  PLUS_SEED,         // 42
  SUB_SEED,          // 43
  MAP_WIDTH,         // 44
  MAP_HEIGHT,        // 45
  MAP_SEED,          // 46
  END_GAME           // 47
};

///
/// \brief The Command class utility class for creating level of indirection between buttons and actions they perform, base class is abstract
///
class Command
{
public:
  virtual ~Command() {}
  virtual void execute() = 0;
};
///
/// \brief The PassiveCommand class for buttons which have no function, so cannot be clicked
///
class PassiveCommand : public Command
{
public:
  ///
  /// \brief PassiveCommand basic constructor
  ///
  PassiveCommand();
  ~PassiveCommand() = default;
  ///
  /// \brief execute function does nothing
  ///
  virtual void execute();
};
///
/// \brief The QuitCommand class for quitting the game
///
class QuitCommand : public Command
{
public:
  ///
  /// \brief QuitCommand constructor takes scene so it knows what to send "quit" message to
  /// \param _scene scene which button refers to
  ///
  QuitCommand(Scene *_scene);
  ~QuitCommand() = default;
  ///
  /// \brief execute quits the given scene
  ///
  virtual void execute();
private:
  Scene *m_scene;
};

///
/// \brief The BuildCommand class used to tell character to build a given building type
///
class BuildCommand : public Command
{
public:
  ///
  /// \brief BuildCommand constructor for building command
  /// \param _character character to instruct
  /// \param _building building wanted
  ///
	BuildCommand(Character *_character, TileType _building);
  ~BuildCommand() = default;
  ///
  /// \brief execute tells character to build given building
  ///
  virtual void execute();
private:
  ///
  /// \brief m_character character to instruct
  ///
  Character *m_character;
  ///
  /// \brief m_building type of building wanted
  ///
	TileType m_building;
};

///
/// \brief The CentreCameraCommand class sends command to scene to reset camera
///
class CentreCameraCommand : public Command
{
public:
  ///
  /// \brief CentreCameraCommand constructor for centre camera command
  /// \param _scene scene to send instruction to
  ///
  CentreCameraCommand(Scene *_scene);
  ~CentreCameraCommand() = default;
  ///
  /// \brief execute send command to scene to centre the camera
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene the scene to tell to centre the camera
  ///
  Scene *m_scene;
};

///
/// \brief The EscapeCommand class sends command to scene to toggle pause or escape current state
///
class EscapeCommand : public Command
{
public:
  ///
  /// \brief PauseCommand constructor for pause command
  /// \param _scene scene to send instruction to
  ///
  EscapeCommand(Scene *_scene);
  ~EscapeCommand() = default;
  ///
  /// \brief execute send command to scene to toggle pause
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene the scene to toggle pause
  ///
  Scene *m_scene;
};
///
/// \brief The PrefsCommand class tell scene to show/hide preferences
///
class PrefsCommand : public Command
{
public:
  ///
  /// \brief PrefsCommand constructor for preferences command
  /// \param _scene scene to send instruction to
  ///
  PrefsCommand(Scene *_scene);
  ~PrefsCommand() = default;
  ///
  /// \brief execute send command to scene to toggle preferences
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene the scene to toggle preferences
  ///
  Scene *m_scene;
};

///
/// \brief The ZoomCommand class allows zooming in/out of a scene
///
class ZoomCommand : public Command
{
public:
  ///
  /// \brief ZoomCommand constructor for zoom command
  /// \param _scene scene to send instruction to
  /// \param _direction
  ///
  ZoomCommand(Scene *_scene, int _direction);
  ~ZoomCommand() = default;
  ///
  /// \brief execute send command to scene to zoom
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene the scene to zoom
  ///
  Scene *m_scene;
  ///
  /// \brief m_direction the direction to zoom in
  ///
  int m_direction;
};
///
/// \brief The MoveCamCommand class moves the camera or stops the camera in the given direction
///
class MoveCamCommand : public Command
{
public:
  ///
  /// \brief MoveCamCommand constructor for MoveCamCommand
  /// \param _scene scene to send instruction to
  /// \param _d direction of command
  /// \param _stop whether to stop in that direction or not
  ///
  MoveCamCommand(Scene *_scene, Direction _d, bool _stop);
  ~MoveCamCommand() = default;
  ///
  /// \brief execute send command to scene to move/stop camera
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene scene to move/stop camera
  ///
  Scene *m_scene;
  ///
  /// \brief m_d which direction to use
  ///
  Direction m_d;
  ///
  /// \brief m_stop whether it is a move command (false) or a stop command (true)
  ///
  bool m_stop;
};

///
/// \brief The SetPrefsCommand class used to set a preference
///
template<class T>
class SetPrefsCommand :public Command
{
public:
  ///
  /// \brief SetPrefsCommand constructor for the set prefs command
  /// \param _key preference key to use
  /// \param _val value to set it to
  ///
  SetPrefsCommand(const std::string &_key, const T &_val);
  ~SetPrefsCommand() = default;
  ///
  /// \brief execute send command to prefs to change one of the settings
  ///
  virtual void execute();
private:
  ///
  /// \brief m_key key to access preference
  ///
  std::string m_key;
  ///
  /// \brief m_val value to set it to
  ///
  T m_val;
};

template<class T>
SetPrefsCommand<T>::SetPrefsCommand(const std::string &_key, const T &_val) :
  m_key(_key),
  m_val(_val)
{}

template<class T>
void SetPrefsCommand<T>::execute()
{
  Prefs::instance()->setPref(m_key, m_val);
}

///
/// \brief The ForageCommand class used to tell character to start foraging
///
class ForageCommand : public Command
{
public:
  ///
  /// \brief ForageCommand constructor for forage command
  /// \param _character character to instruct
  ///
  ForageCommand(Character *_character);
  ~ForageCommand() = default;
  ///
  /// \brief execute tells character to forage
  ///
  virtual void execute();
private:
  ///
  /// \brief m_character character to instruct
  ///
  Character *m_character;
};

///
/// \brief The EatBerriesCommand class used to tell character to find stored berries to eat
///
class EatBerriesCommand : public Command
{
public:
  ///
  /// \brief EatBerriesCommand constructor for eat berries command
  /// \param _character character to instruct
  ///
  EatBerriesCommand(Character *_character);
  ~EatBerriesCommand() = default;
  ///
  /// \brief execute tells character to find berries to eat
  ///
  virtual void execute();
private:
  ///
  /// \brief m_character character to instruct
  ///
  Character *m_character;
};

///
/// \brief The EatFishCommand class used to tell character to find stored fish to eat
///
class EatFishCommand : public Command
{
public:
  ///
  /// \brief EatFishCommand constructor for eat fish command
  /// \param _character character to instruct
  ///
  EatFishCommand(Character *_character);
  ~EatFishCommand() = default;
  ///
  /// \brief execute tells character to find stored fish to eat
  ///
  virtual void execute();
private:
  ///
  /// \brief m_character character to instruct
  ///
  Character *m_character;
};

///
/// \brief The CentreNotificationCommand class to focus camera on a notification's position
///
class CentreNotificationCommand : public Command
{
public:
  ///
  /// \brief CentreNotificationCommand constructor for center notification command
  /// \param _scene current scene being used
  /// \param _map_pos position to move camera to
  ///
  CentreNotificationCommand(Scene *_scene, ngl::Vec2 _map_pos);
  ~CentreNotificationCommand() = default;
  ///
  /// \brief execute send instruction to move camera
  ///
  virtual void execute();
private:
  ///
  /// \brief m_scene scene to send instruction to
  ///
  Scene *m_scene;
  ///
  /// \brief m_map_pos map position to move camera to
  ///
  ngl::Vec2 m_map_pos;
};

///
/// \brief The SavePreferencesCommand class to write preferences out to config file
///
class SavePreferencesCommand : public Command
{
public:
  ///
  /// \brief SavePreferencesCommand constructor for save preferences command
  ///
  SavePreferencesCommand();
  ~SavePreferencesCommand() = default;
  ///
  /// \brief execute writes out preferences to file
  ///
  virtual void execute();
};

class ChangeMapCommand : public Command
{
public:
  ChangeMapCommand(int _dir);
  ~ChangeMapCommand() = default;

  virtual void execute();
private:
  int m_dir;
};

class ChangeWidthCommand : public Command
{
public:
  ChangeWidthCommand(int _dir);
  ~ChangeWidthCommand() = default;

  virtual void execute();
private:
  int m_dir;
};

class ChangeHeightCommand : public Command
{
public:
  ChangeHeightCommand(int _dir);
  ~ChangeHeightCommand() = default;

  virtual void execute();
private:
  int m_dir;
};

class ChangeSeedCommand : public Command
{
public:
  ChangeSeedCommand(int _dir);
  ~ChangeSeedCommand() = default;

  virtual void execute();
private:
  int m_dir;
};

class EndGameCommand :public Command
{
public:
  EndGameCommand(Scene *_scene);
  ~EndGameCommand() = default;

  virtual void execute();
private:
  Scene *m_scene;
};

#endif//__COMMANDS_HPP__
