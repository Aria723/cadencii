/*
 * WaveformZoomUi.cs
 * Copyright © 2011 kbinani
 *
 * This file is part of cadencii.
 *
 * cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPLv3 License.
 *
 * cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
    namespace cadencii
    {

#if CSHARP
            using System;
            using cadencii;
#else
            using namespace org::kbinani::cadencii;
#endif

            public interface WaveformZoomUi
            {
                int getWidth();

                int getHeight();

                void setListener( WaveformZoomUiListener listener );

                void repaint();
            };

    }
