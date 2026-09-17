#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>

namespace SettingsKeys {
inline constexpr auto ThemeName        = "appearance/theme";
inline constexpr auto CurrentLineMode  = "editor/currentLineHighlightMode";
}

enum class CurrentLineHighlight {
    None,
    LineNumberOnly,
    LineBackground,
    Both
};

inline CurrentLineHighlight currentLineHighlightFromString(const QString &s)
{
    if (s == "none")             return CurrentLineHighlight::None;
    if (s == "lineNumberOnly")   return CurrentLineHighlight::LineNumberOnly;
    if (s == "lineBackground")   return CurrentLineHighlight::LineBackground;
    return CurrentLineHighlight::Both;
}

inline QString currentLineHighlightToString(CurrentLineHighlight m)
{
    switch (m) {
    case CurrentLineHighlight::None:            return "none";
    case CurrentLineHighlight::LineNumberOnly:  return "lineNumberOnly";
    case CurrentLineHighlight::LineBackground:  return "lineBackground";
    case CurrentLineHighlight::Both:            return "both";
    }
    return "both";
}

#endif // SETTINGS_H