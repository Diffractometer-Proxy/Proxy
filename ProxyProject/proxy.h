/********************************************************************************
* written by 
*
* Christian Randau
* Georg-August-Universitaet Goettingen
* Fakultaet fuer Geowissenschaften und Geologie
* Abteilung Isotopengeologie Aussenstelle MLZ (FRM II)
* Lichtenbergstr. 1
* 85747 Garching
* 
* christian.randau@frm2.tum.de
*
*
*Sina Mayr
*Technische Universitaet Muenchen
*Physik-Department E21
*James-Franck-Str. 1
*85748 Garching
*Germany
*
*sina.mayr@frm2.tum.de
*
*
*Wolfgang Kreuzpaintner
*Technische Universitaet Muenchen
*Physik-Department E21
*James-Franck-Str. 1
*85748 Garching
*Germany
*
*wolfgang.kreuzpaintner@frm2.tum.de
********************************************************************************/

/********************************************************************************
    Copyright (C) 2016  Christian Randau, Sina Mayr, Wolfgang Kreuzpaintner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************************/

#ifndef PROXY_H
#define PROXY_H

#include <ProxyCommunication.h>

class Proxy : public QObject
{
	Q_OBJECT

public:
	Proxy(QObject *parent);
	~Proxy();

public slots:
	void run();

signals:
	void finished();
};

#endif //PROXY_H
