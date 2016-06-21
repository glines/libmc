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

#ifndef MC_SAMPLES_SCAN_SCAN_H_
#define MC_SAMPLES_SCAN_SCAN_H_

#include <mcxx/scalarField.h>
#include <string>

namespace mc { namespace samples {
  /**
   * This class stores a 3-dimensional scan read from images representing
   * slices in the scan.
   *
   * \todo Allow the slice delta to be specified.
   */
  class Scan {
    private:
      float *m_samples;
      int m_xRes, m_yRes, m_zRes;

      class ScanScalarField : public ScalarField {
        friend Scan;
        private:
          const Scan *m_scan;

          ScanScalarField(const Scan *scan);
        public:
          float operator()(float x, float y, float z) const;
      };
      ScanScalarField m_scalarField;

      void m_init(const std::string &path);
      void m_abortInit();
    public:
      /**
       * Constructs a scan from the slice images at the given path.
       *
       * \param path The path to a directory containing PNG images representing
       * the scan slices in alphabetical order.
       */
      Scan(const std::string &path);

      /**
       * The x-axis resolution of the scan as it was read from file.
       *
       * \return The x-axis resolution of the scan.
       */
      int xRes() const { return m_xRes; }
      /**
       * The y-axis resolution of the scan as it was read from file.
       *
       * \return The y-axis resolution of the scan.
       */
      int yRes() const { return m_yRes; }
      /**
       * The z-axis resolution of the scan as it was read from file.
       *
       * \return The z-axis resolution of the scan.
       */
      int zRes() const { return m_zRes; }
      /**
       * Method to access the sample values of the scan at a given sample
       * lattice point.
       *
       * \param x The x-axis sample lattice coordinate.
       * \param y The y-axis sample lattice coordinate.
       * \param z The z-axis sample lattice coordinate.
       * \return The value of the sample at the x, y, z coordinate of the scan.
       */
      float sample(int x, int y, int z) const;

      /**
       * Returns a scalar field representing the 3D scan. Samples from the
       * sample lattice of the scan are trilinearly interpolated.
       *
       * \return A const reference to a scalar field representation of the
       * scan.
       */
      const ScalarField &scalarField();
  };
} }

#endif
