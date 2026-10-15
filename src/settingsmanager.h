#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

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

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager &instance();

    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void     setValue(const QString &key, const QVariant &value);

    QString stringValue(const QString &key, const QString &def = {}) const;
    int     intValue(const QString &key, int def = 0) const;
    bool    boolValue(const QString &key, bool def = false) const;

    void sync();

signals:
    void settingChanged(const QString &key, const QVariant &value);

private:
    SettingsManager();
    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    QSettings m_settings;
};

namespace SettingsKeys {
inline constexpr auto ThemeName         = "appearance/theme";
inline constexpr auto CurrentLineMode   = "editor/currentLineHighlightMode";
inline constexpr auto EditorZoomPercent = "editor/zoomPercent";
inline constexpr auto NewFileTemplate   = "editor/newFileTemplate";

inline constexpr auto ShowPdfOnStartup  = "preview/showOnStartup";
inline constexpr auto PdfRenderScale    = "preview/renderScale";
inline constexpr auto PdfZoomStep       = "preview/zoomStep";

inline constexpr auto AutoCompileOnSave = "compiler/autoCompileOnSave";

inline constexpr auto RecentFiles       = "files/recent";
inline constexpr auto LastDirectory     = "files/lastDirectory";

inline constexpr auto ShowWelcomeScreen = "startup/showWelcomeScreen";
}

#endif // SETTINGSMANAGER_H