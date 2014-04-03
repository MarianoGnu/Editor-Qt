#include "QGraphicsMapScene.h"
#include <QAction>
#include <QDialogButtonBox>
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QScrollBar>
#include <iomanip>
#include <sstream>
#include "../core.h"
#include "../DialogEvent.h"
#include "../dialogrungame.h"
#include "../mainwindow.h"
#include "qundodraw.h"
#include "qundoevent.h"
#include <data.h>
#include <lmu_reader.h>
#include <lmt_reader.h>

QGraphicsMapScene::QGraphicsMapScene(int id, QGraphicsView *view) :
    QGraphicsScene(view)
{
    m_init = false;
    m_view = view;
    m_view->setMouseTracking(true);
    m_undoStack = new QUndoStack(this);
    n_mapInfo = 0;
    for (unsigned int i = 1; i < Data::treemap.maps.size(); i++)
        if (Data::treemap.maps[i].ID == id)
        {
            m_mapInfo = &(Data::treemap.maps[i]);
            break;
        }
    m_eventMenu = new QMenu(m_view);
    QList<QAction*> actions;
    actions << new QAction(QIcon(":/icons/share/old_playtest.png"),
                           "Start Game Here",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_edit.png"),
                           "Set Start Position",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
                           "New Event",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
                           "Copy Event",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
                           "Cut Event",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
                           "Paste Event",
                           this);
    actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
                           "Delete Event",
                           this);
    connect(actions[0],SIGNAL(triggered()),this,SLOT(on_actionRunHere()));
    connect(actions[1],SIGNAL(triggered()),this, SLOT(on_actionSetStartPosition()));
    connect(actions[2],SIGNAL(triggered()),this,SLOT(on_actionNewEvent()));

    m_eventMenu->addActions(actions);
    Load();
    m_lowerpix = new QGraphicsLayerItem(Core::LOWER, m_map->lower_layer, m_map->width, m_map->height);
    m_lowerpix->setPrimaryAction(QGraphicsLayerItem::Draw);
    m_lowerpix->setSecondaryAction(QGraphicsLayerItem::Select);
    m_upperpix = new QGraphicsLayerItem(Core::UPPER, m_map->upper_layer, m_map->width, m_map->height);
    m_upperpix->setPrimaryAction(QGraphicsLayerItem::Draw);
    m_upperpix->setSecondaryAction(QGraphicsLayerItem::Select);
    addItem(m_lowerpix);
    addItem(m_upperpix);
    QGraphicsBlurEffect * effect = new QGraphicsBlurEffect(this);
    effect->setBlurRadius(2.0);
    effect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
    m_lowerpix->setGraphicsEffect(effect);
    m_upperpix->setGraphicsEffect(new QGraphicsOpacityEffect(this));
    onLayerChanged();
    onToolChanged();
}

QGraphicsMapScene::~QGraphicsMapScene()
{
    delete m_lowerpix;
    delete m_upperpix;
    m_eventpixs.clear();
}

void QGraphicsMapScene::Init()
{
    connect(m_view->verticalScrollBar(),
            SIGNAL(actionTriggered(int)),
            this,
            SLOT(on_user_interaction()));
    connect(m_view->horizontalScrollBar(),
            SIGNAL(actionTriggered(int)),
            this,
            SLOT(on_user_interaction()));
    connect(m_view->verticalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(redrawMap()));
    connect(m_view->horizontalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(redrawMap()));
    connect(m_view->verticalScrollBar(),
            SIGNAL(rangeChanged(int,int)),
            this,
            SLOT(redrawMap()));
    connect(m_view->horizontalScrollBar(),
            SIGNAL(rangeChanged(int,int)),
            this,
            SLOT(redrawMap()));
    connect(m_view->verticalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(on_view_V_Scroll()));
    connect(m_view->horizontalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(on_view_H_Scroll()));
    connect(mCore,
            SIGNAL(toolChanged()),
            this,
            SLOT(onToolChanged()));
    connect(mCore,
            SIGNAL(layerChanged()),
            this,
            SLOT(onLayerChanged()));
    m_view->verticalScrollBar()->setValue(m_mapInfo->scrollbar_y*m_scale);
    m_view->horizontalScrollBar()->setValue(m_mapInfo->scrollbar_x*m_scale);
    m_init = true;
    redrawMap();
}

float QGraphicsMapScene::scale() const
{
    return m_scale;
}

QString QGraphicsMapScene::mapName() const
{
    if (n_mapInfo)
        return QString::fromStdString(n_mapInfo->name);
    return QString::fromStdString(m_mapInfo->name);
}

bool QGraphicsMapScene::isModified() const
{
    return (m_undoStack->count() > 0);
}

int QGraphicsMapScene::id() const
{
    return m_mapInfo->ID;
}

int QGraphicsMapScene::chipsetId() const
{
    return m_map->chipset_id;
}

void QGraphicsMapScene::setLayerData(Core::Layer layer, std::vector<short> data)
{
    if (layer == Core::LOWER)
    {
//        m_lower = data;
        m_map->lower_layer = data;
    }
    else
    {
//        m_upper = data;
        m_map->upper_layer = data;
    }
}

void QGraphicsMapScene::setEventData(int id, const RPG::Event &data)
{
    for (unsigned int i = 0; i < m_map->events.size(); i++)
        if (m_map->events[i].ID == id)
            m_map->events[i] = data;
}

QMap<int, RPG::Event*> *QGraphicsMapScene::mapEvents()
{
    QMap<int, RPG::Event*> *events = new QMap<int, RPG::Event*>();
    for (unsigned int i = 0; i < m_map->events.size(); i++)
        events->insert(m_map->events[i].ID, &m_map->events[i]);
    return events;
}

void QGraphicsMapScene::redrawMap()
{
    if (!m_init)
        return;
    mCore->LoadChipset(m_map->chipset_id);
    this->update();
}

void QGraphicsMapScene::setScale(float scale)
{
    m_scale = scale;
    QList<QGraphicsItem*> itms = items();
    for(int i = 0; i < itms.count(); i++)
    {
        itms[i]->setScale(scale);
    }
    this->setSceneRect(m_lowerpix->boundingRect());
}

void QGraphicsMapScene::onLayerChanged()
{
    //TODO: CANCEL CURRENT ACTION?
    switch (mCore->layer())
    {
    case (Core::LOWER):
        m_lowerpix->graphicsEffect()->setEnabled(false);
        m_upperpix->graphicsEffect()->setEnabled(true);
        break;
    case (Core::UPPER):
        m_lowerpix->graphicsEffect()->setEnabled(true);
        m_upperpix->graphicsEffect()->setEnabled(false);
        break;
    default:
        m_lowerpix->graphicsEffect()->setEnabled(false);
        m_upperpix->graphicsEffect()->setEnabled(false);
        break;
    }
    for (int i = 0; i < m_lines.count(); i++)
        m_lines[i]->setVisible(mCore->layer() == Core::EVENT);
}

void QGraphicsMapScene::onToolChanged()
{
    //TODO: CANCEL CURRENT ACTION?
    switch (mCore->tool())
    {
    case (Core::ZOOM):
        m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_zoom.png"),1,1));
        break;
    case (Core::PENCIL):
        m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_pen.png"),1,1));
        break;
    case (Core::RECTANGLE):
        m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_rectangle.png"),1,1));
        break;
    case (Core::CIRCLE):
        m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_circle.png"),1,1));
        break;
    case (Core::FILL):
        m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_fill.png"),1,1));
        break;
    }
}

void QGraphicsMapScene::Save()
{
    if (!isModified())
        return;
    if (n_mapInfo)
    {
        m_mapInfo->area_rect.b = n_mapInfo->area_rect.b;
        m_mapInfo->area_rect.l = n_mapInfo->area_rect.l;
        m_mapInfo->area_rect.r = n_mapInfo->area_rect.r;
        m_mapInfo->area_rect.t = n_mapInfo->area_rect.t;
        m_mapInfo->background_name = n_mapInfo->background_name;
        m_mapInfo->background_type = n_mapInfo->background_type;
        m_mapInfo->encounters = n_mapInfo->encounters;
        m_mapInfo->encounter_steps = n_mapInfo->encounter_steps;
        m_mapInfo->escape = n_mapInfo->escape;
        m_mapInfo->music.balance = n_mapInfo->music.balance;
        m_mapInfo->music.fadein = n_mapInfo->music.fadein;
        m_mapInfo->music.name = n_mapInfo->music.name;
        m_mapInfo->music.tempo = n_mapInfo->music.tempo;
        m_mapInfo->music.volume = n_mapInfo->music.volume;
        m_mapInfo->music_type = n_mapInfo->music_type;
        m_mapInfo->name = n_mapInfo->name;
        m_mapInfo->teleport = n_mapInfo->teleport;
        LMT_Reader::SaveXml(mCore->filePath(ROOT,EASY_MT).toStdString());
        delete n_mapInfo;
    }
    std::stringstream ss;
    ss << mCore->filePath(ROOT).toStdString()
       << "Map"
       << std::setfill('0')
       << std::setw(4)
       << id()
       << ".emu";
    LMU_Reader::SaveXml(ss.str(), *m_map.get());    
    m_undoStack->clear();
    emit mapSaved();
}

void QGraphicsMapScene::Load()
{
    delete n_mapInfo;
    std::stringstream ss;
    ss << mCore->filePath(ROOT).toStdString()
       << "Map"
       << std::setfill('0')
       << std::setw(4)
       << m_mapInfo->ID
       << ".emu";
    m_map = LMU_Reader::LoadXml(ss.str());
    for (int i = 0; i < m_lines.count(); i++)
        removeItem(m_lines[i]);
    m_lines.clear();
    for (int x = 0; x <= m_map->width; x++)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(x*mCore->tileSize(),
                                                        0,
                                                        x*mCore->tileSize(),
                                                        m_map->height*mCore->tileSize());
        m_lines.append(line);
        line->setScale(m_scale);
        line->setVisible(mCore->layer() == Core::EVENT);
        addItem(line);
    }
    for (int y = 0; y <= m_map->height; y++)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(0,
                                                        y*mCore->tileSize(),
                                                        m_map->width*mCore->tileSize(),
                                                        y*mCore->tileSize());
        m_lines.append(line);
        line->setScale(m_scale);
        line->setVisible(mCore->layer() == Core::EVENT);
        addItem(line);
    }
    m_undoStack->clear();
    emit mapReverted();
}

void QGraphicsMapScene::undo()
{
    m_undoStack->undo();
    if (m_undoStack->index() == 0)
    {
        m_undoStack->clear();
        emit mapReverted();
    }
}

void QGraphicsMapScene::on_actionNewEvent()
{

}
void QGraphicsMapScene::on_actionRunHere()
{
    emit actionRunHereTriggered(id(),lst_x,lst_y);
}

void QGraphicsMapScene::on_actionSetStartPosition()
{
    Data::treemap.start.party_map_id = this->id();
    Data::treemap.start.party_x = lst_x;
    Data::treemap.start.party_y = lst_y;
    LMT_Reader::SaveXml(mCore->filePath(ROOT,EASY_MT).toStdString());
}

void QGraphicsMapScene::on_user_interaction()
{
    m_userInteraction = true;
}

void QGraphicsMapScene::on_view_V_Scroll()
{
    if (!m_userInteraction || !m_init)
        return;
    if (m_view->verticalScrollBar()->isVisible())
    {
        m_mapInfo->scrollbar_y = m_view->verticalScrollBar()->value()/m_scale;
        if (n_mapInfo)
            n_mapInfo->scrollbar_y = m_view->verticalScrollBar()->value()/m_scale;
    }
    m_userInteraction = false;
    LMT_Reader::SaveXml(mCore->filePath(ROOT,EASY_MT).toStdString());
}

void QGraphicsMapScene::on_view_H_Scroll()
{
    if (!m_userInteraction || !m_init)
        return;
    if (m_view->horizontalScrollBar()->isVisible())
    {
        m_mapInfo->scrollbar_x = m_view->horizontalScrollBar()->value()/m_scale;
        if (n_mapInfo)
            n_mapInfo->scrollbar_x = m_view->horizontalScrollBar()->value()/m_scale;
    }
    m_userInteraction = false;
    LMT_Reader::SaveXml(mCore->filePath(ROOT,EASY_MT).toStdString());
}

void QGraphicsMapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        if (sceneRect().contains(event->scenePos()) && mCore->layer() == Core::EVENT)
        {
            lst_x = event->scenePos().x()/s_tileSize;
            lst_y = event->scenePos().y()/s_tileSize;
            m_eventMenu->popup(event->screenPos());
        }
        if (mCore->tool() == Core::ZOOM && m_scale > 0.25)
            setScale(m_scale/2);
    }
    if (event->button() == Qt::LeftButton)
    {
        if (mCore->tool() == Core::ZOOM && m_scale < 2.0)
            setScale(m_scale*2);
    }
}

void QGraphicsMapScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (mCore->layer() != Core::EVENT)
        return;
    for (unsigned int i = 0; i < m_map->events.size(); i++)
        if (event->scenePos().x()/s_tileSize == m_map->events[i].x &&
            event->scenePos().y()/s_tileSize == m_map->events[i].y)
        {
            RPG::Event backup = m_map->events[i];
            int result = DialogEvent::edit(m_view, &m_map->events[i]);
            if (result != QDialogButtonBox::Cancel)
            {
                m_undoStack->push(new QUndoEvent(backup,
                                                 this));
                emit mapChanged();
            }
        }
}
