#include "Commands.hpp"

BuildCommand::BuildCommand(Character *_character, BuildingType _building) :
  m_character(_character),
  m_building(_building)
{

}

BuildCommand::~BuildCommand()
{

}

void BuildCommand::execute()
{
  m_character->build(m_building);
}
