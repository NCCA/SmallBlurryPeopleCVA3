#ifndef IVAL_HPP
#define IVAL_HPP

/// @file IVal.hpp
/// @brief When doing a smooth interpolation between two values, I usually have to define them in a header somewhere and then write cur += (targ - cur) / x
/// This gets pretty tiresome and clutters the code up, especially in Scene.hpp which until recently suffered from my excessive variables used to track
/// the camera transformation. I figure that wrapping this all up into a class lets me clean this up a bit.


template<typename T>
class IVal
{
public:
    /// @brief Constructor, sets the start end and step size.
    IVal(T _start, T _end, float _stepsize)
    {
        m_start = m_cur = _start;
        m_end = _end;
        m_stepsize = _stepsize;
    }

    /// @brief Updates m_cur.
    void update()
    {
        T add = (m_end - m_cur);
        add *= m_stepsize;
        m_cur += add;
    }

    /// @brief Gets m_cur.
    T get() const {return m_cur;}

    /// @brief Getter and setter for start.
    T getStart() const {return m_start;}
    void setStart(const T &_start) {m_start = _start;}

    /// @brief Getter and setter for end.
    T getEnd() const {return m_end;}
    void setEnd(const T &_end) {m_end = _end;}
    void incrEnd(const T &_add) {m_end += _add;}

    /// @brief Getter and setter for step size.
    float getStepsize() const {return m_stepsize;}
    void setStepsize(const float _stepsize) {m_stepsize = _stepsize;}

    /// @brief Resets the IVal, ready to be used again.
    void reset() {m_cur = m_start;}
private:
    /// @brief The start of the IVal. Interpolates smoothly from _start to _end.
    T m_start;
    /// @brief The end of the IVal. m_cur moves towards this.
    T m_end;
    /// @brief Should range from 0 - 1. 0 will not move, 1 will instantly snap m_cur to m_end.
    float m_stepsize;
    /// @brief Tracks the current value.
    T m_cur;
};

#endif
