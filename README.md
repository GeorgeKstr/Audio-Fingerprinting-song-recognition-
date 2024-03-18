Music Track Identifier (C++ App)
Overview
This C++ application provides a command-line interface for identifying music tracks based on their frequency fingerprints. It utilizes various libraries to decode MP3 files, extract spectrograms, perform Fourier transforms, and handle directory iteration.

Features
MP3 Decoding: Utilizes mpg123 for decoding MP3 files.
Image Extraction: Uses libpng and png++ for extracting spectrograms from music tracks.
Directory Iteration: Incorporates dirent for efficient directory iteration to scan music files.
Fourier Transform: Utilizes fftw for transforming time-domain data into frequency-domain data.
Database Storage: Fingerprint information is hashed and stored in a hashmap-style database within a text file.
Cross-Platform: Designed to work in the Windows command-line environment.

Usage
Place your music tracks in the "tracks" folder within the application directory.
Run the application in the command-line environment.
The application will scan the "tracks" folder and analyze each music file.
After scanning, the user can identify tracks using their fingerprints.

Dependencies
mpg123: For MP3 decoding.
libpng and png++: For image extraction (spectrograms).
dirent: For directory iteration.
fftw: For Fourier transforms.
zlib: Dependency of libpng.
