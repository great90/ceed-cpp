#ifndef LAYOUTMANIPULATOR_H
#define LAYOUTMANIPULATOR_H

#include "src/cegui/CEGUIManipulator.h"

// Layout editing specific widget manipulator

class WidgetHierarchyItem;

class LayoutManipulator : public CEGUIManipulator
{
public:

    static QString getValidWidgetName(const QString& name);

    LayoutManipulator(QGraphicsItem* parent);
    virtual ~LayoutManipulator() override;

    virtual QPointF constrainMovePoint(QPointF value) override;
    virtual QRectF constrainResizeRect(QRectF rect, QRectF oldRect) override;

    virtual void notifyResizeStarted(ResizingHandle* handle) override;
    virtual void notifyResizeProgress(QPointF newPos, QRectF newRect) override;
    virtual void notifyResizeFinished(QPointF newPos, QRectF newRect) override;
    virtual void notifyMoveStarted() override;
    virtual void notifyMoveProgress(QPointF newPos) override;
    virtual void notifyMoveFinished(QPointF newPos) override;

    virtual void updateFromWidget(bool callUpdate = false, bool updateAncestorLCs = false) override;
    virtual void detach(bool detachWidget = true, bool destroyWidget = true, bool recursive = true) override;

    virtual bool preventManipulatorOverlap() const override;
    virtual bool useAbsoluteCoordsForMove() const override;
    virtual bool useAbsoluteCoordsForResize() const override;
    virtual bool useIntegersForAbsoluteMove() const override;
    virtual bool useIntegersForAbsoluteResize() const override;

    void setLocked(bool locked);
    void setTreeItem(WidgetHierarchyItem* treeItem) { _treeItem = treeItem; }
    //void setDrawSnapGrid(bool draw) { _drawSnapGrid = draw; }

protected:

    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent* event) override;

    virtual QPen getNormalPen() const override;
    virtual QPen getHoverPen() const override;
    virtual QPen getPenWhileResizing() const override;
    virtual QPen getPenWhileMoving() const override;

    virtual void impl_paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    QString getUniqueChildWidgetName(const QString& base = "Widget");
    qreal snapXCoordToGrid(qreal x);
    qreal snapYCoordToGrid(qreal y);

    WidgetHierarchyItem* _treeItem = nullptr;
    bool _showOutline = true;
    bool _drawSnapGrid = false;
    bool _snapGridNonClientArea = false;
    bool _ignoreSnapGrid = false;
};

#endif // LAYOUTMANIPULATOR_H
