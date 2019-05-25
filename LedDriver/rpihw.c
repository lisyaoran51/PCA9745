/*
 * rpihw.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "rpihw.h"


#define LINE_WIDTH_MAX                           80
#define HW_VER_STRING                            "Revision"

#define PERIPH_BASE_RPI                          0x20000000
#define PERIPH_BASE_RPI2                         0x3f000000

#define VIDEOCORE_BASE_RPI                       0x40000000
#define VIDEOCORE_BASE_RPI2                      0xc0000000

#define RPI_MANUFACTURER_MASK                    (0xf << 16)
#define RPI_WARRANTY_MASK                        (0x3 << 24)

static const rpi_hw_t rpi_hw_info[] = {
    //
    // Model B Rev 1.0
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x02,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x03,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },

    //
    // Model B Rev 2.0
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x04,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x05,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x06,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },

    //
    // Model A
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x07,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x08,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x09,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A",
    },

    //
    // Model B
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x0d,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x0e,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x0f,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B",
    },

    //
    // Model B+
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x10,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B+",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x13,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B+",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x900032,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model B+",
    },

    //
    // Compute Module
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x11,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Compute Module 1",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x14,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Compute Module 1",
    },

    //
    // Pi Zero
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x900092,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Pi Zero v1.2",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x900093,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Pi Zero v1.3",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x920093,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Pi Zero v1.3",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x9200c1,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Pi Zero W v1.1",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x9000c1,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Pi Zero W v1.1",
    },

    //
    // Model A+
    //
    {
        RPI_HWVER_TYPE_PI1,
		0x12,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A+",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x15,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A+",
    },
    {
        RPI_HWVER_TYPE_PI1,
		0x900021,
        PERIPH_BASE_RPI,
        VIDEOCORE_BASE_RPI,
        "Model A+",
    },

    //
    // Pi 2 Model B
    //
    {
        RPI_HWVER_TYPE_PI2,
		0xa01041,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 2",
    },
    {
        RPI_HWVER_TYPE_PI2,
		0xa01040,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 2",
    },
    {
        RPI_HWVER_TYPE_PI2,
		0xa21041,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 2",
    },
    //
    // Pi 2 with BCM2837
    //
    {
        RPI_HWVER_TYPE_PI2,
		0xa22042,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 2",
    },
    //
    // Pi 3 Model B
    //
    {
        RPI_HWVER_TYPE_PI2,
		0xa020d3,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 3 B+",
    },
    {
        RPI_HWVER_TYPE_PI2,
		0xa02082,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 3",
    },
    {
        RPI_HWVER_TYPE_PI2,
		0xa22082,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Pi 3",
    },
    //
    // Pi Compute Module 3
    //
    {
        RPI_HWVER_TYPE_PI2,
		0xa020a0,
        PERIPH_BASE_RPI2,
        VIDEOCORE_BASE_RPI2,
        "Compute Module 3/L3",
    },

};


const rpi_hw_t *rpi_hw_detect(void)
{
    FILE *f = fopen("/proc/cpuinfo", "r");
    char line[LINE_WIDTH_MAX];
    const rpi_hw_t *result = NULL;

    if (!f)
    {
        return NULL;
    }

    while (fgets(line, LINE_WIDTH_MAX - 1, f))
    {
        if (strstr(line, HW_VER_STRING))
        {
            uint32_t rev;
            char *substr;
            unsigned i;

            substr = strstr(line, ": ");
            if (!substr)
            {
                continue;
            }

            errno = 0;
            rev = strtoul(&substr[1], NULL, 16);  // Base 16
            if (errno)
            {
                continue;
            }

            for (i = 0; i < (sizeof(rpi_hw_info) / sizeof(rpi_hw_info[0])); i++)
            {
                uint32_t hwver = rpi_hw_info[i].hwver;

                // Take out warranty and manufacturer bits
                hwver &= ~(RPI_WARRANTY_MASK | RPI_MANUFACTURER_MASK);
                rev &= ~(RPI_WARRANTY_MASK | RPI_MANUFACTURER_MASK);
                
                if (rev == hwver)
                {
                    result = &rpi_hw_info[i];

                    goto done;
                }
            }
        }
    }

done:
    fclose(f);

    return result;
}

