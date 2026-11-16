// Track_Reader.h
// Reads the Spotify tracks CSV file one row at a time.
//
// Track_Reader handles the hard parts of the file for you:
//   * opening the file and skipping the header row
//   * commas inside quoted fields, e.g.  "Outta Me, Onto You"
//   * doubled quotes inside quoted fields, e.g.  ""Funeral March""
//   * converting text to int / double / bool
//
// Typical use:
//
//     Track_Reader reader("tracks_features.csv");
//     while (reader.nextTrack())
//     {
//         string name = reader.getString("name");
//         int    year = reader.getInt("year");
//         ...
//     }

#ifndef TRACK_READER_H
#define TRACK_READER_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Track_Reader
{
public:
    Track_Reader();
    Track_Reader(const string& fileName);
    ~Track_Reader();

    // Opens the file and reads its header row. Returns false on failure.
    bool open(const string& fileName);
    bool isOpen() const;
    void close();

    // Moves to the next track in the file. Returns false when there are no
    // more tracks. Lines with the wrong number of columns are skipped.
    bool nextTrack();

    // Get a value from the current track by its column name (from the
    // header row), e.g. getString("album") or getDouble("tempo").
    // An unknown column name throws an invalid_argument exception.
    string getString(const string& column) const;
    int    getInt(const string& column) const;
    double getDouble(const string& column) const;
    bool   getBool(const string& column) const;   // "True" -> true

    // For the artists / artist_ids columns:
    // ['Artist A', 'Artist B']  ->  {"Artist A", "Artist B"}
    vector<string> getList(const string& column) const;

    int getLineNumber() const;     // line number of the current track
    int getSkippedCount() const;   // how many bad lines were skipped

private:
    ifstream inFile;
    vector<string> columnNames;    // from the header row
    vector<string> fields;         // values of the current track
    int lineNumber;
    int skippedCount;

    int columnIndex(const string& column) const;
    static vector<string> splitCSVLine(const string& line);
    static vector<string> splitList(const string& listText);
};

#endif
