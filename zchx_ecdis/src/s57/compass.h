﻿/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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
 **************************************************************************/

#include <QWidget>
#include "bitmap.h"
#include "_def.h"

class ocpnDC;
class ChartCanvas;

class ocpnCompass
{
public:
      ocpnCompass(ChartCanvas *parent, bool bShowGPS = true);
      ~ocpnCompass();

      bool IsShown() const { return m_shown; }
      void Show(bool show) { m_shown = show; }
      void Paint( ocpnDC& dc );

      void UpdateStatus( bool newColorScheme = false );

      bool MouseEvent( QMouseEvent& event );
      void SetColorScheme( ZCHX::ZCHX_COLOR_SCHEME cs );
      int GetXOffset(void) const { return m_xoffset; }
      int GetYOffset(void) const { return m_yoffset; }
      float GetScaleFactor(){ return m_scale; }
      void SetScaleFactor( float factor);
      
      void Move(const zchxPoint &pt) { m_rect.setTopLeft(pt.toPoint()); }
      QRect GetRect(void) const { return m_rect; }
private:
      void CreateBmp( bool bnew = false );

      ChartCanvas *m_parent;
      wxBitmap m_StatBmp;
      wxBitmap m_MaskBmp;

      QString m_lastgpsIconName;
      double m_rose_angle;

      wxBitmap _img_compass;
      wxBitmap _img_gpsRed;
      int m_xoffset;
      int m_yoffset;
      float m_scale;

      QRect m_rect;
      bool m_shown;
      bool m_bshowGPS;
      ZCHX::ZCHX_COLOR_SCHEME m_cs;
      
#ifdef ocpnUSE_GL
      unsigned int texobj;
#endif
      
};
