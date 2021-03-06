﻿/******************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2013 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef __GLTEXTCACHE_H__
#define __GLTEXTCACHE_H__

#include <stdint.h>
//#include "ocpn_types.h"
#include "_def.h"
#include "bbox.h"
#include <QFile>

class glTextureDescriptor;

#define COMPRESSED_CACHE_MAGIC 0xf013  // change this when the format changes

#define FACTORY_TIMER                   10000

void HalfScaleChartBits( int width, int height, unsigned char *source, unsigned char *target );

class ChartBaseBSB;
class ChartPlugInWrapper;

struct CompressedCacheHeader
{
    uint32_t magic;
    uint32_t format;
    uint32_t chartdate;
    uint32_t m_nentries;
    uint32_t catalog_offset;
    uint32_t chartfile_date;
    uint32_t chartfile_size;
};

struct CatalogEntryKey
{
    int         mip_level;
    ZCHX::ZCHX_COLOR_SCHEME tcolorscheme;
    int         x;
    int         y;
};

struct CatalogEntryValue
{
    int         texture_offset;
    uint32_t    compressed_size;
}; 

#define CATALOG_ENTRY_SERIAL_SIZE 6 * sizeof(uint32_t)

class CatalogEntry
{
public:
    CatalogEntry();
    ~CatalogEntry();
    CatalogEntry(int level, int x0, int y0, ZCHX::ZCHX_COLOR_SCHEME colorscheme);
    int GetSerialSize();
    void Serialize(unsigned char *);
    void DeSerialize(unsigned char *);
    CatalogEntryKey k;
    CatalogEntryValue v;
    
};

typedef QList<CatalogEntry> ArrayOfCatalogEntries;

class glTexTile
{
public:
    glTexTile() { m_coords = m_texcoords = NULL;  m_ncoords = 0;}
    virtual ~glTexTile() { delete [] m_coords; delete [] m_texcoords; }

    QRect rect;
    LLBBox box;
//    LLRegion region;

    int m_ncoords;
    float *m_coords, *m_texcoords;
};

#define MAX_TEX_LEVEL 10

class ChartBase;
class glTexFactory
{
public:
    glTexFactory(ChartBase *chart, int raster_format);
    ~glTexFactory();

    glTextureDescriptor *GetOrCreateTD(const QRect &rect);
    bool BuildTexture(glTextureDescriptor *ptd, int base_level, const QRect &rect);
    bool PrepareTexture( int base_level, const QRect &rect, ZCHX::ZCHX_COLOR_SCHEME color_scheme, int mem_used );
    int GetTextureLevel( glTextureDescriptor *ptd, const QRect &rect, int level,  ZCHX::ZCHX_COLOR_SCHEME color_scheme );
    bool UpdateCacheAllLevels( const QRect &rect, ZCHX::ZCHX_COLOR_SCHEME color_scheme, unsigned char **compcomp_array, int *compcomp_size);
    bool IsLevelInCache( int level, const QRect &rect, ZCHX::ZCHX_COLOR_SCHEME color_scheme );
    QString GetChartPath(){ return m_ChartPath; }
    QString GetHashKey(){ return m_HashKey; }
    void SetHashKey( QString key ){ m_HashKey = key; }
    bool OnTimer();
    void AccumulateMemStatistics(int &map_size, int &comp_size, int &compcomp_size);
    void DeleteTexture(const QRect &rect);
    void DeleteAllTextures( void );
    void DeleteSomeTextures( long target );
    void DeleteAllDescriptors( void );
    bool BackgroundCompressionAsJob() const;
    void PurgeBackgroundCompressionPool();
    void SetLRUTime(int lru) { m_LRUtime = lru; }
    int	 GetLRUTime() { return m_LRUtime; }
    void FreeSome( long target );
    void FreeIfCached();

    glTextureDescriptor *GetpTD( QRect & rect );

    void PrepareTiles(const ViewPort &vp, bool use_norm_vp, ChartBase *pChart);
    glTexTile** GetTiles(int &num) { num = m_ntex; return m_tiles; }
    void GetCenter(double &lat, double &lon) { lat = m_clat, lon = m_clon; }

private:
    bool LoadCatalog(void);
    bool LoadHeader(void);
    bool WriteCatalogAndHeader();

    bool UpdateCachePrecomp(unsigned char *data, int data_size, const QRect &rect, int level,
                                          ZCHX::ZCHX_COLOR_SCHEME color_scheme, bool write_catalog = true);
    bool UpdateCacheLevel( const QRect &rect, int level, ZCHX::ZCHX_COLOR_SCHEME color_scheme, unsigned char *data, int size);
    
    void DeleteSingleTexture( glTextureDescriptor *ptd );

    CatalogEntryValue *GetCacheEntryValue(int level, int x, int y, ZCHX::ZCHX_COLOR_SCHEME color_scheme);
    bool AddCacheEntryValue(const CatalogEntry &p);
    int  ArrayIndex(int x, int y) const { return ((y / m_tex_dim) * m_stride) + (x / m_tex_dim); } 
    void  ArrayXY(QRect *r, int index) const;

    int         n_catalog_entries;

    CatalogEntryValue *m_cache[ZCHX::ZCHX_COLOR_SCHEME_NCOLOR][MAX_TEX_LEVEL];



    QString    m_ChartPath;
    QString    m_HashKey;
    QString    m_CompressedCacheFilePath;
    
    int         m_catalog_offset;
    bool        m_hdrOK;
    bool        m_catalogOK;
    bool        m_newCatalog;

    bool	m_catalogCorrupted;
    
//    FILE     *m_fs;
    QFile*       m_fs;
    uint32_t    m_chart_date_binary;
    uint32_t    m_chartfile_date_binary;
    uint32_t    m_chartfile_size;
    
    int         m_stride;
    int         m_ntex;
    int         m_tex_dim;
    int         m_size_X;
    int         m_size_Y;
    int         m_nx_tex;
    int         m_ny_tex;
    
    int		m_LRUtime;
    
    glTextureDescriptor  **m_td_array;

    double m_clat, m_clon;
    glTexTile **m_tiles;
    int m_prepared_projection_type;
    bool m_north; // used for polar projection
    
};


#endif
