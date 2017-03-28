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
#ifndef CORE_ASCII_PARSER_H
#define CORE_ASCII_PARSER_H

#include <Core-Debug.h>

#include "Core-ASCII-Parser_Global.h"

namespace Core
{

COREASCIIPARSER_EXPORT std::string getASCIIParserVersion();
COREASCIIPARSER_EXPORT int getASCIIParserVersionMajor();
COREASCIIPARSER_EXPORT int getASCIIParserVersionMinor();
COREASCIIPARSER_EXPORT int getASCIIParserVersionPatch();

COREASCIIPARSER_EXPORT std::string str_lower(std::string str);
COREASCIIPARSER_EXPORT std::string str_upper(std::string str);

class COREASCIIPARSER_EXPORT ParserASCIIParameterFiles : public virtual Core::Debug
{
	public:
	typedef struct
	{
		std::string parameterName;
		std::string parameterValue;
	}ASCIIParameterTuple;
	typedef struct
	{
		std::string parameterName;
		std::string parameterValue;
		std::vector<std::string> options;
	}ASCIIParameterTupleWithOption;
	typedef QVector<std::string> ASCIIMatrixLine;
	typedef struct
	{
		std::string valueX;
		std::string valueY;
	}ASCIITuple;

	public:
		ParserASCIIParameterFiles() {}
		~ParserASCIIParameterFiles() {}

	public:
		//Parameters
        bool loadParameterFile(std::string path, std::vector<ASCIIParameterTuple> &asciiTupleVec);
        bool saveParameterFile(std::string path, std::vector<ASCIIParameterTuple> asciiTupleVec);
        bool loadParameterFile(std::string path, std::vector<ASCIIParameterTupleWithOption> &asciiTupleVec);
        bool saveParameterFile(std::string path, std::vector<ASCIIParameterTupleWithOption> asciiTupleVec);
        bool loadParameterFile(std::string path, std::vector<ASCIIMatrixLine> &asciiMatrix);
        bool saveParameterFile(std::string path, std::vector<ASCIIMatrixLine> asciiMatrix);
        bool loadParameterFile(std::string path, std::vector<ASCIITuple> &asciiTuple);
        bool saveParameterFile(std::string path, std::vector<ASCIITuple> asciiTuple);
	private:
        std::vector<ParserASCIIParameterFiles::ASCIIParameterTuple> _parseParameterFileTuple(std::ifstream &in);
		std::vector<ParserASCIIParameterFiles::ASCIIParameterTupleWithOption> _parseParameterFileTupleWithOption(QFile &in);
        std::vector<ParserASCIIParameterFiles::ASCIIMatrixLine> _parseParameterFileMatrix(std::ifstream &in);
		std::vector<ParserASCIIParameterFiles::ASCIITuple> _parseParameterFileTupleList(std::ifstream &in);
};

class COREASCIIPARSER_EXPORT ParserASCIIParameterTupleFiles : public virtual Core::Debug
{
	public:
	typedef struct
	{
		std::string parameterName;
		std::string parameterValue;
	}ASCIIParameterTuple;

	public:
		ParserASCIIParameterTupleFiles() {}
		~ParserASCIIParameterTupleFiles() {}

	public:
		//Parameters
		bool loadParameterFile(std::string path);
		bool loadParameterFile(std::string path, std::vector<ASCIIParameterTuple> &asciiTupleVec);
		bool saveParameterFile(std::string path, std::vector<ASCIIParameterTuple> asciiTupleVec);
		bool getParameter(std::string parameterName, std::string &parameterValue);
		std::vector<ParserASCIIParameterTupleFiles::ASCIIParameterTuple> _parseParameterFile(std::ifstream &in);

private:
	 std::vector<ASCIIParameterTuple> _asciiTupleVec;
};

} // End of NameSpace Core

#endif //CORE_ASCII_PARSER_H
