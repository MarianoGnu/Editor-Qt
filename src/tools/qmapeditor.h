#ifndef QMAPEDITOR_H
#define QMAPEDITOR_H

#include <QOpenGLBuffer>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>
#include <rpg_map.h>

class QMapEditor : public QGLWidget
{
    Q_OBJECT
public:
    explicit QMapEditor(QWidget *parent = 0);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:

    void RegenerateLayerMesh();

    void AttachVertices(float x, float y, int v_count, float depth);
    void AttachUVs(const QVector<QVector2D> &uvs);

    int _x(int index);
    int _y(int index);
    int _index(int x, int y);

    int mapWidth();
    int mapHeight();

    std::auto_ptr<RPG::Map> m_map;
    std::vector<short> m_lower;
    std::vector<short> m_upper;
    QVector<QVector2D> m_UVs;
    QVector<QVector3D> m_vertices;

    QOpenGLBuffer vertexBuffer;
    QOpenGLBuffer uvBuffer;
    QOpenGLShaderProgram transparencyShader;
    //QGLShaderProgram blurShader;
signals:

public slots:

};

#endif // QMAPEDITOR_H
