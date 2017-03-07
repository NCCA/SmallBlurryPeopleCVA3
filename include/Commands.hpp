#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "Character.hpp"

class Command
{
public:
  virtual ~Command() {}
  virtual void execute() = 0;
};

class BuildCommand : public Command
{
public:
  BuildCommand(Character *_character, BuildingType _building);
  ~BuildCommand();
  virtual void execute();
private:
  Character *m_character;
  BuildingType m_building;
};

#endif//__COMMANDS_HPP__
