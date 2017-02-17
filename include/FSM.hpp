
#ifndef _FSM_HPP__
#define _FSM_HPP__

enum class State
{
	GET_WOOD,
	BUILD,
	SLEEP,
	FORAGE,
	MOVE,
	NONE
};

class FSM
{
	public:
		FSM();
		~FSM() = default;
		void setState(int _grid_id);


	private:
		State m_state;
		//Character m_character;

};
#endif//_FSM_HPP__
