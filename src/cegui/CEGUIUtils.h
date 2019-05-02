#ifndef CEGUIUTILS_H
#define CEGUIUTILS_H

#include "qstring.h"

namespace CEGUI
{
    class String;
    class Window;
}

namespace CEGUIUtils
{
    QString stringToQString(const CEGUI::String& str);
    CEGUI::String qStringToString(const QString& str);

    bool serializeWidget(const CEGUI::Window& widget, QDataStream& stream, bool recursive = true);
    CEGUI::Window* deserializeWidget(const QByteArray& data, CEGUI::Window* parent = nullptr);
};

#endif // CEGUIUTILS_H
