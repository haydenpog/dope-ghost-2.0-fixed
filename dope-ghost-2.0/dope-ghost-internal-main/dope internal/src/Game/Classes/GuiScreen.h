#pragma once

class GuiScreen
{
public:
    bool IsInventory(JNIEnv* env);
    bool IsChat(JNIEnv* env);

    bool IsInMenu() const {
        return this != NULL;
    }
};