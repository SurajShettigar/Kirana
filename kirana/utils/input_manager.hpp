#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "input_constants.hpp"
#include "event.hpp"
#include "time.hpp"

#include <vector2.hpp>
#include <initializer_list>

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
    std::unordered_map<Key, KeyAction> m_keyboardKeyStatus;
    std::unordered_map<MouseButton, KeyAction> m_mouseKeyStatus;
    math::Vector2 m_scrollValue;
    math::Vector2 m_mousePosition;

    Event<KeyboardInput> m_onKeyboardInput;
    Event<MouseInput> m_onMouseInput;
    Event<math::Vector2> m_onScrollInput;

    InputManager()
    {
        std::initializer_list<Key> keyList{Key::UNKNOWN,
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
        std::initializer_list<MouseButton> mouseKeyList{
            MouseButton::LEFT,      MouseButton::RIGHT,
            MouseButton::MIDDLE,    MouseButton::BUTTON_01,
            MouseButton::BUTTON_02, MouseButton::BUTTON_03,
            MouseButton::BUTTON_04, MouseButton::BUTTON_05,
        };
        for (const Key k : keyList)
            m_keyboardKeyStatus[k] = KeyAction::NONE;
        for (const MouseButton k : mouseKeyList)
            m_mouseKeyStatus[k] = KeyAction::NONE;
    }
    ~InputManager()
    {
        m_onMouseInput.removeAllListeners();
        m_onKeyboardInput.removeAllListeners();
    }

    inline void m_callKeyboardEvent(KeyboardInput input)
    {
        m_keyboardKeyStatus[input.key] = input.action;
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

    inline bool getKey(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::HELD;
    }
    inline bool getKeyDown(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::DOWN;
    }
    inline bool getKeyUp(Key key)
    {
        return m_keyboardKeyStatus[key] == KeyAction::UP;
    }

    inline bool getMouse(MouseButton key)
    {
        return m_mouseKeyStatus[key] == KeyAction::HELD;
    }
    inline bool getMouseDown(MouseButton key)
    {
        return m_mouseKeyStatus[key] == KeyAction::DOWN;
    }
    inline bool getMouseUp(MouseButton key)
    {
        return m_mouseKeyStatus[key] == KeyAction::UP;
    }

    inline float getMouseScrollOffset()
    {
        return m_scrollValue[1];
    }

    inline math::Vector2 getMousePosition()
    {
        return m_mousePosition;
    }
};

} // namespace kirana::utils::input
#endif