#ifndef TILETRANSLATOR_H
#define TILETRANSLATOR_H

#include <QMap>
#include <QVector>
#include <QVector2D>

class TileTranslator
{
public:

    static short translate(int terrain_id, int _code = 0, int _scode = 0);
    static int translate(short tile_id);
    static QVector<QVector2D> getUvs(short tile_id);

private:
    TileTranslator();

    static TileTranslator *m_instance;

    static TileTranslator *instance()
    {
        if (m_instance == 0)
        {
            m_instance = new TileTranslator();
        }

        return m_instance;
    }

    QMap<int, short> m_dictionary;
    QMap<short, QVector<QVector2D>> m_uvCache;
};

#endif // TILETRANSLATOR_H
