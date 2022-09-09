#include "BuildInfo.hpp"

#ifdef PLATFORM_LINUX

#include "LinuxToImGui.hpp"

ImGuiKey KeySymToImGuiKey(KeySym keySym) {
    switch(keySym) {
    case XK_Tab: 
        return ImGuiKey_Tab;
    case XK_Left: 
        return ImGuiKey_LeftArrow;
    case XK_Right: 
        return ImGuiKey_RightArrow;
    case XK_Up: 
        return ImGuiKey_UpArrow;
    case XK_Down: 
        return ImGuiKey_DownArrow;
    case XK_Page_Up: 
        return ImGuiKey_PageUp;
    case XK_Page_Down: 
        return ImGuiKey_PageDown;
    case XK_Home: 
        return ImGuiKey_Home;
    case XK_End: 
        return ImGuiKey_End;
    case XK_Insert: 
        return ImGuiKey_Insert;
    case XK_Delete: 
        return ImGuiKey_Delete;
    case XK_BackSpace: 
        return ImGuiKey_Backspace;
    case XK_space: 
        return ImGuiKey_Space;
    case XK_Return: 
        return ImGuiKey_Enter;
    case XK_Escape: 
        return ImGuiKey_Escape;
    case XK_Shift_L:
        return ImGuiKey_LeftShift;
    case XK_Shift_R:
        return ImGuiKey_RightShift;
    case XK_Control_L:
        return ImGuiKey_LeftCtrl;
    case XK_Control_R:
        return ImGuiKey_RightCtrl;
    case XK_Alt_L:
        return ImGuiKey_LeftAlt;
    case XK_Alt_R:
        return ImGuiKey_RightAlt;
    case XK_apostrophe: 
        return ImGuiKey_Apostrophe;
    case XK_comma: 
        return ImGuiKey_Comma;
    case XK_minus: 
        return ImGuiKey_Minus;
    case XK_period: 
        return ImGuiKey_Period;
    case XK_slash: 
        return ImGuiKey_Slash;
    case XK_semicolon: 
        return ImGuiKey_Semicolon;
    case XK_equal: 
        return ImGuiKey_Equal;
    case XK_bracketleft: 
        return ImGuiKey_LeftBracket;
    case XK_backslash: 
        return ImGuiKey_Backslash;
    case XK_bracketright: 
        return ImGuiKey_RightBracket;
    case XK_grave: 
        return ImGuiKey_GraveAccent;
    case XK_Caps_Lock: 
        return ImGuiKey_CapsLock;
    case XK_Scroll_Lock: 
        return ImGuiKey_ScrollLock;
    case XK_Num_Lock: 
        return ImGuiKey_NumLock;
    case XK_Pause: 
        return ImGuiKey_Pause;
    case XK_KP_0: 
        return ImGuiKey_Keypad0;
    case XK_KP_1: 
        return ImGuiKey_Keypad1;
    case XK_KP_2: 
        return ImGuiKey_Keypad2;
    case XK_KP_3: 
        return ImGuiKey_Keypad3;
    case XK_KP_4: 
        return ImGuiKey_Keypad4;
    case XK_KP_5: 
        return ImGuiKey_Keypad5;
    case XK_KP_6: 
        return ImGuiKey_Keypad6;
    case XK_KP_7: 
        return ImGuiKey_Keypad7;
    case XK_KP_8: 
        return ImGuiKey_Keypad8;
    case XK_KP_9: 
        return ImGuiKey_Keypad9;
    case XK_KP_Decimal: 
        return ImGuiKey_KeypadDecimal;
    case XK_KP_Divide: 
        return ImGuiKey_KeypadDivide;
    case XK_KP_Multiply: 
        return ImGuiKey_KeypadMultiply;
    case XK_KP_Subtract: 
        return ImGuiKey_KeypadSubtract;
    case XK_KP_Add: 
        return ImGuiKey_KeypadAdd;
    case XK_Super_L: 
        return ImGuiKey_LeftSuper;
    case XK_Super_R: 
        return ImGuiKey_RightSuper;
    case XK_Menu: 
        return ImGuiKey_Menu;
    case XK_0: 
        return ImGuiKey_0;
    case XK_1: 
        return ImGuiKey_1;
    case XK_2: 
        return ImGuiKey_2;
    case XK_3:
        return ImGuiKey_3;
    case XK_4:
        return ImGuiKey_4;
    case XK_5:
        return ImGuiKey_5;
    case XK_6:
        return ImGuiKey_6;
    case XK_7:
        return ImGuiKey_7;
    case XK_8:
        return ImGuiKey_8;
    case XK_9:
        return ImGuiKey_9;
    case XK_A:
    case XK_a:
        return ImGuiKey_A;
    case XK_b:
    case XK_B:
        return ImGuiKey_B;
    case XK_c:
    case XK_C:
        return ImGuiKey_C;
    case XK_d:
    case XK_D:
        return ImGuiKey_D;
    case XK_e:
    case XK_E:
        return ImGuiKey_E;
    case XK_f:
    case XK_F:
        return ImGuiKey_F;
    case XK_g:
    case XK_G:
        return ImGuiKey_G;
    case XK_h:
    case XK_H:
        return ImGuiKey_H;
    case XK_i:
    case XK_I:
        return ImGuiKey_I;
    case XK_j:
    case XK_J:
        return ImGuiKey_J;
    case XK_k:
    case XK_K:
        return ImGuiKey_K;
    case XK_l:
    case XK_L:
        return ImGuiKey_L;
    case XK_m:
    case XK_M:
        return ImGuiKey_M;
    case XK_n:
    case XK_N:
        return ImGuiKey_N;
    case XK_o:
    case XK_O:
        return ImGuiKey_O;
    case XK_p:
    case XK_P:
        return ImGuiKey_P;
    case XK_q:
    case XK_Q:
        return ImGuiKey_Q;
    case XK_r:
    case XK_R:
        return ImGuiKey_R;
    case XK_s:
    case XK_S:
        return ImGuiKey_S;
    case XK_t:
    case XK_T:
        return ImGuiKey_T;
    case XK_u:
    case XK_U:
        return ImGuiKey_U;
    case XK_v:
    case XK_V:
        return ImGuiKey_V;
    case XK_w:
    case XK_W:
        return ImGuiKey_W;
    case XK_x:
    case XK_X:
        return ImGuiKey_X;
    case XK_y:
    case XK_Y:
        return ImGuiKey_Y;
    case XK_z:
    case XK_Z:
        return ImGuiKey_Z;
    case XK_F1: 
        return ImGuiKey_F1;
    case XK_F2: 
        return ImGuiKey_F2;
    case XK_F3: 
        return ImGuiKey_F3;
    case XK_F4: 
        return ImGuiKey_F4;
    case XK_F5: 
        return ImGuiKey_F5;
    case XK_F6: 
        return ImGuiKey_F6;
    case XK_F7: 
        return ImGuiKey_F7;
    case XK_F8: 
        return ImGuiKey_F8;
    case XK_F9: 
        return ImGuiKey_F9;
    case XK_F10: 
        return ImGuiKey_F10;
    case XK_F11: 
        return ImGuiKey_F11;
    case XK_F12: 
        return ImGuiKey_F12;
    default: 
        return ImGuiKey_None;
    }
}
ImGuiMouseButton XButtonToImGuiMouseButton(wfe::uint32_t button) {
    switch(button) {
    case 1:
        return ImGuiMouseButton_Left;
    case 2:
        return ImGuiMouseButton_Middle;
    case 3:
        return ImGuiMouseButton_Right;
    default:
        return ImGuiMouseButton_COUNT;
    }
}

#endif