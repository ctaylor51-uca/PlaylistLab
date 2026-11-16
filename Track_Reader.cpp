// Track_Reader.cpp
// See Track_Reader.h for how to use this class.

#include "Track_Reader.h"

#include <iostream>
#include <stdexcept>

Track_Reader::Track_Reader()
{
    lineNumber = 0;
    skippedCount = 0;
}

Track_Reader::Track_Reader(const string& fileName)
{
    lineNumber = 0;
    skippedCount = 0;
    open(fileName);
}

Track_Reader::~Track_Reader()
{
    close();
}

bool Track_Reader::open(const string& fileName)
{
    close();
    lineNumber = 0;
    skippedCount = 0;
    columnNames.clear();
    fields.clear();

    inFile.open(fileName);
    if (!inFile)
    {
        cout << "Could not open " << fileName << endl;
        return false;
    }

    string header;
    if (!getline(inFile, header))
    {
        cout << fileName << " is empty" << endl;
        inFile.close();
        return false;
    }
    lineNumber = 1;
    columnNames = splitCSVLine(header);
    return true;
}

bool Track_Reader::isOpen() const
{
    return inFile.is_open();
}

void Track_Reader::close()
{
    if (inFile.is_open())
    {
        inFile.close();
    }
}

bool Track_Reader::nextTrack()
{
    if (!inFile.is_open())
    {
        return false;
    }

    string line;
    while (getline(inFile, line))
    {
        lineNumber++;
        if (line.empty() || line == "\r")
        {
            continue;
        }

        fields = splitCSVLine(line);
        if (fields.size() == columnNames.size())
        {
            return true;
        }

        cout << "Skipping bad line " << lineNumber << endl;
        skippedCount++;
    }

    fields.clear();
    return false;
}

string Track_Reader::getString(const string& column) const
{
    return fields[columnIndex(column)];
}

int Track_Reader::getInt(const string& column) const
{
    // stod first so values written like "4.0" still work
    return static_cast<int>(stod(getString(column)));
}

double Track_Reader::getDouble(const string& column) const
{
    return stod(getString(column));
}

bool Track_Reader::getBool(const string& column) const
{
    string value = getString(column);
    return value == "True" || value == "true" || value == "1";
}

vector<string> Track_Reader::getList(const string& column) const
{
    return splitList(getString(column));
}

int Track_Reader::getLineNumber() const
{
    return lineNumber;
}

int Track_Reader::getSkippedCount() const
{
    return skippedCount;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

int Track_Reader::columnIndex(const string& column) const
{
    if (fields.empty())
    {
        throw invalid_argument("No current track: call nextTrack() first");
    }
    for (size_t i = 0; i < columnNames.size(); i++)
    {
        if (columnNames[i] == column)
        {
            return static_cast<int>(i);
        }
    }
    throw invalid_argument("Unknown column: " + column);
}

// Splits one line of CSV text into its fields. A comma only ends a field
// when we are NOT inside double quotes, and "" inside quotes is one quote.
vector<string> Track_Reader::splitCSVLine(const string& line)
{
    vector<string> result;
    string current = "";
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); i++)
    {
        char c = line[i];

        if (inQuotes)
        {
            if (c == '"')
            {
                if (i + 1 < line.length() && line[i + 1] == '"')
                {
                    current += '"';      // "" means one literal quote
                    i++;
                }
                else
                {
                    inQuotes = false;    // closing quote
                }
            }
            else
            {
                current += c;            // commas inside quotes are kept
            }
        }
        else
        {
            if (c == '"')
            {
                inQuotes = true;         // opening quote
            }
            else if (c == ',')
            {
                result.push_back(current);
                current = "";
            }
            else if (c != '\r')          // ignore Windows line endings
            {
                current += c;
            }
        }
    }

    result.push_back(current);           // last field has no comma after it
    return result;
}

// Turns  ['Frédéric Chopin', "Paquito D'Rivera"]  into separate names.
vector<string> Track_Reader::splitList(const string& listText)
{
    vector<string> items;
    string current = "";
    char quoteChar = '\0';               // '\0' means not inside an item

    for (size_t i = 0; i < listText.length(); i++)
    {
        char c = listText[i];

        if (quoteChar == '\0')
        {
            if (c == '\'' || c == '"')
            {
                quoteChar = c;           // start of an item
                current = "";
            }
        }
        else
        {
            if (c == '\\' && i + 1 < listText.length())
            {
                current += listText[i + 1];   // escaped char, e.g. \'
                i++;
            }
            else if (c == quoteChar)
            {
                items.push_back(current);     // end of an item
                quoteChar = '\0';
            }
            else
            {
                current += c;
            }
        }
    }
    return items;
}
