#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "input_constants.hpp"
#include "event.hpp"
#include "time.hpp"

#include <vector2.hpp>
#include <initializer_list>
#include <array>

namespace kirana
{
class Application;
}

namespace kirana::utils::input
{
class InputManager
{
    friend class kirana::Application;

  private:
    const std::vector<Key> m_keyList{Key::UNKNOWN,
                               Key::SPACE,
                               Key::APOSTROPHE,
                               Key::COMMA,
                               Key::MINUS,
                               Key::PERIOD,
                               Key::SLASH,
                               Key::NUM_0,
                               Key::NUM_1,
                               Key::NUM_2,
                               Key::NUM_3,
                               Key::NUM_4,
                               Key::NUM_5,
                               Key::NUM_6,
                               Key::NUM_7,
                               Key::NUM_8,
                               Key::NUM_9,
                               Key::SEMICOLON,
                               Key::EQUAL,
                               Key::A,
                               Key::B,
                               Key::C,
                               Key::D,
                               Key::E,
                               Key::F,
                               Key::G,
                               Key::H,
                               Key::I,
                               Key::J,
                               Key::K,
                               Key::L,
                               Key::M,
                               Key::N,
                               Key::O,
                               Key::P,
                               Key::Q,
                               Key::R,
                               Key::S,
                               Key::T,
                               Key::U,
                               Key::V,
                               Key::W,
                               Key::X,
                               Key::Y,
                               Key::Z,
                               Key::LEFT_BRACKET,
                               Key::BACKSLASH,
                               Key::RIGHT_BRACKET,
                               Key::GRAVE_ACCENT,
                               Key::WORLD_1,
                               Key::WORLD_2,
                               Key::ESCAPE,
                               Key::ENTER,
                               Key::TAB,
                               Key::BACKSPACE,
                               Key::INSERT,
                               Key::DELETE,
                               Key::RIGHT,
                               Key::LEFT,
                               Key::DOWN,
                               Key::UP,
                               Key::PAGE_UP,
                               Key::PAGE_DOWN,
                               Key::HOME,
                               Key::END,
                               Key::CAPS_LOCK,
                               Key::SCROLL_LOCK,
                               Key::NUM_LOCK,
                               Key::PRINT_SCREEN,
                               Key::PAUSE,
                               Key::F1,
                               Key::F2,
                               Key::F3,
                               Key::F4,
                               Key::F5,
                               Key::F6,
                               Key::F7,
                               Key::F8,
                               Key::F9,
                               Key::F10,
                               Key::F11,
                               Key::F12,
                               Key::F13,
                               Key::F14,
                               Key::F15,
                               Key::F16,
                               Key::F17,
                               Key::F18,
                               Key::F19,
                               Key::F20,
                               Key::F21,
                               Key::F22,
                               Key::F23,
                               Key::F24,
                               Key::F25,
                               Key::NUMPAD_0,
                               Key::NUMPAD_1,
                               Key::NUMPAD_2,
                               Key::NUMPAD_3,
                               Key::NUMPAD_4,
                               Key::NUMPAD_5,
                               Key::NUMPAD_6,
                               Key::NUMPAD_7,
                               Key::NUMPAD_8,
                               Key::NUMPAD_9,
                               Key::NUMPAD_DECIMAL,
                               Key::NUMPAD_DIVIDE,
                               Key::NUMPAD_MULTIPLY,
                               Key::NUMPAD_SUBTRACT,
                               Key::NUMPAD_ADD,
                               Key::NUMPAD_ENTER,
                               Key::NUMPAD_EQUAL,
                               Key::LEFT_SHIFT,
                               Key::LEFT_CONTROL,
                               Key::LEFT_ALT,
                               Key::LEFT_SUPER,
                               Key::RIGHT_SHIFT,
                               Key::RIGHT_CONTROL,
                               Key::RIGHT_ALT,
                               Key::RIGHT_SUPER,
                               Key::MENU};
    const std::vector<MouseButton> m_mouseKeyList{
        MouseButton::LEFT,      MouseButton::RIGHT,     MouseButton::MIDDLE,
        MouseButton::BUTTON_01, MouseButton::BUTTON_02, MouseButton::BUTTON_03,
        MouseButton::BUTTON_04, MouseButton::BUTTON_05,
    };
    std::unordered_map<Key, KeyAction> m_keyboardKeyStatus;
    std::unordered_map<MouseButton, KeyAction> m_mouseKeyStatus;
    std::array<Key, 8> m_axisKeys{Key::W,  Key::S,    Key::A,    Key::D,
                                  Key::UP, Key::DOWN, Key::LEFT, Key::RIGHT};
    math::Vector2 m_axis;
    math::Vector2 m_scrollValue;
    math::Vector2 m_prevMousePosition;
    math::Vector2 m_mousePosition;
    bool m_isMouseInside;

    Event<KeyboardInput> m_onKeyboardInput;
    Event<MouseInput> m_onMouseInput;
    Event<math::Vector2> m_onScrollInput;

    InputManager()
    {
        m_resetKeys();
    }
    ~InputManager()
    {
        m_onMouseInput.removeAllListeners();
        m_onKeyboardInput.removeAllListeners();
    }

    inline void m_setAxisValues(KeyboardInput input)
    {
        float value =
            input.action == KeyAction::DOWN || input.action == KeyAction::HELD
                ? 1.0f
                : 0.0f;
        if (input.key == m_axisKeys[0] || input.key == m_axisKeys[4])
            m_axis[1] = value;
        else if (input.key == m_axisKeys[1] || input.key == m_axisKeys[5])
            m_axis[1] = -value;
        if (input.key == m_axisKeys[2] || input.key == m_axisKeys[6])
            m_axis[0] = -value;
        else if (input.key == m_axisKeys[3] || input.key == m_axisKeys[7])
            m_axis[0] = value;
    }

    inline void m_callKeyboardEvent(KeyboardInput input)
    {
        m_keyboardKeyStatus[input.key] = input.action;
        m_setAxisValues(input);
        m_onKeyboardInput(input);
    }


    inline void m_callMouseEvent(MouseInput input)
    {
        m_mouseKeyStatus[input.button] = input.action;
        m_onMouseInput(input);
    }

    inline void m_callScrollEvent(double xOffset, double yOffset)
    {
        m_scrollValue[0] = static_cast<float>(xOffset);
        m_scrollValue[1] = static_cast<float>(yOffset);
        m_onScrollInput(m_scrollValue);
    }

    inline void m_updateMousePosition(const std::array<int, 2> &pos)
    {
        m_prevMousePosition = m_mousePosition;
        m_mousePosition[0] = static_cast<float>(pos[0]);
        m_mousePosition[1] = static_cast<float>(pos[1]);
    }

    inline void m_resetKeys()
    {
        for (const Key k : m_keyList)
            m_keyboardKeyStatus[k] = KeyAction::NONE;
        for (const MouseButton k : m_mouseKeyList)
            m_mouseKeyStatus[k] = KeyAction::NONE;
    }

    inline void update(bool isMouseInside, const std::array<int, 2> &mousePos)
    {
        m_isMouseInside = isMouseInside;
        m_updateMousePosition(mousePos);
    }

    inline void lateUpdate()
    {
        m_resetKeys();
    }

  public:
    inline static InputManager &get()
    {
        static InputManager instance;
        return instance;
    }
    InputManager(const InputManager &inputManager) = delete;

    inline uint32_t addKeyboardInputListener(
        const std::function<void(KeyboardInput)> &callback)
    {
        return m_onKeyboardInput.addListener(callback);
    }
    inline void removeKeyboardListener(uint32_t callbackID)
    {
        m_onKeyboardInput.removeListener(callbackID);
    }
    inline uint32_t addMouseInputListener(
        const std::function<void(MouseInput)> &callback)
    {
        return m_onMouseInput.addListener(callback);
    }
    inline void removeMouseInputListener(uint32_t callbackID)
    {
        m_onMouseInput.removeListener(callbackID);
    }

    inline void setAxisKeys(Key forward = Key::W, Key back = Key::S,
                            Key left = Key::A, Key right = Key::D,
                            Key altForward = Key::UP, Key altBack = Key::DOWN,
                            Key altLeft = Key::LEFT, Key altRight = Key::RIGHT)
    {
        m_axisKeys[0] = forward;
        m_axisKeys[1] = back;
        m_axisKeys[2] = left;
        m_axisKeys[3] = right;
        m_axisKeys[4] = altForward;
        m_axisKeys[5] = altBack;
        m_axisKeys[6] = altLeft;
        m_axisKeys[7] = altRight;
    }

    inline bool getKey(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::DOWN ||
               m_keyboardKeyStatus[key] == KeyAction::HELD;
    }
    inline bool getKeyDown(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::DOWN;
    }
    inline bool getKeyUp(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::UP;
    }

    inline bool getMouseDown(MouseButton key)
    {
        return m_mouseKeyStatus[key] == KeyAction::DOWN;
    }
    inline bool getMouseUp(MouseButton key)
    {
        return m_mouseKeyStatus[key] == KeyAction::UP;
    }

    inline bool isAnyMouseButtonDown(MouseButton *button)
    {
        for (int i = 0; i < m_mouseKeyStatus.size(); i++)
        {
            if (m_mouseKeyStatus[static_cast<MouseButton>(i)] ==
                KeyAction::DOWN)
            {
                *button = static_cast<MouseButton>(i);
                return true;
            }
        }
        return false;
    }

    inline math::Vector2 getAxis()
    {
        return m_axis;
    }

    inline float getMouseScrollOffset()
    {
        return m_scrollValue[1];
    }

    inline math::Vector2 getMousePosition()
    {
        return m_mousePosition;
    }

    inline math::Vector2 getMousePositionDelta()
    {
        return m_mousePosition - m_prevMousePosition;
    }

    inline bool isMouseInside()
    {
        return m_isMouseInside;
    }
};

} // namespace kirana::utils::input
#endif