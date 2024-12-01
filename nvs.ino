/*****************************************************************************
 * nvs.ino - Source code of NVS interface (EEPROM)
 *
 *
 * Copyright (C) 2024  Rafael R. Queiroz   rafaelrq@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *****************************************************************************/


/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/


void getFromNVS(int addr, void *buf, size_t size)
{
    uint8_t *p = (uint8_t *)buf;

    while (size--) {
        *p++ = EEPROM.read(addr++);
    }
}


void saveIntoNVS(int addr, const void *buf, size_t size)
{
    const uint8_t *p = (const uint8_t *)buf;

    while (size--) {
        EEPROM.update(addr++, *p++);
    }
}

