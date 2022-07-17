#pragma once

#include "Core.hpp"

namespace wfe::editor {
    typedef enum : uint8_t {
        CURSOR_TYPE_DEFAULT,
        CURSOR_TYPE_ARROW,
        CURSOR_TYPE_CROSS,
        CURSOR_TYPE_HAND,
        CURSOR_TYPE_NO,
        CURSOR_TYPE_SIZE_ALL,
        CURSOR_TYPE_SIZE_UPDOWN,
        CURSOR_TYPE_SIZE_LEFTRIGHT,
        CURSOR_TYPE_SIZE_DIAGONAL_LEFT,
        CURSOR_TYPE_SIZE_DIAGONAL_RIGHT,
        CURSOR_TYPE_WAIT
    } CursorType;

    struct CursorPos {
        ptrdiff_t x, y;
    };

    /// @brief Returns the cursor's position relative to the main window.
    /// @return The cursor's position.
    CursorPos GetCursorPos();
    /// @brief Returns the cursor's position relative to the screen.
    /// @return The cursor's position.
    CursorPos GetCursorScreenPos();

    /// @brief Sets the cursor's position relative to the main window.
    /// @param newPos The cursor's new position.
    void SetCursorPos(CursorPos newPos);
    /// @brief Sets the cursor's position relative to the screen.
    /// @param newPos The cursor's new position.
    void SetCursorScreenPos(CursorPos newPos);

    /// @brief Returns if the cursor left button is pressed.
    bool8_t CursorPressed();

    /// @brief Returns the cursor's type.
    CursorType GetCursorType();
    /// @brief Sets the cursor's type.
    /// @param newType The cursor's new type.
    void SetCursorType(CursorType newType);
    /// @brief Update's the OS's cursor type. Meant for internal use.
    void UpdateCursorType();
}