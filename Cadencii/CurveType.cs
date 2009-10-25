﻿/*
 * CurveType.cs
 * Copyright (c) 2009 kbinani
 *
 * This file is part of Boare.Cadencii.
 *
 * Boare.Cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPLv3 License.
 *
 * Boare.Cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#if JAVA
package org.kbinani.Cadencii;

import java.io.*;
#else
using System;

namespace Boare.Cadencii {
    using boolean = Boolean;
#endif

    /// <summary>
    /// vsqファイルで編集可能なカーブ・プロパティの種類
    /// </summary>
#if JAVA
    public class CurveType implements Serializable {
#else
    [Serializable]
    public struct CurveType : IEquatable<CurveType>, IComparable<CurveType> {
#endif
        private String m_type;
        private boolean m_is_scalar;
        private int m_minimum;
        private int m_maximum;
        private int m_default;
        private boolean m_is_attach_note;
        private int m_index;

        /// <summary>
        /// ベロシティ(index=-1)
        /// </summary>
        public static readonly CurveType VEL = new CurveType( "VEL", true, true, 0, 127, 64, -1 );
        /// <summary>
        /// ダイナミクス　64(index=0)
        /// </summary>
        public static readonly CurveType DYN = new CurveType( "DYN", false, false, 0, 127, 64, 0 );
        /// <summary>
        /// ブレシネス　0(index=1)
        /// </summary>
        public static readonly CurveType BRE = new CurveType( "BRE", false, false, 0, 127, 0, 1 );
        /// <summary>
        /// ブライトネス　64(index=2)
        /// </summary>
        public static readonly CurveType BRI = new CurveType( "BRI", false, false, 0, 127, 64, 2 );
        /// <summary>
        /// クリアネス　0(index=3)
        /// </summary>
        public static readonly CurveType CLE = new CurveType( "CLE", false, false, 0, 127, 0, 3 );
        /// <summary>
        /// オープニング　127(index=4)
        /// </summary>
        public static readonly CurveType OPE = new CurveType( "OPE", false, false, 0, 127, 127, 4 );
        /// <summary>
        /// ジェンダーファクター　64(index=5)
        /// </summary>
        public static readonly CurveType GEN = new CurveType( "GEN", false, false, 0, 127, 64, 5 );
        /// <summary>
        /// ポルタメントタイミング　64(index=6)
        /// </summary>
        public static readonly CurveType POR = new CurveType( "POR", false, false, 0, 127, 64, 6 );
        public static readonly CurveType PIT = new CurveType( "PIT", false, false, -8192, 8191, 0, 7 );
        public static readonly CurveType PBS = new CurveType( "PBS", false, false, 0, 24, 2, 8 );
        /// <summary>
        /// ビブラートの振動の速さ(index=9)
        /// </summary>
        public static readonly CurveType VibratoRate = new CurveType( "V-Rate", false, true, 0, 127, 64, 9 );
        /// <summary>
        /// ビブラートの振幅の大きさ(index=10)
        /// </summary>
        public static readonly CurveType VibratoDepth = new CurveType( "V-Depth", false, true, 0, 127, 50, 10 );
        /// <summary>
        /// Accent(index=-1)
        /// </summary>
        public static readonly CurveType Accent = new CurveType( "Accent", true, true, 0, 100, 50, -1 );
        /// <summary>
        /// Decay(index=-1)
        /// </summary>
        public static readonly CurveType Decay = new CurveType( "Decay", true, true, 0, 100, 50, -1 );
        /// <summary>
        /// Harmonics(index=11)
        /// </summary>
        public static readonly CurveType harmonics = new CurveType( "Harmonics", false, false, 0, 127, 0, 11 );
        /// <summary>
        /// FX2Depth(index=12)
        /// </summary>
        public static readonly CurveType fx2depth = new CurveType( "FX2Depth", false, false, 0, 127, 0, 12 );
        /// <summary>
        /// reso1freq(index=13)
        /// </summary>
        public static readonly CurveType reso1freq = new CurveType( "reso1freq", false, false, 0, 127, 0, 13 );
        /// <summary>
        /// reso1bw(index=14)
        /// </summary>
        public static readonly CurveType reso1bw = new CurveType( "reso1bw", false, false, 0, 127, 0, 14 );
        /// <summary>
        /// reso1amp(index=15)
        /// </summary>
        public static readonly CurveType reso1amp = new CurveType( "reso1amp", false, false, 0, 127, 0, 15 );
        /// <summary>
        /// reso2freq(index=16)
        /// </summary>
        public static readonly CurveType reso2freq = new CurveType( "reso2freq", false, false, 0, 127, 0, 16 );
        /// <summary>
        /// reso2bw(index=17)
        /// </summary>
        public static readonly CurveType reso2bw = new CurveType( "reso2bw", false, false, 0, 127, 0, 17 );
        /// <summary>
        /// reso2amp(index=18)
        /// </summary>
        public static readonly CurveType reso2amp = new CurveType( "reso2amp", false, false, 0, 127, 0, 18 );
        /// <summary>
        /// reso3freq(index=19)
        /// </summary>
        public static readonly CurveType reso3freq = new CurveType( "reso3freq", false, false, 0, 127, 0, 19 );
        /// <summary>
        /// reso3bw(index=20)
        /// </summary>
        public static readonly CurveType reso3bw = new CurveType( "reso3bw", false, false, 0, 127, 0, 20 );
        /// <summary>
        /// reso3amp(index=21)
        /// </summary>
        public static readonly CurveType reso3amp = new CurveType( "reso3amp", false, false, 0, 127, 0, 21 );
        /// <summary>
        /// reso4freq(index=22)
        /// </summary>
        public static readonly CurveType reso4freq = new CurveType( "reso4freq", false, false, 0, 127, 0, 22 );
        /// <summary>
        /// reso4bw(index=23)
        /// </summary>
        public static readonly CurveType reso4bw = new CurveType( "reso4bw", false, false, 0, 127, 0, 23 );
        /// <summary>
        /// reso4amp(index=24)
        /// </summary>
        public static readonly CurveType reso4amp = new CurveType( "reso4amp", false, false, 0, 127, 0, 24 );
        public static readonly CurveType Env = new CurveType( "Env", true, true, 0, 200, 100, -1 );

        public static readonly CurveType Empty = new CurveType( "Empty", false, false, 0, 0, 0, -1 );

        private CurveType( String type, boolean is_scalar, boolean is_attach_note, int min, int max, int defalt_value, int index ) {
            m_type = type;
            m_is_scalar = is_scalar;
            m_minimum = min;
            m_maximum = max;
            m_default = defalt_value;
            m_is_attach_note = is_attach_note;
            m_index = index;
        }

        public int compareTo( CurveType item ) {
            if ( m_index == -1 ) {
                if ( item.m_index == -1 ) {
                    return m_type.CompareTo( item.m_type );
                } else {
                    return 1;
                }
            } else {
                if ( item.m_index == -1 ) {
                    return -1;
                } else {
                    return m_index - item.m_index;
                }
            }
        }

#if !JAVA
        public int CompareTo( CurveType item ) {
            return compareTo( item );
        }
#endif

#if !JAVA
        public override String ToString() {
            return toString();
        }
#endif

        public String toString() {
            return m_type;
        }

        public int getIndex() {
            return m_index;
        }

        public String getName() {
            return m_type;
        }

        public boolean isAttachNote() {
            return m_is_attach_note;
        }

        public boolean isScalar() {
            return m_is_scalar;
        }

        public int getMaximum() {
            return m_maximum;
        }

        public int getMinimum() {
            return m_minimum;
        }

        public int getDefault() {
            return m_default;
        }

        public boolean equals( CurveType other ) {
            return (m_type == other.m_type && m_is_scalar == other.m_is_scalar);
        }

        public boolean Equals( CurveType other ) {
            return equals( other );
        }
    }

#if !JAVA
}
#endif
