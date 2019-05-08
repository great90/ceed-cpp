#ifndef CEGUIUTILS_H
#define CEGUIUTILS_H

#include "qstring.h"
#include <CEGUI/InputEvent.h>
#include "3rdParty/QtnProperty/Core/Enum.h"

namespace CEGUI
{
    class String;
    class Window;
}

namespace CEGUIUtils
{
    QString stringToQString(const CEGUI::String& str);
    CEGUI::String qStringToString(const QString& str);

    QString getValidWidgetName(const QString& name);

    QString getUniqueChildWidgetName(const CEGUI::Window& parent, const QString& baseName);
    CEGUI::String getUniqueChildWidgetName(const CEGUI::Window& parent, const CEGUI::String& baseName);

    bool serializeWidget(const CEGUI::Window& widget, QDataStream& stream, bool recursive);
    CEGUI::Window* deserializeWidget(QDataStream& stream, CEGUI::Window* parent = nullptr);

    CEGUI::MouseButton qtMouseButtonToMouseButton(Qt::MouseButton button);
    CEGUI::Key::Scan qtKeyToKey(int key);

    // Property framework support
    const QtnEnumInfo& enumHorizontalAlignment();
    const QtnEnumInfo& enumVerticalAlignment();
    const QtnEnumInfo& enumAspectMode();
    const QtnEnumInfo& enumDefaultParagraphDirection();
    const QtnEnumInfo& enumWindowUpdateMode();
    const QtnEnumInfo& enumHorizontalFormatting();
    const QtnEnumInfo& enumVerticalFormatting();
    const QtnEnumInfo& enumHorizontalTextFormatting();
    const QtnEnumInfo& enumVerticalTextFormatting();
};

#endif // CEGUIUTILS_H
