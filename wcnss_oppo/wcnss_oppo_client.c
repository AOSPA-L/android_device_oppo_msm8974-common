/*
 * Copyright (C) 2014-2015, The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0

#define LOG_TAG "wcnss_oppo"

#define SUCCESS 0
#define FAILED -1

#define GENMAC_FILE "/persist/.genmac"
#define MAC_ADDR_SIZE 6

#include <cutils/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static const char oppo_mac_prefix[] = { 0xa8, 0x1b, 0x5a };

int wcnss_init_qmi(void)
{
    /* empty */
    return SUCCESS;
}

int wcnss_qmi_get_wlan_address(unsigned char *pBdAddr)
{
    int i = 0;
    struct stat mac_stat;
    int success = 0;

    // Use a previously stored value on failure (if it exists)
    if (!stat(GENMAC_FILE, &mac_stat)) {
        FILE *genmac = fopen(GENMAC_FILE,"r");
        if (fscanf(genmac, "%c%c%c%c%c%c",
                &pBdAddr[0], &pBdAddr[1], &pBdAddr[2],
                &pBdAddr[3], &pBdAddr[4], &pBdAddr[5]) == 6) {
            ALOGE("Successfully read local MAC address");
            success = 1;
        }
        fclose(genmac);
    }

    // If that fails as well, randomize a MAC
    if (!success) {
        memcpy(pBdAddr, oppo_mac_prefix, sizeof(oppo_mac_prefix));

        // We don't need strong randomness, and if the NV is corrupted
        // any hardware values are suspect, so just seed it with the
        // current time
        srand(time(NULL));

        for (i = sizeof(oppo_mac_prefix) / sizeof(oppo_mac_prefix[0]); i < MAC_ADDR_SIZE; i++) {
            pBdAddr[i] = rand() % 255;
        }

        ALOGI("Using randomized MAC address");

        // Store for reuse
        FILE *genmac = fopen(GENMAC_FILE, "w");
        fwrite(pBdAddr, 1, MAC_ADDR_SIZE, genmac);
        fclose(genmac);
    }

    ALOGI("Using MAC address: %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
            pBdAddr[0], pBdAddr[1], pBdAddr[2],
            pBdAddr[3], pBdAddr[4], pBdAddr[5]);

    return SUCCESS;
}

void wcnss_qmi_deinit(void)
{
    /* empty */
}
