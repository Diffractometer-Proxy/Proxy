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
********************************************************************************/

/********************************************************************************
    Copyright (C) 2016  Christian Randau

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
// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#ifndef CORE_DEBUG_STDAFX_H
#define CORE_DEBUG_STDAFX_H


#if defined(_MSC_VER)
	#pragma warning( disable: 4482 )
	#pragma warning( disable: 4996 )
	#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
#endif

// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.

//Inlcude C Header
#include <time.h>

//Include C++ Header
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//Include QT Header
#include <QtCore>

#endif //CORE_DEBUG_STDAFX_H