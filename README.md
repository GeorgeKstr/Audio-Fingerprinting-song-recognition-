# Music Track Identifier (C++ App)

## Overview

This C++ application provides a command-line interface for identifying music tracks based on their frequency fingerprints. It utilizes various libraries to decode MP3 files, extract spectrograms, perform Fourier transforms, and handle directory iteration.

## Features

- **MP3 Decoding:**
  - Utilizes `mpg123` for decoding MP3 files.
- **Image Extraction:**
  - Uses `libpng` and `png++` for extracting spectrograms from music tracks.
- **Directory Iteration:**
  - Incorporates `dirent` for efficient directory iteration to scan music files.
- **Fourier Transform:**
  - Utilizes `fftw` for transforming time-domain data into frequency-domain data.
- **Database Storage:**
  - Fingerprint information is hashed and stored in a hashmap-style database within a text file.
- **Cross-Platform:**
  - Designed to work in the command-line environment, compatible with Code::Blocks on multiple platforms.

## Usage

1. Place your music tracks in the "tracks" folder within the application directory.
2. Open the Code::Blocks project file.
3. Build the project.
4. Run the application within the Code::Blocks environment or navigate to the project's executable file in the command-line environment.
5. The application will scan the "tracks" folder and analyze each music file.
6. After scanning, the user can identify tracks using their fingerprints.

## Dependencies

- **mpg123:** For MP3 decoding.
- **libpng and png++:** For image extraction (spectrograms).
- **dirent:** For directory iteration.
- **fftw:** For Fourier transforms.
- **zlib:** Dependency of libpng.

## Getting Started

To get started with the project:

1. Clone or download the project repository.
2. Open the Code::Blocks project file in Code::Blocks IDE.
3. Ensure that all necessary dependencies are properly installed on your system.
4. Build the project within Code::Blocks.
5. Follow the usage instructions mentioned above to run the application.


