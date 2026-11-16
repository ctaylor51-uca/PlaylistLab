// main.cpp
// Loads Spotify tracks from a CSV file into a vector of Track structs.
//
// Build:  g++ -std=c++17 main.cpp Track_Reader.cpp -o tracks

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

#include "Track_Reader.h"

using namespace std;

// Use the small sample while testing; switch to "tracks_features.csv"
// for the full 1.2 million tracks.

const string FILE_NAME = "tracks_sample_1000.csv";
//const string FILE_NAME = "tracks_sample_50000.csv";
//const string FILE_NAME = "tracks_features.csv";

struct Track
{
    string id;
    string name;
    string album;
    string album_id;
    vector<string> artists;
    vector<string> artist_ids;
    int    track_number;
    int    disc_number;
    bool   explicit_content; // "explicit" is a C++ keyword, so it gets a new name
    double danceability;
    double energy;
    int    key;
    double loudness;
    int    mode;
    double speechiness;
    double acousticness;
    double instrumentalness;
    double liveness;
    double valence;
    double tempo;
    int    duration_ms;
    int    time_signature;
    int    year;
    string release_date;     // "1999-11-02", or sometimes just "1999"
};

// Copies the reader's current track into a Track struct.
Track makeTrack(const Track_Reader& reader)
{
    Track t;
    t.id = reader.getString("id");
    t.name = reader.getString("name");
    t.album = reader.getString("album");
    t.album_id = reader.getString("album_id");
    t.artists = reader.getList("artists");
    t.artist_ids = reader.getList("artist_ids");
    t.track_number = reader.getInt("track_number");
    t.disc_number = reader.getInt("disc_number");
    t.explicit_content = reader.getBool("explicit");
    t.danceability = reader.getDouble("danceability");
    t.energy = reader.getDouble("energy");
    t.key = reader.getInt("key");
    t.loudness = reader.getDouble("loudness");
    t.mode = reader.getInt("mode");
    t.speechiness = reader.getDouble("speechiness");
    t.acousticness = reader.getDouble("acousticness");
    t.instrumentalness = reader.getDouble("instrumentalness");
    t.liveness = reader.getDouble("liveness");
    t.valence = reader.getDouble("valence");
    t.tempo = reader.getDouble("tempo");
    t.duration_ms = reader.getInt("duration_ms");
    t.time_signature = reader.getInt("time_signature");
    t.year = reader.getInt("year");
    t.release_date = reader.getString("release_date");
    return t;
}

void printTrack(const Track& track)
{
    cout << track.name << endl;
    cout << "  Album:   " << track.album << " (" << track.year << ")" << endl;
    cout << "  Artists: ";
    for (size_t i = 0; i < track.artists.size(); i++)
    {
        if (i > 0)
        {
            cout << ", ";
        }
        cout << track.artists[i];
    }
    cout << endl;
    cout << "  Tempo:   " << track.tempo << " bpm, "
        << track.duration_ms / 1000 << " seconds"
        << " | Energy: " << track.energy
        << " | Valence: " << track.valence << endl;
}

// Helper function to print an entire playlist with a title header.
void printPlaylist(const string& title, const vector<Track>& playlist)
{
    cout << "======================================================" << endl;
    cout << "  " << title << " (" << playlist.size() << " songs)" << endl;
    cout << "======================================================" << endl;
    for (size_t i = 0; i < playlist.size(); i++)
    {
        cout << (i + 1) << ". ";
        printTrack(playlist[i]);
    }
    cout << endl;
}

/*
 * ALGORITHM RATIONALE - EXERCISE PLAYLIST:
 * I wanted an energetic workout playlist suited for cardio and running, so I
 * filtered the tracks for high energy (> 0.70), solid danceability (> 0.55),
 * and an upbeat tempo between 115 and 165 BPM. Next, I randomly shuffled all
 * matching tracks and selected the first 20 so the workout mix stays varied
 * across sessions rather than always picking the exact same songs.
 */
vector<Track> createExercisePlaylist(vector<Track> tracks, int numberOfSongs)
{
    vector<Track> candidates;
    for (size_t i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].energy > 0.70 &&
            tracks[i].danceability > 0.55 &&
            tracks[i].tempo >= 115.0 &&
            tracks[i].tempo <= 165.0)
        {
            candidates.push_back(tracks[i]);
        }
    }

    random_device rd;
    mt19937 gen(rd());
    shuffle(candidates.begin(), candidates.end(), gen);

    if (static_cast<int>(candidates.size()) > numberOfSongs)
    {
        candidates.resize(numberOfSongs);
    }
    return candidates;
}

/*
 * ALGORITHM RATIONALE - STUDY & FOCUS PLAYLIST:
 * Studying requires calm, non-distracting background music without heavy vocals
 * or explicit lyrics. I excluded explicit tracks and filtered for low
 * speechiness (< 0.10), noticeable instrumentalness (> 0.25), and low-to-moderate
 * energy (< 0.50). After collecting all qualifying tracks, I shuffled them and
 * selected 20 songs to create a distraction-free study playlist.
 */
vector<Track> createStudyPlaylist(vector<Track> tracks, int numberOfSongs)
{
    vector<Track> candidates;
    for (size_t i = 0; i < tracks.size(); i++)
    {
        if (!tracks[i].explicit_content &&
            tracks[i].speechiness < 0.10 &&
            tracks[i].instrumentalness > 0.25 &&
            tracks[i].energy < 0.50)
        {
            candidates.push_back(tracks[i]);
        }
    }

    random_device rd;
    mt19937 gen(rd());
    shuffle(candidates.begin(), candidates.end(), gen);

    if (static_cast<int>(candidates.size()) > numberOfSongs)
    {
        candidates.resize(numberOfSongs);
    }
    return candidates;
}

/*
 * ALGORITHM RATIONALE - UPBEAT ROAD TRIP PLAYLIST:
 * A great driving playlist should feel cheerful, energetic, and free of live
 * audience noise. I filtered for positive mood (valence > 0.60), moderate-to-high
 * energy (> 0.55), low liveness (< 0.30 to focus on studio recordings), and
 * standard song lengths between 2 and 6 minutes (120,000 to 360,000 ms). After
 * randomly picking 20 tracks, I sorted them in ascending order of energy so the
 * drive starts relaxed and gradually builds momentum.
 */
vector<Track> createRoadTripPlaylist(vector<Track> tracks, int numberOfSongs)
{
    vector<Track> candidates;
    for (size_t i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].valence > 0.60 &&
            tracks[i].energy > 0.55 &&
            tracks[i].liveness < 0.30 &&
            tracks[i].duration_ms >= 120000 &&
            tracks[i].duration_ms <= 360000)
        {
            candidates.push_back(tracks[i]);
        }
    }

    random_device rd;
    mt19937 gen(rd());
    shuffle(candidates.begin(), candidates.end(), gen);

    if (static_cast<int>(candidates.size()) > numberOfSongs)
    {
        candidates.resize(numberOfSongs);
    }

    // Sort the 20 selected tracks from lowest energy to highest energy
    sort(candidates.begin(), candidates.end(), [](const Track& a, const Track& b) {
        return a.energy < b.energy;
        });

    return candidates;
}

int main()
{
    Track_Reader reader(FILE_NAME);
    if (!reader.isOpen())
    {
        return 1;
    }

    vector<Track> tracks;
    while (reader.nextTrack())
    {
        tracks.push_back(makeTrack(reader));
    }

    cout << "Read " << tracks.size() << " tracks from " << FILE_NAME << endl;
    cout << endl;

    const int PLAYLIST_SIZE = 20;

    vector<Track> exercisePlaylist = createExercisePlaylist(tracks, PLAYLIST_SIZE);
    vector<Track> studyPlaylist = createStudyPlaylist(tracks, PLAYLIST_SIZE);
    vector<Track> roadTripPlaylist = createRoadTripPlaylist(tracks, PLAYLIST_SIZE);

    printPlaylist("HIGH-ENERGY EXERCISE PLAYLIST", exercisePlaylist);
    printPlaylist("DEEP FOCUS STUDY PLAYLIST", studyPlaylist);
    printPlaylist("UPBEAT ROAD TRIP PLAYLIST", roadTripPlaylist);

    return 0;
}