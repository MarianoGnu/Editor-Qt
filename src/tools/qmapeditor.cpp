#include "qmapeditor.h"

#include <QDebug>
#include "../tiletranslator.h"

QMapEditor::QMapEditor(QWidget *parent) :
    QGLWidget(parent)
{
}

void QMapEditor::initializeGL()
{
//#ifdef WIN32
//    glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress((LPCSTR) "glActiveTexture");
//#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    qglClearColor(palette().background().color());


lightingShaderProgram.addShaderFromSourceFile(QGLShader::Vertex, "://lightingVertexShader.vsh");
lightingShaderProgram.addShaderFromSourceFile(QGLShader::Fragment, "://lightingFragmentShader.fsh");
lightingShaderProgram.link();

cubeBuffer.create();
cubeBuffer.bind();
cubeBuffer.allocate(numCubeVertices * (3 + 3 + 2) * sizeof(GLfloat));

int offset = 0;
cubeBuffer.write(offset, cubeVertices.constData(), numCubeVertices * 3 * sizeof(GLfloat));
offset += numCubeVertices * 3 * sizeof(GLfloat);
cubeBuffer.write(offset, cubeNormals.constData(), numCubeVertices * 3 * sizeof(GLfloat));
offset += numCubeVertices * 3 * sizeof(GLfloat);
cubeBuffer.write(offset, cubeTextureCoordinates.constData(), numCubeVertices * 2 * sizeof(GLfloat));

cubeBuffer.release();

cubeTexture = bindTexture(QPixmap("://texture.png"));

coloringShaderProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/coloringVertexShader.vsh");
coloringShaderProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/coloringFragmentShader.fsh");
coloringShaderProgram.link();

    vertexBuffer.create();
}

void QMapEditor::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);

    glOrtho(0,mapWidth(), 0, mapHeight, 4, 0);
}

void QMapEditor::paintGL()
{

}

void QMapEditor::RegenerateLayerMesh()
{
    m_UVs.clear();
    m_vertices.clear();

    const float z_lower = 0;
    const float z_upper = 1;

    for (unsigned int i = 0; i < m_lower.size(); i++)
    {
        QVector<QVector2D>uvs = TileTranslator::getUvs(m_lower[i]);
        AttachVertices((float)_x(i), (float)_y(i), uvs.count(), z_lower);
        AttachUVs(uvs);

        uvs = TileTranslator::getUvs(m_upper[i]);
        AttachVertices((float)_x(i), (float)_y(i), uvs.count(), z_upper);
        AttachUVs(uvs);
    }


    vertexBuffer.bind();
    vertexBuffer.allocate(numSpotlightVertices * (3 + 3) * sizeof(GLfloat));

    offset = 0;
    vertexBuffer.write(offset, m_vertices.constData(), m_vertices.count() * 3 * sizeof(GLfloat));
    offset += m_vertices.constData(), m_vertices.count() * 3 * sizeof(GLfloat);
    cubeBuffer.write(offset, spotlightColors.constData(), numSpotlightVertices * 3 * sizeof(GLfloat));

    vertexBuffer.release();
}

void QMapEditor::AttachVertices(float x, float y, int v_count, float depth)
{

#define regSquare(x, y, size) \
    m_vertices.append(QVector3D(x, y, depth)); \
    m_vertices.append(QVector3D(x+size, y, depth)); \
    m_vertices.append(QVector3D(x, y+size, depth)); \
    m_vertices.append(QVector3D(x+size, y, depth)); \
    m_vertices.append(QVector3D(x+size, y+size, depth)); \
    m_vertices.append(QVector3D(x, y+size, depth))

    switch (v_count)
    {
    case 6:
        regSquare(x,     y,     1);
        break;
    case 24:
        regSquare(x,     y,     0.5);
        regSquare(x+0.5, y,     0.5);
        regSquare(x+0.5, y+0.5, 0.5);
        regSquare(x,     y+0.5, 0.5);
        break;
    default:
        qDebug() << "Wrong vertex count: " << v_count;
        break;
    }
#undef regSquare
}

void QMapEditor::AttachUVs(const QVector<QVector2D> &uvs)
{
    foreach(QVector2D uv, uvs)
        m_UVs.append((uv));
}

int QMapEditor::_x(int index)
{
    return (index%m_map.get()->width);
}

int QMapEditor::_y(int index)
{
    return (index/m_map.get()->width);
}

int QMapEditor::_index(int x, int y)
{
    return (m_map.get()->width*y+x);
}

int QMapEditor::mapWidth()
{
    if (!m_map.get())
        return 0;
    return m_map->width;
}

int QMapEditor::mapHeight()
{
    if (!m_map.get())
        return 0;
    return m_map->height;
}
