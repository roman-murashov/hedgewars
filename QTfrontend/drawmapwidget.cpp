#include <QFile>

#include "drawmapwidget.h"

DrawMapWidget::DrawMapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawMapWidget)
{
    ui->setupUi(this);

    m_scene = 0;
}

DrawMapWidget::~DrawMapWidget()
{
    delete ui;
}

void DrawMapWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DrawMapWidget::setScene(DrawMapScene * scene)
{
    ui->graphicsView->setScene(scene);
}

void DrawMapWidget::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);

    if(ui->graphicsView && ui->graphicsView->scene())
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void DrawMapWidget::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);

    resizeEvent(0);
}

void DrawMapWidget::undo()
{
    if(m_scene) m_scene->undo();
}

void DrawMapWidget::clear()
{
    if(m_scene) m_scene->clear();
}

void DrawMapWidget::save(const QString & fileName)
{
    Q_UNUSED(fileName);
}

void DrawMapWidget::load(const QString & fileName)
{
    Q_UNUSED(fileName);
}
