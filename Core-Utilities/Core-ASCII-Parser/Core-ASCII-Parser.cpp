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
#include "stdafx.h"
#include "Core-ASCII-Parser.h"

using namespace std;
using namespace Core;

#include "VERSION.h"

#if defined(_MSC_VER)
#	pragma message("compile \"Core-ASCII-Parser\" version: "VERSION)
#endif

std::string Core::getASCIIParserVersion()
{
	std::string version = VERSION;
	return version;
}
int Core::getASCIIParserVersionMajor()
{
	std::string version = VERSION;
	return atoi(version.substr(0, version.find_first_of('.')).c_str());
}
int Core::getASCIIParserVersionMinor()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_first_of('.')+1, version.find_last_of('.')).c_str());
}
int Core::getASCIIParserVersionPatch()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_last_of('.')+1).c_str());
}
std::string Core::str_lower(std::string str)
{
	std::string nstr;
	unsigned int laenge = (unsigned int)str.size();
	for(unsigned int i = 0; i < laenge; ++i)
	{
		nstr += tolower(str[i]);
	}
	return nstr;
}
std::string Core::str_upper(std::string str)
{
	std::string nstr;
	unsigned int laenge = (unsigned int)str.size();
	for(unsigned int i = 0; i < laenge; ++i)
	{
		nstr += toupper(str[i]);
	}
	return nstr;
} 

bool ParserASCIIParameterFiles::loadParameterFile(string path, vector<ASCIIParameterTuple> &asciiTupleVec)
{
    ifstream in_File(path.c_str(), ios_base::in);

    //Check the File
    if (!in_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_loadParameter");
        return false;
    }

    //Parse the Para File
    asciiTupleVec = this->_parseParameterFileTuple(in_File);

    in_File.close();

    return true;
}
bool ParserASCIIParameterFiles::saveParameterFile(string path, vector<ASCIIParameterTuple> asciiTupleVec)
{
    ofstream out_File(path.c_str(), ios_base::out);

    //Check the File
    if (!out_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_saveParameter");
        return false;
    }

    //Parse the Para File
    for (unsigned int i=0; i<asciiTupleVec.size(); i++)
    {
        out_File << asciiTupleVec[i].parameterName << " = " << asciiTupleVec[i].parameterValue << endl;
    }

    out_File.close();

    return true;
}
bool ParserASCIIParameterFiles::loadParameterFile(std::string path, std::vector<ASCIIParameterTupleWithOption> &asciiTupleVec)
{
    QFile in_File(QString(path.c_str()));

    //Check the File
	if (!in_File.open(QIODevice::ReadOnly))
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_loadParameter");
        return false;
    }

    //Parse the Para File
    asciiTupleVec = this->_parseParameterFileTupleWithOption(in_File);

    in_File.close();

    return true;
}
bool ParserASCIIParameterFiles::saveParameterFile(std::string path, std::vector<ASCIIParameterTupleWithOption> asciiTupleVec)
{
    ofstream out_File(path.c_str(), ios_base::out);

    //Check the File
    if (!out_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_saveParameter");
        return false;
    }

    //Parse the Para File
    for (unsigned int i=0; i<asciiTupleVec.size(); i++)
    {
        out_File << asciiTupleVec[i].parameterName << " = " << asciiTupleVec[i].parameterValue;
		if (asciiTupleVec[i].options.size() > 0)
			out_File << " | " << asciiTupleVec[i].options[0];
		for (unsigned int j=1; j<asciiTupleVec[i].options.size(); j++)
		{
			out_File << ", " << asciiTupleVec[i].options[j];
		}
		out_File << endl;
    }

    out_File.close();

    return true;
}
bool ParserASCIIParameterFiles::loadParameterFile(std::string path, std::vector<ASCIIMatrixLine> &asciiMatrix)
{
    ifstream in_File(path.c_str(), ios_base::in);

    //Check the File
    if (!in_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_loadParameter");
        return false;
    }

    //Parse the Para File
    asciiMatrix = this->_parseParameterFileMatrix(in_File);

    in_File.close();

    return true;
}
bool ParserASCIIParameterFiles::saveParameterFile(std::string path, std::vector<ASCIIMatrixLine> asciiMatrix)
{
    ofstream out_File(path.c_str(), ios_base::out);

    //Check the File
    if (!out_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_saveParameter");
        return false;
    }

    //Parse the Para File
    for (unsigned int i=0; i<asciiMatrix.size(); i++)
    {
        for (int j=0; j<asciiMatrix[i].size(); j++)
            out_File << (asciiMatrix[i])[j] << " ";
        out_File << endl;
    }

    out_File.close();

    return true;
}
bool ParserASCIIParameterFiles::loadParameterFile(std::string path, std::vector<ASCIITuple> &asciiTuple)
{
    ifstream in_File(path.c_str(), ios_base::in);

    //Check the File
    if (!in_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_loadParameter");
        return false;
    }

    //Parse the Para File
	asciiTuple = this->_parseParameterFileTupleList(in_File);

    in_File.close();

    return true;
}
bool ParserASCIIParameterFiles::saveParameterFile(std::string path, std::vector<ASCIITuple> asciiTuple)
{
    ofstream out_File(path.c_str(), ios_base::out);

    //Check the File
    if (!out_File.is_open())
    {
        this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_saveParameter");
        return false;
    }

    //Parse the Para File
    for (unsigned int i=0; i<asciiTuple.size(); i++)
    {
		out_File << asciiTuple[i].valueX << " " << asciiTuple[i].valueY;
        out_File << endl;
    }

    out_File.close();

    return true;
}

vector<ParserASCIIParameterFiles::ASCIIParameterTuple> ParserASCIIParameterFiles::_parseParameterFileTuple(ifstream &in)
{
    vector<ASCIIParameterTuple> parameterTupleVec;

    char tempChar[300];
    while(!in.eof())
    {
        in.getline(tempChar, 300);
        string line(tempChar);

        //Erase Space at the beginning
        size_t pos = line.find_first_not_of(" \t");
        if(pos != string::npos)
            line.erase(0, pos);

        //Check for uncommented lines
        pos = line.find_first_of(";");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }
        pos = line.find("//");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }

        if(pos != string::npos)
            line.erase(pos, line.size());

        //If Line not Empty put the line to the Parameter Vector
        if (line.size() > 1)
        {
            //Split parameter name and value
            string paraName = line;
            string value = line;
            size_t pos = paraName.find_first_of(" =\t");
            if (pos != string::npos)
            {
                paraName.erase(pos, paraName.size());
                value.erase(0, pos);
            }
            //Erase sign before value
            pos = value.find_first_not_of(" =\t");
            if (pos != string::npos)
                value.erase(0, pos);
            //Erase sign after value
            pos = value.find_first_of(" =\t\r\n");
            if (pos != string::npos)
                value.erase(pos, value.size());

            paraName = str_upper(paraName);
            ASCIIParameterTuple newTuple;
            newTuple.parameterName = paraName;
            newTuple.parameterValue = value;
            parameterTupleVec.push_back(newTuple);
        }
    }

    return parameterTupleVec;
}
std::vector<ParserASCIIParameterFiles::ASCIIParameterTupleWithOption> ParserASCIIParameterFiles::_parseParameterFileTupleWithOption(QFile &in)
{
    vector<ASCIIParameterTupleWithOption> parameterTupleVec;

	while(!in.atEnd())
    {
		QByteArray line_ByteArray = in.readLine();
		QString line(line_ByteArray);
		
        //Erase Space at the beginning and End
		line = line.trimmed();
		
        //Check for commented lines and erase comment part
		//**************************************************
		int pos = line.indexOf(";");
        if (pos == 0)
        {
            //Commented Line
            continue;
        }
		pos = line.indexOf("//");
        if (pos == 0)
        {
            //Commented Line
            continue;
        }
        if(pos != -1)
		{
			//a part of the line is a comment, erase the comment part.
            line = line.left(pos-1);		
			line = line.trimmed();
		}
		//**************************************************
		//Check for commented lines and erase comment part

		//Check if the line is empty
		if (line.isEmpty())
			continue;

		std::string paraName;
		std::string value;
		std::vector<std::string> options;

		//Split parameter name
		pos = line.indexOf("=");	//Left is name and right is value
		paraName = line.left(pos-1).trimmed().toStdString();
		line = line.remove(0, pos+1).trimmed();

		//Split value
		pos = line.indexOf("|");	//Left is value and right are the options
		value = line.left(pos-1).trimmed().toStdString();
		if (pos == -1)
			line.clear();
		else
			line = line.remove(0, pos+1).trimmed();

		//Split options
		while (!line.isEmpty())
		{
			pos = line.indexOf(",");
			if (pos != -1)
			{
				//There are more than one option left
				std::string option = line.left(pos-1).trimmed().toStdString();
				options.push_back(option);
				if (pos == -1)
					line.clear();
				else
					line.remove(0, pos+1).trimmed();
				continue;
			}
			std::string option = line.trimmed().toStdString();
			line.clear();
			options.push_back(option);
		}

        paraName = str_upper(paraName);
		ASCIIParameterTupleWithOption newTuple;
        newTuple.parameterName = paraName;
        newTuple.parameterValue = value;
		newTuple.options = options;
        parameterTupleVec.push_back(newTuple);
    }

	return parameterTupleVec;
}
std::vector<ParserASCIIParameterFiles::ASCIIMatrixLine> ParserASCIIParameterFiles::_parseParameterFileMatrix(std::ifstream &in)
{
    std::vector<ParserASCIIParameterFiles::ASCIIMatrixLine> matrixLineVec;

    char tempChar[1000];
    while(!in.eof())
    {
        in.getline(tempChar, 1000);
        string line(tempChar);

        //Erase Space at the beginning
        size_t pos = line.find_first_not_of(" \t");
        if(pos != string::npos)
            line.erase(0, pos);

        //Check for uncommented lines
        pos = line.find_first_of(";");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }
        pos = line.find("//");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }

        if(pos != string::npos)
            line.erase(pos, line.size());

        //If Line not Empty put the line to the Matrix
        if (line.size() <= 1)
        {
            //Line is Empty
            continue;
        }

        //Put the line to the Matrix
        ASCIIMatrixLine matrixLine;
        while (line.size() > 0)
        {
            //Split Matrix Elements
            string matrixElement = line;
            size_t pos = matrixElement.find_first_of(" ;\t\r\n");
			if (pos == string::npos)
				line.clear();
			else
				line.erase(0, pos+1);
            if (pos != string::npos)
            {
                matrixElement.erase(pos, matrixElement.size());
            }
            //Erase sign before value
            pos = line.find_first_not_of(" ;\t\r\n");
            if (pos != string::npos)
                line.erase(0, pos);
			else
				line.erase(0);

            matrixElement = str_upper(matrixElement);
            matrixLine.push_back(matrixElement);
        }
        matrixLineVec.push_back(matrixLine);
    }

    return matrixLineVec;
}
std::vector<ParserASCIIParameterFiles::ASCIITuple> ParserASCIIParameterFiles::_parseParameterFileTupleList(std::ifstream &in)
{
	std::vector<ParserASCIIParameterFiles::ASCIITuple> tupleList;

    char tempChar[1000];
    while(!in.eof())
    {
        in.getline(tempChar, 1000);
        string line(tempChar);

        //Erase Space at the beginning
        size_t pos = line.find_first_not_of(" \t");
        if(pos != string::npos)
            line.erase(0, pos);

        //Check for uncommented lines
        pos = line.find_first_of(";");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }
        pos = line.find("//");
        if (pos == 0)
        {
            //Uncommented Line
            continue;
        }

        if(pos != string::npos)
            line.erase(pos, line.size());

        //If Line not Empty put the line to the tupleList
        if (line.size() <= 1)
        {
            //Line is Empty
            continue;
        }

        //Put the line to the tupleList
		ASCIITuple tuple;
		unsigned int counter=0;
        while (line.size() > 0)
        {
            //Split Tuple Elements
            string tupleElement = line;
            size_t pos = tupleElement.find_first_of(" ;\t\r\n");
			if (pos == string::npos)
				line.clear();
			else
				line.erase(0, pos+1);
            if (pos != string::npos)
            {
                tupleElement.erase(pos, tupleElement.size());
            }
            //Erase sign before value
            pos = line.find_first_not_of(" ;\t\r\n");
            if (pos != string::npos)
                line.erase(0, pos);
			else
				line.erase(0);

			if (counter == 0)
			{
				tuple.valueX = tupleElement;
				counter++;
				continue;
			}
			if (counter == 1)
			{
				tuple.valueY = tupleElement;
				counter++;
				break;
			}
        }
		if (counter == 2) //exact two values was read.
			tupleList.push_back(tuple);
    }

    return tupleList;
}




bool ParserASCIIParameterTupleFiles::loadParameterFile(std::string path)
{
	ifstream in_File = ifstream(path.c_str(), ios_base::in);

	//Check the File
	if (!in_File.is_open())
	{
		this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_loadParameter");
		return false;
	}

	//Parse the Para File
	this->_asciiTupleVec = this->_parseParameterFile(in_File);

	in_File.close();

	return true;
}

bool ParserASCIIParameterTupleFiles::loadParameterFile(string path, vector<ASCIIParameterTuple> &asciiTupleVec)
{
	if (this->loadParameterFile(path))
	{
		asciiTupleVec = this->_asciiTupleVec;
		return true;
	}
	return false;
}
bool ParserASCIIParameterTupleFiles::saveParameterFile(string path, vector<ASCIIParameterTuple> asciiTupleVec)
{
	ofstream out_File = ofstream(path.c_str(), ios_base::out);

	//Check the File
	if (!out_File.is_open())
	{
		this->writeDebugMessage("Can't open the parameter file: \"" + path + "\"", 1, "ParserASCIIParameterFiles", "_saveParameter");
		return false;
	}

	//Parse the Para File
	for (unsigned int i=0; i<asciiTupleVec.size(); i++)
	{
		out_File << asciiTupleVec[i].parameterName << " = " << asciiTupleVec[i].parameterValue << endl;
	}

	out_File.close();

	return true;
}
bool ParserASCIIParameterTupleFiles::getParameter(std::string parameterName, std::string &parameterValue)
{
	parameterName = str_upper(parameterName);
	//Search parameter
	for(unsigned int i=0; i<this->_asciiTupleVec.size(); i++)
	{
		if (this->_asciiTupleVec[i].parameterName.compare(parameterName) == 0)
		{
			parameterValue = this->_asciiTupleVec[i].parameterValue;
			return true;
		}
	}

	return false;
}
vector<ParserASCIIParameterTupleFiles::ASCIIParameterTuple> ParserASCIIParameterTupleFiles::_parseParameterFile(ifstream &in)
{
	vector<ASCIIParameterTuple> parameterTupleVec;

	char tempChar[300];
	while(!in.eof())
	{
		in.getline(tempChar, 300);
		string line(tempChar);

		//Erase Space at the beginning
		size_t pos = line.find_first_not_of(" \t");
		if(pos != string::npos)
			line.erase(0, pos);

		//Check for uncommented lines
		pos = line.find_first_of(";");
		if (pos == 0)
		{
			//Uncommented Line
			continue;
		}
		pos  = line.find("//");
		if (pos == 0)
		{
			//Uncommented Line
			continue;
		}

		if(pos != string::npos)
			line.erase(pos, line.size());

		//If Line not Empty put the line to the Parameter Vector
		if (line.size() > 1)
		{
			//Split parameter name and value
			string paraName = line;
			string value = line;
			size_t pos = paraName.find_first_of(" =\t");
			if (pos != string::npos)
			{
				paraName.erase(pos, paraName.size());
				value.erase(0, pos);
			}
			//Erase sign before value
			pos = value.find_first_not_of(" =\t");
			if (pos != string::npos)
				value.erase(0, pos);
			//Erase sign after value
			pos = value.find_first_of(" =\t");
			if (pos != string::npos)
				value.erase(pos, value.size());

			paraName = str_upper(paraName);
			ASCIIParameterTuple newTuple;
			newTuple.parameterName = paraName;
			newTuple.parameterValue = value;
			parameterTupleVec.push_back(newTuple);
		}
	}

	return parameterTupleVec;
}