/*  This file is part of the KDE project
 *  Copyright (C) 2008 Daniel Nicoletti <mirttex85-pk@yahoo.com.br>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 * 
 */
#ifndef KPACKAGEKIT_H
#define KPACKAGEKIT_H

#include <KCModule>
#include <QGridLayout>
#include "PkUpdate.h"

class KPackageKit : public KCModule
{
Q_OBJECT
public:
    KPackageKit(QWidget *&parent, const QVariantList &args);

private:
    PkUpdate *view;
    QGridLayout *m_grid;
};

#endif
