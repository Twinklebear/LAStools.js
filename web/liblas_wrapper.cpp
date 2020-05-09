#include <array>
#include <iostream>
#include <limits>
#include <vector>
#include <lasreader.hpp>

struct LASFile {
    LASreader *reader;
    // TODO: later load in the i64 format used by LAS for full precision
    std::vector<float> positions;
    // rgba8 color data, if any
    std::vector<uint8_t> colors;

    // The data set bounds, kept in memory to share w/ JS
    std::array<float, 6> bounds;

    LASFile(const char *fname);
    ~LASFile();

    LASFile(const LASFile &) = delete;
    LASFile &operator=(const LASFile &) = delete;

    void load_points(bool filter_noise);
};

LASFile::LASFile(const char *fname)
{
    LASreadOpener read_opener;
    read_opener.set_file_name(fname);
    reader = read_opener.open();
    bounds[0] = reader->get_min_x();
    bounds[1] = reader->get_min_y();
    bounds[2] = reader->get_min_z();
    bounds[3] = reader->get_max_x();
    bounds[4] = reader->get_max_y();
    bounds[5] = reader->get_max_z();
}

LASFile::~LASFile()
{
    reader->close();
    delete reader;
}

void LASFile::load_points(bool filter_noise)
{
    reader->seek(0);
    const bool has_color = reader->header.point_data_format == 2 ||
                           reader->header.point_data_format == 3 ||
                           reader->header.point_data_format == 5;

    // TODO: later we need to track some handle I think? to release this
    positions.reserve(reader->npoints * 3);
    if (has_color) {
        colors.reserve(reader->npoints * 3);
    }
    const float inv_max_uint16 = 1.f / std::numeric_limits<uint16_t>::max();
    for (size_t i = 0; i < reader->npoints && reader->read_point(); ++i) {
        // Points classified as low point are noise and should be discarded
        if (filter_noise && reader->point.get_classification() == 7) {
            continue;
        }

        reader->point.compute_coordinates();
        for (size_t j = 0; j < 3; ++j) {
            positions.push_back(reader->point.coordinates[j]);
        }

        if (has_color) {
            for (size_t j = 0; j < 4; ++j) {
                colors.push_back(static_cast<uint8_t>(255.0 * reader->point.rgb[j] * inv_max_uint16));
            }
        }
    }
}

extern "C" LASFile *open_las(const char *fname)
{
    return new LASFile(fname);
}

extern "C" void close_las(LASFile *file)
{
    delete file;
}

// Load the points and return the number loaded
extern "C" uint64_t load_points(LASFile *file, bool filter_noise)
{
    file->load_points(filter_noise);
    return file->positions.size() / 3;
}

extern "C" uint64_t get_num_points(LASFile *file)
{
    return file->reader->npoints;
}

extern "C" uint64_t get_num_loaded_points(LASFile *file)
{
    return file->positions.size() / 3;
}

extern "C" float *get_bounds(LASFile *file)
{
    return file->bounds.data();
}

extern "C" float *get_positions(LASFile *file)
{
    return file->positions.data();
}

extern "C" uint8_t *get_colors(LASFile *file)
{
    return file->colors.data();
}

extern "C" int has_colors(LASFile *file)
{
    return !file->colors.empty() ? 1 : 0;
}

