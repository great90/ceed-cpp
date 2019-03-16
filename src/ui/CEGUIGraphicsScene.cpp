#include "src/ui/CEGUIGraphicsScene.h"
#include "src/util/Settings.h"
#include "src/Application.h"
#include "qpainter.h"
#include "qpaintengine.h"

CEGUIGraphicsScene::CEGUIGraphicsScene()
    : timeOfLastRender(time(nullptr))
{
    setCEGUIDisplaySize(800, 600, true);
/*
    self.ceguiInstance = ceguiInstance
    self.fbo = None
*/
    auto&& settings = qobject_cast<Application*>(qApp)->getSettings();
    checkerWidth = settings->getEntryValue("cegui/background/checker_width").toInt();
    checkerHeight = settings->getEntryValue("cegui/background/checker_height").toInt();
    checkerFirstColour = settings->getEntryValue("cegui/background/first_colour").value<QColor>();
    checkerSecondColour = settings->getEntryValue("cegui/background/second_colour").value<QColor>();

    checkerboardBrush = getCheckerboardBrush(checkerWidth, checkerHeight, checkerFirstColour, checkerSecondColour);
}

void CEGUIGraphicsScene::setCEGUIDisplaySize(float width, float height, bool lazyUpdate)
{
    contextWidth = width;
    contextHeight = height;

    qreal qPadding = static_cast<qreal>(padding);
    qreal qWidth = static_cast<qreal>(width);
    qreal qHeight = static_cast<qreal>(height);
    setSceneRect(QRectF(-qPadding, -qPadding, qWidth + 2.0 * qPadding, qHeight + 2.0 * qPadding));

    /*
    if (!lazyUpdate)
        PyCEGUI.System.getSingleton().notifyDisplaySizeChanged(self.ceguiDisplaySize);

    self.fbo = None
    */
}

// We override this and draw CEGUI instead of the whole background.
// This method uses a FBO to implement zooming, scrolling around, etc...
// FBOs are therefore required by CEED and it won't run without a GPU that supports them.
void CEGUIGraphicsScene::drawBackground(QPainter* painter, const QRectF&)
{
    //???!!!use rect arg?!

    // Be robust, this is usually caused by recursive repainting
    if (!painter->paintEngine())
    {
        // TODO: this assert is to check is this the case in Qt 5 still.
        //       Remove it (or the entire check) after testing.
        assert(false);
        return;
    }

    auto painterType = painter->paintEngine()->type();
    if (painterType != QPaintEngine::OpenGL && painterType != QPaintEngine::OpenGL2)
    {
        qWarning("cegui.GraphicsScene: drawBackground needs a "
                 "QOpenGLWidget to be set as a viewport on the graphics view");
        return;
    }

    auto currTime = time(nullptr);
    lastDelta = currTime - timeOfLastRender;
    timeOfLastRender = currTime;
/*
    system = PyCEGUI.System.getSingleton()
    self.ceguiInstance.lastRenderTimeDelta = self.lastDelta
    system.injectTimePulse(self.lastDelta)
    system.getDefaultGUIContext().injectTimePulse(self.lastDelta)
*/
    painter->setPen(Qt::transparent);
    painter->setBrush(checkerboardBrush);
    painter->drawRect(0, 0, static_cast<int>(contextWidth), static_cast<int>(contextHeight));

    painter->beginNativePainting();

/*
    self.ceguiInstance.ensureIsInitialised()

    if self.ceguiDisplaySize != PyCEGUI.System.getSingleton().getRenderer().getDisplaySize():
        # FIXME: Change when multi root is in CEGUI core
        PyCEGUI.System.getSingleton().notifyDisplaySizeChanged(self.ceguiDisplaySize)

    # markAsDirty is called on the default GUI context to work around potential issues with dangling
    # references in the rendering code for some versions of CEGUI.
    system.getDefaultGUIContext().markAsDirty()

    # we have to render to FBO and then scale/translate that since CEGUI doesn't allow
    # scaling the whole rendering root directly

    # this makes sure the FBO is the correct size
    if not self.fbo:
        desiredSize = QtCore.QSize(math.ceil(self.ceguiDisplaySize.d_width), math.ceil(self.ceguiDisplaySize.d_height))
        self.fbo = QtOpenGL.QGLFramebufferObject(desiredSize, GL.GL_TEXTURE_2D)

    self.fbo.bind()

    GL.glClearColor(0, 0, 0, 0)
    GL.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT)

    system.renderAllGUIContexts()

    self.fbo.release()

    # the stretch and translation should be done automatically by QPainter at this point so just
    # this code will do
    if bool(GL.glActiveTexture):
        GL.glActiveTexture(GL.GL_TEXTURE0)

    GL.glEnable(GL.GL_TEXTURE_2D)
    GL.glBindTexture(GL.GL_TEXTURE_2D, self.fbo.texture())

    GL.glEnable(GL.GL_BLEND)
    GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

    # TODO: I was told that this is the slowest method to draw with OpenGL,
    #       with which I certainly agree.
    #
    #       No profiling has been done at all and I don't suspect this to be
    #       a painful performance problem.
    #
    #       Still, changing this to a less pathetic rendering method would be great.

    GL.glBegin(GL.GL_TRIANGLES)

    # top left
    GL.glTexCoord2f(0, 1)
    GL.glVertex3f(0, 0, 0)

    # top right
    GL.glTexCoord2f(1, 1)
    GL.glVertex3f(self.fbo.size().width(), 0, 0)

    # bottom right
    GL.glTexCoord2f(1, 0)
    GL.glVertex3f(self.fbo.size().width(), self.fbo.size().height(), 0)

    # bottom right
    GL.glTexCoord2f(1, 0)
    GL.glVertex3f(self.fbo.size().width(), self.fbo.size().height(), 0)

    # bottom left
    GL.glTexCoord2f(0, 0)
    GL.glVertex3f(0, self.fbo.size().height(), 0)

    # top left
    GL.glTexCoord2f(0, 1)
    GL.glVertex3f(0, 0, 0)

    system.getDefaultGUIContext().markAsDirty()

    GL.glEnd()
*/

    painter->endNativePainting();
}

// Small helper function that generates a brush usually seen in graphics editing tools. The checkerboard brush
// that draws background seen when edited images are transparent.
QBrush CEGUIGraphicsScene::getCheckerboardBrush(int halfWidth, int halfHeight, QColor firstColour, QColor secondColour)
{
    // Disallow too large half sizes to prevent crashes in QPainter and slowness in general
    halfWidth = std::min(halfWidth, 256);
    halfHeight = std::min(halfHeight, 256);

    QBrush ret;
    QPixmap texture(2 * halfWidth, 2 * halfHeight);

    // Render checker
    {
        QPainter painter(&texture);
        painter.fillRect(0, 0, halfWidth, halfHeight, firstColour);
        painter.fillRect(halfWidth, halfHeight, halfWidth, halfHeight, firstColour);
        painter.fillRect(halfWidth, 0, halfWidth, halfHeight, secondColour);
        painter.fillRect(0, halfHeight, halfWidth, halfHeight, secondColour);
    }

    ret.setTexture(texture);

    return ret;
}
