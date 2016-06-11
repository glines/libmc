/*
 * Copyright (c) 2016 Jonathan Glines
 * Jonathan Glines <jonathan@glines.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <algorithm>
#include <cassert>
#include <cstring>
#include <dirent.h>
#include <png.h>
#include <vector>

#include "scan.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

namespace mc { namespace samples {
  Scan::Scan(const std::string &path)
    : m_samples(nullptr), m_scalarField(this)
  {
    m_init(path);
  }

  void Scan::m_init(const std::string &path) {
    /* Read the directory at the given path */
    DIR *d = opendir(path.c_str());
    if (d == nullptr) {
      fprintf(stderr, "Could not open scan directory '%s'\n",
          path.c_str());
    }
    /* Get a list of all .png files in the directory */
    std::vector<std::string> files;
    struct dirent *de;
    while ((de = readdir(d)) != nullptr) {
      int length = strlen(de->d_name);
      char extension[5];
      memcpy(extension, de->d_name + (length - 4), 4);;
      extension[4] = '\0';
      for (int i = 0; extension[i] != '\0'; ++i) {
        extension[i] = tolower(extension[i]);
      }
      if (strcmp(extension, ".png") == 0) {
        /* Add this .png file to the list of image files */
        files.push_back(std::string(de->d_name));
      }
    }
    /* Sort the .png files alphabetically */
    std::sort(files.begin(), files.end());

    /* The conut of .png files in the directory determines the z-axis
     * resolution */
    m_zRes = files.size();

    /* Account for the trivial case of no slices */
    if (files.size() == 0) {
      m_xRes = m_yRes = 0;
      return;
    }

    /* Iterate through all of the image files */
    for (size_t z = 0; z < files.size(); ++z) {
      std::string filepath = path + "/" + files[z];
      FILE *fp = fopen(filepath.c_str(), "rb");
      if (!fp) {
        fprintf(stderr, "Could not open '%s'\n", filepath.c_str());
        m_abortInit();
        return;
      }
      const int PNG_HEADER_SIZE = 8;
      unsigned char header[PNG_HEADER_SIZE];
      fread(header, sizeof(unsigned char), PNG_HEADER_SIZE, fp);
      bool is_png = !png_sig_cmp(header, 0, PNG_HEADER_SIZE);
      if (!is_png) {
        fprintf(stderr, "The file '%s' is not a valid PNG image\n",
            filepath.c_str());
        m_abortInit();
        return;
      }
      png_structp png_ptr = png_create_read_struct(
          PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
      if (!png_ptr) {
        m_abortInit();
        return;
      }
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        m_abortInit();
        return;
      }
      png_init_io(png_ptr, fp);
      png_set_sig_bytes(png_ptr, 8);
      /* Read the entire PNG image into memory */
      png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
      /* Read the dimensions, bit depth, etc. of the image file */
      int width, height, bit_depth, color_type;
      width = png_get_image_width(png_ptr, info_ptr);
      height = png_get_image_height(png_ptr, info_ptr);
      bit_depth = png_get_bit_depth(png_ptr, info_ptr);
      color_type = png_get_color_type(png_ptr, info_ptr);
      if (color_type != PNG_COLOR_TYPE_GRAY) {
        fprintf(stderr, "The PNG file '%s' is not grayscale",
            files[z].c_str());
        m_abortInit();
        return;
      }
      if (bit_depth != 8) {
        fprintf(stderr, "The PNG file '%s' does not use a bit depth of 8",
            files[z].c_str());
        m_abortInit();
        return;
      }
      if (z == 0) {
        /* The dimensions of the first PNG file determine the x and y
         * resolutions of the sample lattice */
        m_xRes = width;
        m_yRes = height;
        /* Allocate memory for our sample lattice */
        m_samples = new float [m_xRes * m_yRes * m_zRes];
      } else {
        /* Make sure that the image for this slice is the same dimension as all
         * of the other slices */
        if (width != m_xRes || height != m_yRes) {
          fprintf(stderr,
              "The PNG image '%s' has dimensions '%dx%d' while it should have dimensions '%dx%d'",
              files[z].c_str(), width, height, m_xRes, m_yRes);
          m_abortInit();
          return;
        }
      }
      /* Iterate through the image pixels and populate this slice in the
       * sample lattice */
      auto row_pointers = png_get_rows(png_ptr, info_ptr);
      for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
          size_t i = x + y * width + z * width * height;
          m_samples[i] = (float)row_pointers[y][x] / (float)255;
          m_samples[i] -= 0.5f;  /* FIXME: This is a hack while we don't have contour value arguments */
        }
      }
      /* Close the PNG image file */
      /* FIXME: Reading files like this probably incurs a lot of overhead
       * allocating memory within libpng */
      /* TODO: Destroy the info struct? */
      fclose(fp);
      png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    }
  }

  void Scan::m_abortInit() {
    /* Free the sample lattice and nullify its pointer */
    delete[] m_samples;
    m_samples = nullptr;
    /* Set the resolution to zero */
    m_xRes = m_yRes = m_zRes = 0;
  }

  float Scan::sample(int x, int y, int z) const {
    assert(x >= 0);
    assert(y >= 0);
    assert(z >= 0);
    assert(x < m_xRes);
    assert(y < m_yRes);
    assert(z < m_zRes);
    int i = x + y * m_xRes + z * m_xRes * m_yRes;
    return m_samples[i];
  }

  const ScalarField &Scan::scalarField() {
    return m_scalarField;
  }

  Scan::ScanScalarField::ScanScalarField(const Scan *scan)
    : m_scan(scan)
  {
  }

  float Scan::ScanScalarField::operator()(float x, float y, float z) const {
    /* Map the input values from the cube within the range -1.0 to 1.0 to the
     * range 0.0 to 1.0 */
    x = (x + 1.0f) / 2.0f;
    y = (y + 1.0f) / 2.0f;
    z = (z + 1.0f) / 2.0f;
    /* TODO: Perform trilinear interpolation on the eight samples that appear
     * in this lattice voxel */
    float result = 0.0f;
    for (int z_index = 0; z_index < 2; ++z_index) {
      for (int y_index = 0; y_index < 2; ++y_index) {
        for (int x_index = 0; x_index < 2; ++x_index) {
          /* Get the value of the sample at this cube vertex from our sample lattice */
          float value = m_scan->sample(
              min(max((int)floor(x * (float)m_scan->xRes()) + x_index,
                  0), m_scan->xRes() - 1),
              min(max((int)floor(y * (float)m_scan->yRes()) + y_index,
                  0), m_scan->yRes() - 1),
              min(max((int)floor(z * m_scan->zRes()) + z_index,
                  0), m_scan->zRes() - 1));
          result +=
            (x_index ? x : 1.0f - x) *
            (y_index ? y : 1.0f - y) *
            (z_index ? z : 1.0f - z)
            * value;
        }
      }
    }
    return result;
  }
} }
