/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#pragma once

#include "../../lightsensor.h"
#include "apds9960.h"

class Apds9960LightSensor : public LightSensor {
    Q_OBJECT

 public:
    explicit Apds9960LightSensor(APDS9960* apds, QObject* parent = nullptr);

    int ambientLight() override { return static_cast<int>(m_ambientLight); }

    Q_INVOKABLE int readAmbientLight() override;

    // Device interface
 protected:
    const QLoggingCategory& logCategory() const override;

 private:
    APDS9960* p_apds;
    uint16_t  m_ambientLight = 100;
    uint16_t  m_r;
    uint16_t  m_g;
    uint16_t  m_b;
    uint16_t  m_c;

    uint16_t calculateIlluminance(uint16_t r, uint16_t g, uint16_t b);
};
