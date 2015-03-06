#include "tiletranslator.h"

short TileTranslator::translate(int terrain_id, int _code, int _scode)
{
    if (terrain_id < 0)
        return 0x7FFF;
    if (isWater(terrain_id))
        return (terrain_id*1000+instance()->m_dictionary[_code]+instance()->m_dictionary[_scode]*50);
    if (isAnimation(terrain_id))
        return (3000+(terrain_id-3)*50+46);
    if (isDblock(terrain_id))
        return (4000+(terrain_id-6)*50+instance()->m_dictionary[_code]);
    if (isEblock(terrain_id))
        return (5000+terrain_id-18);
    if (isFblock(terrain_id))
        return (10000+terrain_id-162);
    return 0x7FFF;
}

int TileTranslator::translate(short tile_id)
{
    if (tile_id < 0)
        return -1;
    else if (tile_id < 3000)
        return tile_id/1000;
    else if (tile_id >= 3000 && tile_id <= 3150)
        return (tile_id-3000)/50+3;
    else if (tile_id >=4000 && tile_id < 4600 )
        return (tile_id-4000)/50+6;
    else if (tile_id >= 5000 && tile_id < 5162)
        return tile_id-5000+18;
    else if (tile_id >= 10000 && tile_id < 10144)
        return tile_id-10000+161;
    else
        return -1;
}

QVector<QVector2D> TileTranslator::getUvs(short tile_id)
{
    return instance()->m_uvCache[tile_id];
}

TileTranslator::TileTranslator()
{
    m_dictionary[UPLEFT]                            = 1;
    m_dictionary[UPRIGHT]                           = 2;
    m_dictionary[UPLEFT+UPRIGHT]                    = 3;
    m_dictionary[DOWNRIGHT]                         = 4;
    m_dictionary[UPLEFT+DOWNRIGHT]                  = 5;
    m_dictionary[UPRIGHT+DOWNRIGHT]                 = 6;
    m_dictionary[UPLEFT+UPRIGHT+DOWNRIGHT]          = 7;
    m_dictionary[DOWNLEFT]                          = 8;
    m_dictionary[DOWNLEFT+UPLEFT]                   = 9;
    m_dictionary[DOWNLEFT+UPRIGHT]                  = 10;
    m_dictionary[DOWNLEFT+UPLEFT+UPRIGHT]           = 11;
    m_dictionary[DOWNLEFT+DOWNRIGHT]                = 12;
    m_dictionary[DOWNLEFT+DOWNRIGHT+UPLEFT]         = 13;
    m_dictionary[DOWNLEFT+DOWNRIGHT+UPRIGHT]        = 14;
    m_dictionary[DOWNLEFT+DOWNRIGHT+UPLEFT+UPRIGHT] = 15;
    m_dictionary[LEFT]                              = 16;
    m_dictionary[LEFT+UPRIGHT]                      = 17;
    m_dictionary[LEFT+DOWNRIGHT]                    = 18;
    m_dictionary[LEFT+UPRIGHT+DOWNRIGHT]            = 19;
    m_dictionary[UP]                                = 20;
    m_dictionary[UP+DOWNRIGHT]                      = 21;
    m_dictionary[UP+DOWNLEFT]                       = 22;
    m_dictionary[UP+DOWNRIGHT+DOWNLEFT]             = 23;
    m_dictionary[RIGHT]                             = 24;
    m_dictionary[RIGHT+DOWNLEFT]                    = 25;
    m_dictionary[RIGHT+UPLEFT]                      = 26;
    m_dictionary[RIGHT+UPLEFT+DOWNLEFT]             = 27;
    m_dictionary[DOWN]                              = 28;
    m_dictionary[DOWN+UPLEFT]                       = 29;
    m_dictionary[DOWN+UPRIGHT]                      = 30;
    m_dictionary[DOWN+UPLEFT+UPRIGHT]               = 31;
    m_dictionary[LEFT+RIGHT]                        = 32;
    m_dictionary[UP+DOWN]                           = 33;
    m_dictionary[UP+LEFT]                           = 34;
    m_dictionary[UP+LEFT+DOWNRIGHT]                 = 35;
    m_dictionary[UP+RIGHT]                          = 36;
    m_dictionary[UP+RIGHT+DOWNLEFT]                 = 37;
    m_dictionary[DOWN+RIGHT]                        = 38;
    m_dictionary[DOWN+RIGHT+UPLEFT]                 = 39;
    m_dictionary[DOWN+LEFT]                         = 40;
    m_dictionary[DOWN+LEFT+UPRIGHT]                 = 41;
    m_dictionary[UP+LEFT+RIGHT]                     = 42;
    m_dictionary[UP+LEFT+DOWN]                      = 43;
    m_dictionary[LEFT+RIGHT+DOWN]                   = 44;
    m_dictionary[UP+RIGHT+DOWN]                     = 45;
    m_dictionary[UP+DOWN+LEFT+RIGHT]                = 46;
    m_dictionary[SAMPLE]                            = 47;

    const float r_width = 1 / 30;
    const float r_halfWidth = 1 / 60;
    QVector<QVector2D> uv();

#define regQuarter(x,y) \
    uv.append(QVector2D(x, 1.0 - y)); \
    uv.append(QVector2D((x) + r_halfWidth, 1.0 - y)); \
    uv.append(QVector2D(x, 1.0 - (y) - r_halfWidth)); \
    uv.append(QVector2D((x) + r_halfWidth, 1.0 - y)); \
    uv.append(QVector2D((x) + r_halfWidth, 1.0 - (y) - r_halfWidth)); \
    uv.append(QVector2D(x, 1.0 - (y) - r_halfWidth))

#define regTile(x,y) \
    uv.append(QVector2D(x, 1.0 - y)); \
    uv.append(QVector2D((x) + r_width, 1.0 - y)); \
    uv.append(QVector2D(x, 1.0 - (y) - r_width)); \
    uv.append(QVector2D((x) + r_width, 1.0 - y)); \
    uv.append(QVector2D((x) + r_width, 1.0 - (y) - r_width)); \
    uv.append(QVector2D(x, 1.0 - (y) - r_width))

    /* TODO: Event UVs */



    /* BindWaterTiles */
    m_uvCache.clear();

    /*
     * TileIDs:
     * 0- WaterA
     * 1- WaterB
     * 2- DeepWater
     * 3- Ground
     */
    //Go throught all posible combinations
    for (int terrain_id = 0; terrain_id < 4; terrain_id++)
    for (int tile_u = 0; tile_u < 4; tile_u++)
    for (int tile_d = 0; tile_d < 4; tile_d++)
    for (int tile_l = 0; tile_l < 4; tile_l++)
    for (int tile_r = 0; tile_r < 4; tile_r++)
    for (int tile_ul = 0; tile_ul < 4; tile_ul++)
    for (int tile_ur = 0; tile_ur < 4; tile_ur++)
    for (int tile_dl = 0; tile_dl < 4; tile_dl++)
    for (int tile_dr = 0; tile_dr < 4; tile_dr++)
    {
        int _code, _scode;
        if (terrain_id == 3)
            // Once main foreach reaches ground, we have finished
             break;
        int u=0,d=0,l=0,r=0,ul=0,ur=0,dl=0,dr=0,sul=0,sur=0,sdl=0,sdr=0;

        if (!isWater(tile_u))
            u = UP;
        if (!isWater(tile_d))
            d = DOWN;
        if (!isWater(tile_l))
            l = LEFT;
        if (!isWater(tile_r))
            r = RIGHT;
        _code = u+d+l+r;
        // DeepWater Special Corners
        if (isDWater(terrain_id))
        {
            if (isABWater(tile_u) && isABWater (tile_l) && isABWater (tile_ul))
                sul = UPLEFT;
            if (isABWater(tile_u) && isABWater (tile_r) && isABWater (tile_ur))
                sur = UPRIGHT;
            if (isABWater(tile_d) && isABWater (tile_l) && isABWater (tile_dl))
                sdl = DOWNLEFT;
            if (isABWater(tile_d) && isABWater (tile_r) && isABWater (tile_dr))
                sdr = DOWNRIGHT;
        }
        else
        {
            if (isDWater (tile_u) && isDWater (tile_l) && isWater(tile_ul))
                sul = UPLEFT;
            if (isDWater (tile_u) && isDWater (tile_r) && isWater(tile_ur))
                sur = UPRIGHT;
            if (isDWater (tile_d) && isDWater (tile_l) && isWater(tile_dl))
                sdl = DOWNLEFT;
            if (isDWater (tile_d) && isDWater (tile_r) && isWater(tile_dr))
                sdr = DOWNRIGHT;
        }
        _scode = sul+sur+sdl+sdr;
        if ((u+l) == 0 && tile_ul == 3)
            ul = UPLEFT;
        if ((u+r) == 0 && tile_ur == 3)
            ur = UPRIGHT;
        if ((d+l) == 0 && tile_dl == 3)
            dl = DOWNLEFT;
        if ((d+r) == 0 && tile_dr == 3)
            dr = DOWNRIGHT;
        _code += ul+ur+dl+dr;
        short id = translate(terrain_id,_code,_scode);

        if (!m_uvCache[id].isEmpty()) //item exist?
            continue;

        uv.clear();

        // Water B uses second block of 3x4 tiles for borders
        // Water A and Deep Water uses first block
        float border_xoffset = (terrain_id == 1) ? r_width * 3 : 0;

        // Register UpperLeft corner
        if (u+l == 5)

            regQuarter(border_xoffset, 0);
        else if (u)
            regQuarter(border_xoffset, 2 * r_width);
        else if (l)
            regQuarter(border_xoffset, r_width);
        else if (ul)
            regQuarter(border_xoffset, 3 * r_width);
        else if (sul)
        {
            if (isABWater (terrain_id))
                regQuarter(0, 5*r_width);
            else
                regQuarter(0, 6*r_width);
        }
        else
        {
            if (isABWater (terrain_id))
                regQuarter(0, 4*r_width);
            else
                regQuarter(0, 7*r_width);
        }

        //Draw UpperRight corner
        if (u+r == 9)
            regQuarter(border_xoffset+r_width, 0);
        else if (u)
            regQuarter(border_xoffset+r_width, 2*r_width);
        else if (r)
            regQuarter(border_xoffset+r_width, r_width);
        else if (ur)
            regQuarter(border_xoffset+r_width, 3*r_width);
        else if (sur)
        {
            if (isABWater (terrain_id))
                regQuarter(r_halfWidth, 5*r_width);
            else
                regQuarter(r_halfWidth, 6*r_width);
        }
        else
        {
            if (isABWater (terrain_id))
                regQuarter(r_halfWidth, 4*r_width);
            else
                regQuarter(r_halfWidth, 7*r_width);
        }

        //Draw LowerRight corner
        if (d+r == 10)
            regQuarter(border_xoffset+r_halfWidth, 0.5*r_width);
        else if (d)
            regQuarter(border_xoffset+r_halfWidth, 2.5*r_width);
        else if (r)
            regQuarter(border_xoffset+r_halfWidth, 1.5*r_width);
        else if (dr)
            regQuarter(border_xoffset+r_halfWidth, 3.5*r_width);
        else if (sdr)
        {
            if (isABWater (terrain_id))
                regQuarter(r_halfWidth, 5.5*r_width);
            else
                regQuarter(r_halfWidth, 6.5*r_width);
        }
        else
        {
            if (isABWater (terrain_id))
                regQuarter(r_halfWidth, 9*r_halfWidth);
            else
                regQuarter(r_halfWidth, 15*r_halfWidth);
        }

        //Draw LowerLeft corner
        if (d+l == 6)
            regQuarter(border_xoffset, 0.5*r_width);
        else if (d)
            regQuarter(border_xoffset, 2.5*r_width);
        else if (l)
            regQuarter(border_xoffset, 1.5*r_width);
        else if (dl)
            regQuarter(border_xoffset, 3.5*r_width);
        else if (sdl)
        {
            if (isABWater (terrain_id))
                regQuarter(0, 5.5*r_width);
            else
                regQuarter(0, 6.5*r_width);
        }
        else
        {
            if (isABWater (terrain_id))
                regQuarter(0, 9*r_halfWidth);
            else
                regQuarter(0, 15*r_halfWidth);
        }

        m_uvCache[id] = uv;
    }

    /* Register AnimationTiles */

    uv.clear();
    regTile(3*r_width,4*r_width);
    m_uvCache[translate(3)] = uv;
    uv.clear();
    regTile(4*r_width,4*r_width);
    m_uvCache[translate(4)] = uv;
    uv.clear();
    regTile(5*r_width,4*r_width);
    m_uvCache[translate(5)] = uv;

    /* BindGroundTiles */

#define regGrQuaterter(x, y) regQuarter(orig_x + x, orig_y + y)

    // Each tileset contains 5 columns with a size of 6x16 tiles
    float tileset_col = 0;

    // Each tileset column contains 4 blocks with a size of 3x4 tiles
    // Anyway we should skip water colums because they are already stored
    float block_col = 0;
    float block_row = 2;

    int terrain_id = 6;

    while (tileset_col < 2)
    {
        float orig_x = (2 * tileset_col + block_col) *3*r_width;
        float orig_y = block_row *  4*r_width;

        Q_UNUSED(orig_x)
        Q_UNUSED(orig_y)
        /*
         * Generate binded cache and store them on hash table
         */

        //Simulate all posible combinations

        std::vector<bool> is_binded;
        is_binded.push_back(true);
        is_binded.push_back(false);

#define forx(_type,_iter,_coll) for (_type::iterator _iter = _coll.begin(); _iter != _coll.end(); _iter++)

        forx(std::vector<bool>, bu, is_binded)
        forx(std::vector<bool>, bd, is_binded)
        forx(std::vector<bool>, bl, is_binded)
        forx(std::vector<bool>, br, is_binded)
        forx(std::vector<bool>, bul, is_binded)
        forx(std::vector<bool>, bur, is_binded)
        forx(std::vector<bool>, bdl, is_binded)
        forx(std::vector<bool>, bdr, is_binded)
#undef forx
        {
            int u = 0, d = 0, l = 0, r = 0;
            if (*bu) u = UP;
            if (*bd) d = DOWN;
            if (*bl) l = LEFT;
            if (*br) r = RIGHT;
            int ul = 0, ur = 0, dl = 0, dr = 0;
            if (u + l == 0 && *bul)
                ul = UPLEFT;
            if (u + r == 0 && *bur)
                ur = UPRIGHT;
            if (d + l == 0 && *bdl)
                dl = DOWNLEFT;
            if (d + r == 0 && *bdr)
                dr = DOWNRIGHT;

            short id = translate (terrain_id, u+d+l+r+ul+ur+dl+dr);
            if (!m_uvCache[id].isEmpty()) //item exist?
                continue;

            uv.clear();

            /*
             * Draw upper_left corner
             */
            if (u+l == 5)
                regGrQuaterter(0, r_width);
            else if (u)
            {
                if (r)
                    regGrQuaterter(r_width*2, r_width);
                else
                    regGrQuaterter(r_width, r_width);
            }
            else if (l)
            {
                if (d)
                    regGrQuaterter(0, r_width*3);
                else
                    regGrQuaterter(0, r_width*2);
            }
            else if (ul)
                regGrQuaterter(r_width*2, 0);
            else //0
            {
                if (d+r == 10)
                    regGrQuaterter(r_width*2, r_width*3);
                else if (r)
                    regGrQuaterter(r_width*2, r_width*2);
                else if (d)
                    regGrQuaterter(r_width*1, r_width*3);
                else
                    regGrQuaterter(r_width*1, r_width*2);
            }
            /*
             * Draw upper_right corner
             */
            if (u+r == 9)
                regGrQuaterter(r_width*2.5, r_width);
            else if (u)
            {
                if(l)
                    regGrQuaterter(r_width*0.5, r_width);
                else
                    regGrQuaterter(r_width*1.5, r_width);
            }
            else if (r)
            {
                if (d)
                    regGrQuaterter(r_width*2.5, r_width*3);
                else
                    regGrQuaterter(r_width*2.5, r_width*2);
            }
            else if (ur)
                regGrQuaterter(r_width*2.5, 0);
            else //0
            {
                if (d+l == 6)
                    regGrQuaterter(r_width*0.5, r_width*3);
                else if (l)
                    regGrQuaterter(r_width*0.5, r_width*2);
                else if (d)
                    regGrQuaterter(r_width*1.5, r_width*3);
                else
                    regGrQuaterter(r_width*1.5, r_width*2);
            }
            /*
             * Draw down_left corner
             */
            if (d+l == 6)
                regGrQuaterter(0, r_width*3.5);
            else if (d)
            {
                if (r)
                    regGrQuaterter(r_width*2, r_width*3.5);
                else
                    regGrQuaterter(r_width, r_width*3.5);
            }
            else if (l)
            {
                if (u)
                    regGrQuaterter(0, r_width*1.5);
                else
                    regGrQuaterter(0, r_width*2.5);
            }
            else if (dl)
                regGrQuaterter(r_width*2, r_halfWidth);
            else
            {
                if (u+r == 9)
                    regGrQuaterter(r_width*2, r_width*1.5);
                else if (r)
                    regGrQuaterter(r_width*2, r_width*2.5);
                else if (u)
                    regGrQuaterter(r_width, r_width*1.5);
                else
                    regGrQuaterter(r_width, r_width*2.5);
            }
            /*
             * Draw down_right corner
             */
            if (d+r == 10)
                regGrQuaterter(r_width*2.5, r_width*3.5);
            else if (d)
            {
                if (l)
                    regGrQuaterter(r_width*0.5, r_width*3.5);
                else
                    regGrQuaterter(r_width*1.5, r_width*3.5);
            }
            else if (r)
            {
                if (u)
                    regGrQuaterter(r_width*2.5, r_width*1.5);
                else
                    regGrQuaterter(r_width*2.5, r_width*2.5);
            }
            else if (dr)
                regGrQuaterter(r_width*2.5, r_halfWidth);
            else
            {
                if (u+l == 5)
                    regGrQuaterter(r_halfWidth,r_width*1.5);
                else if (l)
                    regGrQuaterter(r_halfWidth,r_width*2.5);
                else if (u)
                    regGrQuaterter(r_width*1.5,r_width*1.5);
                else
                    regGrQuaterter(r_width*1.5,r_width*2.5);
            }

            m_uvCache[id] = uv;
        }

        terrain_id++;
        block_col++;

        // Go to the next block
        if (block_col > 1) {
                block_col = 0;
                block_row++;
        }

        // Go to the next column
        if (block_row > 3) {
                block_row = 0;
                tileset_col++;
        }
    }

#undef regGrQuaterter
#undef regQuarter

    /* Register E and F blocks */

    int tile_row = 0;

    while (tileset_col < 5)
    {
        for (float col = 0; col < 6; col+= 1)
        {
            uv.clear();
            float orig_x = tileset_col*6*r_width+col*r_width;
            float orig_y = tile_row*r_width;
            regTile(orig_x, orig_y);
            m_uvCache[translate(terrain_id)] = uv;
            terrain_id++;
        }

        tile_row++;
        if (tile_row == 16)
        {
            tile_row = 0;
            tileset_col++;
        }
    }
#undef regTile
}

