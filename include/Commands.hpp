#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "Character.hpp"
#include "Scene.hpp"

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
  PassiveCommand(int x);
  ~PassiveCommand();
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
  ~QuitCommand();
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
  BuildCommand(Character *_character, BuildingType _building);
  ~BuildCommand();
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
  BuildingType m_building;
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
  ~CentreCameraCommand();
  ///
  /// \brief execute send command to scene to centre the camera
  ///
  virtual void execute();
private:
  Scene *m_scene;
};

#endif//__COMMANDS_HPP__
