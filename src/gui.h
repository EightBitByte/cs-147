#ifndef GUI_H
#define GUI_H

void initializeScreen();
void renderLockScreen();
void updateUser(const std::string& name);
void renderUnlockedScreen();
std::string getCurrentTime();
void updateInstruction(const std::string& instruction);
void updateTime();
void showNamePrompt();
void updateNamePrompt(const std::string& name);
void flashIncorrect(int index);
void renderPattern(std::string pattern);
void renderPatternConfirm (int pattern_len);
void renderArrow (int index);
void updateActionTitle (std::string title);
void renderToolbar();

#endif